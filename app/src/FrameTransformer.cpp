#include "pch.h"

#include "FrameTransformer.h"

webrtc::VideoFrame FrameTransformer::transformFrame(
	const webrtc::VideoFrame& frame)
{
	rtc::scoped_refptr<webrtc::I420BufferInterface> buffer(frame.video_frame_buffer()->ToI420());
	int width = buffer->width();
	int height = buffer->height();
	if (width > 1920 || height > 1080)
	{
		return frame;
	}

	libyuv::I420ToARGB(
		buffer->DataY(),
		buffer->StrideY(),
		buffer->DataU(),
		buffer->StrideU(),
		buffer->DataV(),
		buffer->StrideV(),
		data,
		width * 4,
		width,
		height);

	int h_start = counter % (height - 100);
	int w_start = counter % (width - 100);
	for (int h = h_start; h < h_start + 100; h++)
	{
		for (int w = w_start; w < w_start + 100; w++)
		{
			int index = h * width * 4 + w * 4;
			data[index] = 0;
			data[index + 1] = 0;
			data[index + 2] = 255;
			data[index + 3] = 255;
		}
	}
	++counter;

	rtc::scoped_refptr<webrtc::I420Buffer> new_buffer = webrtc::I420Buffer::Create(width, height);
	libyuv::ARGBToI420(
		data,
		width * 4,
		new_buffer->MutableDataY(),
		buffer->StrideY(),
		new_buffer->MutableDataU(),
		buffer->StrideU(),
		new_buffer->MutableDataV(),
		buffer->StrideV(),
		width,
		height);

	return webrtc::VideoFrame::Builder()
		.set_video_frame_buffer(new_buffer)
		.set_rotation(frame.rotation())
		.set_timestamp_us(frame.timestamp_us())
		.set_id(frame.id())
		.build();
}
