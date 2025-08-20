#ifndef _GLOBAL
#define _GLOBAL

#include "ota_ap.h"

// Changeable UUID for QR Code
static const String serviceUUID = "3aa82ff0-300d-49ed-8b59-b9a674f843f3"; // "9ba06fee-3aec-4deb-882e-1888fc26ecc4"; // "3ea3d02c-fe45-423a-a09f-43895c7c7c5b"; // "6bc58753-dc17-4c09-9658-68442283ff48";

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"     // const
#define COMMAND_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"     // const
#define MOTOR_STATE_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E" // const

// #define BoardLedPin 2
#define MotorControlPin 36
#define PowerRelayPin 15

#define POWER_BUTTON_PIN 23
#define BUTTON_PIN_1 27
#define BUTTON_PIN_2 32
#define BUTTON_PIN_3 33
#define BUTTON_START_PIN_4 25
#define BUTTON_PIN_5 26
#define BUTTON_PIN_6 13
#define BUTTON_PIN_7 14

uint8_t IsMotorRun();

// For Web-Server
extern unsigned long needPress;
extern bool run_permit;
extern unsigned long lastCycle;
extern unsigned long maxCycle;
extern long bootCount;
extern bool needRestart;

// JavaScript file in PROGMEM (declaration, initialization in ota_ap.h)
extern const char jquery_min_js_v3_2_1_gz[] PROGMEM;
extern const size_t jquery_min_js_v3_2_1_gz_len;
extern float myTemperatureRead();

#endif