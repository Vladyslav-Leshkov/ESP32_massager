#ifndef BUTTONS_AND_MODES_H
#define BUTTONS_AND_MODES_H

// Forward declaration for BLECharacteristic
class BLECharacteristic;

// External variables
extern volatile int buttonModes[];
extern volatile bool deviceOn;
extern const int buttonPins[];
extern BLECharacteristic *voltage_charc;

// Function prototypes
void updateButtonMode(int buttonIndex);
void sendButtonStatus(int buttonIndex, bool state = false);
void resetToDefaultModes();
void activateButton(int pin, int buttonIndex);
void printStatus(String status);

#endif