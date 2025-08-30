#ifndef CONFIG_H
#define CONFIG_H

#include "ota_ap.h"

// Changeable UUID for QR Code
static const String serviceUUID = "71f5f50c-5d89-4442-9cc8-800b2c7da72c"; //"3aa82ff0-300d-49ed-8b59-b9a674f843f3";

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"     // const
#define COMMAND_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"     // const
#define MOTOR_STATE_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E" // const

// Wi-Fi Network Credentials (Ваша Wi-Fi мережа до якої підключається ESP32)
#define WIFI_SSID "YourWiFiNetwork"
#define WIFI_PASSWORD "YourWiFiPassword"

// Wi-Fi Access Point (Точка доступу яку створює ESP32, якщо не вдається підключитися до мережі)
#define AP_SSID "RestNow"
#define AP_PASSWORD "12344321"

// Wi-Fi Timeouts
#define WIFI_CONNECT_TIMEOUT 10000 // Timeout for Wi-Fi connection attempt (10 Seconds)
#define WIFI_AP_TIMEOUT 40000      // Duration to keep Wi-Fi AP active (40 Seconds)

// Other Configuration
#define MAX_CON_INTERVAL 62000 // Max connection time for BLE   //  30000 - is Old version
#define MAX_PAUSE_TIME 60000   // Max PAUSE time for session
#define EEPROM_SIZE 64         // EEPROM size for data storage (bytes)

// Pins configuration
#define LOCK_BUTTON_PIN 23
#define BUTTON_PIN_1 27
#define BUTTON_PIN_2 32
#define BUTTON_PIN_3 33
#define POWER_BUTTON_PIN_4 25 // ON/OFF
#define BUTTON_PIN_5 26
#define BUTTON_PIN_6 13
#define BUTTON_PIN_7 14
#define MOTOR_CONTROL_PIN 36 // Voltage Control

#endif // CONFIG_H