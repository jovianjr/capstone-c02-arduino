/*
  WifiMQTT.cpp
*/

#include "Arduino.h"
#include "WifiMQTT.h"
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

IPAddress local_IP;
IPAddress subnet;
IPAddress gateway;

WifiMQTT::WifiMQTT(String ssid, String password, String ip_address, String subnet_mask, String ip_gateway, String mqtt_server, int mqtt_port, String mqtt_topic, String mqtt_client_id)
{
    _ssid = ssid;
    _password = password;

    _ip_address = ip_address;
    _subnet_mask = subnet_mask;
    _ip_gateway = ip_gateway;

    _mqtt_server = mqtt_server;
    _mqtt_port = mqtt_port;
    _mqtt_topic = mqtt_topic;
    _mqtt_client_id = mqtt_client_id;
}

void WifiMQTT::begin()
{
    local_IP.fromString(_ip_address);
    subnet.fromString(_subnet_mask);
    gateway.fromString(_ip_gateway);

    wifiInit();
    mqttInit();
}

void WifiMQTT::wifiInit()
{
    Serial.println();
    Serial.println("Connecting to " + String(_ssid));
    // Configures static IP address
    if (!WiFi.config(local_IP, gateway, subnet))
    {
        Serial.println("STA Failed to configure");
    }
    WiFi.begin(_ssid, _password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: " + WiFi.localIP().toString());
}

void WifiMQTT::mqttconnect()
{
    /* Loop until reconnected */
    while (!client.connected())
    {
        Serial.print("MQTT connecting ...");
        if (client.connect(_mqtt_client_id.c_str()))
        {
            Serial.println("Connected");
        }
        else
        {
            Serial.print("failed, status code =");
            Serial.print(client.state());
            Serial.println("try again in 5 seconds");
            delay(5000);
        }
    }
}

void WifiMQTT::mqttInit()
{
    client.setServer(_mqtt_server.c_str(), _mqtt_port);
}

void WifiMQTT::publish(String message)
{
    client.publish(_mqtt_topic.c_str(), message.c_str());
}