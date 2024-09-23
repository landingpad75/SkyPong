#include <socket.h>

struct WebSocketValue {
    std::string code;
    std::string value;

    WebSocketValue(const std::string& code, const std::string& value)
        : code(code), value(value) {}
};

WebSocketValue parseWebSocketString(const std::string& input) {
    std::size_t delimiterPos = input.find('-');
    std::cout << input << std::endl;
    if (delimiterPos == std::string::npos || delimiterPos == input.size() - 1) {
        throw std::invalid_argument("Invalid input format");
    }

    std::string code = input.substr(0, delimiterPos);
    std::string value = input.substr(delimiterPos + 1);

    return WebSocketValue(code, value);
}

WebSocketClient::WebSocketClient() {
    m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
    m_endpoint.clear_error_channels(websocketpp::log::elevel::all);
    m_endpoint.init_asio();
	m_endpoint.set_open_handler(bind(&WebSocketClient::on_open, this, std::placeholders::_1));
    m_endpoint.set_message_handler(bind(&WebSocketClient::on_message, this, std::placeholders::_1, std::placeholders::_2));
}

void WebSocketClient::connect(const std::string& uri) {
    websocketpp::lib::error_code ec;
    client::connection_ptr con = m_endpoint.get_connection(uri, ec);
    if (ec) {
        std::cerr << "Could not create connection: " << ec.message() << std::endl;
        return;
    }
    m_endpoint.connect(con);
    m_thread = std::thread([this]() { m_endpoint.run(); });
}

void WebSocketClient::on_open(websocketpp::connection_hdl handle) {
	hdl = handle;
}

int WebSocketClient::send_message(std::string msg){
	m_endpoint.send(hdl, msg, websocketpp::frame::opcode::text);
    return 0;
}

void WebSocketClient::on_message(websocketpp::connection_hdl hdl, client::message_ptr msg) {
    std::cout << "Message received: " << msg->get_payload() << std::endl;
    // Handle the message as needed
    std::string payload = msg->get_payload();
	if(msg->get_payload() == "on"){
		this->send_message("OK");
        this->online = true;
        return;
	}

    try {
        WebSocketValue thing = parseWebSocketString(payload);
        if (thing.code == "2") {
            this->players = std::stoi(thing.value);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing WebSocket payload: " << e.what() << std::endl;
        return;
    }
}

WebSocketClient::~WebSocketClient() {
    m_endpoint.stop();
    if (m_thread.joinable()) {
        m_thread.join();
    }
}