#include "MQSensor.h"
#include "WifiMQTT.h"
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define ON HIGH
#define OFF LOW
#define ON_RELAY LOW
#define OFF_RELAY HIGH

// def pin
#define PIN_FAN (18)
#define PIN_WATER_PUMP (19)
#define PIN_LED_GREEN (25)
#define PIN_LED_YELLOW (26)
#define PIN_LED_RED (27)

#define PIN_MQ2 (35)
#define PIN_MQ135 (34)

// def const wifi
const String ssid = "wifi_name";
const String password = "wifi_password";
const String IP = "192.168.1.97";
const String subnetmask = "255.255.255.0";
const String ipgateway = "192.168.1.1";
const String mqtt_server = "192.168.1.6";
const int mqtt_port = 1883;
const String mqtt_topic = "capstone-c02";
const String mqtt_client_id = "CPSTN-02";

// def lib
MQSensor mqsensor(PIN_MQ2, PIN_MQ135);
WifiMQTT wifimqtt(ssid, password, IP, subnetmask, ipgateway, mqtt_server, mqtt_port, mqtt_topic, mqtt_client_id);

// def const treshold
const int THRESHOLD_AMMONIUM_GREEN = 25;
const int THRESHOLD_AMMONIUM_YELLOW = 90;
const int THRESHOLD_SMOKE = 600;

// globals
const int intervals = 3000; // interval publishing mqtt
unsigned long lastTimer;    // timer
float SMOKE_LEVEL;
float AMMONIUM_LEVEL;

void setup()
{
    Serial.begin(115200);
    pinMode(PIN_WATER_PUMP, OUTPUT);
    pinMode(PIN_FAN, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_YELLOW, OUTPUT);
    pinMode(PIN_LED_RED, OUTPUT);

    // set all to off
    digitalWrite(PIN_LED_GREEN, OFF);
    digitalWrite(PIN_LED_YELLOW, OFF);
    digitalWrite(PIN_LED_RED, OFF);
    digitalWrite(PIN_FAN, OFF_RELAY);
    digitalWrite(PIN_WATER_PUMP, OFF_RELAY);

    // wifimqtt.begin();
    mqsensor.begin();
}

void loop()
{
    // wifimqtt.mqttconnect();

    SMOKE_LEVEL = mqsensor.GetSmokeLevel();
    AMMONIUM_LEVEL = mqsensor.GetAmmoniumLevel();

    if (millis() - lastTimer >= intervals)
    {
        String message = mqtt_client_id + "," + String(SMOKE_LEVEL) + "," + String(AMMONIUM_LEVEL);
        wifimqtt.publish(message);
        lastTimer = millis();
    }

    CheckTreshold();
    Serial.print("SMOKE:" + String(SMOKE_LEVEL) + "ppm    AMMONIUM:" + String(AMMONIUM_LEVEL) + "ppm\n");
    delay(200);
}

void CheckTreshold()
{
    if (SMOKE_LEVEL >= THRESHOLD_SMOKE)
    {
        digitalWrite(PIN_LED_RED, ON);
        digitalWrite(PIN_FAN, ON_RELAY);
        digitalWrite(PIN_WATER_PUMP, ON_RELAY);
    }
    else
    {
        digitalWrite(PIN_WATER_PUMP, OFF_RELAY);

        if (AMMONIUM_LEVEL > THRESHOLD_AMMONIUM_YELLOW)
        {
            digitalWrite(PIN_LED_RED, ON);
            digitalWrite(PIN_LED_YELLOW, OFF);
            digitalWrite(PIN_LED_GREEN, OFF);
            digitalWrite(PIN_FAN, ON_RELAY);
        }
        else if (AMMONIUM_LEVEL > THRESHOLD_AMMONIUM_GREEN)
        {
            digitalWrite(PIN_LED_YELLOW, ON);
            digitalWrite(PIN_LED_RED, OFF);
            digitalWrite(PIN_LED_GREEN, OFF);
            digitalWrite(PIN_FAN, ON_RELAY);
        }
        else
        {
            digitalWrite(PIN_LED_GREEN, ON);
            digitalWrite(PIN_LED_YELLOW, OFF);
            digitalWrite(PIN_LED_RED, OFF);
            digitalWrite(PIN_FAN, OFF_RELAY);
        }
    }
}
