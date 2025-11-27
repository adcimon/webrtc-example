#pragma once

#include <api/media_stream_interface.h>

class VideoTrackSource : public webrtc::VideoTrackSourceInterface
{
public:
	void sendFrame(const webrtc::VideoFrame& frame);
	void AddOrUpdateSink(rtc::VideoSinkInterface<webrtc::VideoFrame>* sink, const rtc::VideoSinkWants& wants) override;
	void RemoveSink(rtc::VideoSinkInterface<webrtc::VideoFrame>* sink) override;
	webrtc::MediaSourceInterface::SourceState state() const override;
	bool remote() const override;
	bool is_screencast() const override;
	std::optional<bool> needs_denoising() const override;
	bool GetStats(Stats* stats) override;
	bool SupportsEncodedOutput() const override;
	void GenerateKeyFrame() override;
	void AddEncodedSink(rtc::VideoSinkInterface<webrtc::RecordableEncodedFrame>* sink) override;
	void RemoveEncodedSink(rtc::VideoSinkInterface<webrtc::RecordableEncodedFrame>* sink) override;
	void RegisterObserver(webrtc::ObserverInterface* observer) override;
	void UnregisterObserver(webrtc::ObserverInterface* observer) override;

private:
	rtc::VideoSinkInterface<webrtc::VideoFrame>* sink = nullptr;
};
