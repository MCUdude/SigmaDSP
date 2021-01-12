/**********************************************|
| SigmaDSP library                             |
| https://github.com/MCUdude/SigmaDSP          |
|                                              |
| 3_Second_order_EQ.ino                        |
| This example controls a four band EQs and    |
| a master volume slider.                      |
|                                              |
| See the SigmaStudio project file if you want |
| to learn more, tweak or do modifications.    |
|**********************************************/

// Include Wire and SigmaDSP library
#include <Wire.h>
#include <SigmaDSP.h>

// Include generated parameter file
#include "SigmaDSP_parameters.h"


// The first parameter is the Wire object we'll be using when communicating wth the DSP
// The second parameter is the DSP i2c address, which is defined in the parameter file
// The third parameter is the sample rate
// An optional fourth parameter is the pin to physically reset the DSP
SigmaDSP dsp(Wire, DSP_I2C_ADDRESS, 48000.00f /*,12*/);


// Only needed if an external i2c EEPROM is present + the DSP is in selfboot mode
// The first parameter is the Wire object we'll be using when communicating wth the EEPROM
// The second parameter is the EEPROM i2c address, which is defined in the parameter file
// The third parameter is the EEPROM size in kilobits (kb)
// An optional fourth parameter is the pin to toggle while writing content to EEPROM
//DSPEEPROM ee(Wire, EEPROM_I2C_ADDRESS, 256, LED_BUILTIN);


// Create an instance for each EQ block
secondOrderEQ eqBand1;
secondOrderEQ eqBand2;
secondOrderEQ eqBand3;
secondOrderEQ eqBand4;

void setup()
{
  Serial.begin(9600);
  Serial.println(F("3_Second_order_EQ example\n"));

  Wire.begin();
  dsp.begin();
  //ee.begin();

  delay(2000);


  Serial.println(F("Pinging i2c lines...\n0 -> deveice is present\n2 -> device is not present"));
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


  // Initilize EQ band 1
  eqBand1.filterType = parameters::filterType::lowShelf;
  eqBand1.S = 1;
  eqBand1.Q = 0.7071;

  // Initilize EQ band 2
  eqBand2.filterType = parameters::filterType::peaking;
  eqBand2.Q = 1.41;

  // Initilize EQ band 3
  eqBand3.filterType = parameters::filterType::peaking;
  eqBand3.Q = 1.41;

  // Initilize EQ band 4
  eqBand4.filterType = parameters::filterType::peaking;
  eqBand4.Q = 1.41;

  // Set volume to 0dB
  dsp.volume_slew(MOD_SWVOL1_ALG0_TARGET_ADDR, 0);
}


void loop()
{
  // Adjust band 1
  Serial.println(F("+10dB boost at 80Hz (band1)"));
  eqBand1.freq = 80;
  eqBand1.boost = 10;
  eqBand1.state = parameters::state::on;
  dsp.EQsecondOrder(MOD_MIDEQ1_ALG0_STAGE0_B0_ADDR, eqBand1);
  delay(5000);
  Serial.println(F("-10dB boost at 80Hz (band1)"));
  eqBand1.freq = 80;
  eqBand1.boost = -10;
  dsp.EQsecondOrder(MOD_MIDEQ1_ALG0_STAGE0_B0_ADDR, eqBand1);
  delay(5000);
  Serial.println(F("0dB boost at 80Hz (bypass band1)"));
  eqBand1.state = parameters::state::off;
  dsp.EQsecondOrder(MOD_MIDEQ1_ALG0_STAGE0_B0_ADDR, eqBand1);
  delay(5000);

  // Adjust band 2
  Serial.println(F("+10dB boost at 500Hz (band2)"));
  eqBand2.freq = 500;
  eqBand2.boost = 10;
  // EQ is on by default, so no need to explicit turn it on
  dsp.EQsecondOrder(MOD_MIDEQ1_ALG0_STAGE1_B0_ADDR, eqBand2);
  delay(5000);
  Serial.println(F("0dB boost at 500Hz   (band2)"));
  eqBand2.freq = 500;
  eqBand2.boost = 0;
  dsp.EQsecondOrder(MOD_MIDEQ1_ALG0_STAGE1_B0_ADDR, eqBand2);
  delay(5000);

  // Adjust band 3
  Serial.println(F("+10dB boost at 2500Hz (band3)"));
  eqBand3.freq = 2500;
  eqBand3.boost = 10;
  // EQ is on by default, so no need to explicit turn it on
  dsp.EQsecondOrder(MOD_MIDEQ1_ALG0_STAGE2_B0_ADDR, eqBand3);
  delay(5000);
  Serial.println(F("0dB boost at 2500Hz   (band3)"));
  eqBand3.freq = 2500;
  eqBand3.boost = 0;
  dsp.EQsecondOrder(MOD_MIDEQ1_ALG0_STAGE2_B0_ADDR, eqBand3);
  delay(5000);

  // Adjust band 4
  Serial.println(F("Loop from 0db to +10dB boost at 10000Hz (band4)"));
  eqBand4.freq = 10000;
  // EQ is on by default, so no need to explicit turn it on
  for(int8_t i = 0; i < 11; i++)
  {
    eqBand4.boost = i;
    dsp.EQsecondOrder(MOD_MIDEQ1_ALG0_STAGE3_B0_ADDR, eqBand4);
    delay(500);
  }
  Serial.println(F("Loop from +10db to 0dB boost at 10000Hz (band4)\n"));
  for(int8_t i = 10; i > -1; i--)
  {
    eqBand4.boost = i;
    dsp.EQsecondOrder(MOD_MIDEQ1_ALG0_STAGE3_B0_ADDR, eqBand4);
    delay(500);
  }
}
