#include "pch.h"

#include "AudioDevice.h"

AudioDevice::~AudioDevice()
{
	webrtc::MutexLock lock(&mutex);
	if (thread)
	{
		thread->Stop();
	}
}

rtc::scoped_refptr<AudioDevice> AudioDevice::Create() { return rtc::make_ref_counted<AudioDevice>(); }

int32_t AudioDevice::RegisterAudioCallback(
	webrtc::AudioTransport* audioCallback)
{
	bool just_created = false;
	{
		webrtc::MutexLock lock(&mutex);
		audio_callback = audioCallback;

		if (!thread)
		{
			thread = rtc::Thread::Create();
			thread->Start();
			just_created = true;
		}
	}

	if (just_created)
	{
		thread->PostTask([this] { this->processFrame(); });
	}

	return 0;
}

void AudioDevice::processFrame()
{
	int64_t time1 = rtc::TimeMillis();
	{
		webrtc::MutexLock lock(&mutex);

		size_t nSamplesOut = 0;
		int64_t elapsed_time_ms = 0;
		int64_t ntp_time_ms = 0;
		memset(data, 0, sizeof(data));

		if (audio_callback != nullptr)
		{
			audio_callback->NeedMorePlayData(
				AUDIO_SAMPLES_IN_WEBRTC_FRAME,
				2,
				1,
				AUDIO_SAMPLE_RATE,
				data,
				nSamplesOut,
				&elapsed_time_ms,
				&ntp_time_ms);
		}
	}

	int64_t time2 = rtc::TimeMillis();
	int64_t next_frame_time = time1 + frame_length;
	int64_t wait_time = next_frame_time - time2;

	thread->PostDelayedTask([this]() -> void { this->processFrame(); }, webrtc::TimeDelta::Millis(wait_time));
}

int32_t AudioDevice::ActiveAudioLayer(
	AudioLayer* audioLayer) const
{
	return 0;
};

int32_t AudioDevice::Init() { return 0; };

int32_t AudioDevice::Terminate() { return 0; };

bool AudioDevice::Initialized() const { return 0; };

int16_t AudioDevice::PlayoutDevices() { return 0; };

int16_t AudioDevice::RecordingDevices() { return 0; };

int32_t AudioDevice::PlayoutDeviceName(
	uint16_t index,
	char name[webrtc::kAdmMaxDeviceNameSize],
	char guid[webrtc::kAdmMaxGuidSize])
{
	return 0;
};

int32_t AudioDevice::RecordingDeviceName(
	uint16_t index,
	char name[webrtc::kAdmMaxDeviceNameSize],
	char guid[webrtc::kAdmMaxGuidSize])
{
	return 0;
};

int32_t AudioDevice::SetPlayoutDevice(
	uint16_t index)
{
	return 0;
};

int32_t AudioDevice::SetPlayoutDevice(
	WindowsDeviceType device)
{
	return 0;
};

int32_t AudioDevice::SetRecordingDevice(
	uint16_t index)
{
	return 0;
};

int32_t AudioDevice::SetRecordingDevice(
	WindowsDeviceType device)
{
	return 0;
};

int32_t AudioDevice::PlayoutIsAvailable(
	bool* available)
{
	return 0;
};

int32_t AudioDevice::InitPlayout() { return 0; };

bool AudioDevice::PlayoutIsInitialized() const { return 0; };

int32_t AudioDevice::RecordingIsAvailable(
	bool* available)
{
	return 0;
};

int32_t AudioDevice::InitRecording() { return 0; };

bool AudioDevice::RecordingIsInitialized() const { return 0; };

int32_t AudioDevice::StartPlayout() { return 0; };

int32_t AudioDevice::StopPlayout() { return 0; };

bool AudioDevice::Playing() const { return 0; };

int32_t AudioDevice::StartRecording() { return 0; };

int32_t AudioDevice::StopRecording() { return 0; };

bool AudioDevice::Recording() const { return 0; };

int32_t AudioDevice::InitSpeaker() { return 0; };

bool AudioDevice::SpeakerIsInitialized() const { return 0; };

int32_t AudioDevice::InitMicrophone() { return 0; };

bool AudioDevice::MicrophoneIsInitialized() const { return 0; };

int32_t AudioDevice::SpeakerVolumeIsAvailable(
	bool* available)
{
	return 0;
};

int32_t AudioDevice::SetSpeakerVolume(
	uint32_t volume)
{
	return 0;
};

int32_t AudioDevice::SpeakerVolume(
	uint32_t* volume) const
{
	return 0;
};

int32_t AudioDevice::MaxSpeakerVolume(
	uint32_t* maxVolume) const
{
	return 0;
};

int32_t AudioDevice::MinSpeakerVolume(
	uint32_t* minVolume) const
{
	return 0;
};

int32_t AudioDevice::MicrophoneVolumeIsAvailable(
	bool* available)
{
	return 0;
};

int32_t AudioDevice::SetMicrophoneVolume(
	uint32_t volume)
{
	return 0;
};

int32_t AudioDevice::MicrophoneVolume(
	uint32_t* volume) const
{
	return 0;
};

int32_t AudioDevice::MaxMicrophoneVolume(
	uint32_t* maxVolume) const
{
	return 0;
};

int32_t AudioDevice::MinMicrophoneVolume(
	uint32_t* minVolume) const
{
	return 0;
};

int32_t AudioDevice::SpeakerMuteIsAvailable(
	bool* available)
{
	return 0;
};

int32_t AudioDevice::SetSpeakerMute(
	bool enable)
{
	return 0;
};

int32_t AudioDevice::SpeakerMute(
	bool* enabled) const
{
	return 0;
};

int32_t AudioDevice::MicrophoneMuteIsAvailable(
	bool* available)
{
	return 0;
};

int32_t AudioDevice::SetMicrophoneMute(
	bool enable)
{
	return 0;
};

int32_t AudioDevice::MicrophoneMute(
	bool* enabled) const
{
	return 0;
};

int32_t AudioDevice::StereoPlayoutIsAvailable(
	bool* available) const
{
	return 0;
};

int32_t AudioDevice::SetStereoPlayout(
	bool enable)
{
	return 0;
};

int32_t AudioDevice::StereoPlayout(
	bool* enabled) const
{
	return 0;
};

int32_t AudioDevice::StereoRecordingIsAvailable(
	bool* available) const
{
	return 0;
};

int32_t AudioDevice::SetStereoRecording(
	bool enable)
{
	return 0;
};

int32_t AudioDevice::StereoRecording(
	bool* enabled) const
{
	return 0;
};

int32_t AudioDevice::PlayoutDelay(
	uint16_t* delay_ms) const
{
	*delay_ms = 0;
	return 0;
}

bool AudioDevice::BuiltInAECIsAvailable() const { return 0; };

bool AudioDevice::BuiltInAGCIsAvailable() const { return 0; };

bool AudioDevice::BuiltInNSIsAvailable() const { return 0; };

int32_t AudioDevice::EnableBuiltInAEC(
	bool enable)
{
	return -1;
};

int32_t AudioDevice::EnableBuiltInAGC(
	bool enable)
{
	return -1;
};

int32_t AudioDevice::EnableBuiltInNS(
	bool enable)
{
	return -1;
};

#if defined(WEBRTC_IOS)
int AudioDevice::GetPlayoutAudioParameters(
	AudioParameters* params) const
{
	return 0;
};
int AudioDevice::GetRecordAudioParameters(
	AudioParameters* params) const
{
	return 0;
};
#endif
