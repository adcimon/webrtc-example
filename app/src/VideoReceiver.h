#pragma once

#include <api/video/video_frame.h>
#include <api/video/video_sink_interface.h>

#include "FrameTransformer.h"
#include "VideoTrackSource.h"

class VideoTrackSource;

class VideoReceiver : public rtc::VideoSinkInterface<webrtc::VideoFrame>
{
public:
	explicit VideoReceiver(VideoTrackSource* source);
	void OnFrame(const webrtc::VideoFrame& frame) override;

private:
	VideoTrackSource* source;
	FrameTransformer transformer;
};
