
#include <stdlib.h>
#include "libv4lconvert-flat.h"
#include "jpeg_memsrcdest.h"
#include "../types.h"
#include "libvideo.h"
#include "../log.h"

#ifndef __LIBVIDEO_LIBV4LCONVERT_LIBV4LCONVERT_FLAT_CPP
#define __LIBVIDEO_LIBV4LCONVERT_LIBV4LCONVERT_FLAT_CPP

#ifdef __cplusplus
extern "C" {
#endif

#ifndef N_A
#define N_A 0
#endif

#ifndef UNUSED
//Macro to hide unused parameter warnings
#define UNUSED(x) (void)(x)
#endif

#define GENERATE_CONVERTER_SDWH_0F(id, fn, src_fmt, dst_fmt) {(id), v4lconvert_conversion_signature_sdwh_0f, {.cvt_sdwh_0f = (fn)}, (src_fmt), (dst_fmt), N_A, N_A}

#define GENERATE_CONVERTER_SDWH_1F(id, fn, src_fmt, dst_fmt, flag1) {(id), v4lconvert_conversion_signature_sdwh_1f, {.cvt_sdwh_1f = (fn)}, (src_fmt), (dst_fmt), (flag1), N_A}

#define GENERATE_CONVERTER_SDWH_2F(id, fn, src_fmt, dst_fmt, flag1, flag2) {(id), v4lconvert_conversion_signature_sdwh_1f, {.cvt_sdwh_1f = (fn)}, (src_fmt), (dst_fmt), (flag1), (flag2)}

#define GENERATE_CONVERTER_SD_SF_0F(id, fn, src_fmt, dst_fmt) {(id), v4lconvert_conversion_signature_sd_sf_0f, {.cvt_sd_sf_0f = (fn)}, (src_fmt), (dst_fmt), N_A, N_A}

#define GENERATE_CONVERTER_SD_SF_1F(id, fn, src_fmt, dst_fmt, flag1) {(id), v4lconvert_conversion_signature_sd_sf_1f, {.cvt_sd_sf_1f = (fn)}, (src_fmt), (dst_fmt), (flag1), N_A}

#define GENERATE_CONVERTER_SD_SF_2F(id, fn, src_fmt, dst_fmt, flag1, flag2) {(id), v4lconvert_conversion_signature_sd_sf_2f, {.cvt_sd_sf_2f = (fn)}, (src_fmt), (dst_fmt), (flag1), (flag2)}

#define GENERATE_CONVERTER_SPECIAL(id, src_fmt, dst_fmt, flag1, flag2) {(id), v4lconvert_conversion_signature_special, {.user_defined = NULL}, (src_fmt), (dst_fmt), (flag1), (flag2)}

#define GENERATE_CONVERTER_SDWH_1F_x2(id, fn, src_fmt_0, src_fmt_1, dst_fmt_0, dst_fmt_1) \
	GENERATE_CONVERTER_SDWH_1F(id    , (fn), (src_fmt_0), (dst_fmt_0), 0),\
	GENERATE_CONVERTER_SDWH_1F(id + 1, (fn), (src_fmt_1), (dst_fmt_1), 1)

#define GENERATE_CONVERTER_SD_SF_1F_x2(id, fn, src_fmt_0, src_fmt_1, dst_fmt_0, dst_fmt_1) \
	GENERATE_CONVERTER_SD_SF_1F(id    , (fn), (src_fmt_0), (dst_fmt_0), 0),\
	GENERATE_CONVERTER_SD_SF_1F(id + 1, (fn), (src_fmt_1), (dst_fmt_1), 1)

#define GENERATE_CONVERTER_SD_SF_2F_x4(id, fn, src_fmt_0, src_fmt_1, dst_fmt_0, dst_fmt_1) \
	GENERATE_CONVERTER_SD_SF_2F(id    , (fn), (src_fmt_0), (dst_fmt_0), 0, 0),\
	GENERATE_CONVERTER_SD_SF_2F(id + 1, (fn), (src_fmt_1), (dst_fmt_0), 1, 0),\
	GENERATE_CONVERTER_SD_SF_2F(id + 2, (fn), (src_fmt_0), (dst_fmt_1), 0, 1),\
	GENERATE_CONVERTER_SD_SF_2F(id + 3, (fn), (src_fmt_1), (dst_fmt_1), 1, 1)

//Number of converters in array
#define NUM_V4L_CONVERTERS 51
//Very const
static v4lconvert_converter_t const v4lconvert_converters[NUM_V4L_CONVERTERS] = {
	GENERATE_CONVERTER_SD_SF_2F_x4(0, v4lconvert_rgb24_to_yuv420, RGB32, BGR32, YUV420, YVU420),
	GENERATE_CONVERTER_SDWH_1F_x2(4, v4lconvert_yuv420_to_rgb24, YUV420, YVU420, RGB24, RGB24),
	GENERATE_CONVERTER_SDWH_1F_x2(6, v4lconvert_yuv420_to_bgr24, YUV420, YVU420, BGR24, BGR24),
	GENERATE_CONVERTER_SDWH_0F(8, v4lconvert_yuyv_to_rgb24, YUYV, RGB24),
	GENERATE_CONVERTER_SDWH_0F(9, v4lconvert_yuyv_to_bgr24, YUYV, BGR24),
	GENERATE_CONVERTER_SDWH_1F_x2(10, v4lconvert_yuyv_to_yuv420, YUYV, YUYV, YUV420, YVU420),
	GENERATE_CONVERTER_SDWH_0F(12, v4lconvert_yvyu_to_rgb24, YUYV, RGB24),
	GENERATE_CONVERTER_SDWH_0F(13, v4lconvert_yvyu_to_bgr24, YUYV, BGR24),
	GENERATE_CONVERTER_SDWH_0F(14, v4lconvert_uyvy_to_rgb24, UYVY, RGB24),
	GENERATE_CONVERTER_SDWH_0F(15, v4lconvert_uyvy_to_bgr24, UYVY, BGR24),
	GENERATE_CONVERTER_SDWH_1F_x2(16, v4lconvert_uyvy_to_yuv420, UYVY, UYVY, YUV420, YVU420),
	GENERATE_CONVERTER_SDWH_0F(18, v4lconvert_swap_rgb, RGB24, BGR24),
	GENERATE_CONVERTER_SDWH_0F(19, v4lconvert_swap_rgb, BGR24, RGB24),
	GENERATE_CONVERTER_SD_SF_0F(20, v4lconvert_swap_uv, YUV420, YVU420),//TODO figure out if this is right...
	GENERATE_CONVERTER_SD_SF_0F(21, v4lconvert_swap_uv, YVU420, YUV420),
	GENERATE_CONVERTER_SDWH_0F(22, v4lconvert_grey_to_rgb24, GREY, RGB24),
	GENERATE_CONVERTER_SD_SF_0F(23, v4lconvert_grey_to_yuv420, GREY, YUV420),
	//v4lconvert_y10b_to_rgb24(struct v4lconvert_data *data, const u8 *src, u8 *dest, u32 width, u32 height);
	//v4lconvert_y10b_to_yuv420(struct v4lconvert_data *data, const u8 *src, u8 *dest, u32 width, u32 height);
	GENERATE_CONVERTER_SDWH_0F(24, v4lconvert_rgb565_to_rgb24, RGB565, RGB24),
	GENERATE_CONVERTER_SDWH_0F(25, v4lconvert_rgb565_to_bgr24, RGB565, BGR24),
	GENERATE_CONVERTER_SD_SF_1F_x2(26, v4lconvert_rgb565_to_yuv420, RGB565, RGB565, YUV420, YVU420),
	GENERATE_CONVERTER_SDWH_1F_x2(27, v4lconvert_spca501_to_yuv420, SPCA501, SPCA501, YUV420, YVU420),
	GENERATE_CONVERTER_SDWH_1F_x2(28, v4lconvert_spca505_to_yuv420, SPCA505, SPCA505, YUV420, YVU420),
	GENERATE_CONVERTER_SDWH_1F_x2(32, v4lconvert_spca508_to_yuv420, SPCA508, SPCA508, YUV420, YVU420),
	GENERATE_CONVERTER_SDWH_1F_x2(34, v4lconvert_cit_yyvyuy_to_yuv420, CIT_YYVYUY, CIT_YYVYUY, YUV420, YVU420),
	GENERATE_CONVERTER_SDWH_1F_x2(36, v4lconvert_konica_yuv420_to_yuv420, KONICA420, KONICA420, YUV420, YVU420),
	//GENERATE_CONVERTER_SDWH_1F_x2(36, v4lconvert_m420_to_yuv420, M420, M420, YUV420, YVU420),
	//v4lconvert_m420_to_yuv420(const u8 *src, u8 *ydest, u32 width, u32 height, int yvu);
	//int v4lconvert_cpia1_to_yuv420(struct v4lconvert_data *data, const u8 *src, int src_size, u8 *dst, u32 width, u32 height, int yvu);
	GENERATE_CONVERTER_SDWH_1F_x2(38, v4lconvert_sn9c20x_to_yuv420, SN9C20X_I420, SN9C20X_I420, YUV420, YVU420),
	//int v4lconvert_se401_to_rgb24(struct v4lconvert_data *data, const u8 *src, int src_size, u8 *dest, u32 width, u32 height);

	//int v4lconvert_decode_jpeg_tinyjpeg(struct v4lconvert_data *data, u8 *src, int src_size, u8 *dest, struct v4l2_format *fmt, unsigned int dest_pix_fmt, int flags);

	//int v4lconvert_decode_jpeg_libjpeg(struct v4lconvert_data *data, u8 *src, int src_size, u8 *dest, struct v4l2_format *fmt, unsigned int dest_pix_fmt);

	//int v4lconvert_decode_jpgl(const u8 *src, u32 src_size, unsigned int dest_pix_fmt, u8 *dest, u32 width, u32 height);

	//v4lconvert_decode_spca561(const u8 *src, u8 *dst, u32 width, u32 height);

	GENERATE_CONVERTER_SDWH_0F(40, v4lconvert_decode_sn9c10x, SN9C10X, SBGGR8),

	//int v4lconvert_decode_pac207(struct v4lconvert_data *data, const u8 *inp, int src_size, u8 *outp, u32 width, u32 height);

	//int v4lconvert_decode_mr97310a(struct v4lconvert_data *data, const u8 *src, int src_size, u8 *dst, u32 width, u32 height);

	//int v4lconvert_decode_jl2005bcd(struct v4lconvert_data *data, const u8 *src, int src_size, u8 *dest, u32 width, u32 height);

	GENERATE_CONVERTER_SDWH_0F(41, v4lconvert_decode_sn9c2028, SN9C2028, SBGGR8),
	GENERATE_CONVERTER_SDWH_0F(42, v4lconvert_decode_sq905c, SQ905C, SRGGB8),
	GENERATE_CONVERTER_SDWH_0F(43, v4lconvert_decode_stv0680, STV0680, SRGGB8),
	
	//v4lconvert_bayer_to_rgb24(const u8 *bayer, u8 *rgb, u32 width, u32 height, unsigned int pixfmt);

	//v4lconvert_bayer_to_bgr24(const u8 *bayer, u8 *rgb, u32 width, u32 height, unsigned int pixfmt);

	//v4lconvert_bayer_to_yuv420(const u8 *bayer, u8 *yuv, u32 width, u32 height, unsigned int src_pixfmt, int yvu);

	GENERATE_CONVERTER_SDWH_0F(44, v4lconvert_grey_to_rgb24, HM12, RGB24),
	GENERATE_CONVERTER_SDWH_0F(45, v4lconvert_hm12_to_bgr24, HM12, BGR24),
	GENERATE_CONVERTER_SDWH_1F_x2(46, v4lconvert_hm12_to_yuv420, HM12, HM12, YUV420, YVU420),
	//Virtual JPEG converters. Can't be used, but are placeholders because encoders can support them.
	GENERATE_CONVERTER_SPECIAL(48, GREY, JPEG, 0, 0),
	GENERATE_CONVERTER_SPECIAL(49, RGB24, JPEG, 0, 0),
	GENERATE_CONVERTER_SPECIAL(50, YUV420, JPEG, 0, 0)
	//TODO add other converters
};

static u32 v4lconvert_encoder_applyIMF_sdwh(struct v4lconvert_encoder* self, const u8* src, u8* dst, u32 src_len);
static u32 v4lconvert_encoder_applyIMF_sd_sf(struct v4lconvert_encoder* self, const u8* src, u8* dst, u32 src_len);
static u32 v4lconvert_encoder_encodePixelJPEG(struct v4lconvert_encoder* self, const u8* src, u8* dst, u32 src_len);
static u32 v4lconvert_encoder_encodePlanarJPEG(struct v4lconvert_encoder* self, const u8* src, u8* dst, u32 src_len);

static int v4lconvert_encoder_releaseIMF(struct v4lconvert_encoder* self);
static int v4lconvert_encoder_releaseJPEG(struct v4lconvert_encoder* self);
static u32 v4lconvert_encoder_series_doConvert(struct v4lconvert_encoder_series* self, struct v4lconvert_buffer* buffer);
static inline int computeEncoderPath(unsigned int* map, unsigned int* distances, u32 from, u32 to, unsigned int maxIterations);


static u32 v4lconvert_encoder_applyIMF_sdwh(struct v4lconvert_encoder* self, const u8* src, u8* dst, u32 src_len) {
	UNUSED(src_len);
	v4lconvert_converter_t* converter = self->converter;
	const u32 width = self->src_width;
	const u32 height = self->src_height;
	switch (converter->signature) {
		case v4lconvert_conversion_signature_sdwh_0f:
			(*converter->target.cvt_sdwh_0f)(src, dst, width, height);
			break;
		case v4lconvert_conversion_signature_sdwh_1f:
			(*converter->target.cvt_sdwh_1f)(src, dst, width, height, converter->flag1);
			break;
		case v4lconvert_conversion_signature_sdwh_2f:
			(*converter->target.cvt_sdwh_2f)(src, dst, width, height, converter->flag1, converter->flag2);
			break;
		default:
			return 0;
	}
	return self->dst_len;
}

static u32 v4lconvert_encoder_applyIMF_sd_sf(struct v4lconvert_encoder* self, const u8* src, u8* dst, u32 src_len) {
	UNUSED(src_len);
	v4lconvert_converter_t* converter = self->converter;
	struct v4l2_format* src_fmt = self->imf_v4l2_src_fmt;
	switch (converter->signature) {
		case v4lconvert_conversion_signature_sd_sf_0f:
			converter->target.cvt_sd_sf_0f(src, dst, src_fmt);
			break;
		case v4lconvert_conversion_signature_sd_sf_1f:
			converter->target.cvt_sd_sf_1f (src, dst, src_fmt, converter->flag1);
			break;
		case v4lconvert_conversion_signature_sd_sf_2f:
			converter->target.cvt_sd_sf_2f (src, dst, src_fmt, converter->flag1, converter->flag2);
			break;
		default:
			return 0;
	}
	return self->dst_len;
}

/**
 * v4lconvert_encoder::apply method for encoding pixel formats to JPEG.
 * e.g, RGB or GREY
 */
static u32 v4lconvert_encoder_encodePixelJPEG(struct v4lconvert_encoder* self, const u8* src, u8* dst, u32 src_len) {
	struct jpeg_compress_struct* cinfo = self->jpeg_encode_params.cinfo;
	if (!cinfo)
		return 0;
	jpeg_set_quality(cinfo, self->jpeg_encode_params.quality, TRUE);
	
	// Configure the output to write to the destination buffer
	unsigned long dst_lencpy = (unsigned long) self->dst_len;
	jpeg_mem_dest(cinfo, &dst, &dst_lencpy);
	
	jpeg_start_compress(cinfo, TRUE);
	
	const JSAMPLE* row_ptr;
	const u32 row_stride  = self->jpeg_encode_params.row_stride;
	const u32 height = self->src_height;
	#ifdef SANITY_CHECK
		u32 max_scanline = src_len / row_stride;
	#else
		UNUSED(src_len);
	#endif
	while(cinfo->next_scanline < height) {
		#ifdef SANITY_CHECK
			if (cinfo->next_scanline > max_scanline) {
				jpeg_abort_compress(cinfo);
				return 0;
			}
		#endif
		row_ptr = src + cinfo->next_scanline * row_stride;
		jpeg_write_scanlines(cinfo, (JSAMPARRAY) &row_ptr, 1);
	}
	
	//Finish compressing the JPEG
	jpeg_finish_compress(cinfo);
	
	// Calculate the length of the resulting JPEG
	return self->dst_len - cinfo->dest->free_in_buffer;
}

static u32 v4lconvert_encoder_encodePlanarJPEG(struct v4lconvert_encoder* self, const u8* src, u8* dst, u32 src_len) {
	struct jpeg_compress_struct* cinfo = self->jpeg_encode_params.cinfo;
	if (!cinfo)
		return 0;
	
	jpeg_set_quality(cinfo, self->jpeg_encode_params.quality, TRUE);
	
	// Configure the output to write to the destination buffer
	unsigned long dst_lencpy = (unsigned long) self->dst_len;
	jpeg_mem_dest(cinfo, &dst, &dst_lencpy);
	
	jpeg_start_compress(cinfo, TRUE);
	
	//TODO fix
	
	jpeg_finish_compress(cinfo);
	return 0;
}

static int v4lconvert_encoder_releaseIMF(struct v4lconvert_encoder* self) {
	switch (self->converter->signature) {
		case v4lconvert_conversion_signature_sdwh_0f:
		case v4lconvert_conversion_signature_sdwh_1f:
		case v4lconvert_conversion_signature_sdwh_2f:
			//Nothing to release
			return EXIT_SUCCESS;
		case v4lconvert_conversion_signature_sd_sf_0f:
		case v4lconvert_conversion_signature_sd_sf_1f:
		case v4lconvert_conversion_signature_sd_sf_2f:
			if (self->imf_v4l2_src_fmt) {
				free(self->imf_v4l2_src_fmt);
				self->imf_v4l2_src_fmt = NULL;
			}
			return EXIT_SUCCESS;
		default:
			return EXIT_FAILURE;
	}
}

static int v4lconvert_encoder_releaseJPEG(struct v4lconvert_encoder* self) {
	if (self->jpeg_encode_params.cinfo) {
		jpeg_destroy_compress(self->jpeg_encode_params.cinfo);
		free(self->jpeg_encode_params.cinfo);
		self->jpeg_encode_params.cinfo = NULL;
	}
	if (self->jpeg_encode_params.cerr) {
		free(self->jpeg_encode_params.cerr);
		self->jpeg_encode_params.cerr = NULL;
	}
	return EXIT_SUCCESS;
}

u32 v4lconvert_estimateBufferSize(u32 fmt, u32 width, u32 height) {
	u32 pixels = width * height;
	switch (fmt) {
		case RGB32:
		case BGR32:
			return pixels * 4;
		case RGB24:
		case BGR24:
		case YUV444: //12 bytes per 4 pixels
			return pixels * 3;
		case YUV422:
		case YUV422P:
		case YUYV:
		case YYUV:
		case YVYU:
		case UYVY:
		case VYUY:
		case NV16:
		case NV61:
		case RGB555:
		case RGB555X:
		case RGB565:
		case RGB565X:
		case SBGGR10:
		case SGBRG10:
		case SGRBG10:
		case SRGGB10:
		case SBGGR16:
		case Y16:
			//8 bytes per 4 pixels
			return pixels * 2;
		case NV12:
		case NV21:
		case RGB444:
		case YUV411:
		case YUV420:
		case YVU420:
			//6 bytes per 4 pixels
			return pixels * 3 / 2;
		case Y10:
			return pixels * 5 / 4;
		case PAL8:
			return 256 + pixels * 1;
		case GREY:
		case RGB332:
		case SBGGR8:
		case SGBRG8:
		case SGRBG8:
		case SRGGB8:
		case SGRBG10DPCM8:
			return pixels * 1;
		case Y4:
			return (pixels + 1) / 2;
		default:
			//Estimate big. Really big.
			return pixels * 4;
	}
}

int v4lconvert_encoder_initWithConverter(struct v4lconvert_encoder* encoder, v4lconvert_converter_t* converter, u32 width, u32 height) {
	if (!encoder || !converter)
		return EXIT_FAILURE;
	encoder->converter = converter;
	encoder->src_fmt = converter->src_fmt;
	encoder->dst_fmt = converter->dst_fmt;
	encoder->src_width = width;
	encoder->src_height = height;
	//Not necessarily true, but the default
	encoder->dst_width = width;
	encoder->dst_height = height;
	
	switch (converter->signature) {
		case v4lconvert_conversion_signature_unset:
			break;
		case v4lconvert_conversion_signature_sdwh_0f:
			encoder->apply = v4lconvert_encoder_applyIMF_sdwh;
			encoder->release = v4lconvert_encoder_releaseIMF;
			break;
		case v4lconvert_conversion_signature_sd_sf_0f:
		case v4lconvert_conversion_signature_sd_sf_1f:
		case v4lconvert_conversion_signature_sd_sf_2f:
			encoder->apply = v4lconvert_encoder_applyIMF_sd_sf;
			encoder->release = v4lconvert_encoder_releaseIMF;
			if (!(encoder->imf_v4l2_src_fmt = malloc(sizeof(struct v4l2_format))))
				return EXIT_FAILURE;
			//TODO finish
			switch (encoder->imf_v4l2_src_fmt->type) {
				case V4L2_BUF_TYPE_VIDEO_CAPTURE:
					encoder->imf_v4l2_src_fmt->fmt.pix.width = width;
					encoder->imf_v4l2_src_fmt->fmt.pix.height = height;
					break;
				case V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE:
					encoder->imf_v4l2_src_fmt->fmt.pix_mp.width = width;
					encoder->imf_v4l2_src_fmt->fmt.pix_mp.height = height;
					break;
				case V4L2_BUF_TYPE_VIDEO_OVERLAY:
				case V4L2_BUF_TYPE_VBI_CAPTURE:
				case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE:
					//Not sure how to handle these
				default:
					return EXIT_FAILURE;
			}
			break;
		case v4lconvert_conversion_signature_special:
			//This gets tricky
			if (converter->dst_fmt == JPEG) {
				//JPEG encoder
				encoder->apply = v4lconvert_encoder_encodePixelJPEG;
				encoder->release = v4lconvert_encoder_releaseJPEG;
				
				//Default to quality of 100%
				encoder->jpeg_encode_params.quality = 100;
				
				//Create jpeg encoder
				if (!(encoder->jpeg_encode_params.cinfo = malloc(sizeof(struct jpeg_compress_struct))))
					return EXIT_FAILURE;
				
				if (!(encoder->jpeg_encode_params.cerr = malloc(sizeof(struct jpeg_error_mgr)))) {
					free(encoder->jpeg_encode_params.cinfo);
					return EXIT_FAILURE;
				}
				
				struct jpeg_compress_struct* cinfo = encoder->jpeg_encode_params.cinfo;
				//Initialize the error buffer
				cinfo->err = jpeg_std_error(encoder->jpeg_encode_params.cerr);
				//Initialize the compressor
				jpeg_create_compress(cinfo);
				//Set the image dimensions
				cinfo->image_width = width;
				cinfo->image_height = height;
				switch (converter->src_fmt) {
					case GREY:
						cinfo->input_components = 1;
						encoder->jpeg_encode_params.row_stride = width;
						cinfo->in_color_space = JCS_GRAYSCALE;
						jpeg_set_defaults(cinfo);
						break;
					case RGB24:
						cinfo->input_components = 3;
						encoder->jpeg_encode_params.row_stride = width * 3;
						cinfo->in_color_space = JCS_RGB;
						jpeg_set_defaults(cinfo);
						break;
					case YUV420:
					case YUYV:
					case YVYU:
					case UYVY:
					case VYUY:
						//Different apply method
						encoder->apply = v4lconvert_encoder_encodePlanarJPEG;
						cinfo->input_components = 3;
						encoder->jpeg_encode_params.row_stride = width * 3;
						jpeg_set_defaults(cinfo);
						jpeg_set_colorspace(cinfo, JCS_YCbCr);
						cinfo->raw_data_in = TRUE; // Supply downsampled data
						cinfo->comp_info[0].v_samp_factor = converter->src_fmt == YUV420 ? 2 : 1;
						cinfo->comp_info[0].h_samp_factor = 2;
						// cinfo->comp_info[0].v_samp_factor set below depending on source format
						cinfo->comp_info[1].h_samp_factor = 1;
						cinfo->comp_info[1].v_samp_factor = 1;
						cinfo->comp_info[2].h_samp_factor = 1;
						cinfo->comp_info[2].v_samp_factor = 1;
						//TODO finish
						return EXIT_FAILURE;
						break;
					default:
						//TODO support more formats (esp. YUV420)
						return EXIT_FAILURE;
				}
			} else {
				return EXIT_FAILURE;
			}
			break;
		default:
			return EXIT_FAILURE;
	}
	encoder->src_len = v4lconvert_estimateBufferSize(converter->src_fmt, width, height);
	encoder->dst_len = v4lconvert_estimateBufferSize(converter->dst_fmt, width, height);
	return EXIT_SUCCESS;
}

int v4lconvert_encoder_initForIMF(struct v4lconvert_encoder* encoder, u32 src_fmt, u32 dst_fmt, u32 width, u32 height) {
	v4lconvert_converter_t* converter = v4lconvert_converter_getConverterByConversion(src_fmt, dst_fmt);
	if (!converter)
		return EXIT_FAILURE;
	return v4lconvert_encoder_initWithConverter(encoder, converter, width, height);
}

int v4lconvert_encoder_series_init(struct v4lconvert_encoder_series* self, u32 width, u32 height, u32 numConverters, u32* converterIds) {
	if (!self || (numConverters > 0 && !converterIds))
		return EXIT_FAILURE;
	self->convert = &v4lconvert_encoder_series_doConvert;
	self->num_encoders = numConverters;
	
	if (!self->encoders)
		self->encoders = calloc(numConverters, sizeof(struct v4lconvert_encoder*));
	
	for (unsigned int i = 0; i < numConverters; i++) {
		struct v4lconvert_encoder* encoder = self->encoders[i] = malloc(sizeof(struct v4lconvert_encoder));
		v4lconvert_converter_t* converter = v4lconvert_converter_getConverterById(converterIds[i]);
		if (!encoder || !converter || !v4lconvert_encoder_initWithConverter(encoder, converter, width, height)) {
			//Initialization failure
			v4lconvert_encoder_series_doRelease(self);
			return EXIT_FAILURE;
		}
	}
	
	self->src_fmt = self->encoders[0]->src_fmt;
	self->dst_fmt = self->encoders[numConverters - 1]->dst_fmt;
	return EXIT_SUCCESS;
}

int v4lconvert_encoder_series_doRelease(struct v4lconvert_encoder_series* self) {
	if (self != NULL) {
		if (self->encoders) {
			for (unsigned i = 0; i < self->num_encoders; i++)
				free(self->encoders[i]);
			free(self->encoders);
			self->encoders = NULL;
		}
	}
	return EXIT_SUCCESS;
}

static u32 v4lconvert_encoder_series_doConvert(struct v4lconvert_encoder_series* self, struct v4lconvert_buffer* buffer) {
	struct v4lconvert_encoder* encoder = self->encoders[0];
	u8* bufA;
	u8* bufB;
	if (self->num_encoders & 1) {
		bufA = buffer->buf1;
		bufB = buffer->buf2;
	} else {
		bufA = buffer->buf2;
		bufB = buffer->buf1;
	}
	u32 src_len = buffer->buf0_len;
	if ((src_len = encoder->apply(encoder, buffer->buf0, buffer->buf0, src_len)) == 0)
		return 0;
	for (unsigned int i = 1; i < (self->num_encoders / 2) * 2; i++) {
		encoder = self->encoders[i];
		if ((src_len = encoder->apply(encoder, bufA, bufB, src_len)) == 0)
			return 0;
		encoder = self->encoders[++i];
		if ((src_len = encoder->apply(encoder, bufB, bufA, src_len)) == 0)
			return 0;
	}
	if (self->num_encoders & 1)
		return src_len;
	encoder = self->encoders[self->num_encoders - 1];
	return buffer->buf1_len = encoder->apply(encoder, bufA, buffer->buf1, src_len);
}

int v4lconvert_encoder_series_createBuffers(struct v4lconvert_encoder_series* series, u32 num_buffers, struct v4lconvert_buffer** buffers, int allocate) {
	u32 bufA_len = 0;
	u32 bufB_len = 0;
	for (unsigned int i = 0; i < (series->num_encoders / 2) * 2; i += 2) {
		u32 size = series->encoders[i]->dst_len;
		if (size > bufB_len)
			bufB_len = size;
		size = series->encoders[i + 1]->dst_len;
		if (size > bufA_len)
			bufA_len = size;
	}
	
	if (series->num_encoders & 1) {
		u32 tmp = bufA_len;
		u32 endsize = series->encoders[series->num_encoders - 1]->dst_len;
		bufA_len = bufB_len > endsize ? bufB_len : endsize;
		bufB_len = tmp;
	}
	
	for (unsigned i = 0; i < num_buffers; i++) {
		struct v4lconvert_buffer* buffer = buffers[i];
		if (!buffer) {
			if (allocate)
				buffers[i] = buffer = malloc(sizeof(struct v4lconvert_buffer));
			else
				return EXIT_FAILURE;
		}
		
		buffer->buf0_len = 0;
		buffer->buf1_len = 0;
		buffer->buf0_cap = series->encoders[0]->src_len;
		buffer->buf1_cap = buffer->buf1_len = bufA_len;
		buffer->buf2_cap = bufB_len;
		if (allocate) {
			//Don't allocate buf0, because it might be used by the 
			buffer->buf1 = calloc(buffer->buf1_cap, sizeof(u8*));
			buffer->buf2 = calloc(buffer->buf2_cap, sizeof(u8*));
			if (!buffer->buf1 || !buffer->buf2) {
				free(buffer->buf1);
				free(buffer->buf2);
				buffer->buf1 = NULL;
				buffer->buf2 = NULL;
				//Zero out the remaining buffers
				for (unsigned j = i + 1; j < num_buffers; j++) {
					buffer = buffers[j];
					if (!buffer)
						continue;
					buffer->buf0_len = 0;
					buffer->buf1_len = 0;
					buffer->buf0_cap = 0;
					buffer->buf1_cap = 0;
					buffer->buf2_cap = 0;
					buffer->buf0 = NULL;
					buffer->buf1 = NULL;
					buffer->buf2 = NULL;
				}
				return EXIT_FAILURE;
			}
		}
	}
	return EXIT_SUCCESS;
}

int v4lconvert_buffer_release(struct v4lconvert_buffer* buffer) {
	free(buffer->buf0);
	buffer->buf0 = NULL;
	buffer->buf0_len = 0;
	buffer->buf0_cap = 0;
	
	free(buffer->buf1);
	buffer->buf1 = NULL;
	buffer->buf1_len = 0;
	buffer->buf1_cap = 0;
	
	free(buffer->buf2);
	buffer->buf2 = NULL;
	buffer->buf2_cap = 0;
	
	return EXIT_SUCCESS;
}

static inline int computeEncoderPath(unsigned int* map, unsigned int* distances, u32 from, u32 to, unsigned int maxIterations) {
	distances[to] = 1;
	// Variation of Dijkstra's Algorithm, where each distance is 1
	// Works backwards from the 'to' format to the 'from' format
	for (unsigned int i = 0; i < maxIterations; i++) {
		int progress = 0;
		for (unsigned int j = 0; j < NUM_V4L_CONVERTERS; j++) {
			v4lconvert_converter_t* converter = &(v4lconvert_converters[j]);
			unsigned int distanceTo = distances[converter->dst_fmt];
			if (distanceTo < 1)
				continue;
			if (distances[converter->src_fmt] == 0) {
				// There was no path found to this node previously
				map[converter->src_fmt] = i;
				distances[converter->src_fmt] = distanceTo + 1;
				progress = 1;
				if (converter->src_fmt == from)
					return EXIT_SUCCESS;
			}
		}
		// If no progress has been made this iteration, we can conclude that no path exists
		if (!progress)
			return EXIT_FAILURE;
	}
	// We took up too many iterations (the shortest path was too long)
	return EXIT_FAILURE;
}

int v4lconvert_encoder_series_computeConversion(struct v4lconvert_encoder_series* self, u32 width, u32 height, u32 from, u32 to, unsigned int maxIterations) {
	if (from > NB_SUPPORTED_PALETTES || to > NB_SUPPORTED_PALETTES)
		return EXIT_FAILURE;
	unsigned int* map = calloc(NB_SUPPORTED_PALETTES, sizeof(unsigned int));
	unsigned int* distances = calloc(NB_SUPPORTED_PALETTES, sizeof(unsigned int));
	if (map == NULL || distances == NULL || !computeEncoderPath(map, distances, from, to, maxIterations)) {
		free(map);
		free(distances);
		return EXIT_FAILURE;
	}
	//Walk backwards along the map, building the a single path
	unsigned int distance = distances[from] - 1;
	unsigned int* route = calloc(distance, sizeof(unsigned int));
	for (unsigned int i = 0, fmt = from; i < distance; i++) {
		route[i] = map[fmt];
		fmt = v4lconvert_converters[fmt].dst_fmt;
	}
	free(distances);
	free(map);
	
	#if defined(DEBUG) || true
		dprint(LIBVIDEO_LOG_SOURCE, LIBVIDEO_LOG_DEBUG, "Computed conversion path from %d to %d (len %d): %d", from, to, distance, route[0]);
		for (unsigned i = 1; i < distance; i++)
			dprint(LIBVIDEO_LOG_SOURCE, LIBVIDEO_LOG_DEBUG, " => %d", route[i]);
		dprint(LIBVIDEO_LOG_SOURCE, LIBVIDEO_LOG_DEBUG, "\nFormats: %s", libvideo_palettes[route[0].src_fmt].name);
		for (unsigned i = 0; i < distance; i++)
			dprint(LIBVIDEO_LOG_SOURCE, LIBVIDEO_LOG_DEBUG, " => %s", libvideo_palettes[route[i].dst_fmt].name);
		dprint(LIBVIDEO_LOG_SOURCE, LIBVIDEO_LOG_DEBUG, "\n");
	#endif
	
	//Now initialize the encoder series with the route just computed
	int result = v4lconvert_encoder_series_init(self, width, height, distance, route);
	free(route);
	return result;
}

v4lconvert_converter_t* v4lconvert_converter_getConverterById(unsigned int converterId) {
	if (converterId > NUM_V4L_CONVERTERS)
		return NULL;
	return &(v4lconvert_converters[converterId]);
}

/**
 * 
 */
int v4lconvert_converter_lookupConverterByConversion(u32 from, u32 to) {
	v4lconvert_converter_t* converter;
	
	//TODO use better lookup algorithm than O(n)
	for (int i = 0; i < NUM_V4L_CONVERTERS; i++) {
		converter = &(v4lconvert_converters[i]);
		if ((converter->src_fmt == from) && (converter->dst_fmt == to))
			return i;
	}
	return -1;
}
/**
 * Find a converter that does the conversion that you want
 * @return the converter requested, or NULL
 */
v4lconvert_converter_t* v4lconvert_converter_getConverterByConversion(u32 from, u32 to) {
	//TODO use better lookup algorithm than O(n)
	for (int i = 0; i < NUM_V4L_CONVERTERS; i++) {
		v4lconvert_converter_t* converter = &(v4lconvert_converters[i]);
		if ((converter->src_fmt == from) && (converter->dst_fmt == to))
			return converter;
	}
	return NULL;
}

#ifdef __cplusplus
}
#endif

#endif
