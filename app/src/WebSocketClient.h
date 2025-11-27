#pragma once

#include <functional>
#include <string>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

class WebSocketClient : public std::enable_shared_from_this<WebSocketClient>
{
public:
	using tcp = boost::asio::ip::tcp;

	WebSocketClient(
		boost::asio::io_context& context,
		std::string host,
		std::string port,
		std::string target,
		std::function<void(std::string)> on_message);

	void run();
	void send(const std::string& msg);
	void close();

private:
	void on_resolve(boost::system::error_code error_code, tcp::resolver::results_type results);
	void on_connect(boost::system::error_code error_code);
	void on_handshake(boost::system::error_code error_code);
	void do_read();

	tcp::resolver resolver;
	boost::beast::websocket::stream<tcp::socket> socket;
	boost::beast::flat_buffer buffer;
	std::string host;
	std::string port;
	std::string target;
	std::function<void(std::string)> on_message;
};
