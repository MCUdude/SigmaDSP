/***********************************************|
| SigmaDSP library                              |
| https://github.com/MCUdude/SigmaDSP           |
|                                               |
| 7_Signal_detect_readout.ino                   |
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

// Variables to hold the current signal detect value
uint32_t previousLevelLeft = 1;
uint32_t previousLevelRight = 1;
uint32_t currentLevelLeft = 0;
uint32_t currentLevelRight = 0;

void setup() 
{  
  Serial.begin(9600);
  Serial.println(F("7_Signal_detect_readout example\n"));

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
  // For some reaon, the noise filter in the DSP program causes
  // a somewhat loud click when the program is loaded.
  // This is prevented by muting and unmuting the ADC
  dsp.muteADC(true);
  Serial.print(F("\nLoading DSP program... "));
  loadProgram(dsp);
  Serial.println("Done!\n");
  dsp.muteADC(false);
  delay(1000);



  // Comment out the three code lines above and use this step instead if EEPROM is present
  // The last parameter in writeFirmware is the FW version, and prevents the MCU from overwriting on every reboot
  //ee.writeFirmware(DSP_eeprom_firmware, sizeof(DSP_eeprom_firmware), 0);
  //dsp.reset();
  //delay(2000); // Wait for the FW to load from the EEPROM
}


void loop() 
{
  // Read the signal detect block for each channel
  currentLevelLeft  = dsp.readBack(MOD_SIGNAL_DETECTION_READOUT_LEFT_ALG0_VAL0_ADDR,  MOD_SIGNAL_DETECTION_READOUT_LEFT_ALG0_VAL0_VALUES,  3);
  currentLevelRight = dsp.readBack(MOD_SIGNAL_DETECTION_READOUT_RIGHT_ALG0_VAL0_ADDR, MOD_SIGNAL_DETECTION_READOUT_RIGHT_ALG0_VAL0_VALUES, 3);

  // Print message if there is a change on one of the channels
  if(currentLevelLeft != previousLevelLeft || currentLevelRight != previousLevelRight)
  {
    previousLevelLeft  = currentLevelLeft;
    previousLevelRight = currentLevelRight;
    Serial.print(F("Signal on left channel: "));
    Serial.print((bool)!currentLevelLeft);
    Serial.print(F("\tSignal on right channel: "));
    Serial.println((bool)!currentLevelRight);
  }
  delay(1000);
}
