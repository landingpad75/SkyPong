#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/client.hpp>
#include <thread>
#include <atomic>
#include <cstring>  // For strcmp

using websocketpp::connection_hdl;
using server = websocketpp::server<websocketpp::config::asio>;

std::string uri;

class PingServer {
public:
    PingServer() {
        m_server.set_access_channels(websocketpp::log::alevel::all);
        m_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        m_server.set_message_handler(bind(&PingServer::on_message, this, std::placeholders::_1, std::placeholders::_2));
        m_server.set_open_handler(bind(&PingServer::on_open, this, std::placeholders::_1));
        m_server.set_close_handler(bind(&PingServer::on_close, this, std::placeholders::_1));

        m_server.init_asio();
    }

    void run(uint16_t port) {
        m_server.listen(port);
        m_server.start_accept();
        m_server.run();
    }

private:
    server m_server;

    void on_open(connection_hdl hdl) {
        std::cout << "Client connected!" << std::endl;
        m_server.send(hdl, "Ping", websocketpp::frame::opcode::text);
    }

    void on_message(connection_hdl hdl, server::message_ptr msg) {
        std::cout << "Received: " << msg->get_payload() << std::endl;
        if (msg->get_payload() == "Pong") {
            std::cout << "Client responded with Pong!" << std::endl;
        }
    }

    void on_close(connection_hdl hdl) {
        std::cout << "Client disconnected!" << std::endl;
    }
};

using client = websocketpp::client<websocketpp::config::asio>;

class PingClient {
public:
    PingClient() : m_connected(false) {
        m_client.set_access_channels(websocketpp::log::alevel::all);
        m_client.clear_access_channels(websocketpp::log::alevel::frame_payload);

        m_client.set_message_handler(bind(&PingClient::on_message, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
        m_client.set_open_handler(bind(&PingClient::on_open, this, websocketpp::lib::placeholders::_1));
        m_client.set_close_handler(bind(&PingClient::on_close, this, websocketpp::lib::placeholders::_1));

        m_client.init_asio();
    }

    int sendMessage(const std::string& msg) {
        if (m_connected) {
            websocketpp::lib::error_code ec;
            m_client.send(m_hdl, msg, websocketpp::frame::opcode::text, ec);
            if (ec) {
                return 1;  // Error
            } else {
                return 0;  // Sent successfully
            }
        } else {
            return -1;  // Not connected
        }
    }

    void run(const std::string& uri) {
        websocketpp::lib::error_code ec;
        client::connection_ptr con = m_client.get_connection(uri, ec);
        if (ec) {
            std::cout << "Could not create connection because: " << ec.message() << std::endl;
            return;
        }

        m_client.connect(con);
        m_client.run();
    }

private:
    client m_client;
    connection_hdl m_hdl;
    std::atomic<bool> m_connected;

    void on_open(connection_hdl hdl) {
        m_hdl = hdl;
        m_connected = true;
        std::cout << "Connected to server!" << std::endl;
    }

    void on_message(connection_hdl hdl, client::message_ptr msg) {
        std::cout << "Received: " << msg->get_payload() << std::endl;
        if (msg->get_payload() == "Ping") {
            m_client.send(hdl, "Pong", websocketpp::frame::opcode::text);
            std::cout << "Sent: Pong" << std::endl;
        }
    }

    void on_close(connection_hdl hdl) {
        std::cout << "Disconnected from server!" << std::endl;
        m_connected = false;
    }
};

void start_server() {
    PingServer server;
    uint16_t port = 6969;
    std::cout << "Starting server on port " << port << "..." << std::endl;
    server.run(port);
}

PingClient start_client() {
    PingClient client;
    std::cout << "Connecting to server at " << uri << "..." << std::endl;
    client.run(uri);
    while (true) {
        std::string input;
        std::cin >> input;
        int msg = client.sendMessage(input);
        if (msg == 0)
            std::cout << "Sent: " << input << std::endl;
        else if (msg == 1)
            std::cout << "Error sending message." << std::endl;
        else if (msg == -1)
            std::cout << "No internet/server offline." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "--host") == 0) {
        std::thread server_thread(start_server);
        server_thread.join();  // Wait for the server to finish
    } else {
        std::thread client_thread(start_client, "ws://localhost:6969");
        client_thread.join();  // Wait for the client to finish
    }

    return 0;
}
