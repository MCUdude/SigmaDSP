/*
 * Created:        Thursday, May 3, 2018 8:41:53 AM
 * Description:    5_Signal_generator:IC 1 parameter RAM definitions.
 *
 * This software is distributed in the hope that it will be useful,
 * but is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * This software may only be used to program products purchased from
 * Analog Devices for incorporation by you into audio products that
 * are intended for resale to audio product end users. This software
 * may not be distributed whole or in any part to third parties.
 *
 * Copyright ©2018 Analog Devices, Inc. All rights reserved.
 */
#ifndef __5_SIGNAL_GENERATOR_IC_1_PARAM_H__
#define __5_SIGNAL_GENERATOR_IC_1_PARAM_H__


/* Module Sawtooth - Sawtooth wave*/
#define MOD_SAWTOOTH_COUNT                             2
#define MOD_SAWTOOTH_DEVICE                            "IC1"
#define MOD_SAWTOOTH_ALG0_FREQ_ADDR                    1
#define MOD_SAWTOOTH_ALG0_FREQ_FIXPT                   0x00012C5F
#define MOD_SAWTOOTH_ALG0_FREQ_VALUE                   SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.00916666666666667)
#define MOD_SAWTOOTH_ALG0_FREQ_TYPE                    SIGMASTUDIOTYPE_FIXPOINT
#define MOD_SAWTOOTH_ALG0_ON_ADDR                      2
#define MOD_SAWTOOTH_ALG0_ON_FIXPT                     0x00800000
#define MOD_SAWTOOTH_ALG0_ON_VALUE                     SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1)
#define MOD_SAWTOOTH_ALG0_ON_TYPE                      SIGMASTUDIOTYPE_FIXPOINT

/* Module Sine - Sine Tone*/
#define MOD_SINE_COUNT                                 3
#define MOD_SINE_DEVICE                                "IC1"
#define MOD_STATIC_SINE_ALG0_MASK_ADDR                 3
#define MOD_STATIC_SINE_ALG0_MASK_FIXPT                0x000000FF
#define MOD_STATIC_SINE_ALG0_MASK_VALUE                SIGMASTUDIOTYPE_INTEGER_CONVERT(255)
#define MOD_STATIC_SINE_ALG0_MASK_TYPE                 SIGMASTUDIOTYPE_INTEGER
#define MOD_SINE_ALG0_INCREMENT_ADDR                   4
#define MOD_SINE_ALG0_INCREMENT_FIXPT                  0x000258BF
#define MOD_SINE_ALG0_INCREMENT_VALUE                  SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.0183333333333333)
#define MOD_SINE_ALG0_INCREMENT_TYPE                   SIGMASTUDIOTYPE_FIXPOINT
#define MOD_SINE_ALG0_ON_ADDR                          5
#define MOD_SINE_ALG0_ON_FIXPT                         0x00800000
#define MOD_SINE_ALG0_ON_VALUE                         SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1)
#define MOD_SINE_ALG0_ON_TYPE                          SIGMASTUDIOTYPE_FIXPOINT

/* Module Square - Square wave*/
#define MOD_SQUARE_COUNT                               3
#define MOD_SQUARE_DEVICE                              "IC1"
#define MOD_STATIC_SQUARE_ALG0_MASK_ADDR               6
#define MOD_STATIC_SQUARE_ALG0_MASK_FIXPT              0x000000FF
#define MOD_STATIC_SQUARE_ALG0_MASK_VALUE              SIGMASTUDIOTYPE_INTEGER_CONVERT(255)
#define MOD_STATIC_SQUARE_ALG0_MASK_TYPE               SIGMASTUDIOTYPE_INTEGER
#define MOD_SQUARE_ALG0_FREQ_ADDR                      7
#define MOD_SQUARE_ALG0_FREQ_FIXPT                     0x000258BF
#define MOD_SQUARE_ALG0_FREQ_VALUE                     SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.0183333333333333)
#define MOD_SQUARE_ALG0_FREQ_TYPE                      SIGMASTUDIOTYPE_FIXPOINT
#define MOD_SQUARE_ALG0_ON_ADDR                        8
#define MOD_SQUARE_ALG0_ON_FIXPT                       0x00800000
#define MOD_SQUARE_ALG0_ON_VALUE                       SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1)
#define MOD_SQUARE_ALG0_ON_TYPE                        SIGMASTUDIOTYPE_FIXPOINT

/* Module Triangle - Triangle wave*/
#define MOD_TRIANGLE_COUNT                             7
#define MOD_TRIANGLE_DEVICE                            "IC1"
#define MOD_TRIANGLE_ALG0_TRI0_ADDR                    9
#define MOD_TRIANGLE_ALG0_TRI0_FIXPT                   0x00000000
#define MOD_TRIANGLE_ALG0_TRI0_VALUE                   SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0)
#define MOD_TRIANGLE_ALG0_TRI0_TYPE                    SIGMASTUDIOTYPE_FIXPOINT
#define MOD_TRIANGLE_ALG0_TRI1_ADDR                    10
#define MOD_TRIANGLE_ALG0_TRI1_FIXPT                   0x00800000
#define MOD_TRIANGLE_ALG0_TRI1_VALUE                   SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1)
#define MOD_TRIANGLE_ALG0_TRI1_TYPE                    SIGMASTUDIOTYPE_FIXPOINT
#define MOD_TRIANGLE_ALG0_TRI2_ADDR                    11
#define MOD_TRIANGLE_ALG0_TRI2_FIXPT                   0x00000000
#define MOD_TRIANGLE_ALG0_TRI2_VALUE                   SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0)
#define MOD_TRIANGLE_ALG0_TRI2_TYPE                    SIGMASTUDIOTYPE_FIXPOINT
#define MOD_TRIANGLE_ALG0_TRI3_ADDR                    12
#define MOD_TRIANGLE_ALG0_TRI3_FIXPT                   0xFF800000
#define MOD_TRIANGLE_ALG0_TRI3_VALUE                   SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-1)
#define MOD_TRIANGLE_ALG0_TRI3_TYPE                    SIGMASTUDIOTYPE_FIXPOINT
#define MOD_STATIC_TRIANGLE_ALG0_MASK_ADDR             13
#define MOD_STATIC_TRIANGLE_ALG0_MASK_FIXPT            0x00000003
#define MOD_STATIC_TRIANGLE_ALG0_MASK_VALUE            SIGMASTUDIOTYPE_INTEGER_CONVERT(3)
#define MOD_STATIC_TRIANGLE_ALG0_MASK_TYPE             SIGMASTUDIOTYPE_INTEGER
#define MOD_TRIANGLE_ALG0_FREQ_ADDR                    14
#define MOD_TRIANGLE_ALG0_FREQ_FIXPT                   0x000258BF
#define MOD_TRIANGLE_ALG0_FREQ_VALUE                   SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.0183333333333333)
#define MOD_TRIANGLE_ALG0_FREQ_TYPE                    SIGMASTUDIOTYPE_FIXPOINT
#define MOD_TRIANGLE_ALG0_ON_ADDR                      15
#define MOD_TRIANGLE_ALG0_ON_FIXPT                     0x00800000
#define MOD_TRIANGLE_ALG0_ON_VALUE                     SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1)
#define MOD_TRIANGLE_ALG0_ON_TYPE                      SIGMASTUDIOTYPE_FIXPOINT

/* Module Gen1stOrder1 - General (1st order)*/
#define MOD_GEN1STORDER1_COUNT                         3
#define MOD_GEN1STORDER1_DEVICE                        "IC1"
#define MOD_GEN1STORDER1_ALG0_PARAMB00_ADDR            16
#define MOD_GEN1STORDER1_ALG0_PARAMB00_FIXPT           0x0076E845
#define MOD_GEN1STORDER1_ALG0_PARAMB00_VALUE           SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.928963354004467)
#define MOD_GEN1STORDER1_ALG0_PARAMB00_TYPE            SIGMASTUDIOTYPE_FIXPOINT
#define MOD_GEN1STORDER1_ALG0_PARAMB10_ADDR            17
#define MOD_GEN1STORDER1_ALG0_PARAMB10_FIXPT           0x00000000
#define MOD_GEN1STORDER1_ALG0_PARAMB10_VALUE           SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0)
#define MOD_GEN1STORDER1_ALG0_PARAMB10_TYPE            SIGMASTUDIOTYPE_FIXPOINT
#define MOD_GEN1STORDER1_ALG0_PARAMA00_ADDR            18
#define MOD_GEN1STORDER1_ALG0_PARAMA00_FIXPT           0x000980AF
#define MOD_GEN1STORDER1_ALG0_PARAMA00_VALUE           SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.0742396371978774)
#define MOD_GEN1STORDER1_ALG0_PARAMA00_TYPE            SIGMASTUDIOTYPE_FIXPOINT

/* Module Waveform_select - Mono Switch Nx1*/
#define MOD_WAVEFORM_SELECT_COUNT                      1
#define MOD_WAVEFORM_SELECT_DEVICE                     "IC1"
#define MOD_WAVEFORM_SELECT_MONOSWSLEW_ADDR            19
#define MOD_WAVEFORM_SELECT_MONOSWSLEW_FIXPT           0x00000000
#define MOD_WAVEFORM_SELECT_MONOSWSLEW_VALUE           SIGMASTUDIOTYPE_INTEGER_CONVERT(0)
#define MOD_WAVEFORM_SELECT_MONOSWSLEW_TYPE            SIGMASTUDIOTYPE_INTEGER

/* Module Waveform_gain - Linear Gain*/
#define MOD_WAVEFORM_GAIN_COUNT                        1
#define MOD_WAVEFORM_GAIN_DEVICE                       "IC1"
#define MOD_WAVEFORM_GAIN_GAIN1940ALGNS1_ADDR          20
#define MOD_WAVEFORM_GAIN_GAIN1940ALGNS1_FIXPT         0x00199999
#define MOD_WAVEFORM_GAIN_GAIN1940ALGNS1_VALUE         SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.2)
#define MOD_WAVEFORM_GAIN_GAIN1940ALGNS1_TYPE          SIGMASTUDIOTYPE_FIXPOINT

/* Module Source_select - Stereo Switch Nx2*/
#define MOD_SOURCE_SELECT_COUNT                        1
#define MOD_SOURCE_SELECT_DEVICE                       "IC1"
#define MOD_SOURCE_SELECT_STEREOSWSLEW_ADDR            21
#define MOD_SOURCE_SELECT_STEREOSWSLEW_FIXPT           0x00000000
#define MOD_SOURCE_SELECT_STEREOSWSLEW_VALUE           SIGMASTUDIOTYPE_INTEGER_CONVERT(0)
#define MOD_SOURCE_SELECT_STEREOSWSLEW_TYPE            SIGMASTUDIOTYPE_INTEGER

/* Module SW vol 1 - Single SW slew vol (adjustable)*/
#define MOD_SWVOL1_COUNT                               2
#define MOD_SWVOL1_DEVICE                              "IC1"
#define MOD_SWVOL1_ALG0_TARGET_ADDR                    22
#define MOD_SWVOL1_ALG0_TARGET_FIXPT                   0x00800000
#define MOD_SWVOL1_ALG0_TARGET_VALUE                   SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1)
#define MOD_SWVOL1_ALG0_TARGET_TYPE                    SIGMASTUDIOTYPE_FIXPOINT
#define MOD_SWVOL1_ALG0_STEP_ADDR                      23
#define MOD_SWVOL1_ALG0_STEP_FIXPT                     0x00000800
#define MOD_SWVOL1_ALG0_STEP_VALUE                     SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.000244140625)
#define MOD_SWVOL1_ALG0_STEP_TYPE                      SIGMASTUDIOTYPE_FIXPOINT

#endif
