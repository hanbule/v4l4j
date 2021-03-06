/*
* Copyright (C) 2007-2008 Gilles Gigan (gilles.gigan@gmail.com)
* eResearch Centre, James Cook University (eresearch.jcu.edu.au)
*
* This program was developed as part of the ARCHER project
* (Australian Research Enabling Environment) funded by a
* Systemic Infrastructure Initiative (SII) grant and supported by the Australian
* Department of Innovation, Industry, Science and Research
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public  License as published by the
* Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/
#include <string.h>
#include <stdbool.h>
#include <sys/ioctl.h>		//for ioctl

#include "libvideo.h"
#include "v4l2-query.h"
#include "v4l2-input.h"
#include "libvideo-err.h"
#include "log.h"
#include "list.h"
#include "libvideo-palettes.h"

//forward declarations
static int try_format(unsigned int index, unsigned int width, unsigned int height, struct v4l2_format *dst, struct v4l2_format *src, struct v4lconvert_data *conv);
static bool get_current_resolution(struct video_device *vdev, unsigned int *width, unsigned int *height) __attribute__((nonnull (1, 2, 3)));

/*
 * this function takes an image format int returned by v4l2
 * and finds the matching libvideo id
 * returns -1 if not found
 */
static int find_v4l2_palette(unsigned int v4l2_fmt){
	for (unsigned int i = 0; i < libvideo_palettes_size; i++)
		if(libvideo_palettes[i].v4l2_palette == v4l2_fmt)
			return (signed) i;
	
	info("Error looking up V4L2 format %#x.\n", v4l2_fmt);
	PRINT_REPORT_ERROR();
	return -1;
}

//this function enumerates the frame intervals for a given v4l2 format fmt
//image width and height and modifies the pointer at p to point to either
//NULL, an array of struct frame_intv_discrete or struct frame_intv_continuous.
//It returns FRAME_INTV_UNSUPPORTED, DISCRETE or CONTINUOUS respectively.
static int lookup_frame_intv(struct v4lconvert_data *conv, unsigned int fmt, unsigned int width, unsigned int height, void **p){
	int intv_type = FRAME_INTV_UNSUPPORTED;
	struct frame_intv_discrete *d = NULL;
	size_t discrete_capacity = 0;
	struct frame_intv_continuous *c = NULL;
	
	struct v4l2_frmivalenum intv;
	CLEAR(intv);
	intv.index = 0;
	intv.pixel_format = fmt;
	intv.width = width;
	intv.height = height;

	//while we have a valid frame interval...
	while (v4lconvert_enum_frameintervals(conv, &intv) == 0) {
		//check its type (discrete, continuous / stepwise)
		if(intv.type == V4L2_FRMIVAL_TYPE_DISCRETE) {
				if(intv_type == FRAME_INTV_UNSUPPORTED) {
					intv_type = FRAME_INTV_DISCRETE;
					dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG1, "QRY: Found discrete frame intv:\n");
				}

				dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG1, "QRY:    %d/%d\n", intv.discrete.numerator, intv.discrete.denominator);

				//increase the array size by one for the extra
				//discrete frame interval
				ARRAY_GROW(d, struct frame_intv_discrete, discrete_capacity, intv.index + 1, 1 + 5);

				//fill in the values of the new element
				d[intv.index].numerator = intv.discrete.numerator;
				d[intv.index].denominator = intv.discrete.denominator;

				//prepare the values for the next iteration
				intv.index++;
				intv.pixel_format = fmt;
				intv.width = width;
				intv.height = height;
		} else if((intv.type == V4L2_FRMIVAL_TYPE_CONTINUOUS) || (intv.type == V4L2_FRMIVAL_TYPE_STEPWISE)) {
			dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG1, "QRY: Found %s frame intv:\n", (intv.type == V4L2_FRMIVAL_TYPE_CONTINUOUS) ? "continuous" : "stepwise");

			// WORKAROUND FOR BROKEN DRIVER:
			// reported on ML:
			// http://groups.google.com/group/v4l4j/browse_thread/thread/a80cb345876acf76?hl=en_US#
			// in this instance, ioctl(VIDIOC_ENUM_FRAMEINTERVALS) returns
			// stepwise frame intervals where
			// min frame interval == max frame interval and a weird value
			// for the step (1/10000000). So instead of reporting this
			// stepwise interval, we report it as a discrete one with only
			// a single value.
			if( (intv.stepwise.min.numerator == intv.stepwise.max.numerator) && (intv.stepwise.min.denominator == intv.stepwise.max.denominator) ) {

				dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG1,
					"QRY: using workaround for broken frame interval\n"
					"QRY: Reporting discrete frame intv:\n"
					"QRY: %d/%d\n", intv.stepwise.min.numerator, intv.stepwise.min.denominator);

				intv_type = FRAME_INTV_DISCRETE;

				//increase the array size by one for the extra
				//discrete frame interval
				XREALLOC(d, struct frame_intv_discrete *, (intv.index + 1) * sizeof(struct frame_intv_discrete));

				//fill in the values of the new element
				d[intv.index].numerator = intv.stepwise.min.numerator;
				d[intv.index].denominator = intv.stepwise.min.denominator;

				intv.index++;
			} else {
				//allocate memory for the continuous intv struct
				XMALLOC(c, struct frame_intv_continuous *, sizeof(struct frame_intv_continuous));
				intv_type = FRAME_INTV_CONTINUOUS;

				//copy the values
				dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG1,
					"min - max - step : %d/%d - %d/%d - %d/%d\n",
					intv.stepwise.min.numerator, intv.stepwise.min.denominator,
					intv.stepwise.max.numerator, intv.stepwise.max.denominator,
					intv.stepwise.step.numerator, intv.stepwise.step.denominator
				);
				c->min.numerator = intv.stepwise.min.numerator;
				c->min.denominator = intv.stepwise.min.denominator;
				c->max.numerator = intv.stepwise.max.numerator;
				c->max.denominator = intv.stepwise.max.denominator;
				c->step.numerator = intv.stepwise.step.numerator;
				c->step.denominator = intv.stepwise.step.denominator;
			}
			
			// exit the while loop
			break;
		}
	}
	
	//store the appropriate result in void** arg
	if(intv_type == FRAME_INTV_DISCRETE) {
		//add a struct frame_intv_discrete with null values at the end of
		//the list
		if (intv.index + 1 != discrete_capacity)
			XREALLOC(d, struct frame_intv_discrete *, (intv.index + 1) * sizeof(struct frame_intv_discrete));
		CLEAR(d[intv.index]);
		*p = d;
	} else if(intv_type == FRAME_INTV_CONTINUOUS) {
		*p = c;
	} else {
		*p = NULL;
	}

	return intv_type;
}

//this function enumerates the frame sizes for a given v4l2 format fmt
//and populates the struct palette_info with these sizes
static void lookup_frame_sizes(struct v4lconvert_data *conv, unsigned int fmt, struct palette_info *p) {
	struct v4l2_frmsizeenum s;
	CLEAR(s);

	s.index = 0;
	s.pixel_format = fmt;
	while(v4lconvert_enum_framesizes(conv, &s) == 0) {
		if(s.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
			if(p->size_type == FRAME_SIZE_UNSUPPORTED) {
				dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG1, "QRY: Found discrete supported resolution:\n");
				p->size_type = FRAME_SIZE_DISCRETE;
			}
			XREALLOC(p->discrete, struct frame_size_discrete *, (s.index + 1) * sizeof(struct frame_size_discrete));
			CLEAR(p->discrete[s.index]);
			p->discrete[s.index].height = s.discrete.height;
			p->discrete[s.index].width = s.discrete.width;
			dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG1,
				"QRY: %d x %d\n",
				p->discrete[s.index].width, p->discrete[s.index].height);

			//fill in frame intv for this res
			p->discrete[s.index].interval_type =
					lookup_frame_intv(conv, fmt,
							s.discrete.width, s.discrete.height,
							(void **)&p->discrete[s.index].intv.discrete );

			s.index++;
		} else {
			//continuous & stepwise end up here
			dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG1, "QRY: Found %s supported resolution:\n",
					s.type==V4L2_FRMSIZE_TYPE_CONTINUOUS ? "continuous" : "stepwise");
			p->size_type = FRAME_SIZE_CONTINUOUS;
			//copy data
			XMALLOC(p->continuous, struct frame_size_continuous *, sizeof(struct frame_size_continuous));
			p->continuous->max_height = s.stepwise.max_height;
			p->continuous->min_height = s.stepwise.min_height;
			p->continuous->max_width = s.stepwise.max_width;
			p->continuous->min_width= s.stepwise.min_width;
			p->continuous->step_height = s.stepwise.step_height;
			p->continuous->step_width = s.stepwise.step_width;

			dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG1, "QRY: Width x height (min/max/step) : %d,%d,%d x %d,%d,%d\n",
					p->continuous->min_width,  p->continuous->max_width,  p->continuous->step_width,
					p->continuous->min_height, p->continuous->max_height, p->continuous->step_height);

			// fill in frame intv for min / max res
			p->continuous->interval_type_min_res =
					lookup_frame_intv(conv, fmt, s.stepwise.min_width,
							s.stepwise.min_height,
							(void **) &p->continuous->intv_min_res.continuous);
			p->continuous->interval_type_max_res =
					lookup_frame_intv(conv, fmt, s.stepwise.max_width,
							s.stepwise.max_height,
							(void **) &p->continuous->intv_max_res.continuous);
			break;
		}
	}
	//add a struct frame_size_discrete with null values at the end of
	//the list
	if(p->size_type == FRAME_SIZE_DISCRETE) {
		XREALLOC(p->discrete, struct frame_size_discrete *, (s.index + 1) * sizeof(struct frame_size_discrete));
		CLEAR(p->discrete[s.index]);
	}

}

/*
 * this function checks if the given raw native format can be used for capture.
 * If yes, it adds the given raw palette (fmt) to the end of the list of raw
 * palettes at 'p->raw_palettes' of current size 'size', increment 'size' &
 * returns 0.
 * If the palette cannot be used for capture because libv4lconvert doesnt support
 * it, the matching converted palette is advertised as native if there are no
 * raw formats for it yet (if there are, this step is skipped)and -1 is returned
 */
static bool add_raw_format(struct v4lconvert_data *conv, unsigned int width, unsigned int height, struct palette_info *p, unsigned int fmt, unsigned int *size){
	if(fmt != -1u) {
		//test the given native format fmt to see if it can be used
		//by v4lconvert. Sometimes, the native format that must be used to
		//obtain a converted format CANNOT be used for capture in native format
		//(for instance with SPCA561 webcams). See GC issue 7.
		struct v4l2_format src, dst;
		try_format(fmt, width, height, &dst, &src, conv);
		if(dst.fmt.pix.pixelformat != libvideo_palettes[fmt].v4l2_palette) {
			//the given native format is not supported by libv4lconvert
			dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG,
					"QRY: raw palette %d (%s - %#x) cannot be used for capture - Advertising this 'converted' palette (%s - %d) as a native one\n",
					fmt, libvideo_palettes[fmt].name, libvideo_palettes[fmt].v4l2_palette,
					libvideo_palettes[p->index].name, p->index);
			
			if(*size == 0) {
				//advertise this converted format as a native one
				//ONLY IF we havent already added a raw format (*size==0)
				p->raw_palettes = NULL;
				lookup_frame_sizes(conv, libvideo_palettes[fmt].v4l2_palette, p);
			}
			return false;
		}
		// else
		//this raw format can be used for capture, fall through, and add it
	}
	XREALLOC(p->raw_palettes, int *, (*size + 1) * sizeof(int));
	p->raw_palettes[*size] = (signed) fmt;
	*size += 1;
	return true;
}

/*
 * This function searches the raw_palettes int array of current size 'size
 * for the format fmt.
 * Returns true if it is found, false otherwise.
 */
static bool has_raw_format(int *raw_palettes, int fmt, unsigned int size) {
	for (unsigned int i = 0; i < size; i++)
		if(raw_palettes[i] == fmt)
			return true;
	
	return false;
}

/*
 * This function calles v4lconvert_try_format to check if dst format
 * is supported. If it is, this function will fill in src with the format
 * that must be used to obtain dst
 * returns the result of v4lconvert_try_format
 */
static int try_format(unsigned int index, unsigned int width, unsigned int height, struct v4l2_format *dst, struct v4l2_format *src, struct v4lconvert_data *conv) {
	CLEAR(*dst);
	CLEAR(*src);
	
	dst->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	
	dst->fmt.pix.pixelformat = libvideo_palettes[index].v4l2_palette;
	dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG, "Mapped pfmt (%d) => (%d) %06x %06x %d %s\n", index,
		libvideo_palettes[index].libvideo_palette,
		libvideo_palettes[index].v4l1_palette,
		libvideo_palettes[index].v4l2_palette,
		libvideo_palettes[index].depth,
		libvideo_palettes[index].name);
	dst->fmt.pix.width = width;
	dst->fmt.pix.height = height;
	return v4lconvert_try_format(conv, dst, src);
}

static bool get_current_resolution(struct video_device *vdev, unsigned int *width, unsigned int *height) {
	struct v4l2_format fmt;
	
	memset(&fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(vdev->fd, VIDIOC_G_FMT, &fmt) == 0) {
		*width = fmt.fmt.pix.width;
		*height = fmt.fmt.pix.height;
		return true;
	}

	return false;
}

//this function adds the given palette fmt to the list of
//supported palettes in struct device_info. It also
//checks with libv4l_convert if it is converted from another palette
//it returns 0 if everything went fine, LIBVIDEO_ERR_IOCTL otherwise
static int add_supported_palette(struct video_device *vdev, struct device_info *di, unsigned int fmt) {
	di->nb_palettes++;
	XREALLOC(di->palettes, struct palette_info *, di->nb_palettes * sizeof(struct palette_info));

	struct palette_info *curr = &di->palettes[(di->nb_palettes - 1)];
	CLEAR(*curr);
	curr->index = (signed) fmt;
	curr->size_type = FRAME_SIZE_UNSUPPORTED;

	//check if this format is the result of a conversion from another format
	//by libv4l_convert
	unsigned int width = 640;
	unsigned int height = 480;
	get_current_resolution(vdev, &width, &height);
	struct v4l2_format dst, src;
	if(try_format(fmt, width, height, &dst, &src, di->convert) != 0) {
		dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_ERR, "QRY: Error checking palette %s (libv4l convert says: %s)\n",
				libvideo_palettes[fmt].name,
				v4lconvert_get_error_message(di->convert));
		return LIBVIDEO_ERR_IOCTL;
	}

	if(v4lconvert_needs_conversion(di->convert, &src, &dst)) {
		dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG, "QRY: %s is a converted palette\n", libvideo_palettes[fmt].name);
		unsigned int i = 0;
		{
			int src_palette = find_v4l2_palette(src.fmt.pix.pixelformat);
			if (src_palette == -1)
				// We had a problem looking up the palette
				return LIBVIDEO_ERR_IOCTL;
			
			//it is converted from another format
			//adds the format returned by v4lconvert_needs_conversion
			dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG, "QRY: from %d (%s)\n", src_palette, libvideo_palettes[src_palette].name);
			if(!add_raw_format(di->convert, width, height, curr, (unsigned)src_palette, &i))
			//this raw format can not be used for capture. add_raw_format advertises
			//this converted format as a native one, and we MUST exit here.
				return 0;
		}

		//check if there are other formats that can be converted to this one too
		//by trying other image sizes
		struct v4l2_frmsizeenum s;
		s.index = 0;
		s.pixel_format = dst.fmt.pix.pixelformat;
		while(v4lconvert_enum_framesizes(di->convert, &s) == 0) {
			if(s.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
				//try with this resolution
				//dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG,
				//	"QRY: trying %dx%d\n",s.discrete.width, s.discrete.height);
				try_format(fmt, s.discrete.width, s.discrete.height, &dst, &src, di->convert);

				int src_palette = find_v4l2_palette(src.fmt.pix.pixelformat);
				if (src_palette == -1)
					// We had a problem looking up the palette
					return LIBVIDEO_ERR_IOCTL;
				
				if(v4lconvert_needs_conversion(di->convert, &src, &dst) && !has_raw_format(curr->raw_palettes, src_palette, i)) {
					//it is converted from another format which is not
					//in the array yet. adds the format
					dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG, "QRY:  - from %s (#%08d)\n", libvideo_palettes[src_palette].name, src_palette);
					add_raw_format(di->convert, s.discrete.width, s.discrete.height, curr, (unsigned) src_palette, &i);
					if(i == -1u) {
						PRINT_REPORT_ERROR();
						return LIBVIDEO_ERR_IOCTL;
					}
				} //else no need for conversion or format already seen
				s.index++;
			} else {
				//TODO: frame size are stepwise, try other image sizes to see
				//TODO: if there are other raw image formats
				break;
			}
		}
		//End the list with -1
		add_raw_format(di->convert, width, height, curr, -1u, &i);
	} else {
		dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG, "QRY: %s is a native palette\n", libvideo_palettes[fmt].name);
		//it is not converted from another image format
		curr->raw_palettes = NULL;

		//find supported resolutions
		lookup_frame_sizes(di->convert, libvideo_palettes[fmt].v4l2_palette, curr);
	}

	return 0;
}

//this function checks the supporte palettes
//it returns how many supported palettes there are, or LIBVIDEO_ERR_IOCTL
static int check_palettes_v4l2(struct video_device *vdev) {
	vdev->info->convert = v4lconvert_create(vdev->fd);
	struct device_info *di = vdev->info;
	di->palettes = NULL;
	
	dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG, "QRY: Checking supported palettes.\n");
	
	struct v4l2_fmtdesc fmtd;
	CLEAR(fmtd);
	fmtd.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	
	for (fmtd.index = 0; v4lconvert_enum_fmt(vdev->info->convert, &fmtd) >= 0; fmtd.index++) {
		dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG, "{index:%u,type:%u,flags:%u (%s),description:'%s',pixelformat:%x}\n", fmtd.index,fmtd.type,fmtd.flags, (fmtd.flags & V4L2_FMT_FLAG_EMULATED) ? (fmtd.flags & V4L2_FMT_FLAG_COMPRESSED) ? "V4L2_FMT_FLAG_EMULATED | V4L2_FMT_FLAG_COMPRESSED" : "V4L2_FMT_FLAG_EMULATED" : (fmtd.flags & V4L2_FMT_FLAG_COMPRESSED) ? "V4L2_FMT_FLAG_EMULATED" : "none", fmtd.description,fmtd.pixelformat);
		dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG1, "QRY: looking for palette %c%c%c%c (%#06x)\n", v4l2_fourcc_chars(fmtd.pixelformat), fmtd.pixelformat);
		
		int p = find_v4l2_palette(fmtd.pixelformat);
		if (p != -1) {
			dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG, "QRY: %s supported (%d)\n", libvideo_palettes[p].name, p);
			if(add_supported_palette(vdev, di, (unsigned) p) != 0) {
				if(di->palettes)
					XFREE(di->palettes);
				
				return LIBVIDEO_ERR_IOCTL;
			}
		} else {
			info("libvideo has skipped an unsupported image format:%s (%#x)\n", fmtd.description, fmtd.pixelformat);
			PRINT_REPORT_ERROR();
		}
	}
	
	return (signed) vdev->info->nb_palettes;
}

static bool query_tuner(struct video_input_info *vi, int fd, unsigned int index){
	struct v4l2_tuner t;
	CLEAR(t);
	t.index = index;

	if (ioctl(fd, VIDIOC_G_TUNER, &t) != 0)
		return false;

	dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG,
			"QRY: Tuner: %s - low: %u - high: %u - unit: %s\n",
			t.name, t.rangelow, t.rangehigh,
			t.capability & V4L2_TUNER_CAP_LOW ? "kHz": "MHz");

	XMALLOC(vi->tuner, struct tuner_info *, sizeof(struct tuner_info));
	strncpy(vi->tuner->name, (char *) t.name, NAME_FIELD_LENGTH);
	vi->tuner->index = (int) index;
	vi->tuner->unit = t.capability & VIDEO_TUNER_LOW ? KHZ_UNIT : MHZ_UNIT;
	vi->tuner->rssi = t.signal;
	vi->tuner->type =  t.type == V4L2_TUNER_RADIO ? RADIO_TYPE : TV_TYPE;
	vi->tuner->rangehigh = t.rangehigh;
	vi->tuner->rangelow = t.rangelow;

	dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG,
				"QRY: Tuner: %s - low: %lu - high: %lu - unit: %d\n",
				vi->tuner->name, vi->tuner->rangelow, vi->tuner->rangehigh,
				vi->tuner->unit);

	return true;
}

static inline void free_tuner(struct tuner_info *t) {
	if (t)
		XFREE(t);
}

static void free_video_inputs(struct video_input_info *vi, unsigned int nb) {
	for(unsigned int i = 0; i < nb; i++) {
		free_tuner(vi[i].tuner);
		if (vi[i].nb_stds)
			XFREE(vi[i].supported_stds);
	}
	XFREE(vi);
}

static void add_supported_std(struct video_input_info *vi, int std) {
	dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG, "QRY: Adding standard %d\n", std);
	vi->nb_stds++;
	XREALLOC(vi->supported_stds, int *, vi->nb_stds * sizeof(int));
	vi->supported_stds[(vi->nb_stds - 1)] = std;
}

static inline int check_inputs_v4l2(struct video_device *vdev) {
	struct v4l2_input vi;
	struct device_info *di = vdev->info;
	CLEAR(vi);
	di->inputs = NULL;

	dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG, "QRY: querying inputs\n");

	//Check how many inputs there are
	while (ioctl(vdev->fd, VIDIOC_ENUMINPUT, &vi) != -1)
		vi.index++;

	di->nb_inputs = vi.index;

	dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG, "QRY: found %d inputs\n", di->nb_inputs);

	XMALLOC(di->inputs, struct video_input_info *, di->nb_inputs * sizeof(struct video_input_info));

	for (unsigned i = 0; i < di->nb_inputs; i++) {
		CLEAR(vi);
		CLEAR(di->inputs[i]);
		vi.index = i;
		if (ioctl(vdev->fd, VIDIOC_ENUMINPUT, &vi) == -1) {
			info("Failed to get details of input %d on device %s\n", i, vdev->file);
			free_video_inputs(di->inputs, i);
			return LIBVIDEO_ERR_IOCTL;
		}

		dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG, "QRY: input #%d - '%s' - (%s) - tuner: %d\n",
				i, vi.name, (vi.type == V4L2_INPUT_TYPE_TUNER) ? "Tuner" : "Camera", vi.tuner);

		strncpy(di->inputs[i].name, (char *) vi.name, NAME_FIELD_LENGTH);
		di->inputs[i].index = (signed) i;
		di->inputs[i].type = (vi.type == V4L2_INPUT_TYPE_TUNER) ? INPUT_TYPE_TUNER:INPUT_TYPE_CAMERA;

		if (vi.type & V4L2_INPUT_TYPE_TUNER) {
			dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG, "QRY: Querying tuner\n");
			if (!query_tuner(&di->inputs[i], vdev->fd, vi.tuner)) {
				info("Failed to get details of tuner on input %d of device %s\n", i, vdev->file);
				free_video_inputs(di->inputs,i);
				return LIBVIDEO_ERR_IOCTL;
			}
		} else {
			dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG, "QRY: No tuner\n");
			di->inputs[i].tuner = NULL;
		}

		if(vi.std & V4L2_STD_PAL)
			add_supported_std(&di->inputs[i], PAL);
		if(vi.std & V4L2_STD_NTSC)
			add_supported_std(&di->inputs[i], NTSC);
		if(vi.std & V4L2_STD_SECAM)
			add_supported_std(&di->inputs[i], SECAM);
		if(vi.std == V4L2_STD_UNKNOWN)
			add_supported_std(&di->inputs[i], WEBCAM);

	}
	
	return LIBVIDEO_ERR_SUCCESS;
}

//this function enumerates the frame intervals for a given libvideo video format
//, width and height and modifies the pointer at p to point to either
//NULL, a struct frame_intv_discrete, struct frame_intv_continuous. It returns
//FRAME_INTV_UNSUPPORTED (p points  to NULL),
//FRAME_INTV_DISCRETE (p points to a an array of struct frame_intv_discrete, the
//last struct has its members set to 0) or
//FRAME_INTV_CONTINUOUS (p points to a struct frame_intv_continuous)
//The returned pointer must be freed by the caller (using free()).
static int list_frame_intv(struct device_info *dinfo, unsigned int fmt, unsigned int width, unsigned int height, void **p) {
	return lookup_frame_intv(dinfo->convert, libvideo_palettes[fmt].v4l2_palette, width, height, p);
}

int query_device_v4l2(struct video_device *vdev) {
	dprint(LIBVIDEO_SOURCE_QRY, LIBVIDEO_LOG_DEBUG, "QRY: Querying V4L2 device.\n");
	
	struct v4l2_capability caps;
	if (!check_v4l2(vdev->fd, &caps)) {
		info("Error checking capabilities of V4L2 video device %s\n", vdev->file);
		return LIBVIDEO_ERR_NOCAPS;
	}
	
	//fill name field
	strncpy(vdev->info->name, (char *) caps.card, NAME_FIELD_LENGTH);

	//fill input field
	if(check_inputs_v4l2(vdev) != LIBVIDEO_ERR_SUCCESS) {
		info("Error checking available inputs on V4L2 video device %s\n", vdev->file);
		return LIBVIDEO_ERR_NOCAPS;
	}

	//fill palettes field
	if(check_palettes_v4l2(vdev) == LIBVIDEO_ERR_IOCTL) {
		free_video_inputs(vdev->info->inputs, vdev->info->nb_inputs);
		info("Error checking supported palettes on V4L2 video device %s\n", vdev->file);
		return LIBVIDEO_ERR_NOCAPS;
	}

	vdev->info->list_frame_intv = list_frame_intv;
	
	return 0;
}

void free_video_device_v4l2(struct video_device *vd) {
	//for each palette
	for(unsigned int i = 0; i < vd->info->nb_palettes; i++){
		//check & free the frame size member
		struct palette_info* palette = &(vd->info->palettes[i]);
		if(palette->size_type == FRAME_SIZE_CONTINUOUS){
			
			//check & free the frame interval member
			//min res
			if(palette->continuous->interval_type_min_res == FRAME_INTV_DISCRETE)
				XFREE(palette->continuous->intv_min_res.discrete);
			else if(palette->continuous->interval_type_min_res == FRAME_INTV_CONTINUOUS)
				XFREE(palette->continuous->intv_min_res.continuous);

			//max res
			if(palette->continuous->interval_type_max_res == FRAME_INTV_DISCRETE)
				XFREE(palette->continuous->intv_max_res.discrete);
			else if(palette->continuous->interval_type_max_res == FRAME_INTV_CONTINUOUS)
				XFREE(palette->continuous->intv_max_res.continuous);

			//free image size
			XFREE(palette->continuous);

		} else if(palette->size_type==FRAME_SIZE_DISCRETE) {
			//loop over all frame sizes
			for (unsigned int j = 0; palette->discrete[j].height != 0; j++) {
				//free frame interval
				if(palette->discrete[j].interval_type == FRAME_INTV_DISCRETE)
					XFREE(palette->discrete[j].intv.discrete);
				else if(palette->discrete[j].interval_type == FRAME_INTV_CONTINUOUS)
					XFREE(palette->discrete[j].intv.continuous);
			}

			//free image size
			XFREE(palette->discrete);
		}

		//free the raw palettes array
		if(palette->raw_palettes)
			XFREE(palette->raw_palettes);
	}

	//free the palettes
	XFREE(vd->info->palettes);

	free_video_inputs(vd->info->inputs, vd->info->nb_inputs);

	v4lconvert_destroy(vd->info->convert);
}
