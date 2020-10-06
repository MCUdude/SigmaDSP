/***********************************************|
| SigmaDSP library                              |
| https://github.com/MCUdude/SigmaDSP           |
|                                               |
| 6_Dynamic_bass_boost.ino                      |
| This example we use the DSP to boost the bass |
| using the Dynamic enhancement block.          |
| See the SigmaStudio project file if you want  |
| to learn more, tweak or do modifications.     |
|***********************************************/

// Include Wire and SigmaDSP library
#include <Wire.h>
#include <SigmaDSP.h>

// Include generated parameter file
#include "SigmaDSP_parameters.h"


// The first parameter is the Wire object we'll be using when communicating wth the DSP
// The second parameter is the DSP i2c address, which is defined in the parameter file
// The third parameter is the SigmaDSP type
// An optional fourth parameter is the pin to physically reset the DSP
SigmaDSP dsp(Wire, DSP_I2C_ADDRESS, ADAU1701 /*,12*/);


// Only needed if an external i2c EEPROM is present + the DSP is in selfboot mode
// The first parameter is the Wire object we'll be using when communicating wth the EEPROM
// The second parameter is the EEPROM i2c address, which is defined in the parameter file
// The third parameter is the EEPROM size in kilobits (kb)
// An optional fourth parameter is the pin to toggle while writing content to EEPROM
//DSPEEPROM ee(Wire, EEPROM_I2C_ADDRESS, 256, LED_BUILTIN);


void setup()
{
  Serial.begin(9600);
  Serial.println(F("6_Dynamic_bass_boost example\n"));

  Wire.begin();
  dsp.begin();
  //ee.begin();

  delay(2000);


  Serial.println(F("Pinging i2c bus...\n0 -> deveice is present\n2 -> device is not present"));
  Serial.print(F("DSP response: "));
  Serial.println(dsp.ping());
  //Serial.print(F("EEPROM ping: "));
  //Serial.println(ee.ping());


  // Use this step if no EEPROM is present
  Serial.print(F("\nLoading DSP program... "));
  loadProgram(dsp);
  Serial.println("Done!\n");


  // Comment out the three code lines above and use this step instead if EEPROM is present
  // The last parameter in writeFirmware is the FW version, and prevents the MCU from overwriting on every reboot
  //ee.writeFirmware(DSP_eeprom_firmware, sizeof(DSP_eeprom_firmware), 0);
  //dsp.reset();
  //delay(2000); // Wait for the FW to load from the EEPROM
}


void loop()
{
  Serial.println(F("Set bass boost to 0dB"));
  dsp.dynamicBass(MOD_DYNAMIC_BASS_ALG0_LOWLEVELBOOST_ADDR, 0);
  delay(5000);

  Serial.println(F("Set bass boost to +10dB"));
  dsp.dynamicBass(MOD_DYNAMIC_BASS_ALG0_LOWLEVELBOOST_ADDR, 10);
  delay(5000);

  Serial.println(F("Set bass boost to +20dB"));
  dsp.dynamicBass(MOD_DYNAMIC_BASS_ALG0_LOWLEVELBOOST_ADDR, 20);
  delay(5000);
}
