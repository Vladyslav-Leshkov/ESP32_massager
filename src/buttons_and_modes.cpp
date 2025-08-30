#include <Arduino.h>
#include "buttons_and_modes.h"
#include "config.h"
#include <BLEDevice.h>

// BLE characteristic for voltage notifications
BLECharacteristic *voltage_charc = nullptr;

// Button pin array
const int buttonPins[] = {
    -1,                 // index 0 (not used)
    BUTTON_PIN_1,       // command 1
    BUTTON_PIN_2,       // command 2
    BUTTON_PIN_3,       // command 3
    POWER_BUTTON_PIN_4, // command 4
    BUTTON_PIN_5,       // command 5
    BUTTON_PIN_6,       // command 6
    BUTTON_PIN_7        // command 7
};

// Number of buttons
const int numButtons = 7;

// Interval for voltage checks (ms)
const unsigned long voltageCheckInterval = 1000; // 1 second

// Button modes (1,2,3,7 - Mode:1; 5,6 - OFF; 4 - SLEEP)
volatile int buttonModes[] = {1, 1, 1, 0, 0, 0, 1};

// Device state
volatile bool deviceOn = false;

void printStatus(String status)
{
  Serial.println(status);
  if (deviceOn && voltage_charc)
  {
    voltage_charc->setValue(status.c_str());
    voltage_charc->notify();
  }
}

void updateButtonMode(int buttonIndex)
{
  if (!deviceOn && buttonIndex != 3) // Ignore button presses (except 4) in Device SLEEP mode
    return;

  switch (buttonIndex)
  {
  case 0: // Button 1
  case 2: // Button 3
  case 6: // Button 7
    buttonModes[buttonIndex] = (buttonModes[buttonIndex] % 3) + 1;
    break;
  case 1: // Button 2
    buttonModes[1] = (buttonModes[1] % 3) + 1;
    buttonModes[4] = 0;
    buttonModes[5] = 0;
    break;
  case 4: // Button 5
    if (buttonModes[4] == 0)
    {
      buttonModes[4] = 1;
      buttonModes[5] = 0; // Turn off Button 6
      buttonModes[1] = 0; // Set Button 2 to Mode 0
    }
    else
    {
      buttonModes[4] = 0;
    }
    break;
  case 5: // Button 6
    if (buttonModes[5] == 0)
    {
      buttonModes[5] = 1;
      buttonModes[4] = 0; // Turn off Button 5
      buttonModes[1] = 0; // Set Button 2 to Mode 0
    }
    else
    {
      buttonModes[5] = 0;
    }
    break;
  case 3: // Button 4 (Power)
    buttonModes[3] = !buttonModes[3];
    deviceOn = buttonModes[3];
    if (!deviceOn)
    {
      resetToDefaultModes();
    }
    break;
  }
  sendButtonStatus(buttonIndex, buttonModes[buttonIndex]);
}

void sendButtonStatus(int buttonIndex, bool state)
{
  String status;
  if (buttonIndex == 3 || state)
  {
    status = (buttonIndex == 3) ? "Button 4 Mode: " + String(buttonModes[3] ? "Device ON" : "Device SLEEP")
                                : "Button " + String(buttonIndex + 1) + " Mode: " +
                                      (buttonIndex == 4 || buttonIndex == 5 ? String(buttonModes[buttonIndex] ? "ON" : "OFF") : String(buttonModes[buttonIndex]));
    printStatus(status);
  }
  else if (deviceOn)
  {
    if (buttonIndex == 4 || buttonIndex == 5)
    {
      status = "Button " + String(buttonIndex + 1) + " Mode: " + String(buttonModes[buttonIndex] ? "ON" : "OFF");
    }
    else
    {
      status = "Button " + String(buttonIndex + 1) + " Mode: " + String(buttonModes[buttonIndex]);
    }
    printStatus(status);
  }
}

void resetToDefaultModes()
{
  buttonModes[0] = 1; // Button 1 -> Mode 1
  buttonModes[1] = 1; // Button 2 -> Mode 1
  buttonModes[2] = 1; // Button 3 -> Mode 1
  buttonModes[3] = 0; // Button 4 -> Device SLEEP
  buttonModes[4] = 0; // Button 5 -> OFF
  buttonModes[5] = 0; // Button 6 -> OFF
  buttonModes[6] = 1; // Button 7 -> Mode 1
  deviceOn = false;

  printStatus("Reset to default modes:");
  for (int i = 0; i < numButtons; i++)
  {
    sendButtonStatus(i, true);
  }
}

void activateButton(int pin, int buttonIndex)
{
  digitalWrite(pin, HIGH);
  if (deviceOn)
  {
    printStatus("Button on GPIO " + String(pin) + " activated (HIGH)");
  }
  delay(100);
  digitalWrite(pin, LOW);
  if (deviceOn)
  {
    printStatus("Button on GPIO " + String(pin) + " deactivated (LOW)");
  }
  updateButtonMode(buttonIndex);
}