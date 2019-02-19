#pragma once

#include <ws_common.hpp>
#include <network/tcp_client.hpp>
#include <websockets/data_frame.hpp>
#include <websockets/websockets_endpoint.hpp>
#include <websockets/message.hpp>
#include <functional>

namespace websockets {
	enum class WebsocketsEvent {
		ConnectionOpened,
		ConnectionClosed,
		GotPing, GotPong
	};
    typedef std::function<void(WebsocketsMessage)> MessageCallback;
    typedef std::function<void(WebsocketsEvent, WSString data)> EventCallback;

	class WebsocketsClient : private internals::WebsocketsEndpoint {
	public:
		WebsocketsClient(network::TcpClient* client);
		
		template <class TcpClientTy>
		static WebsocketsClient Create(TcpClientTy* clientPtr = new TcpClientTy) {
			return WebsocketsClient(clientPtr);
		}

		bool connect(WSString host, int port, WSString path);

		void onMessage(MessageCallback callback);
		void onEvent(EventCallback callback);

		bool poll();
		bool available(bool activeTest = false);

		bool send(WSString data);
		bool sendBinary(WSString data);

		bool ping(WSString data = "");
		bool pong(WSString data = "");

		void close();

		~WebsocketsClient();

	private:
		network::TcpClient* _client;
		bool _connectionOpen;
		MessageCallback _messagesCallback;
		EventCallback _eventsCallback;

		void _handlePing(WebsocketsMessage);
		void _handlePong(WebsocketsMessage);
		void _handleClose(WebsocketsMessage);
	};
}