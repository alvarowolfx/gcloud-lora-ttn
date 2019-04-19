
#include <Arduino.h>
#include <Adafruit_CCS811.h>
#include <ArduinoLowPower.h>

#define AU915 1

#include <CayenneLPP.h>
#include <TinyLoRa.h>
#include <SPI.h>
#include "secrets.h"

#define CCS811_ADDR 0x5B
#define VBATPIN A7
#define SLEEP_INTERVAL 10 * 60 * 1000

// LPP Data Packet to Send to TTN
CayenneLPP lpp(51);

Adafruit_CCS811 ccs;
TinyLoRa lora = TinyLoRa(3, 8);

void setup()
{
  Serial.begin(9600);
  Serial.println("TTN Air Quality Sensor");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  lora.setChannel(CH0);
  lora.setDatarate(SF7BW125);

  if (!lora.begin())
  {
    Serial.println("Failed");
    Serial.println("Check your radio");
    while (true)
    {
      Serial.println("LoRa Failed");
      delay(500);
    }
  }
  Serial.println("OK");

  if (!ccs.begin(CCS811_ADDR))
  {
    Serial.println("Failed to start sensor! Please check your wiring.");
    while (true)
    {
      Serial.println("Sensor Failed");
      delay(500);
    }
  }

  //calibrate temperature sensor
  while (!ccs.available())
  {
    Serial.print(".");
    delay(100);
  };
  float temp = ccs.calculateTemperature();
  ccs.setTempOffset(temp - 25.0);
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);

  float temp = ccs.calculateTemperature();
  uint16_t co2 = 0;
  uint16_t tvoc = 0;
  if (!ccs.readData())
  {
    co2 = ccs.geteCO2();
    tvoc = ccs.getTVOC();
    Serial.print("CO2: ");
    Serial.print(co2);
    Serial.print("ppm, TVOC: ");
    Serial.print(tvoc);
    Serial.print("ppb   Temp:");
    Serial.println(temp);
  }
  else
  {
    Serial.println("ERROR!");
    while (1)
    {
      Serial.println("Reading Failed");
      delay(500);
    }
  }

  float batValue = ((analogRead(VBATPIN) * 2) * 3.3) / 1024;

  // encode int as bytes
  lpp.addTemperature(0, temp);
  lpp.addAnalogInput(1, batValue);
  lpp.addAnalogInput(2, co2 / 100.0);
  lpp.addAnalogInput(3, tvoc / 100.0);

  Serial.println("Sending LoRa Data...");
  lora.sendData(lpp.getBuffer(), lpp.getSize(), lora.frameCounter);
  Serial.print("Frame Counter: ");
  Serial.println(lora.frameCounter);
  lora.frameCounter++;

  delay(1000);

  digitalWrite(LED_BUILTIN, LOW);

  LowPower.sleep(SLEEP_INTERVAL);

#ifdef USBCON
  USBDevice.attach();
#endif

  Serial.print("I'm awake now! I slept for ");
  Serial.print(SLEEP_INTERVAL / 1000, DEC);
  Serial.println(" seconds.");
  Serial.println();

  // delay(30 * 1000);
}