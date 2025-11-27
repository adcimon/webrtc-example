#pragma once

#include <api/video/video_frame.h>

class FrameTransformer
{
public:
	webrtc::VideoFrame transformFrame(const webrtc::VideoFrame& frame);

private:
	uint8_t data[1920 * 1080 * 4];
	long counter = 0;
};
