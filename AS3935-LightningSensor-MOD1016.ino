+//#######################################################################################################
 +//#################### Plugin MOD-1016 Lightning Sensor ######################################
 +//#######################################################################################################
 +//Copyright (c) 2017, Embedded Adventures - All rights reserved.
 +//Contact us at source [at] embeddedadventures.com - www.embeddedadventures.com
 +
 +// IN PROGRESS EVERYTHING BELOW IS NOT FINISHED and are COPIED BITS AND PIECES
 + 
  +#include <Wire.h>
  +#include <AS3935.h>
  +#include <SPI.h>
 +
 +#define PLUGIN_214
 +#define PLUGIN_ID_214         214
 +#define PLUGIN_NAME_214       "LightningSensor MOD1016"
 +#define PLUGIN_VALUENAME1_214 "Lightning"
 +#define PLUGIN_VALUENAME2_214 "Distance"
 +
 +#define IRQ_PIN 2
 +#define CS_PIN 10

 +volatile bool detected = false;

 +
 +boolean Plugin_214(byte function, struct EventStruct *event, String& string)
 +{
 +  boolean success = false;
 +  byte timeout;
 +
 +  switch (function)
 +  {
 +    case PLUGIN_DEVICE_ADD:
 +      {
 +        Device[++deviceCount].Number = PLUGIN_ID_214;
 +        Device[deviceCount].Type = DEVICE_TYPE_I2C;
 +        Device[deviceCount].VType = SENSOR_TYPE_DUAL;
 +        Device[deviceCount].Ports = 0;
 +        Device[deviceCount].PullUpOption = false;
 +        Device[deviceCount].InverseLogicOption = false;
 +        Device[deviceCount].FormulaOption = true;
 +        Device[deviceCount].ValueCount = 2;
 +        Device[deviceCount].SendDataOption = true;
 +        Device[deviceCount].TimerOption = true;
 +        Device[deviceCount].GlobalSyncOption = true;
 +        break;
 +      }
 +
 +    case PLUGIN_GET_DEVICENAME:
 +      {
 +        string = F(PLUGIN_NAME_214);
 +        break;
 +      }
 +
 +    case PLUGIN_GET_DEVICEVALUENAMES:
 +      {
 +        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_214));
 +        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[1], PSTR(PLUGIN_VALUENAME2_214));
 +        break;
 +      }
 +
 +
 +    case PLUGIN_INIT:
 +      {
 +        addLog(LOG_LEVEL_INFO, (char*)"INIT : SRF01");
 +        Plugin_100_SRF_Pin = Settings.TaskDevicePin1[event->TaskIndex];
 +        Plugin_100_SRF = new SoftwareSerial(Plugin_100_SRF_Pin, Plugin_100_SRF_Pin);
 +        Plugin_100_SRF01_Cmd(PLUGIN_100_srfAddress, PLUGIN_100_getSoft);
 +        for (timeout = 0; timeout < 10; timeout++) {
 +          if (Plugin_100_SRF->available() < 1 ) {
 +            delay(10);
 +          } else {
 +            int softVer = Plugin_100_SRF->read();
 +            String log = F("SRF01 : Firmware-Version: ");
 +            log += softVer;
 +            addLog(LOG_LEVEL_INFO, log);
 +            success = true;
 +            return success;
 +          };
 +        };
 +        addLog(LOG_LEVEL_ERROR, (char*)"SRF01-Init: protocol timeout!");
 +        success = false;
 +        break;
 +      }
 +
 +    case PLUGIN_READ:
 +      {
 +        Plugin_100_SRF01_Cmd(PLUGIN_100_srfAddress, PLUGIN_100_getRange);
 +        for (timeout = 0; timeout < 10; timeout++) {
 +          if (Plugin_100_SRF->available() < 2 ) {
 +            delay(10);
 +          } else {
 +            byte highByte = Plugin_100_SRF->read();
 +            byte lowByte = Plugin_100_SRF->read();
 +            int dist = ((highByte << 8) + lowByte);
 +            UserVar[event->BaseVarIndex] = dist;
 +            String log = F("SRF1 : Distance: ");
 +            log += dist;
 +            addLog(LOG_LEVEL_INFO, log);
 +            success = true;
 +            return success;
 +          };
 +        };
 +        addLog(LOG_LEVEL_ERROR, (char*)"SRF01  : protocol timeout!");
 +        UserVar[event->BaseVarIndex] = NAN;
 +        success = false;
 +        break;
 +      }
 +
 +  }
 +  return success;
 +}
 +
 +//**************************************************************************/
 +// Send SRF01 Command
 +//**************************************************************************/
 +void Plugin_100_SRF01_Cmd(byte Address, byte cmd) {
 +  Plugin_100_SRF->flush();
 +  pinMode(Plugin_100_SRF_Pin, OUTPUT);
 +  digitalWrite(Plugin_100_SRF_Pin, LOW);
 +  delay(2);
 +  digitalWrite(Plugin_100_SRF_Pin, HIGH);
 +  delay(1);
 +  Plugin_100_SRF->write(Address);
 +  Plugin_100_SRF->write(cmd);
 +  pinMode(Plugin_100_SRF_Pin, INPUT);
 +  Plugin_100_SRF->flush();
 +}
 +
