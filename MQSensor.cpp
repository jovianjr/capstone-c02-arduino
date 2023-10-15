/*
  MQSensor.cpp
*/

#include "Arduino.h"
#include "MQSensor.h"

/************************mq2sensor************************************/
/************************Hardware Related Macros************************************/
#define RL_VALUE_MQ2 (1)                // define the load resistance on the board, in kilo ohms
#define RO_CLEAN_AIR_FACTOR_MQ2 (9.577) // RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,
// which is derived from the chart in datasheet
#define RL_VALUE_MQ135 (1) // define the load resistance on the board, in kilo ohms
#define RO_CLEAN_AIR_FACTOR_MQ135 (3.59)

/***********************Software Related Macros************************************/
#define CALIBARAION_SAMPLE_TIMES (50)     // define how many samples you are going to take in the calibration phase
#define CALIBRATION_SAMPLE_INTERVAL (500) // define the time interal(in milisecond) between each samples in the
                                          // cablibration phase
#define READ_SAMPLE_INTERVAL (50)         // define how many samples you are going to take in normal operation
#define READ_SAMPLE_TIMES (5)             // define the time interal(in milisecond) between each samples in
                                          // normal operation

/**********************Application Related Macros**********************************/
#define GAS_SMOKE (5)
#define GAS_AMMONIUM (10)
#define accuracy_MQ2 (0)   // for linearcurves
#define accuracy_MQ135 (1) // for nonlinearcurves, un comment this line and comment the above line if calculations
                           // are to be done using non linear curve equations
/*****************************Globals************************************************/
float Ro_MQ2 = 0; // Ro is initialized to 10 kilo ohms
float Ro_MQ135 = 10;

MQSensor::MQSensor(int pin_mq2, int pin_mq135)
{
    _pin_mq2 = pin_mq2;
    _pin_mq135 = pin_mq135;
}

void MQSensor::begin()
{
    Serial.print("Calibrating...\n");
    Ro_MQ2 = MQCalibration_MQ2(_pin_mq2); // Calibrating the sensor. Please make sure the sensor is in clean air when you perform the calibration
    Ro_MQ135 = MQCalibration_MQ135(_pin_mq135);
    Serial.print("Calibration is done...\n");
    Serial.print("Ro MQ2=" + String(Ro_MQ2) + "kohm\n");
    Serial.print("Ro MQ135=" + String(Ro_MQ135) + "kohm \n");
}

float MQSensor::GetSmokeLevel()
{
    return MQGetGasPercentage_MQ2(MQRead_MQ2(_pin_mq2) / Ro_MQ2, GAS_SMOKE);
}

float MQSensor::GetAmmoniumLevel()
{
    return MQGetGasPercentage_MQ135(MQRead_MQ135(_pin_mq135) / Ro_MQ135, GAS_AMMONIUM);
}

/****************** MQResistanceCalculation ****************************************
Input:   raw_adc - raw value read from adc, which represents the voltage
Output:  the calculated sensor resistance
Remarks: The sensor and the load resistor forms a voltage divider. Given the voltage
         across the load resistor and its resistance, the resistance of the sensor
         could be derived.
************************************************************************************/
float MQSensor::MQResistanceCalculation_MQ2(int raw_adc)
{
    return (((float)RL_VALUE_MQ2 * (1023 - raw_adc) / raw_adc));
}

float MQSensor::MQResistanceCalculation_MQ135(int raw_adc)
{
    return (((float)RL_VALUE_MQ135 * (1023 - raw_adc) / raw_adc));
}
/***************************** MQCalibration ****************************************
Input:   mq_pin - analog channel
Output:  Ro of the sensor
Remarks: This function assumes that the sensor is in clean air. It use
         MQResistanceCalculation to calculates the sensor resistance in clean air
         and then divides it with RO_CLEAN_AIR_FACTOR. RO_CLEAN_AIR_FACTOR is about
         10, which differs slightly between different sensors.
************************************************************************************/
float MQSensor::MQCalibration_MQ2(int mq_pin)
{
    int i;
    float RS_AIR_val = 0, r0;

    for (i = 0; i < CALIBARAION_SAMPLE_TIMES; i++)
    { // take multiple samples
        RS_AIR_val += MQResistanceCalculation_MQ2(analogRead(mq_pin));
        delay(CALIBRATION_SAMPLE_INTERVAL);
    }
    RS_AIR_val = RS_AIR_val / CALIBARAION_SAMPLE_TIMES; // calculate the average value

    r0 = RS_AIR_val / RO_CLEAN_AIR_FACTOR_MQ2; // RS_AIR_val divided by RO_CLEAN_AIR_FACTOR yields the Ro
                                               // according to the chart in the datasheet

    return r0;
}

float MQSensor::MQCalibration_MQ135(int mq_pin)
{
    int i;
    float RS_AIR_val = 0, r0;

    for (i = 0; i < CALIBARAION_SAMPLE_TIMES; i++)
    { // take multiple samples
        RS_AIR_val += MQResistanceCalculation_MQ135(analogRead(mq_pin));
        delay(CALIBRATION_SAMPLE_INTERVAL);
    }
    RS_AIR_val = RS_AIR_val / CALIBARAION_SAMPLE_TIMES; // calculate the average value

    r0 = RS_AIR_val / RO_CLEAN_AIR_FACTOR_MQ135; // RS_AIR_val divided by RO_CLEAN_AIR_FACTOR yields the Ro
                                                 // according to the chart in the datasheet

    return r0;
}

/*****************************  MQRead *********************************************
Input:   mq_pin - analog channel
Output:  Rs of the sensor
Remarks: This function use MQResistanceCalculation to caculate the sensor resistenc (Rs).
         The Rs changes as the sensor is in the different consentration of the target
         gas. The sample times and the time interval between samples could be configured
         by changing the definition of the macros.
************************************************************************************/
float MQSensor::MQRead_MQ2(int mq_pin)
{
    int i;
    float rs = 0;

    for (i = 0; i < READ_SAMPLE_TIMES; i++)
    {
        rs += MQResistanceCalculation_MQ2(analogRead(mq_pin));
        delay(READ_SAMPLE_INTERVAL);
    }

    rs = rs / READ_SAMPLE_TIMES;

    return rs;
}

float MQSensor::MQRead_MQ135(int mq_pin)
{
    int i;
    float rs = 0;

    for (i = 0; i < READ_SAMPLE_TIMES; i++)
    {
        rs += MQResistanceCalculation_MQ135(analogRead(mq_pin));
        delay(READ_SAMPLE_INTERVAL);
    }

    rs = rs / READ_SAMPLE_TIMES;

    return rs;
}

/*****************************  MQGetGasPercentage **********************************
Input:   rs_ro_ratio - Rs divided by Ro
         gas_id      - target gas type
Output:  ppm of the target gas
Remarks: This function uses different equations representing curves of each gas to
         calculate the ppm (parts per million) of the target gas.
************************************************************************************/
int MQSensor::MQGetGasPercentage_MQ2(float rs_ro_ratio, int gas_id)
{
    if (accuracy_MQ2 == 0)
    {
        if (gas_id == GAS_SMOKE)
        {
            return (pow(10, ((-2.331 * (log10(rs_ro_ratio))) + 3.596)));
        }
    }

    else if (accuracy_MQ135 == 1)
    {
        if (gas_id == GAS_SMOKE)
        {
            return (pow(10, (-0.976 * pow((log10(rs_ro_ratio)), 2) - 2.018 * (log10(rs_ro_ratio)) + 3.617)));
        }
    }
    return 0;
}

int MQSensor::MQGetGasPercentage_MQ135(float rs_ro_ratio, int gas_id)
{
    if (accuracy_MQ135 == 0)
    {
        if (gas_id == GAS_AMMONIUM)
        {
            return (pow(10, ((-2.469 * (log10(rs_ro_ratio))) + 2.005)));
        }
    }

    else if (accuracy_MQ135 == 1)
    {
        if (gas_id == GAS_AMMONIUM)
        {
            return (pow(10, ((-2.469 * (log10(rs_ro_ratio))) + 2.005)));
        }
    }
    return 0;
}
