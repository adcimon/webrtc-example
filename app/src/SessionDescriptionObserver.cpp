#include "pch.h"

#include "SessionDescriptionObserver.h"

rtc::scoped_refptr<SessionDescriptionObserver> SessionDescriptionObserver::Create()
{
	return rtc::make_ref_counted<SessionDescriptionObserver>();
}

void SessionDescriptionObserver::OnSuccess() {}

void SessionDescriptionObserver::OnFailure(
	webrtc::RTCError error)
{
	std::cout << "Error: Set session description failed" << std::endl;
}
