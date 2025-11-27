#include "pch.h"

#include "AudioSource.h"

void AudioSource::sendAudioData(
	const void* audio_data,
	int bits_per_sample,
	int sample_rate,
	size_t number_of_channels,
	size_t number_of_frames)
{
	if (sink)
	{
		sink->OnData(audio_data, bits_per_sample, sample_rate, number_of_channels, number_of_frames);
	}
}

void AudioSource::AddSink(
	webrtc::AudioTrackSinkInterface* sink)
{
	this->sink = sink;
	std::cout << "Audio track sink added to audio source" << std::endl;
}

void AudioSource::RemoveSink(
	webrtc::AudioTrackSinkInterface*)
{
	sink = nullptr;
	std::cout << "Audio track sink removed from audio source" << std::endl;
}

const cricket::AudioOptions AudioSource::options() const { return cricket::AudioOptions(); }

webrtc::MediaSourceInterface::SourceState AudioSource::state() const
{
	return webrtc::MediaSourceInterface::SourceState::kLive;
}

bool AudioSource::remote() const { return false; }

void AudioSource::RegisterObserver(
	webrtc::ObserverInterface*)
{
}

void AudioSource::UnregisterObserver(
	webrtc::ObserverInterface*)
{
}
