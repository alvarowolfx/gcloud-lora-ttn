#include <Arduino.h>
#include <ArduinoLowPower.h>
#include <SparkFunCCS811.h>
#include <CayenneLPP.h>
#include <TinyLoRa.h>
#include <SPI.h>
#include "secrets.h"

#define CCS811_ADDR 0x5B
#define VBATPIN A7
#define SLEEP_INTERVAL 10 * 60 * 1000

// #define DEBUG Serial
#define DEBUG_ENABLED true
#if DEBUG_ENABLED
#define Println(x) Serial.println(x)
#define Print(x) Serial.print(x)
#define PrintNum(x, y) Serial.print(x, y)
#else
#define Println(x)
#define Print(x)
#define PrintNum(x, y)
#endif

// LPP Data Packet to Send to TTN
CayenneLPP lpp(51);

CCS811 ccs(CCS811_ADDR);
TinyLoRa lora = TinyLoRa(3, 8);

void printSensorError();
void printDriverError(CCS811Core::status errorCode);

void setup()
{
  delay(2000);
  if (DEBUG_ENABLED)
  {
    Serial.begin(9600);
    delay(2000);
  }
  Println("TTN Air Quality Sensor");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Wire.begin();

  lora.setChannel(CH0);
  lora.setDatarate(SF7BW125);

  if (!lora.begin())
  {
    Println("Failed");
    Println("Check your radio");
    Println("LoRa Failed");
    delay(500);
    NVIC_SystemReset();
  }
  Println("OK");

  CCS811Core::status returnCode = ccs.begin();
  Print("CCS811 begin exited with: ");
  printDriverError(returnCode);
  Println();

  ccs.setRefResistance(9950);

  if (returnCode != CCS811Core::SENSOR_SUCCESS)
  {
    Println("Failed to start sensor! Please check your wiring.");
    NVIC_SystemReset();
    return;
  }

  //calibrate temperature sensor
  boolean calibrationState = false;
  while (!ccs.dataAvailable())
  {
    Print(".");
    delay(100);
    digitalWrite(LED_BUILTIN, calibrationState);
    calibrationState = !calibrationState;
  };
  digitalWrite(LED_BUILTIN, true);
  ccs.readNTC();

  Print(" Converted temperature : ");
  float readTemperature = ccs.getTemperature();
  PrintNum(readTemperature, 2);
  Println(" deg C");

  ccs.setEnvironmentalData(50, readTemperature);
  delay(30 * 1000);
}

void loop()
{
  digitalWrite(LED_BUILTIN, HIGH);

  ccs.readNTC();

  Print(" Converted temperature : ");
  float temp = ccs.getTemperature();
  PrintNum(temp, 2);
  Println(" deg C");

  ccs.setEnvironmentalData(50, temp);

  ccs.readAlgorithmResults();

  uint16_t co2 = 0;
  uint16_t tvoc = 0;
  if (!ccs.checkForStatusError())
  {
    co2 = ccs.getCO2();
    tvoc = ccs.getTVOC();
    Print("CO2: ");
    Print(co2);
    Print("ppm, TVOC: ");
    Print(tvoc);
    Print("ppb  Temp:");
    Println(temp);
  }
  else
  {
    Println("ERROR!");
    printSensorError();
  }

  float batValue = ((analogRead(VBATPIN) * 2) * 3.3) / 1024;

  lpp.reset();

  lpp.addTemperature(0, temp);
  lpp.addAnalogInput(1, batValue);
  lpp.addLuminosity(2, co2);
  lpp.addLuminosity(3, tvoc);

  Println("Sending LoRa Data...");
  lora.sendData(lpp.getBuffer(), lpp.getSize(), lora.frameCounter);
  Print("Frame Counter: ");
  Println(lora.frameCounter);
  lora.frameCounter++;

  delay(2000);

  digitalWrite(LED_BUILTIN, LOW);

  LowPower.sleep(SLEEP_INTERVAL);

#ifdef USBCON
  // USBDevice.attach();
#endif
  // delay(SLEEP_INTERVAL);

  Print("I'm awake now! I slept for ");
  PrintNum(SLEEP_INTERVAL / 1000, DEC);
  Println(" seconds.");
  Println();

  //delay(30 * 1000);
}

void printDriverError(CCS811Core::status errorCode)
{
  switch (errorCode)
  {
  case CCS811Core::SENSOR_SUCCESS:
    Print("SUCCESS");
    break;
  case CCS811Core::SENSOR_ID_ERROR:
    Print("ID_ERROR");
    break;
  case CCS811Core::SENSOR_I2C_ERROR:
    Print("I2C_ERROR");
    break;
  case CCS811Core::SENSOR_INTERNAL_ERROR:
    Print("INTERNAL_ERROR");
    break;
  case CCS811Core::SENSOR_GENERIC_ERROR:
    Print("GENERIC_ERROR");
    break;
  default:
    Print("Unspecified error.");
  }
}

//printSensorError gets, clears, then prints the errors
//saved within the error register.
void printSensorError()
{
  uint8_t error = ccs.getErrorRegister();

  if (error == 0xFF) //comm error
  {
    Println("Failed to get ERROR_ID register.");
  }
  else
  {
    Print("Error: ");
    if (error & 1 << 5)
      Print("HeaterSupply");
    if (error & 1 << 4)
      Print("HeaterFault");
    if (error & 1 << 3)
      Print("MaxResistance");
    if (error & 1 << 2)
      Print("MeasModeInvalid");
    if (error & 1 << 1)
      Print("ReadRegInvalid");
    if (error & 1 << 0)
      Print("MsgInvalid");
    Println();
  }
}