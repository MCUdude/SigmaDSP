/**********************************************|
| SigmaDSP library                             |
| https://github.com/MCUdude/SigmaDSP          |
|                                              |
| 5_Signal_generator.ino                       |
| This example we use the DSP as a crude       |
| Signal generator.                            |
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
  Serial.println(F("5_Signal_generator example\n"));

  // dsp and ee supports re-mapping of the SDA and SCL signals, but
  // can only be done if the microcontroller supports it (ESP8266 for example)
  dsp.begin(/* SDA, SCL */);
  //ee.begin(/* SDA, SCL */);

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

  // Set volume to -30dB
  dsp.volume_slew(MOD_SWVOL1_ALG0_TARGET_ADDR, -30);
}


void loop() 
{
  Serial.println(F("Sine sweep"));
  dsp.mux(MOD_WAVEFORM_SELECT_MONOSWSLEW_ADDR, 0);
  dsp.mux(MOD_SOURCE_SELECT_STEREOSWSLEW_ADDR, 0);
  for(uint16_t i = 0; i < 300; i++)
  {
    // Do a log sweep from 50Hz to 19050Hz in 300 steps
    frequency = pow(10, (log10(19000)/300*i))+50;
    dsp.sineSource(MOD_STATIC_SINE_ALG0_MASK_ADDR, frequency);
    delay(25);
  }

  Serial.println(F("Triangle sweep"));
  dsp.mux(MOD_WAVEFORM_SELECT_MONOSWSLEW_ADDR, 1);
  for(uint16_t i = 0; i < 300; i++)
  {
    // Do a log sweep from 50Hz to 19050Hz in 300 steps
    frequency = pow(10, (log10(19000)/300*i))+50;
    dsp.triangleSource(MOD_TRIANGLE_ALG0_TRI0_ADDR, frequency);
    delay(25);
  }

  Serial.println(F("Sawtooth sweep"));
  dsp.mux(MOD_WAVEFORM_SELECT_MONOSWSLEW_ADDR, 2);
  for(uint16_t i = 0; i < 300; i++)
  {
    // Do a log sweep from 50Hz to 19050Hz in 300 steps
    frequency = pow(10, (log10(19000)/300*i))+50;
    dsp.sawtoothSource(MOD_SAWTOOTH_ALG0_FREQ_ADDR, frequency);
    delay(25);
  }

  Serial.println(F("Square sweep"));
  dsp.mux(MOD_WAVEFORM_SELECT_MONOSWSLEW_ADDR, 3);
  for(uint16_t i = 0; i < 300; i++)
  {
    // Do a log sweep from 50Hz to 19050Hz in 300 steps
    frequency = pow(10, (log10(19000)/300*i))+50;
    dsp.squareSource(MOD_STATIC_SQUARE_ALG0_MASK_ADDR, frequency);
    delay(25);
  }

  Serial.println(F("Audio source"));
  dsp.mux(MOD_SOURCE_SELECT_STEREOSWSLEW_ADDR, 1);
  delay(10000);
}
