#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/client.hpp>

typedef websocketpp::client<websocketpp::config::asio> client;
using websocketpp::connection_hdl;

class PingClient {
public:
    PingClient() {
        // Set logging settings
        m_client.set_access_channels(websocketpp::log::alevel::all);
        m_client.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize the client's message handler
        m_client.set_message_handler(bind(&PingClient::on_message, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
        m_client.set_open_handler(bind(&PingClient::on_open, this, websocketpp::lib::placeholders::_1));
        m_client.set_close_handler(bind(&PingClient::on_close, this, websocketpp::lib::placeholders::_1));

        // Start the ASIO io_service run loop
        m_client.init_asio();
    }

    void run(const std::string& uri) {
        websocketpp::lib::error_code ec;
        client::connection_ptr con = m_client.get_connection(uri, ec);
        if (ec) {
            std::cout << "Could not create connection because: " << ec.message() << std::endl;
            return;
        }

        // Connect to the server
        m_client.connect(con);
        m_client.run();
    }

private:
    client m_client;

    void on_open(connection_hdl hdl) {
        std::cout << "Connected to server!" << std::endl;
    }

    void on_message(connection_hdl hdl, client::message_ptr msg) {
        std::cout << "Received: " << msg->get_payload() << std::endl;

        // Respond with "Pong" if we receive "Ping"
        if (msg->get_payload() == "Ping") {
            m_client.send(hdl, "Pong", websocketpp::frame::opcode::text);
            std::cout << "Sent: Pong" << std::endl;
        }
    }

    void on_close(connection_hdl hdl) {
        std::cout << "Disconnected from server!" << std::endl;
    }
};

int main() {
    PingClient client;
    std::string uri = "ws://localhost:9002";

    std::cout << "Connecting to server at " << uri << "..." << std::endl;
    client.run(uri);

    return 0;
}
