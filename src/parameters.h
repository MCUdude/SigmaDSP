#ifndef PARAMETERS_H
#define PARAMETERS_H

// Math constants
#define pi 3.1415926f


/**
 * @brief DSP function parameters. 
 *        This is where all the usable constant DSP parameters are located.
 *        You can use it like so: parameters::phase::inverted
 */
namespace parameters 
{
  enum filterType {
    peaking,
    parametric,
    lowShelf,
    highShelf,
    lowpass,
    highpass,
    bandpass,
    bandstop,
    butterworthLowpass,
    butterworthHighpass,
    besselLowpass,
    besselHighpass,
  };
 
  enum phase {
    deg_0       = 0,
    nonInverted = 0,
    deg_180     = 1,
    inverted    = 1,
  };
  
  enum state {
    off,
    on,
  };   
};


// Compressor typedef
typedef struct compressor_t 
{
  float threshold = 0.0; // -90 / +6 [dB]
  float ratio     = 1.0; // 1 - 100
  float rms_tc    = 1.0; // 1 - 500 [ms]
  float hold      = 0.0; // 0 - 500 [ms]
  float decay     = 0.0; // 0 - 2000 [ms]
  float postgain  = 0.0; // -30 / +24 [dB]
} compressor;
 

// Tone control typedef
typedef struct toneCtrl_t
{
  float boost_Bass_dB   = 0.0;    // [dB]
  float boost_Treble_dB = 0.0;    // [dB]
  float freq_Bass;                // [Hz]      
  float freq_Treble     = 5000.0; // [Hz]
  uint8_t phase         = parameters::phase::deg_0; // parameters::phase::deg_0/deg_180
  uint8_t state         = parameters::state::on;    // parameters::state::on/off
} toneCtrl;


// 1st order equalizer typedef
typedef struct firstOrderEQ_t
{
  float freq;                // Range 20-20000 [Hz]
  float gain         = 0.0;  // Range +/-15 [dB]
  uint8_t filterType = parameters::filterType::peaking; // parameters::filterType::[type]
  uint8_t phase      = parameters::phase::deg_0;        // parameters::phase::deg_0/deg_180
  uint8_t state      = parameters::state::on;           // parameters::state::on/off
} firstOrderEQ;


// 2nd order equalizer typedef
typedef struct secondOrderEQ_t
{
  float Q            = 1.41; // Parametric, Peaking, range 0-16
  float S            = 0.0;  // Slope (Shelving), range 0-2 (only used with lowShelf and highShelf)
  float bandwidth    = 0.0;  // Bandwidth in octaves, range 0-11 (only used with bandPass and bandStop)
  float boost        = 0.0;  // Range +/-15 [dB]
  float freq;                // Range 20-20000 [Hz]
  float gain         = 0.0;  // Range +/-15 [dB]
  uint8_t filterType = parameters::filterType::peaking; // parameters::filterType::[type]
  uint8_t phase      = parameters::phase::deg_0;        // parameters::phase::deg_0/deg_180
  uint8_t state      = parameters::state::on;           // parameters::state::on/off
} secondOrderEQ;

#endif
