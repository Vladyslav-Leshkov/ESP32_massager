#ifndef MYWEBSERVER_H
#define MYWEBSERVER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <AsyncTCP.h>
#include <pgmspace.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <Update.h>
#include "global.h"

// WiFi AP configuration
extern IPAddress ap_ip;
extern IPAddress ap_mask;
extern AsyncWebServer server;
extern DNSServer dnsServer;

// HTML constants
extern const char *login_html;
extern const char *server_html;
extern float myTemperatureRead();

// Function declarations
void onJavaScript(AsyncWebServerRequest *request);
void handleNotFound(AsyncWebServerRequest *request);
void handleGoogle(AsyncWebServerRequest *request);
void handleIn(AsyncWebServerRequest *request);
void handleOn(AsyncWebServerRequest *request);
void handle1(AsyncWebServerRequest *request);
void handle2(AsyncWebServerRequest *request);
void handle3(AsyncWebServerRequest *request);
void handle4(AsyncWebServerRequest *request);
void handle5(AsyncWebServerRequest *request);
void handle6(AsyncWebServerRequest *request);
void handle7(AsyncWebServerRequest *request);
void handle8(AsyncWebServerRequest *request);
void handleRestart(AsyncWebServerRequest *request);
void WebServerSetup();
void WebServerRun();
void WebServerLoop();

#endif