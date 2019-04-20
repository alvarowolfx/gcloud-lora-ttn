
#include <Arduino.h>
#include <SparkFunCCS811.h>
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

CCS811 ccs(CCS811_ADDR);
TinyLoRa lora = TinyLoRa(3, 8);

void printSensorError();
void printDriverError(CCS811Core::status errorCode);

void setup()
{
  Serial.begin(9600);
  Serial.println("TTN Air Quality Sensor");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Wire.begin();

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

  CCS811Core::status returnCode = ccs.begin();
  Serial.print("CCS811 begin exited with: ");
  printDriverError(returnCode);
  Serial.println();

  ccs.setRefResistance(9950);

  if (returnCode != CCS811Core::SENSOR_SUCCESS)
  {
    Serial.println("Failed to start sensor! Please check your wiring.");
    return;
  }

  //calibrate temperature sensor
  while (!ccs.dataAvailable())
  {
    Serial.print(".");
    delay(100);
  };
  ccs.readNTC();

  Serial.print(" Converted temperature : ");
  float readTemperature = ccs.getTemperature();
  Serial.print(readTemperature, 2);
  Serial.println(" deg C");

  ccs.setEnvironmentalData(50, readTemperature);
  delay(30 * 1000);
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);

  ccs.readNTC();

  Serial.print(" Converted temperature : ");
  float temp = ccs.getTemperature();
  Serial.print(temp, 2);
  Serial.println(" deg C");

  ccs.setEnvironmentalData(50, temp);

  ccs.readAlgorithmResults();

  uint16_t co2 = 0;
  uint16_t tvoc = 0;
  if (!ccs.checkForStatusError())
  {
    co2 = ccs.getCO2();
    tvoc = ccs.getTVOC();
    Serial.print("CO2: ");
    Serial.print(co2);
    Serial.print("ppm, TVOC: ");
    Serial.print(tvoc);
    Serial.print("ppb  Temp:");
    Serial.println(temp);
  }
  else
  {
    Serial.println("ERROR!");
    printSensorError();
  }

  float batValue = ((analogRead(VBATPIN) * 2) * 3.3) / 1024;

  lpp.reset();

  lpp.addTemperature(0, temp);
  lpp.addAnalogInput(1, batValue);
  lpp.addLuminosity(2, co2);
  lpp.addLuminosity(3, tvoc);

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

  //delay(30 * 1000);
}

void printDriverError(CCS811Core::status errorCode)
{
  switch (errorCode)
  {
  case CCS811Core::SENSOR_SUCCESS:
    Serial.print("SUCCESS");
    break;
  case CCS811Core::SENSOR_ID_ERROR:
    Serial.print("ID_ERROR");
    break;
  case CCS811Core::SENSOR_I2C_ERROR:
    Serial.print("I2C_ERROR");
    break;
  case CCS811Core::SENSOR_INTERNAL_ERROR:
    Serial.print("INTERNAL_ERROR");
    break;
  case CCS811Core::SENSOR_GENERIC_ERROR:
    Serial.print("GENERIC_ERROR");
    break;
  default:
    Serial.print("Unspecified error.");
  }
}

//printSensorError gets, clears, then prints the errors
//saved within the error register.
void printSensorError()
{
  uint8_t error = ccs.getErrorRegister();

  if (error == 0xFF) //comm error
  {
    Serial.println("Failed to get ERROR_ID register.");
  }
  else
  {
    Serial.print("Error: ");
    if (error & 1 << 5)
      Serial.print("HeaterSupply");
    if (error & 1 << 4)
      Serial.print("HeaterFault");
    if (error & 1 << 3)
      Serial.print("MaxResistance");
    if (error & 1 << 2)
      Serial.print("MeasModeInvalid");
    if (error & 1 << 1)
      Serial.print("ReadRegInvalid");
    if (error & 1 << 0)
      Serial.print("MsgInvalid");
    Serial.println();
  }
}