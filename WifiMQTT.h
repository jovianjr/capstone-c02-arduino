/*
  WifiMQTT.h
*/
#ifndef WifiMQTT_h
#define WifiMQTT_h

#include "Arduino.h"

class WifiMQTT
{
public:
  WifiMQTT(String ssid, String password, String ip_address, String subnet_mask, String ip_gateway, String mqtt_server, int mqtt_port, String mqtt_topic, String mqtt_client_id);
  void begin();
  void mqttconnect();
  void publish(String message);

private:
  String _ssid, _password, _ip_address, _subnet_mask, _ip_gateway, _mqtt_server, _mqtt_topic, _mqtt_client_id;
  int _mqtt_port;

  void wifiInit();
  void mqttInit();
};

#endif