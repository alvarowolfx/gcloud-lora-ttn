
#include <Arduino.h>
#include <Adafruit_CCS811.h>
#include <ArduinoLowPower.h>

#define AU915 1

#include <TinyLoRa.h>
#include <SPI.h>
#include "secrets.h"

#define NODE_ID 1
#define CCS811_ADDR 0x5B
#define VBATPIN A7
#define SLEEP_INTERVAL 10 * 60 * 1000

// Data Packet to Send to TTN
unsigned char loraData[9];

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

  // encode float as int
  int16_t tempInt = round(temp * 100);
  int16_t batInt = round(batValue * 100);

  // encode int as bytes
  //byte payload[2];
  loraData[0] = NODE_ID;

  loraData[1] = highByte(tempInt);
  loraData[2] = lowByte(tempInt);

  loraData[3] = highByte(co2);
  loraData[4] = lowByte(co2);

  loraData[5] = highByte(tvoc);
  loraData[6] = lowByte(tvoc);

  loraData[7] = highByte(batInt);
  loraData[8] = lowByte(batInt);

  Serial.println("Sending LoRa Data...");
  lora.sendData(loraData, sizeof(loraData), lora.frameCounter);
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