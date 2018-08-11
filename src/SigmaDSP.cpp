#include "SigmaDSP.h"
#include "Arduino.h"

/*********************************************************
**                    PUBLIC METHODS                    **
*********************************************************/

/***************************************
Function: SigmaDSP()
Purpose:  Constructor of the class.
Inputs:   uint8_t i2cAddress; DSP i2c address
          uint8_t device;     The DSP type, e.g ADAU1701
          uint8_t resetPin;   pin to reset DSP (optional parameter)
Returns:  None
***************************************/
SigmaDSP::SigmaDSP(uint8_t i2cAddress, uint8_t device, int8_t resetPin) 
  : _dspAddress(i2cAddress), _deviceType(device), _resetPin(resetPin)
{

}


/***************************************
Function: begin()
Purpose:  Starts the i2c interface
Inputs:   uint8_t resetPin; IO pin to reset the DSP
          uint8_t sdaPin;   SDA pin (optional parameter)
          uint8_t sclPin;   SCL pin (optional parameter)
Returns:  None
***************************************/
void SigmaDSP::begin(uint8_t sdaPin, uint8_t sclPin)
{
    // Ignore SDA and SCL pins if AVR is used
    #if defined(__AVR__)
      (void)sdaPin;
      (void)sclPin;
      Wire.begin();
    #else
      Wire.begin(sdaPin, sclPin);
    #endif
  
  // Reset DSP if pin is present
  if(_resetPin >= 0)
  {
    pinMode(_resetPin, OUTPUT);
    digitalWrite(_resetPin, HIGH);
    reset();
  }  
}


/***************************************
Function: i2cClock()
Purpose:  Sets the i2c clock speed
Inputs:   uint32_t clock; clock speed in Hz
Returns:  None
***************************************/
void SigmaDSP::i2cClock(uint32_t clock)
{
  Wire.setClock(clock);
}


/***************************************
Function: i2cClock()
Purpose:  Physically resets the DSP
Inputs:   None
Returns:  None
***************************************/
void SigmaDSP::reset()
{
  if(_resetPin >= 0) // Only run of reset pin is present
  {
    digitalWrite(_resetPin, LOW);
    delay(200);
    digitalWrite(_resetPin, HIGH);
  }
}


/***************************************
Function: ping()
Purpose:  Sends a i2c ping message
Inputs:   none
Returns:  0 - success: ack received
          2 - error: address send, nack received
          3 - error: data send, nack received
          4 - error: unknown i2c error
***************************************/
uint8_t SigmaDSP::ping()
{
  Wire.beginTransmission(_dspAddress);
  return Wire.endTransmission();
}


/***************************************
Function: mux()
Purpose:  Controls a mux. The mux can only be a single address block, which
          is the clickless SW slew mux
Inputs:   uint16_t startMemoryAddress;  DSP memory address
          uint8_t index;                Index (the signal) you want to switch to (0, 1, 2, ...)
          uint8_t numberOfIndexes;      The total number of indexes (2 or more)
Returns:  None
***************************************/
void SigmaDSP::mux(uint16_t startMemoryAddress, uint8_t index, uint8_t numberOfIndexes)
{
  // Index number is actually not needed, but kept for compatibility
  (void)numberOfIndexes;
  
  // Create buffer to store the data
  uint8_t storeData[5];
  
  // Convert and write block
  intToFixed(index, storeData);
  
  safeload_writeRegister(startMemoryAddress, storeData, true);
}


/***************************************
Function: demux()
Purpose:  Controls a demux.
Inputs:   uint16_t startMemoryAddress;  DSP memory address
          uint8_t index;                Index (the signal) you want to switch to (0, 1, 2, ...)
          uint8_t numberOfIndexes;      The total number of indexes (2 or more)
Returns:  None
***************************************/
void SigmaDSP::demux(uint16_t startMemoryAddress, uint8_t index, uint8_t numberOfIndexes)
{
  // Create buffer to store the data
  uint8_t storeData[5];
  uint8_t i = 0;
  
  // Load leading zeros
  for(; i < index; i++)
  {
    floatToFixed(0, storeData);
    safeload_writeRegister(startMemoryAddress++, storeData, false);
  }
  
  // Load index
  floatToFixed(1, storeData);
  i++;
  safeload_writeRegister(startMemoryAddress++, storeData, (i == numberOfIndexes ? true: false));  
  
  // Load tailing zeros
  for(; i < numberOfIndexes; i++)
  {
    floatToFixed(0, storeData);
    safeload_writeRegister(startMemoryAddress++, storeData, (i == numberOfIndexes-1 ? true: false));
  }
}


/***************************************
Function: gain()
Purpose:  Adjusts the gain of a channel
Inputs:   uint16_t startMemoryAddress; DSP memory address
          float gain;                  Actual gain value (1 = 0dB gain)
          uint8_t channels;            Number of channels this gain cell has (default 1)
Returns:  None
***************************************/
void SigmaDSP::gain(uint16_t startMemoryAddress, float gain, uint8_t channels)
{
  // Create buffer to store the data
  uint8_t storeData[5];
  
  // Convert and write block
  floatToFixed(gain, storeData);
  
  for(uint8_t i = 0; i < channels - 1; i++)
    safeload_writeRegister(startMemoryAddress++, storeData, false);
    
  safeload_writeRegister(startMemoryAddress, storeData, true);
}


/***************************************
Function: volume_slew()
Purpose:  Volume control with slew
Inputs:   uint16_t startMemoryAddress; DSP memory address
          float dB;                    Volume to set in dB
          uint8_t slew;                slew rate (optional, default 12)
Returns:  None
***************************************/
void SigmaDSP::volume_slew(uint16_t startMemoryAddress, float dB, uint8_t slew)
{
  uint8_t storeData[5];
 
  float volume = pow(10, dB / 20); // 10^(dB / 20)
  int32_t slewrate = 0x400000 / (1 << (slew - 1)); // 0x400000/2^(slew - 1))
  
  floatToFixed(volume, storeData);
  safeload_writeRegister(startMemoryAddress, storeData, false);
  
  intToFixed(slewrate, storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, true);
}


/***************************************
Function: hardClip()
Purpose:  Hard clip with separate negative 
          and positive threshold
Inputs:   uint16_t startMemoryAddress; DSP memory address
          float highThreshold;         High threshold 0 -> 1.0
          float lowThreshold;          Low threshold -1.0 -> 0
Returns:  None
***************************************/
void SigmaDSP::hardClip(uint16_t startMemoryAddress, float highThreshold, float lowThreshold)
{
  uint8_t storeData[5];
  
  floatToFixed(highThreshold, storeData);
  writeRegister(startMemoryAddress, 5, storeData);
  //safeload_writeRegister(startMemoryAddress, storeData, false);
  
  floatToFixed(lowThreshold, storeData);
  writeRegister(++startMemoryAddress, 5, storeData);
  //safeload_writeRegister(++startMemoryAddress, storeData, true);
}


 /***************************************
Function: softClip()
Purpose:  Soft clip with adjustable curve.
          Higher alpha -> smoother clipping curve 
Inputs:   uint16_t startMemoryAddress; DSP memory address
          float alpha;                 Clipping coefficient (0.1 -> 10.0)
Returns:  None
***************************************/
void SigmaDSP::softClip(uint16_t startMemoryAddress, float alpha)
{
  uint8_t storeData[5];
  
  const float oneThird = 0.333;
  const float twoThird = 0.666;

  floatToFixed(alpha, storeData);
  safeload_writeRegister(startMemoryAddress, storeData, false);
  
  floatToFixed(1/alpha, storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, false);
  
  floatToFixed(oneThird, storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, false);
  
  floatToFixed(twoThird, storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, true);
}


/***************************************
Function: dcSource()
Purpose:  This function controls a DC source cell
Inputs:   uint16_t startMemoryAddress; DSP memory address
          float value;                 DC value level range +/-1.0    
Returns:  None
***************************************/ 
void SigmaDSP::dcSource(uint16_t startMemoryAddress, float value)
{
  uint8_t storeData[5];
  
  floatToFixed(value, storeData);
  safeload_writeRegister(startMemoryAddress, storeData, true);
}


/***************************************
Function: sineSource()
Purpose:  This function controls a sine source cell
Inputs:   uint16_t startMemoryAddress;   DSP memory address
          float frequency;               Frequency in Hz of the signal     
Returns:  None
***************************************/ 
void SigmaDSP::sineSource(uint16_t startMemoryAddress, float frequency)
{
  float value = (1.00/24000.00)*frequency;

  uint8_t storeData[5];

  intToFixed(0xff, storeData);
  safeload_writeRegister(startMemoryAddress, storeData, false);   // Mask
  
  floatToFixed(value, storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, false); // Increment
  
  floatToFixed(1.0, storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, true);  // On/off
}


/***************************************
Function: squareSource()
Purpose:  This function controls a square source cell
Inputs:   uint16_t startMemoryAddress;   DSP memory address
          float frequency;               Frequency in Hz of the signal     
Returns:  None
***************************************/ 
void SigmaDSP::squareSource(uint16_t startMemoryAddress, float frequency)
{
  sineSource(startMemoryAddress, frequency);  // Same algorithm as sine source
}


/***************************************
Function: sawtoothSource()
Purpose:  This function controls a sawtooth source cell
Inputs:   uint16_t startMemoryAddress;   DSP memory address
          float frequency;               Frequency in Hz of the signal     
Returns:  None
***************************************/
void SigmaDSP::sawtoothSource(uint16_t startMemoryAddress, float frequency)
{
  float value = (0.50/24000.00)*frequency;
  
  uint8_t storeData[5]; 

  floatToFixed(value, storeData);
  safeload_writeRegister(startMemoryAddress, storeData, false);  // Increment
  
  floatToFixed(1.0, storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, true); // On/off
}


/***************************************
Function: triangleSource()
Purpose:  This function controls a triangle source cell
Inputs:   uint16_t startMemoryAddress;   DSP memory address
          float frequency;               Frequency in Hz of the signal     
Returns:  None
***************************************/
void SigmaDSP::triangleSource(uint16_t startMemoryAddress, float frequency)
{
  float value = (0.50/24000.00)*frequency;

  uint8_t storeData[5];
  
  floatToFixed(0.00, storeData);
  safeload_writeRegister(startMemoryAddress, storeData, false);    // Triangle algorithm
  
  floatToFixed(1.00, storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, false);
  
  floatToFixed(0.00, storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, false);
  
  floatToFixed(-1.00, storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, false);
  
  intToFixed(0x00000003, storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, false); // Mask
  
  floatToFixed(value, storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, false); // Increment
  
  floatToFixed(1.0, storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, true);  // On/off
}



/***************************************
Function: audioDelay()
Purpose:  This function controls a delay cell
Inputs:   uint16_t startMemoryAddress;   DSP memory address
          float delay_ms;                Delay value
Returns:  None

Delay ranges:
0.0-42.6ms  (ADAU170x) @ 48kHz
0.0-21.3ms  (ADAU170x) @ 96kHz
0.0-10.6ms  (ADAU170x) @ 192kHz
0.0-170.6ms (ADAU140x) @ 48kHz
0.0-85.3ms  (ADAU140x) @ 96kHz
0.0-42.6ms  (ADAU140x) @ 192kHz

WARNING!!! Delays calculated are theoretical assuming you 
have 100% data memory available in your Sigma Studio design. 
Data memory is shared among other blocks in Sigma Studio so
in practice, this much data memory is not available to the user
because every block in a design uses a few data memory locations
for its processing. The SigmaStudio compiler manages the data
RAM and indicates if the number of addresses needed in the
design exceeds the maximum available.
***************************************/
void SigmaDSP::audioDelay(uint16_t startMemoryAddress, float delayMs)
{
  uint32_t ticks = (uint32_t)(delayMs*0.001/(1/FS));
  
  if(ticks > 2048) 
    ticks = 2048;
  
  // Create buffer to store converted data
  uint8_t storeData[5]; 
   
  // Convert and write block
  intToFixed(ticks, storeData); 
  safeload_writeRegister(startMemoryAddress, storeData, true);
}


/***************************************
Function: EQfirstOrder()
Purpose:  Adjusts a first order EQ
Inputs:   uint16_t startMemoryAddress;   DSP memory address
          firstOrderEQ_t &equalizer;     Equalizer param struct        
Returns:  None
***************************************/
void SigmaDSP::EQfirstOrder(uint16_t startMemoryAddress, firstOrderEQ_t &equalizer)
{
  float w0, gainLinear;
  float b0, b1, a1;
  float coefficients[3];

  w0=2*PI*equalizer.freq/FS; //2*PI*freq/FS
  gainLinear = pow(10,(equalizer.gain/20)); //10^(gain/20)

  switch(equalizer.filterType)
  {  
// Lowpass
    case parameters::filterType::lowpass:
    default:
      a1 = pow(2.7,-w0);
      b0 = gainLinear * (1.0 - a1);
      b1 = 0;
      break;

// Highpass
    case parameters::filterType::highpass:
      a1 = pow(2.7,-w0);
      b0 = gainLinear * a1;
      b1 = -a1 * gainLinear;
      break;
  }

  if(equalizer.state == parameters::state::on)
  {
    if(equalizer.phase == parameters::phase::nonInverted) // 0 deg
    {
      coefficients[0] = b0;
      coefficients[1] = b1;
      coefficients[2] = a1;
    }
    else //if(equalizer.phase == parameters::phase::inverted) // 180 deg
    {
      coefficients[0] = -1*b0;
      coefficients[1] = -1*b1;
      coefficients[2] = a1; // This coefficient does not change sign
    }
  }
  else //if(equalizer.state == parameters::state::off)
  {
    coefficients[0] = 1.00;
    coefficients[1] = 0.00;
    coefficients[2] = 0.00;
  }
  
  
  // Create buffer to store converted data
  uint8_t storeData[5]; 
   
  // Convert and write first block
  floatToFixed(coefficients[0], storeData);
  safeload_writeRegister(startMemoryAddress, storeData, false);
  
  // Convert and write second block
  floatToFixed(coefficients[1], storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, false);
  
  // Convert and write third block
  floatToFixed(coefficients[2], storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, true);
}


/***************************************
Function: EQsecondOrder()
Purpose:  Adjusts a second order EQ
Inputs:   uint16_t startMemoryAddress;   DSP memory address
          secondOrderEQ_t &equalizer;    Equalizer param struct        
Returns:  None
***************************************/
void SigmaDSP::EQsecondOrder(uint16_t startMemoryAddress, secondOrderEQ_t &equalizer)
{
  float A, w0, alpha, gainLinear;
  float b0, b1, b2, a0, a1, a2;
  float coefficients[4];

  A=pow(10,(equalizer.boost/40));           // 10^(boost/40)
  w0=2*PI*equalizer.freq/FS;                // 2*PI*freq/FS
  gainLinear = pow(10,(equalizer.gain/20)); // 10^(gain/20)

  switch(equalizer.filterType)
  {
// Parametric
    case parameters::filterType::parametric:
// Peaking
    case parameters::filterType::peaking:  
    default:  
      alpha = sin(w0)/(2*equalizer.Q);
      a0 =  1 + alpha/A;
      a1 = -2 * cos(w0);
      a2 =  1 - alpha/A;
      b0 = (1 + alpha*A) * gainLinear;
      b1 = -(2 * cos(w0)) * gainLinear;
      b2 = (1 - alpha*A) * gainLinear;
      break;

// Low shelf
    case parameters::filterType::lowShelf:
      alpha=sin(w0)/2*sqrt((A+1/A)*(1/equalizer.S-1)+2);
      a0 = (A+1)+(A-1)*cos(w0)+2*sqrt(A)*alpha;
      a1 = -2*((A-1)+(A+1)*cos(w0));
      a2 = (A+1)+(A-1)*cos(w0)-2*sqrt(A)*alpha;
      b0 = A*((A+1)-(A-1)*cos(w0)+2*sqrt(A)*alpha)*gainLinear;
      b1 = 2*A*((A-1)-(A+1)*cos(w0)) * gainLinear;
      b2 = A*((A+1)-(A-1)*cos(w0)-2*sqrt(A)*alpha)*gainLinear;
      break;

// High shelf
    case parameters::filterType::highShelf:
      alpha = sin(w0)/2 * sqrt((A + 1/A)*(1/equalizer.S - 1) + 2);
      a0 = (A+1) - (A-1)*cos(w0) + 2*sqrt(A)*alpha;
      a1 = 2*((A-1) - (A+1)*cos(w0));
      a2 = (A+1) - (A-1)*cos(w0) - 2*sqrt(A)*alpha;
      b0 = A*((A+1) + (A-1)*cos(w0) + 2*sqrt(A)*alpha) * gainLinear;
      b1 = -2*A*((A-1) + (A+1)*cos(w0)) * gainLinear;
      b2 = A*((A+1) + (A-1)*cos(w0) - 2*sqrt(A)*alpha) * gainLinear;
      break;

// Lowpass
    case parameters::filterType::lowpass:
      alpha = sin(w0)/(2*equalizer.Q);
      a0 = 1 + alpha;
      a1 = -2*cos(w0);
      a2 = 1 - alpha;
      b0 = (1 - cos(w0)) * (gainLinear/2);
      b1 = 1 - cos(w0)  * gainLinear;
      b2 = (1 - cos(w0)) * (gainLinear/2);
      break;

// Highpass
    case parameters::filterType::highpass:
      alpha = sin(w0)/(2*equalizer.Q);
      a0 = 1 + alpha;
      a1 = -2*cos(w0);
      a2 = 1 - alpha;
      b0 = (1 + cos(w0)) * (gainLinear/2);
      b1 = -(1 + cos(w0)) * gainLinear;
      b2 = (1 + cos(w0)) * (gainLinear/2);
      break;

// Bandpass
    case parameters::filterType::bandpass:
      alpha = sin(w0) * sinh(log(2)/(2 * equalizer.bandwidth * w0/sin(w0)));
      a0 = 1 + alpha;
      a1 = -2*cos(w0);
      a2 = 1 - alpha;
      b0 = alpha * gainLinear;
      b1 = 0;
      b2 = -alpha * gainLinear;
      break;

// Bandstop
    case parameters::filterType::bandstop: 
      alpha = sin(w0) * sinh(log(2)/(2 * equalizer.bandwidth * w0/sin(w0)));
      a0 = 1 + alpha;
      a1 = -2*cos(w0);
      a2 = 1 - alpha;
      b0 = 1 * gainLinear;
      b1 = -2*cos(w0) * gainLinear;
      b2 = 1 * gainLinear;
      break;

// Butterworth lowpass
    case parameters::filterType::butterworthLowpass:
      alpha = sin(w0) / 2.0 * 1/sqrt(2);
      a0 = 1 + alpha;
      a1 = -2*cos(w0);
      a2 = 1 - alpha;
      b0 = (1 - cos(w0)) * gainLinear / 2;
      b1 = 1 - cos(w0) * gainLinear;
      b2 = (1 - cos(w0)) * gainLinear / 2;
      break;

// Butterworth highpass
    case parameters::filterType::butterworthHighpass:
      alpha = sin(w0) / 2.0 * 1/sqrt(2);
      a0 = 1 + alpha;
      a1 = -2*cos(w0);
      a2 = 1 - alpha;
      b0 = (1 + cos(w0)) * gainLinear / 2;
      b1 = -(1 + cos(w0)) * gainLinear;
      b2 = (1 + cos(w0)) * gainLinear / 2;
      break;

// Bessel lowpass
    case parameters::filterType::besselLowpass:
      alpha = sin(w0) / 2.0 * 1/sqrt(3) ;
      a0 = 1 + alpha;
      a1 = -2*cos(w0);
      a2 = 1 - alpha;
      b0 = (1 - cos(w0)) * gainLinear / 2;
      b1 = 1 - cos(w0) * gainLinear;
      b2 = (1 - cos(w0)) * gainLinear / 2;
      break;
      
// Bessel highpass
    case parameters::filterType::besselHighpass:
      alpha = sin(w0) / 2.0 * 1/sqrt(3) ;
      a0 = 1 + alpha;
      a1 = -2*cos(w0);
      a2 = 1 - alpha;
      b0 = (1 + cos(w0)) * gainLinear / 2;
      b1 = -(1 + cos(w0)) * gainLinear;
      b2 = (1 + cos(w0)) * gainLinear / 2;
      break;
  }

  // For Sigma DSP implementation we need to normalize all the coefficients respect to a0
  // and inverting by sign a1 and a2
  if(a0 != 0.00 && equalizer.state == parameters::state::on)
  {
    if(equalizer.phase == parameters::phase::nonInverted) // 0 deg
    {
      coefficients[0] = b0/a0;
      coefficients[1] = b1/a0;
      coefficients[2] = b2/a0;
      coefficients[3] = -1*a1/a0;
      coefficients[4] = -1*a2/a0;
    }
    else //if(equalizer.phase == parameters::phase::inverted) // 180 deg
    {
      coefficients[0] = -1*b0/a0;
      coefficients[1] = -1*b1/a0;
      coefficients[2] = -1*b2/a0;
      coefficients[3] = -1*a1/a0; // This coefficient does not change sign!
      coefficients[4] = -1*a2/a0; // This coefficient does not change sign!
    }
  }
  else //if(equalizer.state == parameters::state::off)
  {
    coefficients[0] = 1.00;
    coefficients[1] = 0;
    coefficients[2] = 0;
    coefficients[3] = 0;
    coefficients[4] = 0;
  }


  // Create buffer to store converted data
  uint8_t storeData[5]; 
   
  // Convert and write first block
  floatToFixed(coefficients[0], storeData);
  safeload_writeRegister(startMemoryAddress, storeData, false);
  
  // Convert and write second block
  floatToFixed(coefficients[1], storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, false);
  
  // Convert and write third block
  floatToFixed(coefficients[2], storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, false);
  
  // Convert and write fourth block
  floatToFixed(coefficients[3], storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, false);
  
  // Convert and write fifth block
  floatToFixed(coefficients[4], storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, true);
}

 
/***************************************
Function: toneControl()
Purpose:  This function manages a baxandall low-high dual tone control
Inputs:   uint16_t startMemoryAddress;   DSP memory address
          toneCtrl_t &toneCtrl;          Tone control param struct        
Returns:  None
***************************************/
void SigmaDSP::toneControl(uint16_t startMemoryAddress, toneCtrl_t &toneCtrl)
{
  float tb,bb,wT,wB,Knum_T,Kden_T,Knum_B,Kden_B,alpha0,beta1,alpha1,beta2,alpha2,beta3,alpha3,beta4;
  float b0,b1,b2,a0,a1,a2;
  float coefficients[4];

  tb = pow(10, toneCtrl.boost_Treble_dB / 20.0);
  bb = pow(10, toneCtrl.boost_Bass_dB / 20.0);

  wT = tan(PI * toneCtrl.freq_Treble / FS);
  wB = tan(PI * toneCtrl.freq_Bass / FS);

  Knum_T = 2 / (1 + (1.0 / tb));
  Kden_T = 2 / (1 + tb);
  Knum_B = 2.0 / (1.0 + (1.0 / bb));
  Kden_B = 2.0 / (1.0 + bb);

  alpha0 = wT + Kden_T;
  beta1 = wT + Knum_T;
  alpha1 = wT - Kden_T;
  beta2 = wT - Knum_T;

  alpha2 = (wB*Kden_B) + 1;
  beta3 = (wB*Knum_B) - 1;
  alpha3 = (wB*Kden_B) - 1;
  beta4 = (wB*Knum_B) + 1;

  a0 = alpha0 * alpha2;
  a1 = (alpha0 * alpha3) + (alpha1 * alpha2);
  a2 = alpha1 * alpha3;
  b0 = beta1 * beta3;
  b1 = (beta1 * beta4) + (beta2 * beta3);
  b2 = beta2 * beta4;

  // For Sigma DSP implementation we need to normalize all the coefficients respect to a0
  // and inverting a1 and a2 by sign
  if(a0 != 0.00 && toneCtrl.state == parameters::state::on)
  {
    if(toneCtrl.phase == parameters::phase::nonInverted) // 0 deg
    {
      coefficients[0] = b0/a0;
      coefficients[1] = b1/a0;
      coefficients[2] = b2/a0;
      coefficients[3] = -1*a1/a0;
      coefficients[4] = -1*a2/a0;
    }
    else //if(toneCtrl.phase == parameters::phase::inverted) // 180 deg
    {
      coefficients[0] = -1*b0/a0;
      coefficients[1] = -1*b1/a0;
      coefficients[2] = -1*b2/a0;
      coefficients[3] = a1/a0;
      coefficients[4] = a2/a0;
    }
  }
  else //if(toneCtrl.state == parameters::state::off)
  {
    coefficients[0] = 1.00;
    coefficients[1] = 0;
    coefficients[2] = 0;
    coefficients[3] = 0;
    coefficients[4] = 0;
  }

  // Create buffer to store converted data
  uint8_t storeData[5]; 
   
  // Convert and write first block
  floatToFixed(coefficients[0], storeData);
  safeload_writeRegister(startMemoryAddress, storeData, false);
  
  // Convert and write second block
  floatToFixed(coefficients[1], storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, false);
  
  // Convert and write third block
  floatToFixed(coefficients[2], storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, false);
  
  // Convert and write fourth block
  floatToFixed(coefficients[3], storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, false);
  
  // Convert and write fifth block
  floatToFixed(coefficients[4], storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, true);
}


/**
 * This function control a state variable filter block
 * @param dspAddress - the physical I2C address (7-bit format)
 * @param address - the param address of the cell
 * @param frequency - frequency range: 1-19148 Hz
 * @param q - q range: 1.28:10
 */
void SigmaDSP::stateVariable(uint16_t startMemoryAddress, float freq, float q)
{
  // Create buffer to store converted data
  uint8_t storeData[5]; 
  
  float param1 = 2*sin(PI*freq/FS);
  float param2 = 1/q;

  // Convert and write
  floatToFixed(param1, storeData);
  safeload_writeRegister(startMemoryAddress, storeData, false);
  
  floatToFixed(param2, storeData);
  safeload_writeRegister(++startMemoryAddress, storeData, true);
}


/***************************************
Function: CompressorRMS()
Purpose:  This function calculates the curve and the other parameters of a rms compressor block
          Set ratio = 1 to disable compressor
Inputs:   uint16_t startMemoryAddress;   DSP memory address
          compressor_t &compressor;      Compressor param struct        
Returns:  None
***************************************/
void SigmaDSP::compressorRMS(uint16_t startMemoryAddress, compressor_t &compressor)
{
  uint8_t i,count;

  float curve[34];
  float x[34];
  float y[34];
  float delta = 0.00;
  float coeff = 0.00;

  float dbps = 0.00;
  float attack_par = 0.00;
  float hold_par = 0.00;
  float decay_par = 0.00;
  float postgain_par = 0.00;

  linspace(-90, 6, 34, x);

  count = 0;

  coeff = 1/compressor.ratio;
  for(i = 0; i < 34; i++) // This algorithm creates the curve with 1 1 1 1 1 1 1 0.9 0.8 0.72 0.64 and so on coefficients
  {
    if(x[i] >= compressor.threshold)
    {
      count++;
      if(count == 1)
        delta = x[i]*coeff-x[i]*1;
      y[i] = (x[i] * coeff)-delta;
    }
    else
      y[i] = x[i] * 1;
  }

  for(i = 0; i < 34; i++)  // Coefficients of the curve calculation
    curve[i] = (pow(10, y[i]/20)) / (pow(10, x[i]/20));  // Coefficients are the ratio between the linearized values of vect. y and x

  // Create buffer to store converted data
  uint8_t storeData[5]; 
   
  // Parameter load into Sigma DSP
  for(i = 0; i < 34; i++)
  {
    floatToFixed(curve[i], storeData);
    safeload_writeRegister(startMemoryAddress++, storeData, false);
  }
  
  // Conversion dBps -> ms
  // dBps = 121;
  // TCms = (20/(dBps*2.3))*1000

  // RMS TC (dB/s)
  dbps = (20/(compressor.attack*2.3))*1e3;
  attack_par = abs(1.0 - pow(10,(dbps/(10*FS))));
  floatToFixed(attack_par, storeData);
  safeload_writeRegister(startMemoryAddress++, storeData, true);

  #ifdef COMPRESSORWITHPOSTGAIN
    postgain_par = pow(10, compressor.postgain/40);
    floatToFixed(postgain_par, storeData);
    safeload_writeRegister(startMemoryAddress++, storeData, true);
  #endif

  // Hold
  hold_par = compressor.hold*FS/1000;
  floatToFixed(hold_par, storeData);
  safeload_writeRegister(startMemoryAddress++, storeData, true);

  // Decay (dB/s)
  dbps = (20/(compressor.decayMs*2.3))*1e3;
  decay_par = dbps/(96*FS);
  floatToFixed(decay_par, storeData);
  safeload_writeRegister(startMemoryAddress, storeData, true);
}


/***************************************
Function: CompressorPeak()
Purpose:  This function calculates the curve and the other parameters of a peak compressor block
          Set ratio = 1 to disable compressor
Inputs:   uint16_t startMemoryAddress;   DSP memory address
          compressor_t &compressor;      Compressor param struct        
Returns:  None
***************************************/
void SigmaDSP::compressorPeak(uint16_t startMemoryAddress, compressor_t &compressor)
{
  uint8_t i,count;

  float curve[33];
  float x[33];
  float y[33];
  float delta = 0.00;
  float coeff = 0.00;

  float dbps = 0.00;
  //float attack_par = 0.00;
  float hold_par = 0.00;
  float decay_par = 0.00;
  float postgain_par = 0.00;

  linspace(-90, 6, 33, x);

  count = 0;

  coeff = 1/compressor.ratio;
  for(i = 0; i < 33; i++) // This algorithm creates the curve with 1 1 1 1 1 1 1 0.9 0.8 0.72 0.64 and so on coefficients
  {
    if(x[i] >= compressor.threshold)
    {
      count++;
      if(count == 1)
        delta = x[i]*coeff-x[i]*1;
      y[i] = (x[i] * coeff)-delta;
    }
    else
      y[i] = x[i] * 1;
  }

  for(i = 0; i < 33; i++)  // Coefficients of the curve calculation
    curve[i] = (pow(10, y[i]/20)) / (pow(10, x[i]/20));  // Coefficients are the ratio between the linearized values of vect. y and x
  
  // Create buffer to store converted data
  uint8_t storeData[5]; 

  // Parameter load into Sigma DSP
  for(i = 0; i < 33; i++)
  {
    floatToFixed(curve[i], storeData);
    safeload_writeRegister(startMemoryAddress++, storeData, false);
  }

  #ifdef COMPRESSORWITHPOSTGAIN
    postgain_par = pow(10, compressor.postgain/40);
    floatToFixed(postgain_par, storeData);
    safeload_writeRegister(startMemoryAddress++, storeData, true);
  #endif

  // Hold
  hold_par = compressor.hold*FS/1000;
  floatToFixed(hold_par, storeData);
  safeload_writeRegister(startMemoryAddress++, storeData, true);

  // Decay (dB/s)
  dbps = (20/(compressor.decayMs*2.3))*1e3;
  decay_par = dbps/(96*FS);
  floatToFixed(decay_par, storeData);
  safeload_writeRegister(startMemoryAddress, storeData, true);
}


/***************************************
Function: safeload_writeRegister()
Purpose:  Writes 5 bytes of data to the parameter memory of the DSP, the first byte is 0x00
Inputs:   uint16_t startMemoryAddress;   DSP memory address
          uint8_t *data;                 Data array to write
          bool finished;                 Indicates if this is the last packet or not
Returns:  None
***************************************/
void SigmaDSP::safeload_writeRegister(uint16_t memoryAddress, uint8_t *data, bool finished)
{
  uint8_t buf[2];

  buf[0] = (memoryAddress >> 8) & 0xFF;
  buf[1] = memoryAddress & 0xFF;
  
  writeRegister(0x0815 + _safeload_count, 2, buf); // Load safeload address 0
  
  // Q: Why is the safeload registers five bytes long, while I'm loading four-byte parameters into the RAM using these registers?
  // A: The safeload registers are also used to load the slew RAM data, which is five bytes long. For parameter RAM writes using safeload,
  // the first byte of the safeload register can be set to 0x00.
    
  // Needs 5 bytes of data
  writeRegister(0x0810 + _safeload_count, 5, data); // Load safeload data 0
  
  _safeload_count++;
  
  if(finished == true || _safeload_count >= 5) // Max 5 safeload memory registers
  {
    buf[0] = 0x00;
    buf[1] = 0x3C;
    writeRegister(0x081C, 2, buf); // IST (initiate safeload transfer bit)
    _safeload_count = 0;
  }
}


/***************************************
Function: writeRegister()
Purpose:  Sends data to the DSP
Inputs:   uint16_t startMemoryAddress;   DSP memory address
          uint8_t length;                Number of bytes to write
          uint8_t *data;                 Data array to write
Returns:  None
***************************************/
void SigmaDSP::writeRegister(uint16_t memoryAddress, uint8_t length, uint8_t *data)
{
  uint8_t LSByte = (uint8_t)memoryAddress & 0xFF;
  uint8_t MSByte = memoryAddress >> 8;

  Wire.beginTransmission(_dspAddress); // Begin write
    
  Wire.write(MSByte); // Send high address
  Wire.write(LSByte); // Send low address
    
  for(uint8_t i = 0; i < length; i++)
    Wire.write(data[i]);  // Send all bytes in passed array
      
  Wire.endTransmission(); // Write out data to I2C and stop transmitting
}


/***************************************
Function: writeRegisterBlock()
Purpose:  Sends data to the DSP from PROGMEM
Inputs:   uint16_t startMemoryAddress;   DSP memory address
          uint16_t length;               Number of bytes to write
          const uint8_t *data;           Data array to write
          uint8_t registerSize;          Number of bytes each register can hold
Returns:  None
***************************************/
void SigmaDSP::writeRegisterBlock(uint16_t memoryAddress, uint16_t length, const uint8_t *data, uint8_t registerSize)
{
  uint8_t MSByte = 0;
  uint8_t LSByte = 0;
  uint16_t bytesSent = 0;
  
  // Run until all bytes are sent
  while(bytesSent < length)
  {  
    // Convert address to 8-bit
    MSByte = memoryAddress >> 8;
    LSByte = (uint8_t)memoryAddress & 0xFF;
    
    
    Wire.beginTransmission(_dspAddress);
    Wire.write(MSByte); // Send high address
    Wire.write(LSByte); // Send low address
    for(uint8_t i = 0; i < registerSize; i++) // Send n bytes
    {
      Wire.write(pgm_read_byte(&data[bytesSent])); 
      bytesSent++;
    }
    Wire.endTransmission();
    
    memoryAddress++; // Increase address
  }
}


/***************************************
Function: floatTofixed()
Purpose:  Converts a 5.23 float value to 5-byte HEX and stores it to a buffer
Inputs:   float value;      Value to convert
          uint8_t *buffer;  Buffer to store the converted data to
Returns:  None
***************************************/
void SigmaDSP::floatToFixed(float value, uint8_t *buffer)
{
  // Convert float to 4 byte hex
  int32_t fixedval = (value * ((int32_t)1 << 23));

  // Store the 4 bytes in the passed buffer
  buffer[0] = 0x00; // First must be empty
  buffer[1] = (fixedval >> 24) & 0xFF;
  buffer[2] = (fixedval >> 16) & 0xFF;
  buffer[3] = (fixedval >>  8) & 0xFF;
  buffer[4] = fixedval & 0xFF;
}


/***************************************
Function: intToFixed()
Purpose:  Converts a 28.0 integer value to 5-byte HEX and stores it to a buffer
Inputs:   int32_t value;      Value to convert
          uint8_t *buffer;    Buffer to store the converted data to
Returns:  None
***************************************/
void SigmaDSP::intToFixed(int32_t value, uint8_t *buffer)
{
  // Store the 4 bytes in the passed buffer
  buffer[0] = 0x00; // First must be empty
  buffer[1] = (value >> 24) & 0xFF;
  buffer[2] = (value >> 16) & 0xFF;
  buffer[3] = (value >>  8) & 0xFF;
  buffer[4] = value & 0xFF;
}


/***************************************
Function: floatToInt()
Purpose:  Converts a 5.23 float value to int 28.0
Inputs:   float value;    Value to convert
Returns:  int32_t;        Converted value
***************************************/
int32_t SigmaDSP::floatToInt(float value)
{
  // Convert float 5.23 to int 28.0
  return (value * ((int32_t)1 << 23));
}


/***************************************
Function: linspace()
Purpose:  Calculates the points of a linearly spaced vector
Inputs:   float x1;     Start point
          float x2;     End point
          float n;      number of points to calculate
          float *vect;  Array to store calculated data
Returns:  None
***************************************/
void SigmaDSP::linspace(float x1, float x2, float n, float *vect)
{
  float k = (abs(x1) + abs(x2))/(n);

  for(uint8_t i = 0; i < n; i++)
    vect[i] = x1+(k*i);
}
