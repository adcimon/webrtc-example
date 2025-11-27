#pragma once

#include <api/media_stream_interface.h>

#include "AudioSource.h"

class AudioSource;

class AudioReceiver : public webrtc::AudioTrackSinkInterface
{
public:
	explicit AudioReceiver(AudioSource* source);
	void OnData(
		const void* audio_data,
		int bits_per_sample,
		int sample_rate,
		size_t number_of_channels,
		size_t number_of_frames) override;

private:
	AudioSource* source;
};
