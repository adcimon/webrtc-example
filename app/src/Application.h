#pragma once

#include <memory>
#include <thread>

#include <api/peer_connection_interface.h>

#include <boost/asio.hpp>
#include <boost/json.hpp>

class AudioReceiver;
class AudioSource;
class VideoReceiver;
class VideoTrackSource;
class WebSocketClient;

class Application : public webrtc::PeerConnectionObserver, public webrtc::CreateSessionDescriptionObserver
{
public:
	Application();
	~Application();

	void run();
	void handleMessage(const std::string& msg);

	void OnAddTrack(
		rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
		const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>& streams) override;
	void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
	void OnConnectionChange(webrtc::PeerConnectionInterface::PeerConnectionState new_state) override;
	void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override;
	void OnTrack(rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver) override;
	void OnRemoveTrack(rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver) override;
	void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override;
	void OnRenegotiationNeeded() override;
	void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override;
	void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override;
	void OnIceConnectionReceivingChange(bool receiving) override;
	void OnStandardizedIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override;
	void OnIceCandidateError(
		const std::string& address,
		int port,
		const std::string& url,
		int error_code,
		const std::string& error_text) override;

	void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
	void OnFailure(webrtc::RTCError error) override;

private:
	rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory;
	rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection;

	rtc::scoped_refptr<AudioSource> audio_source;
	rtc::scoped_refptr<VideoTrackSource> video_source;
	std::unique_ptr<AudioReceiver> audio_receiver;
	std::unique_ptr<VideoReceiver> video_receiver;

	std::shared_ptr<WebSocketClient> socket;
	boost::asio::io_context context;
};
