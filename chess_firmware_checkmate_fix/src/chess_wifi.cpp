#include "chess_wifi.h"
#include "leds.h"

const char* WIFI_SSID = "Device-Northwestern";

// Replace this with your server's IP while connected to Device-Northwestern.
const char* SERVER_URL = "http://18.221.166.110:3000/api/test";
const char* SERVER_IP = "18.221.166.110";
const uint16_t SERVER_PORT = 3000;

// physical player color
char physicalPlayerColor = '?';

WebSocketsClient webSocket;
bool socketConnected = false;

String virtual_move = "";
bool virtual_move_was_capture = false;

bool move_rejected = false;
bool game_won = false;
bool game_lost = false;
bool game_draw = false;
bool game_result_needs_virtual_move_completion = false;

String last_status_message = "";
String last_rejected_move = "";
String last_capture_move = "";

int lastHandledMoveVersion = -1;
int lastHandledGameOverVersion = -1;

bool wifi_connect() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    delay(1000);

    WiFi.begin(WIFI_SSID);

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
        Serial.print("HTTP response length: ");
        Serial.println(payload.length());
    }

    http.end();

    return httpCode == 200;
}

bool parse_uci_move(const char* move, int& fromRow, int& fromCol, int& toRow, int& toCol) {
    if (move == nullptr || strlen(move) < 4) {
        return false;
    }

    char fromFile = move[0];
    char fromRank = move[1];
    char toFile   = move[2];
    char toRank   = move[3];

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

String build_move_string(const char* from, const char* to, const char* promotion) {
    if (from == nullptr || to == nullptr) {
        return "";
    }

    if (strlen(from) != 2 || strlen(to) != 2) {
        return "";
    }

    String move = String(from) + String(to);
    if (promotion != nullptr && strlen(promotion) > 0) {
        move += promotion[0];
    }

    return move;
}

String move_from_json(JsonVariantConst moveObj) {
    if (moveObj.isNull()) {
        return "";
    }

    if (moveObj.is<const char*>()) {
        const char* move = moveObj.as<const char*>();
        return move == nullptr ? "" : String(move);
    }

    const char* lan = moveObj["lan"];
    if (lan != nullptr && strlen(lan) >= 4) {
        return String(lan);
    }

    const char* move = moveObj["move"];
    if (move != nullptr && strlen(move) >= 4) {
        return String(move);
    }

    const char* uci = moveObj["uci"];
    if (uci != nullptr && strlen(uci) >= 4) {
        return String(uci);
    }

    const char* from = moveObj["from"];
    const char* to = moveObj["to"];
    const char* promotion = moveObj["promotion"];

    return build_move_string(from, to, promotion);
}

bool move_object_was_capture(JsonVariantConst moveObj) {
    if (moveObj.isNull()) {
        return false;
    }

    bool isCapture = moveObj["isCapture"] | false;
    if (isCapture) {
        return true;
    }

    const char* captured = moveObj["captured"];
    if (captured != nullptr && strlen(captured) > 0) {
        return true;
    }

    const char* flags = moveObj["flags"];
    if (flags != nullptr) {
        // chess.js flags include "c" for normal capture and "e" for en passant.
        return strchr(flags, 'c') != nullptr || strchr(flags, 'e') != nullptr;
    }

    return false;
}

bool root_was_capture(JsonDocument& doc) {
    if (doc["captured"].is<bool>()) {
        return doc["captured"].as<bool>();
    }

    bool isCapture = doc["isCapture"] | false;
    if (isCapture) {
        return true;
    }

    const char* captured = doc["captured"];
    if (captured != nullptr && strlen(captured) > 0) {
        return true;
    }

    return move_object_was_capture(doc["acceptedMove"]);
}

bool is_physical_source(const char* source) {
    if (source == nullptr) {
        return false;
    }

    return strcmp(source, "physicalboard") == 0 ||
           strcmp(source, "esp32") == 0 ||
           strcmp(source, "physical") == 0;
}

bool is_opponent_source(const char* source) {
    if (source == nullptr) {
        return false;
    }

    return strcmp(source, "engine") == 0 ||
           strcmp(source, "virtualboard") == 0;
}

bool is_physical_player_color(const char* color) {
    return color != nullptr &&
           (physicalPlayerColor == 'w' || physicalPlayerColor == 'b') &&
           color[0] == physicalPlayerColor &&
           color[1] == '\0';
}

void update_physical_player_color(JsonDocument& doc, bool fromPhysicalBoard) {
    JsonVariantConst acceptedMove = doc["acceptedMove"];
    const char* color = acceptedMove["color"];
    if (color == nullptr) {
        color = doc["color"];
    }

    if (color == nullptr ||
        (strcmp(color, "w") != 0 && strcmp(color, "b") != 0)) {
        return;
    }

    char inferredColor = fromPhysicalBoard
        ? color[0]
        : (color[0] == 'w' ? 'b' : 'w');

    if (physicalPlayerColor != inferredColor) {
        physicalPlayerColor = inferredColor;
        Serial.print("Physical player color inferred as ");
        Serial.println(physicalPlayerColor == 'b' ? "black" : "white");
    }
}

bool is_physical_move(JsonDocument& doc) {
    const char* source = doc["source"];
    if (is_opponent_source(source)) {
        return false;
    }

    if (is_physical_source(source)) {
        return true;
    }

    JsonVariantConst acceptedMove = doc["acceptedMove"];
    const char* color = acceptedMove["color"];
    if (color == nullptr) {
        color = doc["color"];
    }

    return is_physical_player_color(color);
}

void queue_virtual_move(String move, bool wasCapture) {
    if (move.length() < 4) {
        return;
    }

    virtual_move = move;
    virtual_move_was_capture = wasCapture;

    if (wasCapture) {
        last_capture_move = move;
    }

    Serial.print("Queued opponent move: ");
    Serial.print(virtual_move);
    if (wasCapture) {
        Serial.print(" capture");
    }
    Serial.println();
}


bool move_belongs_to_physical_player(JsonDocument& doc, JsonVariantConst moveObj) {
    const char* source = doc["source"];
    if (is_opponent_source(source)) {
        return false;
    }

    if (is_physical_source(source)) {
        return true;
    }

    const char* color = moveObj["color"];
    if (color == nullptr) {
        color = doc["color"];
    }

    return is_physical_player_color(color);
}

bool queue_final_opponent_move_if_present(JsonDocument& doc) {
    JsonVariantConst moveObj = doc["acceptedMove"];

    if (moveObj.isNull()) {
        moveObj = doc["lastMove"];
    }

    String move = move_from_json(moveObj);
    bool wasCapture = move_object_was_capture(moveObj);

    if (move.length() < 4) {
        const char* rootMove = doc["move"];
        if (rootMove != nullptr && strlen(rootMove) >= 4) {
            move = String(rootMove);
            wasCapture = root_was_capture(doc);
        }
    }

    if (move.length() < 4) {
        return false;
    }

    if (move_belongs_to_physical_player(doc, moveObj)) {
        return false;
    }

    queue_virtual_move(move, wasCapture);
    game_result_needs_virtual_move_completion = true;
    Serial.println("Game-over move is an opponent move; waiting for physical completion before animation.");
    return true;
}

void set_game_result_flags(bool isDrawResult, const char* winner, const char* reason) {
    last_status_message = reason == nullptr ? "Game over" : String(reason);

    if (isDrawResult) {
        game_draw = true;
        game_won = false;
        game_lost = false;
        return;
    }

    if (is_physical_player_color(winner)) {
        game_won = true;
        game_lost = false;
        game_draw = false;
    }
    else {
        game_won = false;
        game_lost = true;
        game_draw = false;
    }
}

void handle_game_over_if_needed(JsonDocument& doc, int version) {
    JsonVariantConst status = doc["status"];

    if (status.isNull()) {
        return;
    }

    bool isGameOver = status["isGameOver"] | false;
    bool isCheckmate = status["isCheckmate"] | false;
    bool isDraw = status["isDraw"] | false;
    bool isStalemate = status["isStalemate"] | false;

    const char* statusName = status["status"];
    const char* winner = status["winner"];
    const char* reason = status["reason"];

    bool statusNameIsGameOver =
        statusName != nullptr &&
        (strcmp(statusName, "checkmate") == 0 ||
         strcmp(statusName, "draw") == 0 ||
         strcmp(statusName, "stalemate") == 0);

    if (!isGameOver && !isCheckmate && !isDraw && !isStalemate && !statusNameIsGameOver) {
        return;
    }

    if (version >= 0 && version == lastHandledGameOverVersion) {
        return;
    }

    if (version >= 0) {
        lastHandledGameOverVersion = version;
    }

    bool drawResult = isDraw || isStalemate ||
        (statusName != nullptr &&
         (strcmp(statusName, "draw") == 0 || strcmp(statusName, "stalemate") == 0));

    bool queuedFinalMove = queue_final_opponent_move_if_present(doc);

    set_game_result_flags(drawResult, winner, reason);

    // wait to show the result if the opponent's move is not on the board yet
    game_result_needs_virtual_move_completion =
        queuedFinalMove || virtual_move != "";

    Serial.print("RX game over: ");
    Serial.print(statusName == nullptr ? "unknown" : statusName);
    if (winner != nullptr) {
        Serial.print(" winner=");
        Serial.print(winner);
    }
    if (reason != nullptr) {
        Serial.print(" reason=");
        Serial.print(reason);
    }
    Serial.println(" (animation pending)");
}

void handle_accepted_move(JsonDocument& doc) {
    int version = doc["version"] | -1;

    if (version >= 0 && version == lastHandledMoveVersion) {
        handle_game_over_if_needed(doc, version);
        return;
    }

    JsonVariantConst acceptedMove = doc["acceptedMove"];

    String move = move_from_json(acceptedMove);

    if (move.length() < 4) {
        const char* moveStr = doc["move"];
        if (moveStr != nullptr) {
            move = String(moveStr);
        }
    }

    if (move.length() < 4) {
        Serial.println("Accepted move message did not include a parseable move");
        handle_game_over_if_needed(doc, version);
        return;
    }

    bool wasCapture = root_was_capture(doc);
    bool fromPhysicalBoard = is_physical_move(doc);
    update_physical_player_color(doc, fromPhysicalBoard);

    const char* source = doc["source"];
    Serial.print("RX accepted: ");
    Serial.print(move);
    Serial.print(" src=");
    Serial.print(source == nullptr ? "unknown" : source);
    if (wasCapture) {
        Serial.print(" capture");
    }
    Serial.println();

    if (version >= 0) {
        lastHandledMoveVersion = version;
    }

    // opponent move
    if (!fromPhysicalBoard) {
        queue_virtual_move(move, wasCapture);
    }

    handle_game_over_if_needed(doc, version);
}

void handle_rejected_move(JsonDocument& doc) {
    const char* reason = doc["reason"];
    if (reason == nullptr) {
        reason = doc["message"];
    }

    String rejectedMove = move_from_json(doc["attemptedMove"]);

    if (rejectedMove.length() < 4) {
        rejectedMove = move_from_json(doc["attemptedMessage"]);
    }

    if (rejectedMove.length() < 4) {
        const char* moveStr = doc["move"];
        if (moveStr != nullptr) {
            rejectedMove = String(moveStr);
        }
    }

    if (rejectedMove.length() >= 4) {
        last_rejected_move = rejectedMove;
    }

    move_rejected = true;
    last_status_message = reason == nullptr ? "Move rejected" : String(reason);

    Serial.print("RX rejected");
    if (rejectedMove.length() >= 4) {
        Serial.print(" ");
        Serial.print(rejectedMove);
    }
    if (reason != nullptr) {
        Serial.print(": ");
        Serial.print(reason);
    }
    Serial.println();

    show_invalid_move();
}

void handle_legacy_virtual_move(JsonDocument& doc) {
    const char* move = doc["move"];

    if (move == nullptr) {
        Serial.println("Virtual board message missing move");
        return;
    }

    Serial.print("RX virtual move: ");
    Serial.println(move);

    queue_virtual_move(String(move), false);
}

DeserializationError parse_server_json(JsonDocument& doc, uint8_t* payload, size_t length) {
    StaticJsonDocument<1024> filter;

    filter["type"] = true;
    filter["version"] = true;
    filter["source"] = true;
    filter["move"] = true;
    filter["color"] = true;
    filter["captured"] = true;
    filter["isCapture"] = true;
    filter["reason"] = true;
    filter["message"] = true;
    filter["playerInput"] = true;
    filter["opponent"] = true;
    filter["playerColor"] = true;
    filter["engineElo"] = true;

    filter["attemptedMove"]["from"] = true;
    filter["attemptedMove"]["to"] = true;
    filter["attemptedMove"]["lan"] = true;
    filter["attemptedMove"]["move"] = true;
    filter["attemptedMove"]["uci"] = true;
    filter["attemptedMove"]["promotion"] = true;

    filter["attemptedMessage"]["from"] = true;
    filter["attemptedMessage"]["to"] = true;
    filter["attemptedMessage"]["lan"] = true;
    filter["attemptedMessage"]["move"] = true;
    filter["attemptedMessage"]["uci"] = true;
    filter["attemptedMessage"]["promotion"] = true;

    filter["acceptedMove"]["from"] = true;
    filter["acceptedMove"]["to"] = true;
    filter["acceptedMove"]["lan"] = true;
    filter["acceptedMove"]["move"] = true;
    filter["acceptedMove"]["uci"] = true;
    filter["acceptedMove"]["captured"] = true;
    filter["acceptedMove"]["isCapture"] = true;
    filter["acceptedMove"]["flags"] = true;
    filter["acceptedMove"]["promotion"] = true;
    filter["acceptedMove"]["san"] = true;
    filter["acceptedMove"]["piece"] = true;
    filter["acceptedMove"]["color"] = true;

    filter["lastMove"]["from"] = true;
    filter["lastMove"]["to"] = true;
    filter["lastMove"]["lan"] = true;
    filter["lastMove"]["move"] = true;
    filter["lastMove"]["uci"] = true;
    filter["lastMove"]["captured"] = true;
    filter["lastMove"]["isCapture"] = true;
    filter["lastMove"]["flags"] = true;
    filter["lastMove"]["promotion"] = true;
    filter["lastMove"]["san"] = true;
    filter["lastMove"]["piece"] = true;
    filter["lastMove"]["color"] = true;

    filter["status"]["status"] = true;
    filter["status"]["winner"] = true;
    filter["status"]["winnerName"] = true;
    filter["status"]["result"] = true;
    filter["status"]["reason"] = true;
    filter["status"]["turn"] = true;
    filter["status"]["turnName"] = true;
    filter["status"]["isGameOver"] = true;
    filter["status"]["isCheckmate"] = true;
    filter["status"]["isDraw"] = true;
    filter["status"]["isStalemate"] = true;

    return deserializeJson(doc, payload, length, DeserializationOption::Filter(filter));
}

void handle_server_message(uint8_t* payload, size_t length) {
    DynamicJsonDocument doc(4096);

    DeserializationError error = parse_server_json(doc, payload, length);

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
        handle_accepted_move(doc);
    }
    else if (strcmp(type, "game:state") == 0) {
        if (!doc["acceptedMove"].isNull()) {
            handle_accepted_move(doc);
        }
        else {
            int version = doc["version"] | -1;
            handle_game_over_if_needed(doc, version);
        }
    }
    else if (strcmp(type, "game:over") == 0) {
        int version = doc["version"] | -1;
        handle_game_over_if_needed(doc, version);
    }
    else if (strcmp(type, "move:rejected") == 0) {
        handle_rejected_move(doc);
    }
    else if (strcmp(type, "move:virtualboard") == 0) {
        handle_legacy_virtual_move(doc);
    }
    else if (strcmp(type, "server:hello") == 0) {
        const char* message = doc["message"];
        Serial.print("Server hello: ");
        Serial.println(message == nullptr ? "" : message);
    }
    else if (strcmp(type, "game:config") == 0) {
        const char* playerColor = doc["playerColor"];
        if (playerColor != nullptr &&
            (strcmp(playerColor, "w") == 0 || strcmp(playerColor, "b") == 0)) {
            physicalPlayerColor = playerColor[0];
        }

        Serial.print("Game config: input=");
        Serial.print(doc["playerInput"] | "unknown");
        Serial.print(" opponent=");
        Serial.print(doc["opponent"] | "unknown");
        Serial.print(" physicalColor=");
        if (physicalPlayerColor == 'b') {
            Serial.println("black");
        }
        else if (physicalPlayerColor == 'w') {
            Serial.println("white");
        }
        else {
            Serial.println("unknown");
        }
    }
    else if (strcmp(type, "error") == 0) {
        const char* message = doc["message"];
        Serial.print("Server error: ");
        Serial.println(message == nullptr ? "unknown" : message);

        last_status_message = message == nullptr ? "Server error" : String(message);
        move_rejected = true;

        show_invalid_move();
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
            // Do not print the full JSON payload here. game:state messages are
            // large and make the serial monitor hard to read.
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
    webSocket.enableHeartbeat(15000, 3000, 2);

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

String square_to_chess_notation(int row, int col) {
    char file = 'a' + col;
    char rank = '8' - row;

    String square = "";
    square += file;
    square += rank;

    return square;
}

String move_to_uci(int fromRow, int fromCol, int toRow, int toCol) {
    return square_to_chess_notation(fromRow, fromCol) +
           square_to_chess_notation(toRow, toCol);
}

void send_board_move(int from_row, int from_col, int to_row, int to_col) {
    if (!socketConnected) {
        Serial.println("Cannot send move: WebSocket not connected");
        move_rejected = true;
        last_status_message = "WebSocket not connected";
        show_invalid_move();
        return;
    }

    String move = move_to_uci(from_row, from_col, to_row, to_col);

    StaticJsonDocument<512> message;

    message["type"] = "move:physicalboard";
    message["source"] = "esp32";
    message["move"] = move;

    message["from_row"] = from_row;
    message["from_col"] = from_col;
    message["to_row"] = to_row;
    message["to_col"] = to_col;

    message["from"] = square_to_chess_notation(from_row, from_col);
    message["to"] = square_to_chess_notation(to_row, to_col);

    String output;
    serializeJson(message, output);

    Serial.print("TX physical move: ");
    Serial.print(move);
    Serial.printf(" (%i,%i)->(%i,%i)\r\n", from_row, from_col, to_row, to_col);

    webSocket.sendTXT(output);
}

String get_virtual_move() {
    return virtual_move;
}

bool get_virtual_move_was_capture() {
    return virtual_move_was_capture;
}

void clear_virtual_move() {
    virtual_move = "";
    virtual_move_was_capture = false;
}

void clear_move_rejected() {
    move_rejected = false;
    last_rejected_move = "";
}

bool was_move_rejected() {
    return move_rejected;
}

bool was_game_won() {
    return game_won;
}

void clear_game_won() {
    game_won = false;
}

bool was_game_lost() {
    return game_lost;
}

void clear_game_lost() {
    game_lost = false;
}

bool was_game_draw() {
    return game_draw;
}

void clear_game_draw() {
    game_draw = false;
}

bool game_result_waiting_for_virtual_move() {
    return game_result_needs_virtual_move_completion;
}

void require_virtual_move_before_game_result() {
    game_result_needs_virtual_move_completion = true;
}

void clear_game_result_virtual_wait() {
    game_result_needs_virtual_move_completion = false;
}

String get_last_status_message() {
    return last_status_message;
}

void clear_last_status_message() {
    last_status_message = "";
}

String get_last_rejected_move() {
    return last_rejected_move;
}

String get_last_capture_move() {
    return last_capture_move;
}

void clear_last_capture_move() {
    last_capture_move = "";
}
