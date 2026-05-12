#include "chess_wifi.h"

const char* WIFI_SSID = "Cameron's iPhone xs Max (2)";
const char* WIFI_PASS = "lmgtyt13";

// Replace this with your laptop IP while connected to the hotspot
const char* SERVER_URL = "http://172.20.10.7:3000/api/test";

const char* SERVER_IP = "172.20.10.7";
const uint16_t SERVER_PORT = 3000;

WebSocketsClient webSocket;
bool socketConnected = false;

String virtual_move = "";

bool wifi_connect() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    delay(1000);

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    Serial.print("Connecting to WiFi");

    unsigned long startTime = millis();

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");

        if (millis() - startTime > 30000) {
            Serial.println();
            Serial.println("WiFi connection timed out");
            Serial.print("WiFi status: ");
            Serial.println(WiFi.status());
            return false;
        }
    }

    Serial.println();
    Serial.println("WiFi connected");

    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());

    return true;
}


bool test_server_http() {
    HTTPClient http;

    http.begin(SERVER_URL);

    int httpCode = http.GET();

    Serial.print("HTTP Response code: ");
    Serial.println(httpCode);

    if (httpCode > 0) {
        String payload = http.getString();
        Serial.println("Server response:");
        Serial.println(payload);
    }

    http.end();

    return httpCode == 200;
}

bool parse_uci_move(const char* move, int& fromRow, int& fromCol, int& toRow, int& toCol) {
    if (move == nullptr) {
        return false;
    }

    if (strlen(move) < 4) {
        return false;
    }

    char fromFile = move[0]; // e
    char fromRank = move[1]; // 7
    char toFile   = move[2]; // e
    char toRank   = move[3]; // 5

    if (fromFile < 'a' || fromFile > 'h') return false;
    if (toFile   < 'a' || toFile   > 'h') return false;
    if (fromRank < '1' || fromRank > '8') return false;
    if (toRank   < '1' || toRank   > '8') return false;

    fromCol = fromFile - 'a';
    toCol   = toFile - 'a';

    fromRow = '8' - fromRank;
    toRow   = '8' - toRank;

    return true;
}

void handle_server_message(uint8_t* payload, size_t length) {
    StaticJsonDocument<256> doc;

    DeserializationError error = deserializeJson(doc, payload, length);

    if (error) {
        Serial.print("JSON parse failed: ");
        Serial.println(error.c_str());
        return;
    }

    const char* type = doc["type"];

    if (type == nullptr) {
        Serial.println("Message missing type");
        return;
    }

    if (strcmp(type, "move:accepted") == 0) {
        const char* move = doc["move"];
        Serial.print("Move accepted by server: ");
        Serial.println(move);
    }

    else if (strcmp(type, "move:virtualboard") == 0) {
        const char* move = doc["move"];

        Serial.print("Virtual board move received: ");
        Serial.println(move);

        virtual_move = move;

        int fromRow, fromCol, toRow, toCol;

        if (parse_uci_move(move, fromRow, fromCol, toRow, toCol)) {
            Serial.printf(
                "Parsed virtual move: from row %i col %i to row %i col %i\r\n",
                fromRow, fromCol, toRow, toCol
            );

            // TODO: call LEDs here
            // show_move_leds(fromRow, fromCol, toRow, toCol);
        } else {
            Serial.println("Invalid move format");
        }
    }

    else {
        Serial.print("Unknown server message type: ");
        Serial.println(type);
    }
}

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
    switch (type) {
        case WStype_CONNECTED:
            Serial.println("WebSocket connected to server");
            socketConnected = true;

            webSocket.sendTXT("{\"type\":\"esp32:hello\",\"message\":\"hello from ESP32\"}");
            break;

        case WStype_DISCONNECTED:
            Serial.println("WebSocket disconnected");
            socketConnected = false;
            break;

        case WStype_TEXT:
            Serial.print("Message from server: ");
            Serial.println((char*)payload);
            handle_server_message(payload, length);
            break;

        default:
            break;
    }
}



void websocket_begin() {
    webSocket.begin(SERVER_IP, SERVER_PORT, "/");
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000);

    Serial.println("WebSocket client started");
}

void websocket_loop() {
    webSocket.loop();
}

bool websocket_is_connected() {
    return socketConnected;
}

void send_hello_to_server() {
    if (!socketConnected) {
        Serial.println("Cannot send hello: WebSocket not connected");
        return;
    }

    webSocket.sendTXT("{\"type\":\"esp32:hello\",\"message\":\"manual hello from ESP32\"}");
}
// helper functions to convert row and col to and from to a full move
String square_to_chess_notation(int row, int col) {
    char file = 'a' + col;
    char rank = '8' - row;

    String square = "";
    square += file;
    square += rank;

    return square;
}
// see above comment
String move_to_uci(int fromRow, int fromCol, int toRow, int toCol) {
    return square_to_chess_notation(fromRow, fromCol) +
           square_to_chess_notation(toRow, toCol);
}

void send_board_move(int from_row, int from_col, int to_row, int to_col) {
    if (!socketConnected) {
        Serial.println("Cannot send move: WebSocket not connected");
        return;
    }

    // convert indexes to a move
    String move = move_to_uci(from_row, from_col, to_row, to_col);
    // make move into a json message to send to server
    StaticJsonDocument<256> message;

    message["type"] = "move:physicalboard";
    message["move"] = move;
    message["from_row"] = from_row;
    message["from_col"] = from_col;
    message["to_row"] = to_row;
    message["to_col"] = to_col;

    // make json output
    String output;
    serializeJson(message, output);

    Serial.print("Sending physical board move to server: ");
    Serial.println(output);

    webSocket.sendTXT(output);
}


String get_virtual_move(){
    return virtual_move;
}

void clear_virtual_move(){
    virtual_move = "";
}