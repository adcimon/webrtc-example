cmake_minimum_required(VERSION 4.0.3)
project(webrtc-example VERSION 1.0.0)

# --------------------
# Compiler Options
# --------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()

# --------------------
# Directories
# --------------------

# External

set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/external" CACHE PATH "Path to external dependencies")

# WebRTC

set(WEBRTC_DIR "${EXTERNAL_DIR}/webrtc")
set(WEBRTC_INC_DIR "${WEBRTC_DIR}/include")
set(WEBRTC_LIB_DIR "${WEBRTC_DIR}/lib")

include_directories(
	${WEBRTC_INC_DIR}
	${WEBRTC_INC_DIR}/third_party/abseil-cpp
	${WEBRTC_INC_DIR}/third_party/abseil-cpp/absl
	${WEBRTC_INC_DIR}/third_party/libyuv/include
)

link_directories(
	${WEBRTC_LIB_DIR}
)

# --------------------
# Targets
# --------------------

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
	"${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_precompile_headers(${PROJECT_NAME} PRIVATE
	src/pch.h
)

# Boost

find_package(boost_asio CONFIG REQUIRED)
find_package(boost_asio_core CONFIG REQUIRED)
find_package(boost_asio_deadline_timer CONFIG REQUIRED)
find_package(boost_asio_spawn CONFIG REQUIRED)
find_package(boost_beast CONFIG REQUIRED)
find_package(boost_json CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS headers)

target_link_libraries(${PROJECT_NAME} PRIVATE
	Boost::asio
	Boost::asio_core
	Boost::asio_deadline_timer
	Boost::asio_spawn
	Boost::beast
	Boost::json
	Boost::headers
)

# WebRTC

target_compile_definitions(${PROJECT_NAME} PRIVATE
	WEBRTC_ALLOW_DEPRECATED_NAMESPACES
)

if(UNIX)
	target_compile_definitions(${PROJECT_NAME} PRIVATE
		WEBRTC_LINUX
		WEBRTC_POSIX=1
		__STDC_CONSTANT_MACROS=1
	)
	target_link_libraries(${PROJECT_NAME} PRIVATE
		dl
		Dmoguids
		libwebrtc.a
		Msdmo
		pthread
		Secur32
		wmcodecdspuuid
		Winmm
	)
endif()

if(APPLE)
	target_compile_definitions(${PROJECT_NAME} PRIVATE
		WEBRTC_MAC
		WEBRTC_POSIX=1
	)
	target_link_libraries(${PROJECT_NAME} PRIVATE
		libwebrtc.a
	)
endif()

if(WIN32)
	target_compile_definitions(${PROJECT_NAME} PRIVATE
		ABSL_ALLOCATOR_NOTHROW=1
		CERT_CHAIN_PARA_HAS_EXTRA_FIELDS
		DYNAMIC_ANNOTATIONS_ENABLED=0
		HAVE_SCTP
		NOMINMAX
		NTDDI_VERSION=NTDDI_WIN10_RS2
		NVALGRIND
		PSAPI_VERSION=2
		RTC_ENABLE_VP9
		UNIQUE
		USE_AURA=1
		WEBRTC_ENABLE_PROTOBUF=0
		WEBRTC_INCLUDE_INTERNAL_AUDIO_DEVICE
		WEBRTC_LIBRARY_IMPL
		WEBRTC_NON_STATIC_TRACE_EVENT_HANDLERS=0
		WEBRTC_WIN
		WIN10=_WIN32_WINNT_WIN10
		WIN32
		WIN32_LEAN_AND_MEAN
		__STD_C
		__WRL_NO_DEFAULT_LIB__
		_HAS_EXCEPTIONS=0
		_ITERATOR_DEBUG_LEVEL=0
		_SILENCE_ALL_CXX20_DEPRECATION_WARNINGS
		_ATL_NO_OPENGL
		_CRT_RAND_S
		_CRT_SECURE_NO_WARNINGS
		_SECURE_ATL
		_UNICODE
		_WINDOWS
	)
	target_link_libraries(${PROJECT_NAME} PRIVATE
		dmoguids.lib
		iphlpapi.lib
		msdmo.lib
		secur32.lib
		Strmiids.lib
		webrtc.lib
		winmm.lib
		wmcodecdspuuid.lib
		WS2_32
	)
endif()
