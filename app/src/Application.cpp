#include "pch.h"

#include "Application.h"

#include "AudioDevice.h"
#include "AudioReceiver.h"
#include "AudioSource.h"
#include "SessionDescriptionObserver.h"
#include "VideoReceiver.h"
#include "VideoTrackSource.h"
#include "WebSocketClient.h"

Application::Application() {}

Application::~Application() {}

void Application::run()
{
	rtc::InitializeSSL();

	std::string input;
	std::cout << "Send offer? (y/n)" << std::endl;
	std::cin >> input;
	std::transform(input.begin(), input.end(), input.begin(), [](unsigned char c) { return std::tolower(c); });
	bool send_offer = (input == "y" || input == "yes");

	auto signaling_thread = rtc::Thread::CreateWithSocketServer();
	signaling_thread->Start();

	factory = webrtc::CreatePeerConnectionFactory(
		/* Network Thread */ nullptr,
		/* Worker Thread */ nullptr,
		/* Signaling Thread*/ signaling_thread.get(),
		AudioDevice::Create(),
		webrtc::CreateBuiltinAudioEncoderFactory(),
		webrtc::CreateBuiltinAudioDecoderFactory(),
		std::make_unique<webrtc::VideoEncoderFactoryTemplate<
			webrtc::LibvpxVp8EncoderTemplateAdapter,
			webrtc::LibvpxVp9EncoderTemplateAdapter,
			webrtc::OpenH264EncoderTemplateAdapter,
			webrtc::LibaomAv1EncoderTemplateAdapter>>(),
		std::make_unique<webrtc::VideoDecoderFactoryTemplate<
			webrtc::LibvpxVp8DecoderTemplateAdapter,
			webrtc::LibvpxVp9DecoderTemplateAdapter,
			webrtc::OpenH264DecoderTemplateAdapter,
			webrtc::Dav1dDecoderTemplateAdapter>>(),
		nullptr,
		nullptr);

	webrtc::PeerConnectionInterface::RTCConfiguration config;
	config.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;

	webrtc::PeerConnectionInterface::IceServer server;
	server.urls.push_back("stun:stun.l.google.com:19302");
	config.servers.push_back(server);

	webrtc::PeerConnectionDependencies pc_dependencies(this);
	auto error_or_peer_connection = factory->CreatePeerConnectionOrError(config, std::move(pc_dependencies));
	if (!error_or_peer_connection.ok())
	{
		std::cout << "Error: Create peer connection failed" << std::endl;
		return;
	}

	peer_connection = std::move(error_or_peer_connection.value());

	audio_source = rtc::make_ref_counted<AudioSource>();
	auto audio_track = factory->CreateAudioTrack("audio_label", audio_source.get());
	peer_connection->AddTrack(audio_track, {"stream_id"});

	video_source = rtc::make_ref_counted<VideoTrackSource>();
	auto video_track = factory->CreateVideoTrack(video_source, "video_label");
	peer_connection->AddTrack(video_track, {"stream_id"});

	audio_receiver = std::make_unique<AudioReceiver>(audio_source.get());
	video_receiver = std::make_unique<VideoReceiver>(video_source.get());

	if (send_offer)
	{
		signaling_thread->PostDelayedTask(
			[&]() { peer_connection->CreateOffer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions()); },
			webrtc::TimeDelta::Seconds(5));
	}

	socket = std::make_shared<WebSocketClient>(
		context, "localhost", "9000", "/", [this](std::string msg) { handleMessage(msg); });

	socket->run();

	context.run();
}

void Application::handleMessage(
	const std::string& msg)
{
	boost::system::error_code error_code;

	boost::json::value json = boost::json::parse(boost::json::string_view(msg), error_code);
	if (error_code)
	{
		return;
	}

	if (!json.is_object())
	{
		return;
	}

	boost::json::object& json_msg = json.as_object();

	if (json_msg.contains("offer") && json_msg["offer"].is_object())
	{
		std::cout << "[IN] " << msg << std::endl;

		auto& offer = json_msg["offer"].as_object();

		std::string type = boost::json::value_to<std::string>(offer["type"]);
		if (type == "offer")
		{
			std::string sdp = boost::json::value_to<std::string>(offer["sdp"]);
			std::optional<webrtc::SdpType> sdp_type = webrtc::SdpTypeFromString(type);

			webrtc::SdpParseError error;
			auto remote_offer = webrtc::CreateSessionDescription(*sdp_type, sdp, &error);
			if (!remote_offer)
			{
				return;
			}

			auto observer = SessionDescriptionObserver::Create();
			peer_connection->SetRemoteDescription(observer.get(), remote_offer.release());

			peer_connection->CreateAnswer(this, webrtc::PeerConnectionInterface::RTCOfferAnswerOptions());
		}

		return;
	}

	if (json_msg.contains("answer") && json_msg["answer"].is_object())
	{
		std::cout << "[IN] " << msg << std::endl;

		auto& answer = json_msg["answer"].as_object();

		std::string type = boost::json::value_to<std::string>(answer["type"]);
		if (type == "answer")
		{
			std::string sdp = boost::json::value_to<std::string>(answer["sdp"]);
			std::optional<webrtc::SdpType> sdp_type = webrtc::SdpTypeFromString(type);

			webrtc::SdpParseError error;
			auto remote_answer = webrtc::CreateSessionDescription(*sdp_type, sdp, &error);
			if (!remote_answer)
			{
				return;
			}

			auto observer = SessionDescriptionObserver::Create();
			peer_connection->SetRemoteDescription(observer.get(), remote_answer.release());
		}

		return;
	}

	if (json_msg.contains("candidate") && json_msg["candidate"].is_object())
	{
		std::cout << "[IN] " << msg << std::endl;

		auto& candidate = json_msg["candidate"].as_object();

		std::string sdp_mid = boost::json::value_to<std::string>(candidate["sdpMid"]);
		int sdp_mlineindex = boost::json::value_to<int>(candidate["sdpMLineIndex"]);
		std::string sdp_candidate = boost::json::value_to<std::string>(candidate["candidate"]);

		webrtc::SdpParseError error;
		auto remote_candidate = webrtc::CreateIceCandidate(sdp_mid, sdp_mlineindex, sdp_candidate, &error);
		if (remote_candidate)
		{
			peer_connection->AddIceCandidate(remote_candidate);
		}

		return;
	}
}

void Application::OnAddTrack(
	rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver,
	const std::vector<rtc::scoped_refptr<webrtc::MediaStreamInterface>>& streams)
{
	std::cout << "Add track" << std::endl;

	auto* track = receiver->track().release();

	if (track->kind() == webrtc::MediaStreamTrackInterface::kAudioKind)
	{
		static_cast<webrtc::AudioTrackInterface*>(track)->AddSink(audio_receiver.get());
	}

	if (track->kind() == webrtc::MediaStreamTrackInterface::kVideoKind)
	{
		static_cast<webrtc::VideoTrackInterface*>(track)->AddOrUpdateSink(video_receiver.get(), rtc::VideoSinkWants());
	}

	track->Release();
}

void Application::OnIceCandidate(
	const webrtc::IceCandidateInterface* candidate)
{
	std::string sdp_candidate;
	candidate->ToString(&sdp_candidate);

	boost::json::object json_candidate;
	json_candidate["sdpMid"] = candidate->sdp_mid();
	json_candidate["sdpMLineIndex"] = candidate->sdp_mline_index();
	json_candidate["candidate"] = sdp_candidate;

	boost::json::object json;
	json["candidate"] = json_candidate;

	if (socket)
	{
		std::string msg = boost::json::serialize(json);
		std::cout << "[OUT] " << msg << std::endl;
		socket->send(msg);
	}
}

void Application::OnConnectionChange(
	webrtc::PeerConnectionInterface::PeerConnectionState new_state)
{
	std::cout << webrtc::PeerConnectionInterface::AsString(new_state) << std::endl;
}

void Application::OnSignalingChange(
	webrtc::PeerConnectionInterface::SignalingState new_state)
{
}

void Application::OnTrack(
	rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver)
{
}

void Application::OnRemoveTrack(
	rtc::scoped_refptr<webrtc::RtpReceiverInterface> receiver)
{
}

void Application::OnDataChannel(
	rtc::scoped_refptr<webrtc::DataChannelInterface> channel)
{
}

void Application::OnRenegotiationNeeded() {}

void Application::OnIceConnectionChange(
	webrtc::PeerConnectionInterface::IceConnectionState new_state)
{
}

void Application::OnIceGatheringChange(
	webrtc::PeerConnectionInterface::IceGatheringState new_state)
{
}

void Application::OnIceConnectionReceivingChange(
	bool receiving)
{
}

void Application::OnStandardizedIceConnectionChange(
	webrtc::PeerConnectionInterface::IceConnectionState new_state)
{
}

void Application::OnIceCandidateError(
	const std::string& address,
	int port,
	const std::string& url,
	int error_code,
	const std::string& error_text)
{
}

void Application::OnSuccess(
	webrtc::SessionDescriptionInterface* desc)
{
	auto observer = SessionDescriptionObserver::Create();
	peer_connection->SetLocalDescription(observer.get(), desc);

	webrtc::SdpType sdp_type = desc->GetType();
	std::string type = webrtc::SdpTypeToString(sdp_type);
	std::string sdp;
	desc->ToString(&sdp);

	boost::json::object json_desc;
	json_desc["type"] = type;
	json_desc["sdp"] = sdp;

	boost::json::object json;
	json[type] = json_desc;

	if (socket)
	{
		std::string msg = boost::json::serialize(json);
		std::cout << "[OUT] " << msg << std::endl;
		socket->send(msg);
	}
}

void Application::OnFailure(
	webrtc::RTCError)
{
	std::cout << "Error: Create session description failed" << std::endl;
}
