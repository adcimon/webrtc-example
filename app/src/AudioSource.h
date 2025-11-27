#pragma once

#include <api/media_stream_interface.h>

class AudioSource : public webrtc::AudioSourceInterface
{
public:
	void sendAudioData(
		const void* audio_data,
		int bits_per_sample,
		int sample_rate,
		size_t number_of_channels,
		size_t number_of_frames);
	void AddSink(webrtc::AudioTrackSinkInterface* sink) override;
	void RemoveSink(webrtc::AudioTrackSinkInterface* sink) override;
	const cricket::AudioOptions options() const override;
	webrtc::MediaSourceInterface::SourceState state() const override;
	bool remote() const override;
	void RegisterObserver(webrtc::ObserverInterface* observer) override;
	void UnregisterObserver(webrtc::ObserverInterface* observer) override;

private:
	webrtc::AudioTrackSinkInterface* sink = nullptr;
};
