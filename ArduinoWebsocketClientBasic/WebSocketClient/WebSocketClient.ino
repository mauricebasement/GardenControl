/*
 * WebSocketClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <WebSocketsClient.h>

#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;


#define USE_SERIAL Serial

bool btnState = false;
bool online = false;
int buttonState = 0;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t lenght) {

    switch(type) {
        case WStype_DISCONNECTED:
            if (online) {
                USE_SERIAL.printf("WS client disconnected.\n");
                online = false;
            }
            break;
        case WStype_CONNECTED:
            if (!online) {
               USE_SERIAL.printf("[WSc] Connected to url: %s\n",  payload);
               online = false;
            }
            break;
        case WStype_TEXT:
            String pl = (char *) payload;
            if (pl == "led_on") digitalWrite(D3, 1);
            if (pl == "led_off") digitalWrite(D3, 0);
            /*if (ledOn == payload) {
                // turn led on
                digitalWrite(D3, 1);
                USE_SERIAL.printf("[WSc] LED On!!!%s\n");
            } if (ledOff == payload) {
                // turn led off
                digitalWrite(D3, 0);
            }*/
            USE_SERIAL.printf("[WSc] payload: %s\n", payload);
            
			      // send message to server
          	//webSocket.sendTXT("connect josh");
            break;
        //case WStype_BIN:
            //USE_SERIAL.printf("[WSc] get binary lenght: %u\n", lenght);
            //hexdump(payload, lenght);
            // send data to server
            // webSocket.sendBIN(payload, lenght);
            //break;
    }

}

void setup() {

    pinMode(D7, INPUT);
    pinMode(D3, OUTPUT);
  
    // USE_SERIAL.begin(921600);
    USE_SERIAL.begin(115200);

    //Serial.setDebugOutput(true);
    USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

      for(uint8_t t = 4; t > 0; t--) {
          USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
          USE_SERIAL.flush();
          delay(1000);
      }

    WiFiMulti.addAP("wificonred", "95440279");

    //WiFi.disconnect();
    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }

    webSocket.begin("192.168.0.103", 1880,"/ws/moritz");
    //webSocket.setAuthorization("user", "Password"); // HTTP Basic Authorization
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    webSocket.loop();

    buttonState = digitalRead(D7);
 
    // send button state to websocket
    if ( btnState != buttonState ) {
      if (buttonState == HIGH) {
        webSocket.sendTXT("led_on");
        btnState = buttonState;
      } else {
        webSocket.sendTXT("led_off");
        btnState = buttonState;
      }
    }
}
