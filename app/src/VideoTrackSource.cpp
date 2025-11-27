#include "pch.h"

#include "VideoTrackSource.h"

void VideoTrackSource::sendFrame(
	const webrtc::VideoFrame& frame)
{
	if (sink)
	{
		sink->OnFrame(frame);
	}
}

void VideoTrackSource::AddOrUpdateSink(
	rtc::VideoSinkInterface<webrtc::VideoFrame>* sink,
	const rtc::VideoSinkWants&)
{
	this->sink = sink;
	std::cout << "Video sink added or updated to video track source" << std::endl;
}

void VideoTrackSource::RemoveSink(
	rtc::VideoSinkInterface<webrtc::VideoFrame>*)
{
	sink = nullptr;
	std::cout << "Video sink removed from video track source" << std::endl;
}

webrtc::MediaSourceInterface::SourceState VideoTrackSource::state() const
{
	return webrtc::MediaSourceInterface::SourceState::kLive;
}

bool VideoTrackSource::remote() const { return false; }

bool VideoTrackSource::is_screencast() const { return false; }

std::optional<bool> VideoTrackSource::needs_denoising() const { return std::nullopt; }

bool VideoTrackSource::GetStats(
	Stats*)
{
	return false;
}

bool VideoTrackSource::SupportsEncodedOutput() const { return false; }

void VideoTrackSource::GenerateKeyFrame() {}

void VideoTrackSource::AddEncodedSink(
	rtc::VideoSinkInterface<webrtc::RecordableEncodedFrame>*)
{
}

void VideoTrackSource::RemoveEncodedSink(
	rtc::VideoSinkInterface<webrtc::RecordableEncodedFrame>*)
{
}

void VideoTrackSource::RegisterObserver(
	webrtc::ObserverInterface*)
{
}

void VideoTrackSource::UnregisterObserver(
	webrtc::ObserverInterface*)
{
}
