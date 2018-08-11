/*
 * Created:        Sunday, May 6, 2018 2:10:30 PM
 * Description:    4_State_variable_filter:IC 1 parameter RAM definitions.
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
 *
 *
 */
#ifndef __4_STATE_VARIABLE_FILTER_IC_1_PARAM_H__
#define __4_STATE_VARIABLE_FILTER_IC_1_PARAM_H__


/* Module StateVarFilter1 - State Variable*/
#define MOD_STATEVARFILTER1_COUNT                      2
#define MOD_STATEVARFILTER1_DEVICE                     "IC1"
#define MOD_STATEVARFILTER1_ALG0_STATEVARALG1FREQ_ADDR 0
#define MOD_STATEVARFILTER1_ALG0_STATEVARALG1FREQ_FIXPT 0x0010BE42
#define MOD_STATEVARFILTER1_ALG0_STATEVARALG1FREQ_VALUE SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.130806258460286)
#define MOD_STATEVARFILTER1_ALG0_STATEVARALG1FREQ_TYPE SIGMASTUDIOTYPE_FIXPOINT
#define MOD_STATEVARFILTER1_ALG0_STATEVARALG1ONEOVERQ_ADDR 1
#define MOD_STATEVARFILTER1_ALG0_STATEVARALG1ONEOVERQ_FIXPT 0x00B50BF2
#define MOD_STATEVARFILTER1_ALG0_STATEVARALG1ONEOVERQ_VALUE SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1.41442715700141)
#define MOD_STATEVARFILTER1_ALG0_STATEVARALG1ONEOVERQ_TYPE SIGMASTUDIOTYPE_FIXPOINT

/* Module Mux - Mono Switch Nx1*/
#define MOD_MUX_COUNT                                  1
#define MOD_MUX_DEVICE                                 "IC1"
#define MOD_MUX_MONOSWSLEW_ADDR                        2
#define MOD_MUX_MONOSWSLEW_FIXPT                       0x00000000
#define MOD_MUX_MONOSWSLEW_VALUE                       SIGMASTUDIOTYPE_INTEGER_CONVERT(0)
#define MOD_MUX_MONOSWSLEW_TYPE                        SIGMASTUDIOTYPE_INTEGER

/* Module SW vol 1 - Single SW slew vol (adjustable)*/
#define MOD_SWVOL1_COUNT                               2
#define MOD_SWVOL1_DEVICE                              "IC1"
#define MOD_SWVOL1_ALG0_TARGET_ADDR                    3
#define MOD_SWVOL1_ALG0_TARGET_FIXPT                   0x00800000
#define MOD_SWVOL1_ALG0_TARGET_VALUE                   SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1)
#define MOD_SWVOL1_ALG0_TARGET_TYPE                    SIGMASTUDIOTYPE_FIXPOINT
#define MOD_SWVOL1_ALG0_STEP_ADDR                      4
#define MOD_SWVOL1_ALG0_STEP_FIXPT                     0x00000800
#define MOD_SWVOL1_ALG0_STEP_VALUE                     SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.000244140625)
#define MOD_SWVOL1_ALG0_STEP_TYPE                      SIGMASTUDIOTYPE_FIXPOINT

#endif
