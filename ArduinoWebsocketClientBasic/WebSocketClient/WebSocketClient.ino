#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <WebSocketsClient.h>

#include <Hash.h>

#include <ArduinoJson.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

#define USE_SERIAL Serial
#define SERIAL_DEBUG true

bool btnState = false;
int buttonState = 0;
String payLoad;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t lenght) {

    switch(type) {
        case WStype_DISCONNECTED:
            if(SERIAL_DEBUG)USE_SERIAL.printf("WS client disconnected.\n");
            break;
        case WStype_CONNECTED:
            if(SERIAL_DEBUG)USE_SERIAL.printf("[WSc] Connected to url: %s\n",  payload);
            break;
        case WStype_TEXT:
            if(SERIAL_DEBUG)USE_SERIAL.printf("[WSc] payload: %s\n", payload);
            payLoad = (char *) payload;
            //handle payload here
            
            break;
    }
}

void setup() {
  if(SERIAL_DEBUG)USE_SERIAL.begin(115200);
  if(SERIAL_DEBUG)USE_SERIAL.setDebugOutput(true);
  if(SERIAL_DEBUG)USE_SERIAL.println();
  if(SERIAL_DEBUG)USE_SERIAL.println();
  if(SERIAL_DEBUG)USE_SERIAL.println();

  DynamicJsonBuffer  jsonBuffer;
  char id[] =
     "{\"array\":[{\"uid\":\"1\",\"type\":\"<http://vocab.peePonic.com/waterPump>\",\"api\":[{\"name\":\"on\",\"bus\":\"gpio\",\"pin\":4,\"direction\":\"OUTPUT\",\"value\":1},{\"name\":\"off\",\"bus\":\"gpio\",\"pin\":4,\"direction\":\"OUTPUT\",\"value\":0}]},{\"uid\":\"2\",\"type\":\"<http://vocab.peePonic.com/waterPump>\",\"api\":[{\"name\":\"on\",\"bus\":\"gpio\",\"pin\":4,\"direction\":\"OUTPUT\",\"value\":1},{\"name\":\"off\",\"bus\":\"gpio\",\"pin\":4,\"direction\":\"OUTPUT\",\"value\":0}]},{\"uid\":\"3\",\"type\":\"<http://vocab.peePonic.com/airPump>\",\"api\":[{\"name\":\"none\"}]},{\"uid\":\"4\",\"type\":\"<http://vocab.peePonic.com/waterSensor>\",\"api\":[{\"name\":\"get\",\"bus\":\"gpio\",\"pin\":\"A0\"}]}]}";
  JsonObject& json = jsonBuffer.parseObject(id);

  // Test if parsing succeeds.
  if (!json.success()) {
    USE_SERIAL.println("parseObject() failed");
    return;
  }

  
      for(uint8_t t = 4; t > 0; t--) {
          if(SERIAL_DEBUG)USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
          if(SERIAL_DEBUG)USE_SERIAL.flush();
          delay(1000);
      }
    
    //How to implement that?? SD Storage?
    WiFiMulti.addAP("wificonred", "95440279");

    //WiFi.disconnect();
    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    webSocket.begin("192.168.0.103", 1880,"/ws/moritz");
    //webSocket.setAuthorization("user", "Password"); // HTTP Basic Authorization
    webSocket.onEvent(webSocketEvent);

    webSocket.sendTXT("I am a D1 mini, my ID is:id1"); //Let Flow know about board type and uid

    // Parse String and Setup Pinmodes 
  //for (int i = 0; json["array"][i]["uid"] != ""; i++) {
     //for(int j = 0; json["array"][i]["api"][j] != ""; j++) {
    String bus = json["array"][0]["api"]["bus"];
    USE_SERIAL.println(bus);
      if(bus.equals("gpio")) {
        int pin = json["array"][0]["api"]["pin"];
        USE_SERIAL.println(pin);
        //String pinmode = json["array"][i]["api"]["pin"];
        
        /*if (pinmode == "OUTPUT") {
          pinMode(pin, OUTPUT);
          USE_SERIAL.println(pinmode);
        }
        if (pinmode == "INPUT") {
          pinMode(pin, INPUT);
          USE_SERIAL.println(pinmode);
        }*/
        
    //  }
     }
  //}


}

void loop() {
    webSocket.loop();
}
