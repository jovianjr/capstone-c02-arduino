/*
  MQSensor.h
*/
#ifndef MQSensor_h
#define MQSensor_h

#include "Arduino.h"

class MQSensor
{
public:
  MQSensor(int pin_mq2, int pin_mq135);
  void begin();
  float GetSmokeLevel();
  float GetAmmoniumLevel();

private:
  int _pin_mq2;
  int _pin_mq135;
  float MQResistanceCalculation_MQ2(int raw_adc);
  float MQResistanceCalculation_MQ135(int raw_adc);
  float MQCalibration_MQ2(int mq_pin);
  float MQCalibration_MQ135(int mq_pin);
  float MQRead_MQ2(int mq_pin);
  float MQRead_MQ135(int mq_pin);
  int MQGetGasPercentage_MQ2(float rs_ro_ratio, int gas_id);
  int MQGetGasPercentage_MQ135(float rs_ro_ratio, int gas_id);
};

#endif