// // #include "chess_wifi.h"

// // const char* WIFI_SSID = "Cameron's iPhone xs Max (2)";
// // const char* WIFI_PASS = "lmgtyt13";

// // // Replace this with your laptop IP while connected to the hotspot
// // const char* SERVER_URL = "http://172.20.10.12:3000/api/test";

// // const char* SERVER_IP = "172.20.10.12";
// // const uint16_t SERVER_PORT = 3000;

// // WebSocketsClient webSocket;
// // bool socketConnected = false;

// // String virtual_move = "";

// // bool wifi_connect() {
// //     WiFi.mode(WIFI_STA);
// //     WiFi.disconnect(true);
// //     delay(1000);

// //     WiFi.begin(WIFI_SSID, WIFI_PASS);

// //     Serial.print("Connecting to WiFi");

// //     unsigned long startTime = millis();

// //     while (WiFi.status() != WL_CONNECTED) {
// //         delay(500);
// //         Serial.print(".");

// //         if (millis() - startTime > 30000) {
// //             Serial.println();
// //             Serial.println("WiFi connection timed out");
// //             Serial.print("WiFi status: ");
// //             Serial.println(WiFi.status());
// //             return false;
// //         }
// //     }

// //     Serial.println();
// //     Serial.println("WiFi connected");

// //     Serial.print("ESP32 IP: ");
// //     Serial.println(WiFi.localIP());

// //     return true;
// // }


// // bool test_server_http() {
// //     HTTPClient http;

// //     http.begin(SERVER_URL);

// //     int httpCode = http.GET();

// //     Serial.print("HTTP Response code: ");
// //     Serial.println(httpCode);

// //     if (httpCode > 0) {
// //         String payload = http.getString();
// //         Serial.println("Server response:");
// //         Serial.println(payload);
// //     }

// //     http.end();

// //     return httpCode == 200;
// // }

// // bool parse_uci_move(const char* move, int& fromRow, int& fromCol, int& toRow, int& toCol) {
// //     if (move == nullptr) {
// //         return false;
// //     }

// //     if (strlen(move) < 4) {
// //         return false;
// //     }

// //     char fromFile = move[0]; // e
// //     char fromRank = move[1]; // 7
// //     char toFile   = move[2]; // e
// //     char toRank   = move[3]; // 5

// //     if (fromFile < 'a' || fromFile > 'h') return false;
// //     if (toFile   < 'a' || toFile   > 'h') return false;
// //     if (fromRank < '1' || fromRank > '8') return false;
// //     if (toRank   < '1' || toRank   > '8') return false;

// //     fromCol = fromFile - 'a';
// //     toCol   = toFile - 'a';

// //     fromRow = '8' - fromRank;
// //     toRow   = '8' - toRank;

// //     return true;
// // }

// // void handle_server_message(uint8_t* payload, size_t length) {
// //     StaticJsonDocument<256> doc;

// //     DeserializationError error = deserializeJson(doc, payload, length);

// //     if (error) {
// //         Serial.print("JSON parse failed: ");
// //         Serial.println(error.c_str());
// //         return;
// //     }

// //     const char* type = doc["type"];

// //     if (type == nullptr) {
// //         Serial.println("Message missing type");
// //         return;
// //     }

// //     if (strcmp(type, "move:accepted") == 0) {
// //         const char* move = doc["move"];
// //         Serial.print("Move accepted by server: ");
// //         Serial.println(move);
// //     }

// //     else if (strcmp(type, "move:virtualboard") == 0) {
// //         const char* move = doc["move"];

// //         Serial.print("Virtual board move received: ");
// //         Serial.println(move);

// //         virtual_move = String(move);

// //         int fromRow, fromCol, toRow, toCol;

// //         if (parse_uci_move(move, fromRow, fromCol, toRow, toCol)) {
// //             Serial.printf(
// //                 "Parsed virtual move: from row %i col %i to row %i col %i\r\n",
// //                 fromRow, fromCol, toRow, toCol
// //             );

// //             // TODO: call LEDs here
// //             // show_move_leds(fromRow, fromCol, toRow, toCol);
// //         } else {
// //             Serial.println("Invalid move format");
// //         }
// //     }

// //     else {
// //         Serial.print("Unknown server message type: ");
// //         Serial.println(type);
// //     }
// // }

// // void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
// //     switch (type) {
// //         case WStype_CONNECTED:
// //             Serial.println("WebSocket connected to server");
// //             socketConnected = true;

// //             webSocket.sendTXT("{\"type\":\"esp32:hello\",\"message\":\"hello from ESP32\"}");
// //             break;

// //         case WStype_DISCONNECTED:
// //             Serial.println("WebSocket disconnected");
// //             socketConnected = false;
// //             break;

// //         case WStype_TEXT:
// //             Serial.print("Message from server: ");
// //             Serial.println((char*)payload);
// //             handle_server_message(payload, length);
// //             break;

// //         default:
// //             break;
// //     }
// // }



// // void websocket_begin() {
// //     webSocket.begin(SERVER_IP, SERVER_PORT, "/");
// //     webSocket.onEvent(webSocketEvent);
// //     webSocket.setReconnectInterval(5000);

// //     Serial.println("WebSocket client started");
// // }

// // void websocket_loop() {
// //     webSocket.loop();
// // }

// // bool websocket_is_connected() {
// //     return socketConnected;
// // }

// // void send_hello_to_server() {
// //     if (!socketConnected) {
// //         Serial.println("Cannot send hello: WebSocket not connected");
// //         return;
// //     }

// //     webSocket.sendTXT("{\"type\":\"esp32:hello\",\"message\":\"manual hello from ESP32\"}");
// // }
// // // helper functions to convert row and col to and from to a full move
// // String square_to_chess_notation(int row, int col) {
// //     char file = 'a' + col;
// //     char rank = '8' - row;

// //     String square = "";
// //     square += file;
// //     square += rank;

// //     return square;
// // }
// // // see above comment
// // String move_to_uci(int fromRow, int fromCol, int toRow, int toCol) {
// //     return square_to_chess_notation(fromRow, fromCol) +
// //            square_to_chess_notation(toRow, toCol);
// // }

// // void send_board_move(int from_row, int from_col, int to_row, int to_col) {
// //     if (!socketConnected) {
// //         Serial.println("Cannot send move: WebSocket not connected");
// //         return;
// //     }

// //     // convert indexes to a move
// //     String move = move_to_uci(from_row, from_col, to_row, to_col);
// //     // make move into a json message to send to server
// //     StaticJsonDocument<256> message;

// //     message["type"] = "move:physicalboard";
// //     message["move"] = move;
// //     message["from_row"] = from_row;
// //     message["from_col"] = from_col;
// //     message["to_row"] = to_row;
// //     message["to_col"] = to_col;

// //     // make json output
// //     String output;
// //     serializeJson(message, output);

// //     Serial.print("Sending physical board move to server: ");
// //     Serial.println(output);

// //     webSocket.sendTXT(output);
// // }


// // String get_virtual_move(){
// //     return virtual_move;
// // }

// // void clear_virtual_move(){
// //     virtual_move = "";
// // }

// #include "chess_wifi.h"

// const char* WIFI_SSID = "Cameron's iPhone xs Max (2)";
// const char* WIFI_PASS = "lmgtyt13";

// // Replace this with your laptop IP while connected to the hotspot
// const char* SERVER_URL = "http://172.20.10.12:3000/api/test";

// const char* SERVER_IP = "172.20.10.12";
// const uint16_t SERVER_PORT = 3000;

// WebSocketsClient webSocket;
// bool socketConnected = false;

// String virtual_move = "";

// bool move_rejected = false;

// bool game_won = false;

// bool game_lost = false;

// bool wifi_connect() {
//     WiFi.mode(WIFI_STA);
//     WiFi.disconnect(true);
//     delay(1000);

//     WiFi.begin(WIFI_SSID, WIFI_PASS);

//     Serial.print("Connecting to WiFi");

//     unsigned long startTime = millis();

//     while (WiFi.status() != WL_CONNECTED) {
//         delay(500);
//         Serial.print(".");

//         if (millis() - startTime > 30000) {
//             Serial.println();
//             Serial.println("WiFi connection timed out");
//             Serial.print("WiFi status: ");
//             Serial.println(WiFi.status());
//             return false;
//         }
//     }

//     Serial.println();
//     Serial.println("WiFi connected");

//     Serial.print("ESP32 IP: ");
//     Serial.println(WiFi.localIP());

//     return true;
// }


// bool test_server_http() {
//     HTTPClient http;

//     http.begin(SERVER_URL);

//     int httpCode = http.GET();

//     Serial.print("HTTP Response code: ");
//     Serial.println(httpCode);

//     if (httpCode > 0) {
//         String payload = http.getString();
//         Serial.println("Server response:");
//         Serial.println(payload);
//     }

//     http.end();

//     return httpCode == 200;
// }

// bool parse_uci_move(const char* move, int& fromRow, int& fromCol, int& toRow, int& toCol) {
//     if (move == nullptr) {
//         return false;
//     }

//     if (strlen(move) < 4) {
//         return false;
//     }

//     char fromFile = move[0]; // e
//     char fromRank = move[1]; // 7
//     char toFile   = move[2]; // e
//     char toRank   = move[3]; // 5

//     if (fromFile < 'a' || fromFile > 'h') return false;
//     if (toFile   < 'a' || toFile   > 'h') return false;
//     if (fromRank < '1' || fromRank > '8') return false;
//     if (toRank   < '1' || toRank   > '8') return false;

//     fromCol = fromFile - 'a';
//     toCol   = toFile - 'a';

//     fromRow = '8' - fromRank;
//     toRow   = '8' - toRank;

//     return true;
// }


// bool parse_chess_square(const char* square, int& row, int& col) {
//     if (square == nullptr) {
//         return false;
//     }

//     if (strlen(square) < 2) {
//         return false;
//     }

//     char file = square[0];
//     char rank = square[1];

//     if (file < 'a' || file > 'h') return false;
//     if (rank < '1' || rank > '8') return false;

//     col = file - 'a';
//     row = '8' - rank;

//     return true;
// }

// bool extract_move_from_json(JsonVariant moveNode, String& moveOut, int& fromRow, int& fromCol, int& toRow, int& toCol) {
//     moveOut = "";

//     if (moveNode.isNull()) {
//         return false;
//     }

//     // Case 1: the move is directly a string, like "e2e4" or "e7e8q".
//     if (moveNode.is<const char*>()) {
//         const char* move = moveNode.as<const char*>();

//         if (parse_uci_move(move, fromRow, fromCol, toRow, toCol)) {
//             moveOut = String(move);
//             return true;
//         }

//         return false;
//     }

//     // Case 2: the move is an object, like:
//     // { "from": "e2", "to": "e4", "san": "e4", "lan": "e2e4" }
//     if (!moveNode.is<JsonObject>()) {
//         return false;
//     }

//     JsonObject moveObject = moveNode.as<JsonObject>();

//     const char* move = moveObject["move"];
//     if (move == nullptr) move = moveObject["uci"];
//     if (move == nullptr) move = moveObject["lan"];

//     if (move != nullptr && parse_uci_move(move, fromRow, fromCol, toRow, toCol)) {
//         moveOut = String(move);
//         return true;
//     }

//     const char* fromSquare = moveObject["from"];
//     const char* toSquare = moveObject["to"];

//     if (parse_chess_square(fromSquare, fromRow, fromCol) &&
//         parse_chess_square(toSquare, toRow, toCol)) {

//         moveOut = String(fromSquare) + String(toSquare);

//         const char* promotion = moveObject["promotion"];
//         if (promotion != nullptr && strlen(promotion) > 0) {
//             moveOut += promotion[0];
//         }

//         return true;
//     }

//     return false;
// }

// void queue_virtual_move_for_leds(const String& move, int fromRow, int fromCol, int toRow, int toCol) {
//     virtual_move = move;

//     Serial.print("Queued virtual/remote move for physical board: ");
//     Serial.println(virtual_move);

//     Serial.printf(
//         "Parsed move: from row %i col %i to row %i col %i\r\n",
//         fromRow, fromCol, toRow, toCol
//     );

//     // TODO: call LEDs here
//     // show_move_leds(fromRow, fromCol, toRow, toCol);
// }

// bool parse_message_type(uint8_t* payload, size_t length, String& typeOut) {
//     StaticJsonDocument<64> filter;
//     filter["type"] = true;

//     StaticJsonDocument<128> doc;

//     DeserializationError error = deserializeJson(
//         doc,
//         payload,
//         length,
//         DeserializationOption::Filter(filter)
//     );

//     if (error) {
//         Serial.print("JSON type parse failed: ");
//         Serial.println(error.c_str());
//         return false;
//     }

//     const char* type = doc["type"];

//     if (type == nullptr) {
//         Serial.println("Message missing type");
//         return false;
//     }

//     typeOut = String(type);
//     return true;
// }

// void handle_game_state_message(uint8_t* payload, size_t length) {
//     // game:state messages can be large because they may include board, history,
//     // legal moves, FEN, PGN, status, etc.
//     //
//     // This filter tells ArduinoJson to keep only the fields the ESP32 needs.
//     // That prevents "NoMemory" errors from trying to store the entire message.
//     StaticJsonDocument<768> filter;

//     filter["type"] = true;
//     filter["version"] = true;
//     filter["source"] = true;
//     filter["turnName"] = true;

//     filter["acceptedMove"]["move"] = true;
//     filter["acceptedMove"]["uci"] = true;
//     filter["acceptedMove"]["lan"] = true;
//     filter["acceptedMove"]["from"] = true;
//     filter["acceptedMove"]["to"] = true;
//     filter["acceptedMove"]["promotion"] = true;
//     filter["acceptedMove"]["san"] = true;

//     filter["lastMove"]["move"] = true;
//     filter["lastMove"]["uci"] = true;
//     filter["lastMove"]["lan"] = true;
//     filter["lastMove"]["from"] = true;
//     filter["lastMove"]["to"] = true;
//     filter["lastMove"]["promotion"] = true;
//     filter["lastMove"]["san"] = true;

//     filter["status"]["status"] = true;
//     filter["status"]["reason"] = true;
//     filter["status"]["winnerName"] = true;
//     filter["status"]["isGameOver"] = true;

//     DynamicJsonDocument doc(2048);

//     DeserializationError error = deserializeJson(
//         doc,
//         payload,
//         length,
//         DeserializationOption::Filter(filter)
//     );

//     if (error) {
//         Serial.print("game:state parse failed: ");
//         Serial.println(error.c_str());
//         return;
//     }

//     int version = doc["version"] | -1;
//     const char* source = doc["source"] | "";
//     const char* turnName = doc["turnName"] | "";
//     const char* gameStatus = doc["status"]["status"] | "";
//     const char* reason = doc["status"]["reason"] | "";
//     bool isGameOver = doc["status"]["isGameOver"] | false;

//     Serial.print("Game state received");
//     if (version >= 0) {
//         Serial.print(" v");
//         Serial.print(version);
//     }
//     if (strlen(source) > 0) {
//         Serial.print(" from ");
//         Serial.print(source);
//     }
//     Serial.println();

//     if (strlen(turnName) > 0) {
//         Serial.print("Turn: ");
//         Serial.println(turnName);
//     }

//     if (strlen(gameStatus) > 0) {
//         Serial.print("Status: ");
//         Serial.println(gameStatus);
//     }

//     if (strlen(reason) > 0) {
//         Serial.print("Reason: ");
//         Serial.println(reason);
//     }

//     if (isGameOver) {
//         const char* winnerName = doc["status"]["winnerName"] | "";
//         Serial.print("Game over");
//         if (strlen(winnerName) > 0) {
//             Serial.print(". Winner: ");
//             Serial.print(winnerName);
//         }
//         Serial.println();
//     }

//     String move;
//     int fromRow, fromCol, toRow, toCol;

//     // acceptedMove only exists on a newly accepted move. This is the one we
//     // should use for LEDs so reconnecting to an old game does not retrigger LEDs.
//     if (extract_move_from_json(doc["acceptedMove"], move, fromRow, fromCol, toRow, toCol)) {
//         Serial.print("Accepted move in game state: ");
//         Serial.println(move);

//         // If the move came from this physical board, don't queue it as a virtual
//         // move. If it came from the browser/server/remote opponent, queue it.
//         if (strcmp(source, "physicalboard") != 0 && strcmp(source, "esp32") != 0) {
//             queue_virtual_move_for_leds(move, fromRow, fromCol, toRow, toCol);
//         } else {
//             Serial.println("Move came from physical board; not queueing LED move.");
//         }

//         return;
//     }

//     // lastMove is useful for debugging, but do not automatically queue LEDs from
//     // lastMove because it is also sent when the ESP32 first connects.
//     if (extract_move_from_json(doc["lastMove"], move, fromRow, fromCol, toRow, toCol)) {
//         Serial.print("Last move in current game: ");
//         Serial.println(move);
//     }
// }

// void handle_legacy_move_message(uint8_t* payload, size_t length, const char* label) {
//     StaticJsonDocument<128> filter;
//     filter["type"] = true;
//     filter["move"] = true;

//     StaticJsonDocument<384> doc;

//     DeserializationError error = deserializeJson(
//         doc,
//         payload,
//         length,
//         DeserializationOption::Filter(filter)
//     );

//     if (error) {
//         Serial.print("Legacy move parse failed: ");
//         Serial.println(error.c_str());
//         return;
//     }

//     const char* move = doc["move"];

//     Serial.print(label);
//     Serial.print(": ");
//     Serial.println(move == nullptr ? "(null)" : move);

//     int fromRow, fromCol, toRow, toCol;

//     if (parse_uci_move(move, fromRow, fromCol, toRow, toCol)) {
//         queue_virtual_move_for_leds(String(move), fromRow, fromCol, toRow, toCol);
//     } else {
//         Serial.println("Invalid move format");
//     }
// }

// void handle_move_rejected_message(uint8_t* payload, size_t length) {
//     StaticJsonDocument<512> filter;

//     filter["type"] = true;
//     filter["reason"] = true;
//     filter["message"] = true;
//     filter["move"] = true;

//     filter["attemptedMessage"]["move"] = true;
//     filter["attemptedMessage"]["from"] = true;
//     filter["attemptedMessage"]["to"] = true;
//     filter["attemptedMessage"]["from_row"] = true;
//     filter["attemptedMessage"]["from_col"] = true;
//     filter["attemptedMessage"]["to_row"] = true;
//     filter["attemptedMessage"]["to_col"] = true;

//     filter["attemptedMove"]["move"] = true;
//     filter["attemptedMove"]["uci"] = true;
//     filter["attemptedMove"]["lan"] = true;
//     filter["attemptedMove"]["from"] = true;
//     filter["attemptedMove"]["to"] = true;

//     DynamicJsonDocument doc(1024);

//     DeserializationError error = deserializeJson(
//         doc,
//         payload,
//         length,
//         DeserializationOption::Filter(filter)
//     );

//     if (error) {
//         Serial.print("move:rejected parse failed: ");
//         Serial.println(error.c_str());
//         return;
//     }

//     const char* reason = doc["reason"];
//     if (reason == nullptr) reason = doc["message"];

//     Serial.print("Move rejected by server");
//     if (reason != nullptr) {
//         Serial.print(": ");
//         Serial.print(reason);
//     }
//     Serial.println();
//     move_rejected = true;

//     String move;
//     int fromRow, fromCol, toRow, toCol;

//     if (extract_move_from_json(doc["attemptedMove"], move, fromRow, fromCol, toRow, toCol) ||
//         extract_move_from_json(doc["attemptedMessage"], move, fromRow, fromCol, toRow, toCol) ||
//         extract_move_from_json(doc["move"], move, fromRow, fromCol, toRow, toCol)) {

//         Serial.print("Rejected move was: ");
//         Serial.println(move);
//     }
// }

// void handle_simple_log_message(uint8_t* payload, size_t length, const char* label) {
//     StaticJsonDocument<128> filter;
//     filter["type"] = true;
//     filter["message"] = true;
//     filter["reason"] = true;

//     StaticJsonDocument<512> doc;

//     DeserializationError error = deserializeJson(
//         doc,
//         payload,
//         length,
//         DeserializationOption::Filter(filter)
//     );

//     if (error) {
//         Serial.print("Simple message parse failed: ");
//         Serial.println(error.c_str());
//         return;
//     }

//     const char* message = doc["message"];
//     if (message == nullptr) message = doc["reason"];

//     Serial.print(label);
//     if (message != nullptr) {
//         Serial.print(": ");
//         Serial.print(message);
//     }
//     Serial.println();
// }

// void handle_server_message(uint8_t* payload, size_t length) {
//     String typeName;

//     if (!parse_message_type(payload, length, typeName)) {
//         return;
//     }

//     if (typeName == "game:state") {
//         handle_game_state_message(payload, length);
//     }

//     else if (typeName == "move:rejected") {
//         handle_move_rejected_message(payload, length);
//     }

//     else if (typeName == "server:hello") {
//         handle_simple_log_message(payload, length, "Server hello");
//     }

//     else if (typeName == "error") {
//         handle_simple_log_message(payload, length, "Server error");
//     }

//     else if (typeName == "game:legalmoves") {
//         // For now, the ESP32 does not need to store the full legal move list.
//         // The browser can use it for highlighting, but the physical board mainly
//         // needs accepted moves and rejections.
//         Serial.println("Legal moves message received; ignoring full move list on ESP32.");
//     }

//     // Backward compatibility with the older server messages.
//     else if (typeName == "move:accepted") {
//         handle_simple_log_message(payload, length, "Move accepted by server");
//     }

//     else if (typeName == "move:virtualboard") {
//         handle_legacy_move_message(payload, length, "Virtual board move received");
//     }

//     else {
//         Serial.print("Unknown server message type: ");
//         Serial.println(typeName);
//     }
// }

// void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
//     switch (type) {
//         case WStype_CONNECTED:
//             Serial.println("WebSocket connected to server");
//             socketConnected = true;

//             webSocket.sendTXT("{\"type\":\"esp32:hello\",\"message\":\"hello from ESP32\"}");
//             break;

//         case WStype_DISCONNECTED:
//             Serial.println("WebSocket disconnected");
//             socketConnected = false;
//             break;

//         case WStype_TEXT:
//             Serial.print("Message from server: ");
//             for (size_t i = 0; i < length; i++) {
//                 Serial.print((char)payload[i]);
//             }
//             Serial.println();

//             handle_server_message(payload, length);
//             break;

//         default:
//             break;
//     }
// }



// void websocket_begin() {
//     webSocket.begin(SERVER_IP, SERVER_PORT, "/");
//     webSocket.onEvent(webSocketEvent);
//     webSocket.setReconnectInterval(5000);

//     Serial.println("WebSocket client started");
// }

// void websocket_loop() {
//     webSocket.loop();
// }

// bool websocket_is_connected() {
//     return socketConnected;
// }

// void send_hello_to_server() {
//     if (!socketConnected) {
//         Serial.println("Cannot send hello: WebSocket not connected");
//         return;
//     }

//     webSocket.sendTXT("{\"type\":\"esp32:hello\",\"message\":\"manual hello from ESP32\"}");
// }
// // helper functions to convert row and col to and from to a full move
// String square_to_chess_notation(int row, int col) {
//     char file = 'a' + col;
//     char rank = '8' - row;

//     String square = "";
//     square += file;
//     square += rank;

//     return square;
// }
// // see above comment
// String move_to_uci(int fromRow, int fromCol, int toRow, int toCol) {
//     return square_to_chess_notation(fromRow, fromCol) +
//            square_to_chess_notation(toRow, toCol);
// }

// void send_board_move(int from_row, int from_col, int to_row, int to_col) {
//     if (!socketConnected) {
//         Serial.println("Cannot send move: WebSocket not connected");
//         return;
//     }

//     String move = move_to_uci(from_row, from_col, to_row, to_col);

//     StaticJsonDocument<384> message;

//     message["type"] = "move:physicalboard";
//     message["source"] = "esp32";
//     message["move"] = move;

//     // Keep the original flat fields because the current server parser accepts them.
//     message["from_row"] = from_row;
//     message["from_col"] = from_col;
//     message["to_row"] = to_row;
//     message["to_col"] = to_col;

//     // Also include a more descriptive nested shape for future server versions.
//     JsonObject from = message.createNestedObject("from");
//     from["row"] = from_row;
//     from["col"] = from_col;
//     from["square"] = square_to_chess_notation(from_row, from_col);

//     JsonObject to = message.createNestedObject("to");
//     to["row"] = to_row;
//     to["col"] = to_col;
//     to["square"] = square_to_chess_notation(to_row, to_col);

//     String output;
//     serializeJson(message, output);

//     Serial.print("Sending physical board move to server: ");
//     Serial.println(output);

//     webSocket.sendTXT(output);
// }


// String get_virtual_move(){
//     return virtual_move;
// }

// void clear_virtual_move(){
//     virtual_move = "";
// }

// void clear_move_rejected(){
//     move_rejected = false;
//     return;
// }

// bool was_move_rejected(){
//     return move_rejected;
// }

#include "chess_wifi.h"
#include "leds.h"

const char* WIFI_SSID = "Cameron's iPhone xs Max (2)";
const char* WIFI_PASS = "lmgtyt13";

// Replace this with your laptop IP while connected to the hotspot
const char* SERVER_URL = "http://172.20.10.12:3000/api/test";

const char* SERVER_IP = "172.20.10.12";
const uint16_t SERVER_PORT = 3000;

// Change this if the physical board is playing black.
// The server uses "w" for white and "b" for black.
const char* PHYSICAL_PLAYER_COLOR = "w";

WebSocketsClient webSocket;
bool socketConnected = false;

String virtual_move = "";
bool virtual_move_was_capture = false;

bool move_rejected = false;
bool game_won = false;
bool game_lost = false;
bool game_draw = false;

String last_status_message = "";
String last_rejected_move = "";
String last_capture_move = "";

// Used to avoid showing the same accepted move twice when the server sends both
// move:accepted and game:state for the same version.
int lastHandledMoveVersion = -1;
int lastHandledGameOverVersion = -1;

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

void queue_virtual_move(String move, bool wasCapture) {
    if (move.length() < 4) {
        return;
    }

    virtual_move = move;
    virtual_move_was_capture = wasCapture;

    if (wasCapture) {
        last_capture_move = move;
    }

    Serial.print("Queued virtual move for physical board: ");
    Serial.print(virtual_move);
    Serial.print(" capture=");
    Serial.println(wasCapture ? "yes" : "no");
}

void print_clean_move_summary(JsonDocument& doc, const String& move, bool wasCapture, const char* source) {
    JsonVariantConst acceptedMove = doc["acceptedMove"];

    const char* color = acceptedMove["color"];
    if (color == nullptr) {
        color = doc["color"];
    }

    const char* san = acceptedMove["san"];
    const char* piece = acceptedMove["piece"];

    Serial.println("----- Accepted move -----");

    Serial.print("Move: ");
    Serial.println(move);

    Serial.print("Source: ");
    Serial.println(source == nullptr ? "unknown" : source);

    Serial.print("Color move: ");
    if (color != nullptr && strcmp(color, "w") == 0) {
        Serial.println("white");
    }
    else if (color != nullptr && strcmp(color, "b") == 0) {
        Serial.println("black");
    }
    else {
        Serial.println("unknown");
    }

    if (san != nullptr) {
        Serial.print("SAN: ");
        Serial.println(san);
    }

    if (piece != nullptr) {
        Serial.print("Piece: ");
        Serial.println(piece);
    }

    Serial.print("Capture: ");
    Serial.println(wasCapture ? "yes" : "no");

    Serial.println("-------------------------");
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

    if (!isGameOver &&
        !isCheckmate &&
        !isDraw &&
        !isStalemate &&
        (statusName == nullptr ||
         (strcmp(statusName, "checkmate") != 0 &&
          strcmp(statusName, "draw") != 0 &&
          strcmp(statusName, "stalemate") != 0))) {
        return;
    }

    if (version >= 0 && version == lastHandledGameOverVersion) {
        return;
    }

    if (version >= 0) {
        lastHandledGameOverVersion = version;
    }

    Serial.println("----- Game over -----");

    Serial.print("Status: ");
    Serial.println(statusName == nullptr ? "unknown" : statusName);

    Serial.print("Reason: ");
    Serial.println(reason == nullptr ? "none" : reason);

    Serial.print("Winner: ");
    Serial.println(winner == nullptr ? "none" : winner);

    Serial.println("---------------------");

    last_status_message = reason == nullptr ? "Game over" : String(reason);

    if (isDraw || isStalemate ||
        (statusName != nullptr &&
         (strcmp(statusName, "draw") == 0 || strcmp(statusName, "stalemate") == 0))) {
        game_draw = true;
        game_won = false;
        game_lost = false;
        show_draw_animation();
        return;
    }

    if (winner != nullptr && strcmp(winner, PHYSICAL_PLAYER_COLOR) == 0) {
        game_won = true;
        game_lost = false;
        game_draw = false;
        show_win_animation();
    }
    else {
        game_won = false;
        game_lost = true;
        game_draw = false;
        show_loss_animation();
    }
}

void handle_accepted_move(JsonDocument& doc, bool directAcceptedMessage) {
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
        Serial.println("Accepted move message did not include a parseable move.");
        handle_game_over_if_needed(doc, version);
        return;
    }

    bool wasCapture = root_was_capture(doc);

    const char* source = doc["source"];
    bool fromPhysicalBoard = is_physical_source(source);

    print_clean_move_summary(doc, move, wasCapture, source);

    if (version >= 0) {
        lastHandledMoveVersion = version;
    }

    if (fromPhysicalBoard) {
        Serial.println("Accepted physical board move; not queueing LEDs.");
        handle_game_over_if_needed(doc, version);
        return;
    }

    queue_virtual_move(move, wasCapture);

    if (wasCapture) {
        show_capture_move(move);
    }
    else {
        show_move(move);
    }

    handle_game_over_if_needed(doc, version);
}

void handle_rejected_move(JsonDocument& doc) {
    const char* reason = doc["reason"];
    if (reason == nullptr) {
        reason = doc["message"];
    }

    Serial.println("----- Move rejected -----");

    Serial.print("Reason: ");
    Serial.println(reason == nullptr ? "unknown reason" : reason);

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
        Serial.print("Rejected move: ");
        Serial.println(rejectedMove);
        last_rejected_move = rejectedMove;
    }

    Serial.println("-------------------------");

    move_rejected = true;
    last_status_message = reason == nullptr ? "Move rejected" : String(reason);

    show_invalid_move();
}

void handle_legacy_virtual_move(JsonDocument& doc) {
    const char* move = doc["move"];

    if (move == nullptr) {
        Serial.println("Virtual board message missing move.");
        return;
    }

    Serial.print("Virtual board move received: ");
    Serial.println(move);

    queue_virtual_move(String(move), false);

    int fromRow, fromCol, toRow, toCol;

    if (parse_uci_move(move, fromRow, fromCol, toRow, toCol)) {
        Serial.printf(
            "Parsed virtual move: from row %i col %i to row %i col %i\r\n",
            fromRow,
            fromCol,
            toRow,
            toCol
        );

        show_move(String(move));
    }
    else {
        Serial.println("Invalid virtual move format.");
    }
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

    return deserializeJson(
        doc,
        payload,
        length,
        DeserializationOption::Filter(filter)
    );
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
        Serial.println("Message missing type.");
        return;
    }

    if (strcmp(type, "move:accepted") == 0) {
        handle_accepted_move(doc, true);
    }
    else if (strcmp(type, "game:state") == 0) {
        if (!doc["acceptedMove"].isNull()) {
            handle_accepted_move(doc, false);
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
        Serial.println("Cannot send move: WebSocket not connected.");
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

    Serial.print("Sending physical board move to server: ");
    Serial.println(output);

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

