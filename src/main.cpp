#include <Arduino.h>
#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#define USE_LittleFS

#include <FS.h>
#ifdef USE_LittleFS
  #define SPIFFS LITTLEFS
  #include <LITTLEFS.h>
#else
  #include <SPIFFS.h>
#endif 

#ifdef ESP8266
  #define JSON_BUFFER_SIZE 9216
#else
  #define JSON_BUFFER_SIZE 20480
#endif

StaticJsonDocument<JSON_BUFFER_SIZE> doc;
AsyncWebServer server(80);
// AsyncWebSocket ws("/ws");

const char* ssid = "<ssid>";
const char* password = "<password>";

float jog;
float shooting_speed;
float jog_speed;
float deshake_delay;
float shoot_delay;
float number_images;
float start_point;
float end_point;

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

// void sendDataWs(AsyncWebSocketClient * client)
// {
//   if (!ws.count()) return;
//   AsyncWebSocketMessageBuffer * buffer;

//   { //scope JsonDocument so it releases its buffer
//     #ifdef WLED_USE_DYNAMIC_JSON
//     DynamicJsonDocument doc(JSON_BUFFER_SIZE);
//     #else
//     if (!requestJSONBufferLock(12)) return;
//     #endif
//     JsonObject state = doc.createNestedObject("state");
//     serializeState(state);
//     JsonObject info  = doc.createNestedObject("info");
//     serializeInfo(info);
//     size_t len = measureJson(doc);
//     buffer = ws.makeBuffer(len);
//     if (!buffer) {
//       releaseJSONBufferLock();
//       return; //out of memory
//     }
//     serializeJson(doc, (char *)buffer->get(), len +1);
//     releaseJSONBufferLock();
//   } 
//   if (client) {
//     client->text(buffer);
//   } else {
//     ws.textAll(buffer);
//   }
// }

void initServer() {
    // ws.onEvent(wsEvent);
    // server.addHandler(&ws);
    

    server.onRequestBody([](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total) {
        if (request->url() == "/post") {
            DeserializationError error = deserializeJson(doc, (const char*)data);

            if (error || doc.isNull()) {
                request->send(400, "application/json", F("{\"error\": body missing}"));
                return;
            }
            //* commands *//
            if (doc.containsKey("jog")){
                // jogMotor(doc["jog"]);
            }
            if (doc.containsKey("enable_shooting")){ // true start - abort false
                // isShooting(doc["enable_shooting"]);
            }
            if (doc.containsKey("test_image")){
                // testImage();
            }
            // if (doc.containsKey("start_point")){
            //     start_point = (doc["start_point"]);
            // }
            // if (doc.containsKey("end_point")){
            //     end_point = (doc["end_point"]);
            // }

            //* settings *//
            if (doc.containsKey("number_images")){
                number_images = (doc["number_images"]);
            }
            if (doc.containsKey("shooting_speed")){
                shooting_speed = (doc["shooting_speed"]);
            }
            if (doc.containsKey("jog_speed")){
                jog_speed = (doc["jog_speed"]);
            }
            if (doc.containsKey("deshake_delay")){
                deshake_delay = (doc["deshake_delay"]);
            }
            if (doc.containsKey("shoot_delay")){
                shoot_delay = (doc["shoot_delay"]);
            }


            request->send(200, "application/json", "{\"status\":\"whatever done\"}");
        }
    });

    server.serveStatic("/index.html", SPIFFS, "/index.html");
    server.serveStatic("/", SPIFFS, "/index.html");
    server.onNotFound(notFound);
    server.begin();
}

void setup() {
    // init Serial
    Serial.begin(115200);

    // init FS
    Serial.printf("Mounting FS...");
    if (SPIFFS.begin()) {
        Serial.printf("FS ready");
    }else{
        Serial.printf("Failed to mount file system");
    }

    // init WIFI
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
    }
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // init server
    initServer();
}

void loop() {

}