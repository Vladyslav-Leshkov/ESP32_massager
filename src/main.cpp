#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>
#include <EEPROM.h>
#include <soc/rtc.h>
#include <esp_system.h>
#include <rom/ets_sys.h>
#include "global.h"
#include "myWebServer.h"
#include "ota_ap.h"
#include "buttons_and_modes.h"

// // Changeable UUID for QR Code
// String serviceUUID = "3aa82ff0-300d-49ed-8b59-b9a674f843f3"; // "9ba06fee-3aec-4deb-882e-1888fc26ecc4"; // "3ea3d02c-fe45-423a-a09f-43895c7c7c5b"; // "6bc58753-dc17-4c09-9658-68442283ff48";

long bootCount = 0;

void CheckConIntervalProcess();
void CheckTimeBetweenKeepProcess();
void CheckPauseTimeProcess();
void SwitchOff();
void CheckMotorProcess();
void PressButton(int pin, int duration_ms);

BLECharacteristic *
    command_charc;
BLECharacteristic *motor_state_charc;

BLEServer *ble_server;
bool run_permit = false;
uint8_t prev_motor_state = 0;
String command = "";
unsigned long last_keep_time = 0;
unsigned long time_bet_keep = 0;
unsigned long start_con_time = 0;
unsigned long max_con_interval = 30000;
uint16_t conn_id = 0;
bool client_connected = false;
int session_id = 0;
bool session_pause = false;
unsigned long max_pause_time = 60000;
unsigned long start_pause_time = 0;
int eeprom_addres = 0;

float current_mA = 0.0;

unsigned long lastSecond = 0; // Не використовується, можна видалити
unsigned long lastMillisecond = 0;
unsigned long lastMillisec = 0;
unsigned long lastMillis = 0;
unsigned long maxCycle = 0;
unsigned long lastCycle = 0;
unsigned long cycle = 0;
unsigned long mA = 0;
unsigned long mAmA = 0;
unsigned long needPress = 0;

long a1 = 0;
long b1 = 0;
bool needRestart = false;

float myTemperatureRead()
{
    return temperatureRead();
}

int get_session_id(String message)
{
    size_t delimiter_index = message.lastIndexOf(' ');
    if (delimiter_index != -1)
        return message.substring(delimiter_index + 1).toInt();
    else
        return 0;
}

class ServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *server, esp_ble_gatts_cb_param_t *param)
    {
        start_con_time = millis();
        conn_id = server->getConnId();
        client_connected = true;
        EEPROM.write(eeprom_addres, conn_id);
        eeprom_addres++;
        if (eeprom_addres > 32)
            eeprom_addres = 0;
    }
    void onDisconnect(BLEServer *server)
    {
        client_connected = false;
        BLEDevice::startAdvertising();
    }
};

void PressButton(int pin, int duration_ms)
{
    digitalWrite(pin, HIGH);
    delay(duration_ms);
    digitalWrite(pin, LOW);
    // Serial.printf("[BUTTON] Pressed pin %d for %d ms\n", pin, duration_ms);
}

class RestProcessCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *charc)
    {
        String message = String(charc->getValue().c_str());
        Serial.println("[BLE] Received command: " + message);

        int _session_id = get_session_id(message);
        size_t delimiter_index = message.indexOf(' ');
        size_t last_delimiter = message.lastIndexOf(' ');
        String command = (delimiter_index != -1) ? message.substring(0, delimiter_index) : message;

        if (session_id == 0 || session_id == _session_id)
        {
            if (command == "RUN")
            {
                if (delimiter_index != -1 && last_delimiter != -1)
                {
                    if (!run_permit && !IsMotorRun())
                    {
                        digitalWrite(POWER_BUTTON_PIN, HIGH); // HIGE = unlock
                        Serial.println("POWER_BUTTON_PIN, HIGH");
                        delay(100);
                        PressButton(BUTTON_START_PIN_4, 100);
                        Serial.println("PIN_4, 100");
                    }

                    run_permit = true;
                    size_t length = (delimiter_index == last_delimiter) ? message.length() : last_delimiter - delimiter_index - 1;
                    // Увага! Костиль для тестування!
                    // Зміна: множимо отриманий час на 2, щоб подвоїти інтервал для KEEPALIVE (наприклад, 60 секунд стають 120 секундами)
                    time_bet_keep = (unsigned long)(message.substring(delimiter_index + 1, delimiter_index + length + 1).toInt() * 1000 * 2);
                    last_keep_time = millis();
                    session_id = _session_id;
                    // digitalWrite(BoardLedPin, HIGH);
                    command_charc->setValue("0");
                    Serial.println("[BLE] Sent to client: 0 (RUN success)");
                }
                else
                {
                    command_charc->setValue("1");
                    Serial.println("[BLE] Sent to client: 1 (RUN invalid format)");
                }
                command = "";
            }
            else if (command == "STOP")
            {
                if (run_permit && IsMotorRun())
                {
                    Serial.println("Pressing START button to stop motor...");
                    PressButton(BUTTON_START_PIN_4, 100);
                    delay(100);
                    Serial.println("Sending POWER OFF...");
                    digitalWrite(POWER_BUTTON_PIN, LOW);
                }

                run_permit = false;
                session_id = 0;
                // digitalWrite(BoardLedPin, LOW);
                command_charc->setValue("0");
                Serial.println("[BLE] Sent to client: 0 (STOP success)");
                command = "";
            }
            else if (command == "KEEPALIVE")
            {
                last_keep_time = millis();
                command_charc->setValue("0");
                Serial.println("[BLE] Sent to client: 0 (KEEPALIVE success)");
            }
            else if (command == "STATUS")
            {
                Serial.println("STATUS???");
                uint8_t session_state = run_permit ? 1 : 0;
                char session_state_str[2];
                snprintf(session_state_str, sizeof(session_state_str), "%d", session_state);
                command_charc->setValue(session_state_str);
                Serial.printf("[BLE] Sent to client: %s (STATUS response)\n", session_state_str);
            }
            else if (command == "UPDATE")
            {
                WebServerSetup();
                WebServerLoop();
            }
            else if (command == "PAUSE")
            {
                Serial.println("PAUSE???");
                if (delimiter_index != -1 && last_delimiter != -1)
                {
                    size_t length = (delimiter_index == last_delimiter) ? message.length() : last_delimiter - delimiter_index - 1;
                    max_pause_time = (unsigned long)(message.substring(delimiter_index + 1, delimiter_index + length + 1).toInt() * 1000);
                    if (max_pause_time > 0)
                    {
                        if (!session_pause)
                        {
                            Serial.println("[PAUSE] Activating pause");
                            PressButton(BUTTON_START_PIN_4, 100);
                            delay(100);

                            // Checking if the motor is turned off
                            if (IsMotorRun() == 0)
                            {
                                digitalWrite(POWER_BUTTON_PIN, LOW); // LOW = lock
                                Serial.println("Blocked");
                            }
                        }
                        session_pause = true;
                        start_pause_time = millis();
                    }
                    else
                    {
                        if (session_pause)
                        {
                            digitalWrite(POWER_BUTTON_PIN, HIGH);
                            Serial.println("POWER_BUTTON_PIN, HIGH");
                            delay(50);
                            PressButton(BUTTON_START_PIN_4, 100);
                            Serial.println("[PAUSE] Resumed from pause");
                        }
                        last_keep_time = millis();
                        session_pause = false;
                        run_permit = true;
                        Serial.println("[PAUSE] run_permit set to true");
                    }
                    command_charc->setValue("0");
                    Serial.println("[BLE] Sent to client: 0 (PAUSE success)");
                }
                else
                {
                    command_charc->setValue("1");
                    Serial.println("[BLE] Sent to client: 1 (PAUSE invalid format)");
                }
                command = "";
            }
            else if (command == "LOGS")
            {
                String logs_str;
                for (int i = 0; i < 32; i++)
                {
                    logs_str += String(EEPROM.read(i)) + String(" ");
                }
                command_charc->setValue(logs_str.c_str());
                Serial.printf("[BLE] Sent to client: %s (LOGS response)\n", logs_str.c_str());
            }
            else
            {
                command_charc->setValue("2");
                Serial.println("[BLE] Sent to client: 2 (Unknown command)");
            }
        }
        else
        {
            command_charc->setValue("1");
            Serial.println("[BLE] Sent to client: 1 (Invalid session ID)");
        }
    }
};

void GPIO_Init()
{
    // pinMode(BoardLedPin, OUTPUT);
    pinMode(MotorControlPin, INPUT);
    // pinMode(PowerRelayPin, OUTPUT);
    // digitalWrite(PowerRelayPin, HIGH);
    pinMode(POWER_BUTTON_PIN, OUTPUT);
    digitalWrite(POWER_BUTTON_PIN, LOW);
    pinMode(BUTTON_PIN_1, OUTPUT);
    digitalWrite(BUTTON_PIN_1, LOW);
    pinMode(BUTTON_PIN_2, OUTPUT);
    digitalWrite(BUTTON_PIN_2, LOW);
    pinMode(BUTTON_PIN_3, OUTPUT);
    digitalWrite(BUTTON_PIN_3, LOW);
    pinMode(BUTTON_START_PIN_4, OUTPUT);
    digitalWrite(BUTTON_START_PIN_4, LOW);
    pinMode(BUTTON_PIN_5, OUTPUT);
    digitalWrite(BUTTON_PIN_5, LOW);
    pinMode(BUTTON_PIN_6, OUTPUT);
    digitalWrite(BUTTON_PIN_6, LOW);
    pinMode(BUTTON_PIN_7, OUTPUT);
    digitalWrite(BUTTON_PIN_7, LOW);
}

void BLE_Init()
{
    Serial.begin(9600);
    BLEDevice::init("RestNow_BLE");
    // create server
    ble_server = BLEDevice::createServer();
    ble_server->setCallbacks(new ServerCallbacks());
    // create service
    BLEService *service = ble_server->createService(SERVICE_UUID);
    // create command characteristic
    command_charc = service->createCharacteristic(COMMAND_UUID, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ);
    command_charc->setCallbacks(new RestProcessCallbacks());
    // create motor_state characteristic
    motor_state_charc = service->createCharacteristic(MOTOR_STATE_UUID, BLECharacteristic::PROPERTY_NOTIFY);
    motor_state_charc->addDescriptor(new BLE2902());
    // Start the service
    service->start();
    // Start advertising
    BLEAdvertising *advertising = BLEDevice::getAdvertising();
    // advertising->addServiceUUID(ADVERTISING_UUID);
    // BLEUUID bleUUID(serviceUUID);       // Convert String to BLEUUID
    // BLEUUID bleUUID(serviceUUID.c_str());
    BLEUUID bleUUID(serviceUUID.c_str());
    advertising->addServiceUUID(bleUUID);
    advertising->setMinPreferred(0x12);
    advertising->start();
    Serial.println("BLE Advertising started!");
}

uint8_t IsMotorRun()
{
    static unsigned long lastVoltageCheck = 0;
    const unsigned long voltageCheckInterval = 3000;
    long sum = 0;

    for (int i = 0; i < 50; i++)
    {
        sum += analogRead(MotorControlPin);
        delay(1);
    }
    float voltage = (sum / 50.0) * (3.3 / 4095.0);

    if (millis() - lastVoltageCheck >= voltageCheckInterval)
    {
        Serial.print("GPIO36 voltage: ");
        Serial.println(voltage, 2);
        lastVoltageCheck = millis();
    }

    bool newDeviceOn = (voltage >= 0.09 && voltage <= 1.84); // <= 1.85
    if (newDeviceOn != deviceOn)
    {
        buttonModes[3] = newDeviceOn;
        deviceOn = newDeviceOn;
        if (!deviceOn)
        {
            // resetToDefaultModes();
        }
        else
        {
            // sendButtonStatus(3, true);
        }
    }

    return newDeviceOn ? 1 : 0;
}

IRAM_ATTR void isr1()
{
    if ((REG_READ(GPIO_IN_REG) >> 1) & 0x1)
    {
        if (0 == a1)
            a1 = millis();
    }
    else if (0 == b1)
        b1 = millis();
}

void setup()
{
    setCpuFrequencyMhz(80);
    attachInterrupt(1, isr1, CHANGE);
    GPIO_Init();
    BLE_Init();
    EEPROM.begin(64);
    WebServerSetup();

    uint32_t *rtc_mem = (uint32_t *)(0x50000000);
    bootCount = *rtc_mem;
    bootCount++;
    *rtc_mem = bootCount;
}

void loop()
{
    if (1000 <= (millis() - lastMillis))
    {
        lastMillis = millis();
        lastCycle = cycle;
        if (maxCycle < cycle)
            maxCycle = cycle;
        cycle = 0;
    }

    if (100 <= (millis() - lastMillisec))
    {
        lastMillisec = millis();
        WebServerLoop();
        CheckTimeBetweenKeepProcess();
        CheckPauseTimeProcess();
        CheckMotorProcess();
        if (needRestart)
            ESP.restart();
    }

    if (1 <= (millis() - lastMillisecond))
    {
        lastMillisecond = millis();
        cycle++;
        if (3000 < (millis() - a1) && (a1 != 0))
            a1 = 0;
        if (3000 < (millis() - b1) && (b1 != 0))
            b1 = 0;
    }

    if (0 == needPress)
        needPress = 0;
    else if (1 == needPress)
        needPress = millis();
    else if (100 > (millis() - needPress))
        digitalWrite(BUTTON_START_PIN_4, HIGH);
    else if (200 > (millis() - needPress))
        digitalWrite(BUTTON_START_PIN_4, LOW);
    else
    {
        needPress = 0;
    }
}

void CheckConIntervalProcess()
{
    if (client_connected)
    {
        if (millis() - start_con_time > max_con_interval)
        {
            ble_server->disconnect(conn_id);
        }
    }
}

void CheckTimeBetweenKeepProcess()
{
    if (run_permit)
    {
        if (millis() - last_keep_time > time_bet_keep)
        {
            if (session_pause == false)
            {
                run_permit = false;
                // digitalWrite(BoardLedPin, LOW);
            }
        }
    }
}

void CheckPauseTimeProcess()
{
    if (session_pause)
    {
        if (millis() - start_pause_time > max_pause_time)
        {
            session_pause = false;
            run_permit = true;
            digitalWrite(POWER_BUTTON_PIN, HIGH);
            Serial.println("POWER_BUTTON_PIN, HIGH");
            delay(50);
            PressButton(BUTTON_START_PIN_4, 100);
            Serial.println("[PAUSE] Resumed from pause, run_permit set to true");
        }
    }
}

void SwitchOff()
{
    Serial.println("SwitchOff???");
    if (0 == needPress)
    {
        session_id = 0;
        PressButton(BUTTON_START_PIN_4, 100);
        Serial.println("SwitchOff = POWER OFF");
        delay(100);

        // Checking if the motor is turned off
        if (IsMotorRun() == 0)
        {
            digitalWrite(POWER_BUTTON_PIN, LOW);
            Serial.println("Blocked");
        }
    }
}

void CheckMotorProcess()
{
    uint8_t motor_state = IsMotorRun();

    if (prev_motor_state != motor_state)
    {
        if (motor_state_charc != nullptr && ble_server->getConnectedCount() > 0)
        {
            char motor_state_str[2];
            snprintf(motor_state_str, sizeof(motor_state_str), "%d", motor_state);

            motor_state_charc->setValue(motor_state_str);
            motor_state_charc->notify();
            Serial.printf("[BLE] motor_state_charc notified with value: %s\n", motor_state_str);
        }
        else
        {
            Serial.println("[WARN] motor_state_charc is nullptr or no BLE client connected");
        }

        prev_motor_state = motor_state;
    }
    if (motor_state == 1 && (!run_permit || session_pause))
    {
        SwitchOff();
    }
}

// Прошу пробачення за ненайкращий вигляд цього коду, але без належної оплати за мій труд я код покращувати не буду.
// P.S. Вся моя робота відповідає всім вимогам замовника і навіть більше, я і так зробив набагато більше ніж мав зробити.