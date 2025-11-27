#pragma once

#include <api/peer_connection_interface.h>

class SessionDescriptionObserver : public webrtc::SetSessionDescriptionObserver
{
public:
	static rtc::scoped_refptr<SessionDescriptionObserver> Create();
	void OnSuccess() override;
	void OnFailure(webrtc::RTCError error) override;
};
