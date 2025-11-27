#include "pch.h"

#include "VideoReceiver.h"

#include "VideoTrackSource.h"

VideoReceiver::VideoReceiver(
	VideoTrackSource* source)
{
	this->source = source;
}

void VideoReceiver::OnFrame(
	const webrtc::VideoFrame& frame)
{
	source->sendFrame(transformer.transformFrame(frame));
}
