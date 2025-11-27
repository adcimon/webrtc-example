#include "pch.h"

#include "WebSocketClient.h"

WebSocketClient::WebSocketClient(
	boost::asio::io_context& context,
	std::string host,
	std::string port,
	std::string target,
	std::function<void(std::string)> on_message)
	: resolver(context), socket(context), host(std::move(host)), port(std::move(port)), target(std::move(target)),
	  on_message(std::move(on_message))
{
}

void WebSocketClient::run()
{
	resolver.async_resolve(
		host, port, [self = shared_from_this()](auto ec, auto results) { self->on_resolve(ec, results); });
}

void WebSocketClient::on_resolve(
	boost::system::error_code error_code,
	tcp::resolver::results_type results)
{
	if (error_code)
	{
		std::cerr << "Resolve: " << error_code.message() << std::endl;
		return;
	}

	boost::asio::async_connect(
		socket.next_layer(),
		results.begin(),
		results.end(),
		[self = shared_from_this()](auto error_code, auto) { self->on_connect(error_code); });
}

void WebSocketClient::on_connect(
	boost::system::error_code error_code)
{
	if (error_code)
	{
		std::cerr << "Connect: " << error_code.message() << std::endl;
		return;
	}

	socket.async_handshake(
		host, target, [self = shared_from_this()](auto error_code) { self->on_handshake(error_code); });
}

void WebSocketClient::on_handshake(
	boost::system::error_code error_code)
{
	if (error_code)
	{
		std::cerr << "Handshake: " << error_code.message() << std::endl;
		return;
	}
	do_read();
}

void WebSocketClient::do_read()
{
	socket.async_read(
		buffer,
		[self = shared_from_this()](auto error_code, std::size_t bytes)
		{
			if (error_code)
			{
				std::cerr << "Read: " << error_code.message() << std::endl;
				return;
			}

			std::string msg = boost::beast::buffers_to_string(self->buffer.data());
			self->buffer.consume(bytes);

			if (self->on_message)
			{
				self->on_message(msg);
			}

			self->do_read();
		});
}

void WebSocketClient::send(
	const std::string& msg)
{
	boost::asio::post(
		socket.get_executor(),
		[self = shared_from_this(), msg]()
		{
			self->socket.async_write(
				boost::asio::buffer(msg),
				[self](auto error_code, std::size_t)
				{
					if (error_code)
					{
						std::cerr << "Write: " << error_code.message() << std::endl;
					}
				});
		});
}

void WebSocketClient::close()
{
	boost::asio::post(
		socket.get_executor(),
		[self = shared_from_this()]()
		{
			boost::system::error_code error_code;
			self->socket.close(boost::beast::websocket::close_code::normal, error_code);
			if (error_code)
			{
				std::cerr << "Close: " << error_code.message() << std::endl;
			}
		});
}
