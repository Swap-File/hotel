#include "Particle.h"
#include "AM2302.h"
#include "SparkFun_VEML6030_Ambient_Light_Sensor.h"

SparkFun_Ambient_Light light(0x48);
AM2302 *sensor;
#define PIR_DOUT 15
SerialLogHandler logHandler;

SYSTEM_MODE(AUTOMATIC);

float temp_max;
float temp_min;
float humid_max;
float humid_min;
long lux_max;
long lux_min;
volatile bool motion;
int fps;

char msg_send[100];

uint32_t lastReset = 0;

void motiondetect(void)
{
  motion = true;
}

void reset_values(void)
{
  temp_max = -200;
  temp_min = 200;
  motion = false;
  humid_max = 0;
  humid_min = 100;
  fps = 0;
  lux_max = 0;
  lux_min = 10000000;
}

void setup()
{
  lastReset = millis();
  Serial.begin(115200);
  reset_values();

  // temp and humidity init
  sensor = new AM2302(D8);

  // motion sensor init
  pinMode(PIR_DOUT, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIR_DOUT), motiondetect, RISING);

  // light sensor init
  Wire.begin();
  light.begin();
  light.setGain(0.125);
  light.setIntegTime(100);
}

void loop()
{
  fps++;
  Particle.process();

  //reboot daily
  if (millis() - lastReset > 1440 * 60000UL)
  {
    System.reset(RESET_NO_WAIT);
  }

  // do lux
  int IntegTime = light.readIntegTime();
  if (IntegTime != 100) // test for connection issues
  {
    // reinit and double check data
    light.powerOn();
    light.setGain(0.125);
    light.setIntegTime(100);
    long luxVal = light.readLight();
  }
  else
  {
    long luxVal = light.readLight();
    lux_min = min(luxVal, lux_min);
    lux_max = max(luxVal, lux_max);
  }
  // do temp and humidity once every 2 seconds
  bool result = false;

  static uint32_t last_temp_time = 0;
  if ((last_temp_time + 2000) < millis())
  {
    ATOMIC_BLOCK()
    {
      result = sensor->sample();
    }
    last_temp_time = millis();
  }

  if (result)
  {
    double temp = sensor->getTemp();
    temp = 1.8 * temp + 32;
    double humidity = sensor->getHumidity();
    temp_max = max(temp, temp_max);
    temp_min = min(temp, temp_min);
    humid_max = max(humidity, humid_max);
    humid_min = min(humidity, humid_min);
  }

  static uint32_t lastPrint = 60000; // dont output until we have a minute of data
  if ((lastPrint + 60000) < millis())
  {
    sprintf(msg_send, "%.1f,%.1f,%.1f,%.1f,%ld,%ld,%d", temp_min, temp_max, humid_min, humid_max, lux_min, lux_max, motion);
    Particle.process();
    Particle.publish("msg", msg_send, NO_ACK);
    reset_values();
    lastPrint = millis();
  }
  delay(100);
}
