/*
 * Created:        Tuesday, May 1, 2018 1:27:51 PM
 * Description:    1_first_order_EQ:IC 1 parameter RAM definitions.
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
#ifndef __1_FIRST_ORDER_EQ_IC_1_PARAM_H__
#define __1_FIRST_ORDER_EQ_IC_1_PARAM_H__


/* Module Gen 1st Order1 - General (1st order)*/
#define MOD_GEN1STORDER1_COUNT                         3
#define MOD_GEN1STORDER1_DEVICE                        "IC1"
#define MOD_GEN1STORDER1_ALG0_PARAMB00_ADDR            0
#define MOD_GEN1STORDER1_ALG0_PARAMB00_FIXPT           0xFF81A748
#define MOD_GEN1STORDER1_ALG0_PARAMB00_VALUE           SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-0.987082520837763)
#define MOD_GEN1STORDER1_ALG0_PARAMB00_TYPE            SIGMASTUDIOTYPE_FIXPOINT
#define MOD_GEN1STORDER1_ALG0_PARAMB10_ADDR            1
#define MOD_GEN1STORDER1_ALG0_PARAMB10_FIXPT           0x00800000
#define MOD_GEN1STORDER1_ALG0_PARAMB10_VALUE           SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1)
#define MOD_GEN1STORDER1_ALG0_PARAMB10_TYPE            SIGMASTUDIOTYPE_FIXPOINT
#define MOD_GEN1STORDER1_ALG0_PARAMA00_ADDR            2
#define MOD_GEN1STORDER1_ALG0_PARAMA00_FIXPT           0x007E58B8
#define MOD_GEN1STORDER1_ALG0_PARAMA00_VALUE           SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.987082520837763)
#define MOD_GEN1STORDER1_ALG0_PARAMA00_TYPE            SIGMASTUDIOTYPE_FIXPOINT

/* Module Gen 1st Order2 - General (1st order)*/
#define MOD_GEN1STORDER2_COUNT                         3
#define MOD_GEN1STORDER2_DEVICE                        "IC1"
#define MOD_GEN1STORDER2_ALG0_PARAMB00_ADDR            3
#define MOD_GEN1STORDER2_ALG0_PARAMB00_FIXPT           0xFF81A748
#define MOD_GEN1STORDER2_ALG0_PARAMB00_VALUE           SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-0.987082520837763)
#define MOD_GEN1STORDER2_ALG0_PARAMB00_TYPE            SIGMASTUDIOTYPE_FIXPOINT
#define MOD_GEN1STORDER2_ALG0_PARAMB10_ADDR            4
#define MOD_GEN1STORDER2_ALG0_PARAMB10_FIXPT           0x00800000
#define MOD_GEN1STORDER2_ALG0_PARAMB10_VALUE           SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1)
#define MOD_GEN1STORDER2_ALG0_PARAMB10_TYPE            SIGMASTUDIOTYPE_FIXPOINT
#define MOD_GEN1STORDER2_ALG0_PARAMA00_ADDR            5
#define MOD_GEN1STORDER2_ALG0_PARAMA00_FIXPT           0x007E58B8
#define MOD_GEN1STORDER2_ALG0_PARAMA00_VALUE           SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.987082520837763)
#define MOD_GEN1STORDER2_ALG0_PARAMA00_TYPE            SIGMASTUDIOTYPE_FIXPOINT

/* Module SW vol 1 - Single SW slew vol (adjustable)*/
#define MOD_SWVOL1_COUNT                               2
#define MOD_SWVOL1_DEVICE                              "IC1"
#define MOD_SWVOL1_ALG0_TARGET_ADDR                    6
#define MOD_SWVOL1_ALG0_TARGET_FIXPT                   0x00800000
#define MOD_SWVOL1_ALG0_TARGET_VALUE                   SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1)
#define MOD_SWVOL1_ALG0_TARGET_TYPE                    SIGMASTUDIOTYPE_FIXPOINT
#define MOD_SWVOL1_ALG0_STEP_ADDR                      7
#define MOD_SWVOL1_ALG0_STEP_FIXPT                     0x00000800
#define MOD_SWVOL1_ALG0_STEP_VALUE                     SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.000244140625)
#define MOD_SWVOL1_ALG0_STEP_TYPE                      SIGMASTUDIOTYPE_FIXPOINT

#endif
