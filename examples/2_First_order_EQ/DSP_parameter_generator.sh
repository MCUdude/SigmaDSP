#!/bin/bash
set -e 

#####################################################################
#             SIGMADSP PARAMETER FILE GENERATOR SCRIPT              #
#     Created by MCUdude - https://github.com/MCUdude/SigmaDSP      #
#                                                                   #
# This script will search for the following files in the current    #
# directory and all sub directories:                                #
#  [ProjectName]_IC_1_PARAM.h                                       #
#  [ProjectName]_IC_1.h                                             #
#  [ProjectName]_IC_2.h                                             #
#  E2Prom.Hex                                                       #
#                                                                   #
# Here's an example on how you may arrange your project structure:  #
#  My_Arduino_DSP_Project/                                          #
#    L My_Arduino_DSP_Project.ino                                   #
#    L DSP_parameter_generator.ps1                                  #
#    L Sigma_studio_files/                                          #
#      L [ProjectName].dspproj                                      #
#      L export/                                                    #
#        L [ProjectName]_IC_1_PARAM.h                               #
#        L [ProjectName]_IC_1.h                                     #
#        L [ProjectName]_IC_2.h                                     #
#        L E2Prom.Hex                                               #
#                                                                   #
# It's important that the DSP is named IC 1 and the EEPROM IC 2 in  #
# Sigma Studio, or else this script won't work.                     #
#                                                                   #
# Note that [ProjectName]_IC_2.h and E2Prom.Hex isn't mandatory     #
# files. You will not be able to load firmware to the EEPROM        #
# without these files though.                                       #
#                                                                   #
# Run  $ chmod +x DSP_parameter_generator.sh  to make this shell    #
# script executable.                                                #
#####################################################################


# First parameter - Parameter file
# Change the filename (*_IC_1_PARAM.h) if the script can't find the parameter file
DSP_PARAM_FILE=$(find $(dirname "$0") -name "*_IC_1_PARAM.h")

# Second parameter - EEPROM hex file (must end with *rom.hex)
# Change the filename (*rom.[Hh][Ee][Xx]) if the script can't find the parameter file
EEPROM_HEXFILE=$(find $(dirname "$0") -name "*rom.[Hh][Ee][Xx]")

# Third parameter - DSP program file
# Change the filename (*_IC_1.h) if the script can't find the parameter file
DSP_PROGRAM_FILE=$(find $(dirname "$0") -name "*_IC_1.h")

# Fourth parameter - EEPROM program file
# Change the filename (*_IC_2.h) if the script can't find the parameter file
EEPROM_PROGRAM_FILE=$(find $(dirname "$0") -name "*_IC_2.h")


# Check if all files are present or multiple files found. 
# Print the missing file names, throw an error and stop script if true
if [ "$EEPROM_PROGRAM_FILE" = "" ]; then echo -e "\x1B[43mWARNING! File [*_IC_2.h] not found!         "; FILE_ERROR=false;
elif [[ "$EEPROM_PROGRAM_FILE" = *[[:space:]]/* ]]; then echo -e "\x1B[101mMultiple [*_IC_2.h] files found!"; FILE_ERROR=true; fi
if [ "$EEPROM_HEXFILE" = "" ]; then echo -e "\x1B[43mWARNING! File [E2Prom.Hex] not found!\nEEPROM loading not possible.                "; FILE_ERROR=false;
elif [[ "$EEPROM_HEXFILE" = *[[:space:]]/* ]]; then echo -e "\x1B[101mMultiple [*rom.[Hh][Ee][Xx]] files found!"; FILE_ERROR=true; fi
if [ "$DSP_PARAM_FILE" = "" ]; then echo -e "\x1B[101mFile [*_IC_1_PARAM.h] not found!"; FILE_ERROR=true;
elif [[ "$DSP_PARAM_FILE" = *[[:space:]]/* ]]; then echo -e "\x1B[101mMultiple [*_IC_1_PARAM.h] files found"; FILE_ERROR=true; fi
if [ "$DSP_PROGRAM_FILE" = "" ]; then echo -e "\x1B[101mFile [*_IC_1.h] not found!"; FILE_ERROR=true;
elif [[ "$DSP_PROGRAM_FILE" = *[[:space:]]/* ]]; then echo -e "\x1B[101mMultiple [*_IC_1.h] files found!"; FILE_ERROR=true; fi
if [ "$FILE_ERROR" = true ]; then echo -e "\x1B[0m"; exit; fi


# Choose the DSP macros we need in order to control the program modules
echo -e "\x1B[0m"
echo -e "\x1B[7m\n   SIGMADSP SOURCE FILE GENERATOR SCRIPT    \n"
echo -e "\x1B[0mBuilding header..."
echo -e "\x1B[0mExtracting i2c address macros from"
echo -e "\x1B[1m$DSP_PROGRAM_FILE"
echo -e "\x1B[1m$EEPROM_PROGRAM_FILE"
echo -e "\x1B[31m"
awk -v timestamp="$(date +%d.%m.%Y\ %H:%M:%S)" '
BEGIN {

  # Add guards
  printf("#ifndef SIGMADSP_PARAMETERS_H\n")
  printf("#define SIGMADSP_PARAMETERS_H\n\n")
  
  # Include SigmaDSP library
  printf("#include <SigmaDSP.h>\n\n")
  
  # Print header
  printf("/****************************************************************************\n")
  printf("| File name: SigmaDSP_parameters.h                                          |\n")
  printf("| Generation tool: AWK + bash                                               |\n")
  printf("| Date and time: %s                                        |\n"      , timestamp)
  printf("|                                                                           |\n")
  printf("| ADAU1701 parameter and program file header                                |\n")
  printf("| SigmaDSP library and its content is developed and maintained by MCUdude.  |\n")
  printf("| https://github.com/MCUdude/SigmaDSP                                       |\n")
  printf("|                                                                           |\n")
  printf("| Huge thanks to the Aida DSP team who have reverse engineered a lot of the |\n")
  printf("| Sigma DSP algorithms and made them open source and available to everyone. |\n")
  printf("| This library would never have existed if it wasn\x27t for the Aida DSP team  |\n")
  printf("| and their incredible work.                                                |\n")
  printf("|                                                                           |\n")
  printf("| This file have been generated with the Sigmastudio_parameter_generator.sh |\n")
  printf("| script. This file contains all the DSP function block parameters and      |\n")
  printf("| addresses. It also contains the program that will be loaded to the        |\n")
  printf("| external EEPROM.                                                          |\n")
  printf("|                                                                           |\n")
  printf("| The *_COUNT macro holds the number of addresses in memory each complete   |\n")
  printf("| module takes.                                                             |\n")
  printf("|                                                                           |\n")
  printf("| The *_ADDR macro holds the current address for the module. Use this macro |\n")
  printf("| when changing the behaviour of the modules (EQs, volume etc.).            |\n")
  printf("|                                                                           |\n")
  printf("| The *_FIXFT macros holds the default value of the module. Use this when   |\n")
  printf("| restoring the default parameters.                                         |\n")
  printf("|                                                                           |\n")
  printf("| The DSP_eeprom_firmware[] array contains the DSP firmware, and can be     |\n")
  printf("| loaded using the writeFirmware method in the DSPEEPROM class.             |\n")
  printf("| When stored in the external i2c EEPROM, the firmware is automatically     |\n")
  printf("| loaded into the DSP on boot if the SELFBOOT pin is tied to Vcc.           |\n")
  printf("|                                                                           |\n")
  printf("| If you want to load the DSP firmware directly without using an external   |\n")
  printf("| EEPROM, you can simply run loadProgram() (located at the bottom of this   |\n")
  printf("| file) where you pass the SigmaDSP object as the only parameter.           |\n")
  printf("****************************************************************************/\n\n\n")
  
  printf("/* 7-bit i2c addresses */\n")
}

{
  # Track down the DSP and EEPROM i2c addresses and convert them into 7-bit
  if ($1 ~ "#define" && $2 ~ "DEVICE_ADDR_IC_1")
    printf($1 " DSP_I2C_ADDRESS (%.4s >> 1) & 0xFE\n", $3)
  
  if ($1 ~ "#define" && $2 ~ "DEVICE_ADDR_IC_2")  
    printf($1 " EEPROM_I2C_ADDRESS (%.4s >> 1) & 0xFE\n\n", $3)  
} ' "$DSP_PROGRAM_FILE" "$EEPROM_PROGRAM_FILE" > temp1



# Extract relevant comments and macros
echo -e "\x1B[0mExtract relevant macros from"
echo -e "\x1B[1m$DSP_PARAM_FILE"
echo -e "\x1B[31m"
awk '
{
  sub("\r","") # Get rid of all CR characters from the input file
  
  # Print out each module comment
  if ($0 ~ "/* Module.") 
    printf("\n" $0 "\n")
  
  # Print every line where column two ends with _COUNT
  if ($2 ~ "._COUNT") 
    printf($0 "\n\n")
  
  # Print every line where column two ends with _FIXPT
  if ($2 ~ "._FIXPT") 
    printf($0 "\n\n")
  
  # Print every line where column two ends with _ADDR
  if ($2 ~ "._ADDR")  
    printf($0 "\n")
} ' "$DSP_PARAM_FILE" > temp2



# Skip EEPROM array if file is not present
if [ "$EEPROM_HEXFILE" == "" ]; then 
  awk '' > temp3
else  
{
  # Generate C style array out from passed hex file
  echo -e "\x1B[0mExtract and format EEPROM array from"
  echo -e "\x1B[1m$EEPROM_HEXFILE"
  echo -e "\x1B[31m"
  awk '
  BEGIN {
    FS=" * , *"
    RS="^$"  
    printf("\n\n")
    printf("/* This array contains the entire DSP program,\nand should be loaded into the external i2c EEPROM */\n")
  }
  
  {
    printf("const uint8_t PROGMEM DSP_eeprom_firmware[" NF-1 "] =\n{\n")
    for(i = 1; i < NF; i++) 
    {
      sub("\r","") # Get rid of all CR characters from the input file
      printf("%s", $i ", ")
    }
  }

  END {
    printf("\n};")
  }' "$EEPROM_HEXFILE" > temp3
}
fi



# Generate program data
echo -e "\x1B[0mExtract and format arrays from:"
echo -e "\x1B[1m$DSP_PROGRAM_FILE"
echo -e "\x1B[31m"
awk '
BEGIN {
  printf("\n\n")
  j = 1
}

{
  sub("\r","\n") # Get rid of all CR characters from the input file
  
  if (NR >= 28) # Start from line 28
  {
    # Print out relevant comments
    if ($0 ~ "/* DSP." || $0 ~"/* Register.")
      printf("\n" $0 "\n")         
    
    if ($2 == "PROGRAM_SIZE_IC_1")
      printf("#define PROGRAM_SIZE " $3 "\n")
      
    if ($2 == "PROGRAM_ADDR_IC_1")
    {
      printf("#define PROGRAM_ADDR " $3 "\n")
      printf("#define PROGRAM_REGSIZE 5\n\n")
    }
        
    if ($1 == "ADI_REG_TYPE" && $2 ~ "Program_Data.")
      printf("const uint8_t PROGMEM DSP_program_data[PROGRAM_SIZE] = \n{\n")  
        
    if ($2 == "PARAM_SIZE_IC_1")
      printf("#define PARAMETER_SIZE " $3 "\n")
      
    if ($2 == "PARAM_ADDR_IC_1")
    {
      printf("#define PARAMETER_ADDR " $3 "\n")
      printf("#define PARAMETER_REGSIZE 4\n\n")
    }
    
    if ($1 == "ADI_REG_TYPE" && $2 ~ "Param_Data.")
      printf("const uint8_t PROGMEM DSP_parameter_data[PARAMETER_SIZE] = \n{\n")
         
    if ($1 == "ADI_REG_TYPE" && $2 ~ "R0_COREREGISTER.")
    {
      printf("#define CORE_REGISTER_R0_SIZE 2\n")
      printf("#define CORE_REGISTER_R0_ADDR 0x081C\n")
      printf("#define CORE_REGISTER_R0_REGSIZE 2\n\n")
      printf("const uint8_t PROGMEM DSP_core_register_R0_data[CORE_REGISTER_R0_SIZE] = \n{\n")
    }
    
    if ($2 == "R3_HWCONFIGURATION_IC_1_SIZE")
    {
      printf("#define HARDWARE_CONF_SIZE " $3 "\n")
      printf("#define HARDWARE_CONF_ADDR 0x081C\n")
      printf("#define HARDWARE_CONF_REGSIZE 1\n\n")
    }
    
    if ($1 == "ADI_REG_TYPE" && $2 ~ "R3_HWCONFIGURATION.")
      printf("const uint8_t PROGMEM DSP_hardware_conf_data[HARDWARE_CONF_SIZE] = \n{\n")
    
    if ($1 == "ADI_REG_TYPE" && $2 ~ "R4_COREREGISTER.")
    {
      printf("#define CORE_REGISTER_R4_SIZE 2\n")
      printf("#define CORE_REGISTER_R4_ADDR 0x081C\n")
      printf("#define CORE_REGISTER_R4_REGSIZE 2\n\n")
      printf("const uint8_t PROGMEM DSP_core_register_R4_data[CORE_REGISTER_R4_SIZE] = \n{\n")
    }
       
    # Print out all lines that thats with 0x
    if ($1 ~ "0x.")
      printf($0)
    
    # Print out all end brackets
    if ($1 ~ "}.")
      printf($0 "\n\n")
    
    # Escape  
    if ($1 == "*")
      {exit}
  }   
}

END {
  printf("\n")
  printf("/* Run this function to load DSP firmware directly */\n")
  printf("void loadProgram(SigmaDSP &myDSP)\n")
  printf("{\n")
  printf("  myDSP.writeRegisterBlock(CORE_REGISTER_R0_ADDR, CORE_REGISTER_R0_SIZE, DSP_core_register_R0_data, CORE_REGISTER_R0_REGSIZE);\n")
  printf("  myDSP.writeRegisterBlock(PROGRAM_ADDR, PROGRAM_SIZE, DSP_program_data, PROGRAM_REGSIZE);\n")
  printf("  myDSP.writeRegisterBlock(PARAMETER_ADDR, PARAMETER_SIZE, DSP_parameter_data, PARAMETER_REGSIZE);\n")
  printf("  myDSP.writeRegisterBlock(HARDWARE_CONF_ADDR, HARDWARE_CONF_SIZE, DSP_hardware_conf_data, HARDWARE_CONF_REGSIZE);\n")
  printf("  myDSP.writeRegisterBlock(CORE_REGISTER_R4_ADDR, CORE_REGISTER_R4_SIZE, DSP_core_register_R4_data, CORE_REGISTER_R4_REGSIZE);\n")
  printf("}\n\n")
}' "$DSP_PROGRAM_FILE" > temp4



# Concatenate the four generated files and turn them into SigmaDSP_parameters.h
echo -e "\x1B[0mConcatenating all temporary files into one..."
awk '
{
  printf($0 "\n")
}

END {
printf("\n#endif") 
} ' "temp1" "temp2" "temp3" "temp4" > "$(dirname "$0")/SigmaDSP_parameters.h"



# Remove temp files
echo -e "\x1B[0mDeleting all temporary files..."
rm temp1 temp2 temp3 temp4

# Echo finish
echo -e "\x1B[7m\n                   DONE                    \n"
echo -e "\x1B[0m"
