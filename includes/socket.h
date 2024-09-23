#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/client.hpp>

typedef websocketpp::client<websocketpp::config::asio> client;

class WebSocketClient {
	public:
		bool online = false;
		int players = 0;
		int time = 0;
		WebSocketClient();
		~WebSocketClient();

		void connect(const std::string& uri);
		void on_message(websocketpp::connection_hdl hdl, client::message_ptr msg);
		int send_message(std::string msg);
		void on_open(websocketpp::connection_hdl hdl);
	private:
		
		websocketpp::connection_hdl hdl;
		client m_endpoint;
		std::thread m_thread;	
};