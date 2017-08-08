//#######################################################################################################
//#################### Plugin MOD-1016 Lightning Sensor ######################################
//#######################################################################################################
//include embedded adventures libary https://github.com/embeddedadventures/MOD-1016


// IN PROGRESS EVERYTHING BELOW IS NOT FINISHED and are COPIED BITS AND PIECES

+  #include <Wire.h>
+  #include <SPI.h>
  #include <SoftwareSerial.h>
  #include <AS3935.h>
  /*
  Copyright (c) 2017, Embedded Adventures
  All rights reserved.
  Contact us at source [at] embeddedadventures.com
  www.embeddedadventures.com
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
  - Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  - Neither the name of Embedded Adventures nor the names of its contributors
    may be used to endorse or promote products derived from this software
    without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.
  */

  // AS3935 MOD-1016 Lightning Sensor ESPEasy test sketch
  // Written originally by Embedded Adventures



  #define PLUGIN_214
  #define PLUGIN_ID_214         214
  #define PLUGIN_NAME_214       "LightningSensor MOD1016"
  #define PLUGIN_VALUENAME1_214 "Lightning"
  #define PLUGIN_VALUENAME2_214 "Distance"

  #define IRQ_PIN 2
  #define CS_PIN 10
  #define MOD1016_ADDRESS          (0x03)  // default address (0x03 = datasheet)
  #define debug true

  volatile bool detected = false;


  boolean Plugin_214(byte function, struct EventStruct *event, String& string)
  {
    boolean success = false;
    byte timeout;

    switch (function)
    {
      case PLUGIN_DEVICE_ADD:
        {
          Device[++deviceCount].Number = PLUGIN_ID_214;
          Device[deviceCount].Type = DEVICE_TYPE_I2C;
          Device[deviceCount].VType = SENSOR_TYPE_DUAL;
          Device[deviceCount].Ports = 0;
          Device[deviceCount].PullUpOption = false;
          Device[deviceCount].InverseLogicOption = false;
          Device[deviceCount].FormulaOption = true;
          Device[deviceCount].ValueCount = 2;
          Device[deviceCount].SendDataOption = true;
          Device[deviceCount].TimerOption = true;
          Device[deviceCount].GlobalSyncOption = true;
          break;
        }

        case PLUGIN_GET_DEVICENAME:
        {
          string = F(PLUGIN_NAME_214);
          break;
        }

        case PLUGIN_GET_DEVICEVALUENAMES:
        {
          strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_214));
          strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[1], PSTR(PLUGIN_VALUENAME2_214));
          break;
        }


        case PLUGIN_INIT:
        {
          addLog(LOG_LEVEL_INFO, (char*)"INIT : MOD-1016 (AS3935) Lightning Sensor");
          //I2C
          Wire.begin();
          mod1016.init(IRQ_PIN);
          //SPI
          //SPI.begin();
          //mod1016.init(IRQ_PIN, CS_PIN);

          //Tune Caps, Set AFE, Set Noise Floor
          autoTuneCaps(IRQ_PIN);

          //mod1016.setTuneCaps(7);
          mod1016.setOutdoors();
          mod1016.setNoiseFloor(5);
          String log = F("TUNE\tIN/OUT\tNOISEFLOOR");
          log += "\n";
          Hex TUNE = mod1016.getTuneCaps();
          Bin IN_OUT = mod1016.getAFE();
          Hex NOISEFLOOR = mod1016.getNoiseFloor();
          log += F(TUNE "\t" IN_OUT "\t" NOISEFLOOR "\n");

          if debug {
            Serial.println("TUNE\tIN/OUT\tNOISEFLOOR");
            Serial.print(mod1016.getTuneCaps(), HEX);
            Serial.print("\t");
            Serial.print(mod1016.getAFE(), BIN);
            Serial.print("\t");
            Serial.println(mod1016.getNoiseFloor(), HEX);
            Serial.print("\n");
          }

          pinMode(IRQ_PIN, INPUT);
          attachInterrupt(digitalPinToInterrupt(IRQ_PIN), alert, RISING);
          mod1016.getIRQ();
          log += "after interrupt";
          addLog(LOG_LEVEL_INFO, log);
          success = true;
          return success;
        };
        
CHANGE UP TO HERE

        void loop() {
          if (detected) {
            translateIRQ(mod1016.getIRQ());
            detected = false;
          }
        };

        void alert() {
          detected = true;
        };

        void translateIRQ(uns8 irq) {
          switch(irq) {
              case 1:
                Serial.println("NOISE DETECTED");
                break;
              case 4:
                Serial.println("DISTURBER DETECTED");
                break;
              case 8:
                Serial.println("LIGHTNING DETECTED");
                printDistance();
                break;
            }
        };

        void printDistance() {
          int distance = mod1016.calculateDistance();
          if (distance == -1)
            Serial.println("Lightning out of range");
          else if (distance == 1)
            Serial.println("Distance not in table");
          else if (distance == 0)
            Serial.println("Lightning overhead");
          else {
            Serial.print("Lightning ~");
            Serial.print(distance);
            Serial.println("km away\n");
          }
        };


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
