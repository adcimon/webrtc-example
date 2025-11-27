#include "pch.h"

#include "Application.h"

int main(
	int argc,
	char* argv[])
{
	auto app = rtc::make_ref_counted<Application>();
	app->run();
	return 0;
}
