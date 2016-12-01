
#include "../types.h"
#include "libvideo.h"
#include "libv4lconvert-priv.h"

#ifndef __LIBVIDEO_LIBV4LCONVERT_LIBV4LCONVERT_FLAT_H
#define __LIBVIDEO_LIBV4LCONVERT_LIBV4LCONVERT_FLAT_H

#ifdef __cplusplus
extern "C" {
#endif
#ifndef LIBV4L_PUBLIC
#if __GNUC__ >= 4
#define LIBV4L_PUBLIC __attribute__ ((visibility("default")))
#else
#define LIBV4L_PUBLIC
#endif
#endif

LIBV4L_PUBLIC enum v4lconvert_conversion_signature {
	/**
	 * Value if this signature is unset (struct's will defaultly initialize with this value)
	 */
	v4lconvert_conversion_signature_unset = 0,
	/**
	 * For signature <code>void convert(const u8* src, u8* dst, u32 width, u32 height);</code>
	 */
	v4lconvert_conversion_signature_sdwh_0f,
	v4lconvert_conversion_signature_sdwh_1f,
	v4lconvert_conversion_signature_sdwh_2f,
	v4lconvert_conversion_signature_sd_sf_0f,
	v4lconvert_conversion_signature_sd_sf_1f,
	v4lconvert_conversion_signature_sd_sf_2f,
	v4lconvert_conversion_signature_special
};

LIBV4L_PUBLIC union v4lconvert_conversion_fn {
	void (*cvt_sdwh_0f) (const u8* src, u8* dst, u32 width, u32 height);
	void (*cvt_sdwh_1f) (const u8* src, u8* dst, u32 width, u32 height, int flag1);
	void (*cvt_sdwh_2f) (const u8* src, u8* dst, u32 width, u32 height, int flag1, int flag2);
	void (*cvt_sd_sf_0f) (const u8* src, u8* dst, const struct v4l2_format* src_fmt);
	void (*cvt_sd_sf_1f) (const u8* src, u8* dst, const struct v4l2_format* src_fmt, int flag1);
	void (*cvt_sd_sf_2f) (const u8* src, u8* dst, const struct v4l2_format* src_fmt, int flag1, int flag2);
	void* user_defined;
};

LIBV4L_PUBLIC enum v4lconvert_conversion_type {
	/**
	 * Unknown transformation
	 */
	v4lconvert_conversion_type_unknown = 0,
	/**
	 * Identity conversion, such as memcpy
	 */
	v4lconvert_conversion_type_identity,
	/**
	 * Image format conversion
	 */
	v4lconvert_conversion_type_imf,
	/**
	 * Image crop
	 */
	v4lconvert_conversion_type_crop,
	/**
	 * Pad border around image
	 */
	v4lconvert_conversion_type_pad,
	/**
	 * Scale image
	 */
	v4lconvert_conversion_type_scale,
	/**
	 * Rotate image
	 */
	v4lconvert_conversion_type_rotate,
	/**
	 * Rotate image 90deg clockwise
	 */
	v4lconvert_conversion_type_rotate90,
	/**
	 * Rotate image 180deg
	 * Same as hflip + vflip
	 */
	v4lconvert_conversion_type_rotate180,
	/**
	 * Flip image horizontally
	 */
	v4lconvert_conversion_type_hflip,
	/**
	 * Flip image vertically
	 */
	v4lconvert_conversion_type_vflip
};

#define V4LCONVERT_CONVERSION_TYPE_FLAG(type) (1u << v4lconvert_conversion_type_##type)

LIBV4L_PUBLIC struct v4lconvert_converter {
	u32 id;
	enum v4lconvert_conversion_signature signature;
	union v4lconvert_conversion_fn target;
	enum v4lconvert_conversion_type type;
	u32 src_fmt;
	u32 dst_fmt;
	int flag1;
	int flag2;
	unsigned int cost;
};

typedef const struct v4lconvert_converter v4lconvert_converter_t;

LIBV4L_PUBLIC struct v4lconvert_buffer {
	/**
	 * If true, buffer 0 should NOT be overwritten.
	 */
	bool preserve_buf0;
	/**
	 * Capacity of buffer 0
	 */
	u32 buf0_cap;
	u32 buf1_cap;
	u32 buf2_cap;
	u32 buf0_len;
	u32 buf1_len;
	/**
	 * Input buffer
	 */
	u8* buf0;
	/**
	 * Output buffer
	 */
	u8* buf1;
	/**
	 * Intermediate buffer. May be null.
	 */
	u8* buf2;
};

LIBV4L_PUBLIC struct v4lconvert_encoder {
	/**
	 * 
	 * @return number of bytes written to dst, or 0 if error
	 * If error, errno may be set.
	 */
	u32 (*apply) (struct v4lconvert_encoder* self, const u8* src, u32 src_len, u8* dst, u32 dst_len);
	/**
	 * Safely release memory used by encoder. Does not release the encoder itself.
	 * DO NOT call any methods on encoder after this method has been invoked.
	 */
	int (*release) (struct v4lconvert_encoder* self);
	u32 src_fmt;
	u32 dst_fmt;
	
	size_t src_len;
	unsigned int src_width;
	unsigned int src_height;
	/**
	 * Estimates the minimum size the destination buffer can be.
	 */
	size_t dst_len;
	unsigned int dst_width;
	unsigned int dst_height;
	
	//Private members. Please don't touch.
	v4lconvert_converter_t* converter;
	union {
		struct v4l2_format* imf_v4l2_src_fmt;
		struct {
			u32 row_stride;
			int quality;
			struct jpeg_compress_struct* cinfo;
			struct jpeg_error_mgr* cerr;
		} jpeg_encode_params;
		struct {
			signed int top;
			signed int left;
			//dst_width & dst_height are used to determine the other two sides
		} crop_params;
	};
};

LIBV4L_PUBLIC struct v4lconvert_encoder_series {
	/**
	 * Method to invoke actual conversion
	 */
	u32 (*convert) (struct v4lconvert_encoder_series* self, struct v4lconvert_buffer* buffers);
	/**
	 * Release memory owned by this struct.
	 */
	bool (*release) (struct v4lconvert_encoder_series* self);
	/**
	 * Source format
	 */
	u32 src_fmt;
	/**
	 * Destination format (the format that this encoder series encodes to)
	 */
	u32 dst_fmt;
	
	
	size_t src_len;
	size_t dst_len;
	/**
	 * Number of encoders in series
	 */
	size_t num_encoders;
	/**
	 * Array of encoders used.
	 * Please don't change it.
	 */
	struct v4lconvert_encoder** encoders;
};

LIBV4L_PUBLIC struct v4lconvert_conversion_request {
	struct v4l2_format src_fmt;
	signed int rotation;
	bool flipHorizontal;
	bool flipVertical;
	float scaleFactor;
	// Values for crop/pad
	signed int top_offset;
	signed int left_offset;
	struct v4l2_format dst_fmt;
};

LIBV4L_PUBLIC u32 v4lconvert_estimateBufferSize(u32 fmt, u32 width, u32 height);

LIBV4L_PUBLIC int v4lconvert_encoder_initWithConverter(struct v4lconvert_encoder* encoder, v4lconvert_converter_t* converter, u32 width, u32 height);
LIBV4L_PUBLIC int v4lconvert_encoder_initForIMF(struct v4lconvert_encoder* encoder, u32 src_fmt, u32 dst_fmt, u32 width, u32 height);

LIBV4L_PUBLIC bool v4lconvert_encoder_series_create(struct v4lconvert_encoder_series* self, struct v4lconvert_conversion_request* request, char** errmsg) __attribute__((nonnull (1, 2)));
LIBV4L_PUBLIC int v4lconvert_encoder_series_init(struct v4lconvert_encoder_series* self, u32 width, u32 height, u32 numConverters, u32* converterIds);
LIBV4L_PUBLIC int v4lconvert_encoder_series_computeConversion(struct v4lconvert_encoder_series* self, u32 width, u32 height, u32 from, u32 to, unsigned int maxIterations);
LIBV4L_PUBLIC int v4lconvert_encoder_series_doRelease(struct v4lconvert_encoder_series* self);

/**
 * Allocate/compute buffers
 * @param series Series to create buffers for
 * @param num_buffers number of buffers to create (length of buffers array)
 * @param buffers Array of buffers to create
 * @param allocate boolean, whether to allocate the buffers themselves or not
 * @return 1 on success, 0 on failure
 */
LIBV4L_PUBLIC int v4lconvert_encoder_series_createBuffers(struct v4lconvert_encoder_series* series, u32 num_buffers, struct v4lconvert_buffer** buffers, int allocate);
LIBV4L_PUBLIC int v4lconvert_buffer_release(struct v4lconvert_buffer* buffer);

LIBV4L_PUBLIC v4lconvert_converter_t* v4lconvert_converter_getConverterById(unsigned int converterId);
LIBV4L_PUBLIC int v4lconvert_converter_lookupConverterByConversion(u32 from, u32 to);
LIBV4L_PUBLIC v4lconvert_converter_t* v4lconvert_converter_getConverterByConversion(u32 from, u32 to);

#ifdef __cplusplus
}
#endif

#endif