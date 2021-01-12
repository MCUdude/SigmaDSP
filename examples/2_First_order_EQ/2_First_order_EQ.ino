/**********************************************|
| SigmaDSP library                             |
| https://github.com/MCUdude/SigmaDSP          |
|                                              |
| 2_First_order_EQ.ino                         |
| This example controls two one-band EQs and   |
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
firstOrderEQ eq1;
firstOrderEQ eq2;

void setup() 
{  
  Serial.begin(9600);
  Serial.println(F("SigmaDSP 2_First_order_EQ example\n"));

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

  // Set volume to 0dB
  dsp.volume_slew(MOD_SWVOL1_ALG0_TARGET_ADDR, 0);
}


void loop() 
{ 
  // Set lowpass filter on both EQs and boost by +6dB at 100Hz
  Serial.println(F("100Hz lowpass on both EQs. +6dB gain at 100Hz"));
  eq1.filterType = parameters::filterType::lowpass;
  eq1.freq = 100;
  eq1.gain = 6;
  eq1.state = parameters::state::on; 
  eq2.filterType = parameters::filterType::lowpass;
  eq2.freq = 100;
  eq2.gain = 6;
  eq2.state = parameters::state::on;
  dsp.EQfirstOrder(MOD_GEN1STORDER1_ALG0_PARAMB00_ADDR, eq1);
  dsp.EQfirstOrder(MOD_GEN1STORDER2_ALG0_PARAMB00_ADDR, eq2);
  delay(5000);

  // Turn off both EQs for a flat response
  Serial.println(F("EQ off. 0dB gain and flat response"));
  eq1.state = parameters::state::off; 
  eq1.state = parameters::state::off; 
  dsp.EQfirstOrder(MOD_GEN1STORDER1_ALG0_PARAMB00_ADDR, eq1);
  dsp.EQfirstOrder(MOD_GEN1STORDER2_ALG0_PARAMB00_ADDR, eq2);
  delay(5000);

  // Set lowpass filter on both EQs and boost by +6dB at 100Hz
  Serial.println(F("1kHz highpass on both EQs. 0dB gain at 1kHz\n"));
  eq1.filterType = parameters::filterType::highpass;
  eq1.freq = 1000;
  eq1.gain = 0;
  eq1.state = parameters::state::on; 
  eq2.filterType = parameters::filterType::highpass;
  eq2.freq = 1000;
  eq2.gain = 0;
  eq2.state = parameters::state::on;
  dsp.EQfirstOrder(MOD_GEN1STORDER1_ALG0_PARAMB00_ADDR, eq1);
  dsp.EQfirstOrder(MOD_GEN1STORDER2_ALG0_PARAMB00_ADDR, eq2);
  delay(5000);
}
