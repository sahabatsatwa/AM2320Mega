#include "Adafruit_Sensor.h"
#include "Adafruit_AM2320.h"
#include "cxm1500geInterface.h"
#include <stdio.h>
#include "generatePayload_am2320.h"

cxm1500geInterface *eltres;
Adafruit_AM2320 am2320 = Adafruit_AM2320();

float tempTemperature;
float tempHum;

byte flagReadySend = 0;

class myEltresCallback : public cxm1500geCallback
{
  void onGPSconnected()
  {
    Serial.println("---GPS connected---");
    flagReadySend = 1;
  };
  /*the SetDataSend function edit-able,  but the structure of function can't edit*/
  void onSetDataSend(char *dataWillSend, CXM1500GENMEAGGAInfo *GGAInfo, byte japanCorrection)
  {
      tempHum = am2320.readHumidity();
      tempTemperature = am2320.readTemperature();
      
      Serial.print("Temperature: ");
      Serial.print(tempTemperature);
      Serial.println(" degrees C");
    
      Serial.print("Relative Humidity: ");
      Serial.print(tempHum);
      Serial.println(" %");

      tempHum *= 10;
      tempTemperature *= 10;
    }

    int16_t hum = (int16_t) tempHum;
    int16_t temperature = (int16_t) tempTemperature;    

    byte typePayload = 9;
    byte classService = 0;
    byte coin;
    getDataWillSend(dataWillSend, GGAInfo, japanCorrection, typePayload, classService, hum, temperature, coin);
  };
};
char version[6];
char resp[100];

void setup()
{
  /* This part is belongs to ELTRES Library
   * do not modify it.
   */
  
  Serial.begin(115200);

  am2320.begin();

  eltres = new cxm1500geInterface(1, "10"); // can remove reboot 24 hours. example eltres = new cxm1500geInterface(1); ->  this main variable timeReboot is NULL
  eltres->setCallback(new myEltresCallback); // this function must call to set callback class

  eltres->getVersion(version);
  Serial.print(F("Library's version  = "));
  Serial.print(version);
  Serial.print("\n");
  eltres->init();
  eltres->setStateEvent(1);

  /* --end of ELTRES Library */
}

byte test = 0;
void loop()
{

  if (flagReadySend == 1)
  {
    eltres->send(); // This function to send payload if GPS ready, the payload was setting on onSetDataSend function.
    eltres->sleepCxm(80000, 100000);
    delay(10);
  }
  else
  {
    eltres->checkSYSEvent();
  }
  /*
    86400000 milliseconds in a day
    3600000 milliseconds in an hour
    60000 milliseconds in a minute
    1000 milliseconds in a second
    */
  eltres->rebootInterval(1800000); // 30min Reboot for speed check. if no-reboot interval, can remove this function
                                   /* note :  if reboot interval active in log appear message "--- Reboot Interval "
       but reboot 24 hours base UTC active in log appear message "--- Reboot 24 Hours Time ---"
       and reboot also can reboot-interval only.
    */
}
