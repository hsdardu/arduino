#include <OneWire.h>
#include <DallasTemperature.h>
#include <LeweiClient.h>

#include <SPI.h>
#include <Ethernet.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress insideThermometer, outsideThermometer;

//put your api key here,find it in lewei50.com->my account->account setting
#define LW_USERKEY "b568ec55d7e648b59b9ec94eec08be5e"
//put your gateway number here,01 as default
#define LW_GATEWAY "01"
 
//delay between updates
#define POST_INTERVAL (1*1000)
 
LeWeiClient *lwc;

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

  // Start up the library
  sensors.begin();

  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 
  if (!sensors.getAddress(outsideThermometer, 1)) Serial.println("Unable to find address for Device 1"); 
  Serial.print("temp_D2:");
  printAddress(insideThermometer);
  Serial.print("temp_32:");
  printAddress(outsideThermometer);

  lwc = new LeWeiClient(LW_USERKEY, LW_GATEWAY);
}

float getTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  return tempC;
}

float getData(DeviceAddress deviceAddress)
{
  return getTemperature(deviceAddress);
}

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
  Serial.println();
}


void loop(void)
{ 
  if (lwc) {
    Serial.print("Requesting temperatures...");
    sensors.requestTemperatures();
    Serial.println("DONE");
    float itemp = getTemperature(insideThermometer);
    float otemp = getTemperature(outsideThermometer);
    lwc->append("temp_D2", itemp);
    lwc->append("temp_32", otemp);
    lwc->send();
	
    Serial.println("*** send completed ***");
 
    delay(POST_INTERVAL);
  }
}

