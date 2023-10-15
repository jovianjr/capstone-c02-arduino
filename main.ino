#include "MQSensor.h"
#include "WifiMQTT.h"
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// def pin
#define PIN_FAN (5)
#define PIN_WATER_PUMP (23)
#define PIN_LED_GREEN (27)
#define PIN_LED_YELLOW (26)
#define PIN_LED_RED (25)

#define PIN_MQ2 (35)
#define PIN_MQ135 (34)

// def const wifi
const String ssid = "Pocky";
const String password = "empekempek123";
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

    wifimqtt.begin();
    mqsensor.begin();

    // set all to low
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_YELLOW, LOW);
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_FAN, LOW);
    digitalWrite(PIN_WATER_PUMP, LOW);
}

void loop()
{
    wifimqtt.mqttconnect();

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
    if (SMOKE_LEVEL < THRESHOLD_SMOKE)
    {
        if (AMMONIUM_LEVEL < THRESHOLD_AMMONIUM_GREEN)
        {
            digitalWrite(PIN_LED_GREEN, HIGH);
        }
        else if (AMMONIUM_LEVEL < THRESHOLD_AMMONIUM_YELLOW)
        {
            digitalWrite(PIN_LED_YELLOW, HIGH);
        }
        else
        {
            digitalWrite(PIN_LED_RED, HIGH);
        }
    }
    else
    {
        digitalWrite(PIN_LED_RED, HIGH);
    }
}