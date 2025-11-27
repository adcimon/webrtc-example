#include "pch.h"

#include "AudioReceiver.h"

#include "AudioSource.h"

AudioReceiver::AudioReceiver(
	AudioSource* source)
{
	this->source = source;
}

void AudioReceiver::OnData(
	const void* audio_data,
	int bits_per_sample,
	int sample_rate,
	size_t number_of_channels,
	size_t number_of_frames)
{
	source->sendAudioData(audio_data, bits_per_sample, sample_rate, number_of_channels, number_of_frames);
}
