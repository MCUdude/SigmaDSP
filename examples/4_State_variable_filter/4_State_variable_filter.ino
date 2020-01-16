/**********************************************|
| SigmaDSP library                             |
| https://github.com/MCUdude/SigmaDSP          |
|                                              |
| 4_State_variable_filter.ino                  |
| This example we adjust a state variable      |
| filter, and select between the lowpass,      |
| highpass and bandpass option.                |
|                                              |
| See the SigmaStudio project file if you want |
| to learn more, tweak or do modifications.    | 
|**********************************************/

// Include SigmaDSP library
#include <SigmaDSP.h>

// Include generated parameter file
#include "SigmaDSP_parameters.h"


// The first parameter is the i2c address, which is defined in the parameter file.
// The next parameter is the SigmaDSP type
// An optional third parameter is the pin to physically reset the DSP
SigmaDSP dsp(DSP_I2C_ADDRESS, ADAU1701 /*,12*/);


// Only needed if an external i2c EEPROM is present + the DSP is in selfboot mode
// The first parameter is the i2c address, which is defined in the parameter file.
// The second parameter is the EEPROM size in kilobits
// An optional third parameter is the pin to toggle while writing content to EEPROM
//DSPEEPROM ee(EEPROM_I2C_ADDRESS, 256, LED_BUILTIN);

// Variable to store calculated sweep tone
uint16_t frequency;

void setup() 
{  
  Serial.begin(9600);
  Serial.println(F("SigmaDSP 4_State_variable_filter example\n"));

  // dsp and ee supports re-mapping of the SDA and SCL signals, but
  // can only be done if the microcontroller supports it (ESP8266 for example)
  dsp.begin(/* Wire, SDA, SCL */);
  //ee.begin(/* Wire, SDA, SCL */);

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

  // Set volume to 0dB
  dsp.volume_slew(MOD_SWVOL1_ALG0_TARGET_ADDR, 0);
}


void loop() 
{
  Serial.print(F("F=500Hz Q=0.71"));
  dsp.stateVariable(MOD_STATEVARFILTER1_ALG0_STATEVARALG1FREQ_ADDR, 500, 0.71);
  Serial.println(F(" - Lowpass selected"));
  dsp.mux(MOD_MUX_MONOSWSLEW_ADDR, 0);
  delay(5000);

  Serial.print(F("F=1000Hz Q=0.71"));
  dsp.stateVariable(MOD_STATEVARFILTER1_ALG0_STATEVARALG1FREQ_ADDR, 1000, 0.71);
  Serial.println(F(" - Highpass selected"));
  dsp.mux(MOD_MUX_MONOSWSLEW_ADDR, 1);
  delay(5000);

  Serial.print(F("F=3000Hz Q=0.71"));
  dsp.stateVariable(MOD_STATEVARFILTER1_ALG0_STATEVARALG1FREQ_ADDR, 3000, 0.71);
  Serial.println(F(" - Bandpass selected"));
  dsp.mux(MOD_MUX_MONOSWSLEW_ADDR, 2);
  delay(5000);
}
