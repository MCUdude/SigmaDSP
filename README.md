# SigmaDSP
SigmaDSP is an intuitive library used for controlling an Analog Devices Sigma DSP over i2c using an Arduino or an Arduino compatible board.

  
# Table of contents
* [What is SigmaDSP and why do we need a library for it?](#what-is-sigmadsp-and-why-do-we-need-a-library-for-it)
* [Supported DSPs](#supported-dsps)
* [Exporting a SigmaStudio project for use with the SigmaDSP library](#exporting-a-sigmastudio-project-for-use-with-the-sigmadsp-library)
* [The parameter generator script](#the-parameter-generator-script)
* [Exporting a SigmaStudio project](#exporting-a-sigmastudio-project)
* [Executing the parameter generator script](#executing-the-parameter-generator-script)
  - [Windows](#windows)
  - [Mac/Linux](#maclinux)
   
## What is SigmaDSP and why do we need a library for it?
SigmaDSP is a product lineup of programble DSPs for audio use by Analog Devices Inc. SigmaDSPs are known for their easy to use programming environment SigmaStudio, where the firmware is built in a graphical enviroment where you add "blocks" or "modules" to the program flow. Such blocks may be volume sliders, EQs, signal generators and so on. This is great if you want to get started quickly, but sadly Analog Devices have not focused on how to control these module parameters from external devices. Some of the "modules" are even based on closed source alorithms and will (most likely) never be possible to control outside of SigmaStudio.

On the other side many of the modules are fairly easy to reverse engineer, and control of these can therefore be implemented in a library like this. Thanks the enourmous effort the [AidaDSP team](https://github.com/AidaDSP/AidaDSP) have put into this reverse engineering. This library would have existed if it weren't for them.
  
   
## Supported DSPs
Only the ADAU1701 is supported for now. I'm planing to add the ADAU1442/ADAU1445/ADAU1446 in the future.
  
  
## Exporting a SigmaStudio project for use with the SigmaDSP library
Even though Analog Devices haven't focused much on external control over i2c it is possible to export the SigmaStudio project. This result in a bunch of header files where important data and parameter macros are spread across these files. This makes it a pain to keep track of everything.

To solve this issue I've created a script that extract the necessary information in these files and formats it into a single header file that can be used with this library. I call this script *DSP_parameter_generator*. Windows users may use the PowerShell version (DSP_parameter_generator.ps1), while Mac and Linux users may use the shell version (DSP_parameter_generator.sh).
  
  
## The parameter generator script
First let's talk about what this script expects in order to function properly. The SigmaDSP does not contain a non-volatile memory, so the entire program must be loaded into RAM on boot. If the DSP is power cycled or gets reset, the program must be loaded again. This loading can be done in two ways and this library support both. See the [examples](https://github.com/MCUdude/SigmaDSP/tree/master/examples) on how this may be implemented.

* The first option is to store the entire DSP program in the microcontroller flash memory. The microcontroller will have to load the program over i2c when the DSP is powered or resat.
* The other option is to use an external i2c EEPROM memory such as the 24C64. SigmaDSPs are able to load frimware from an external i2c EEPROM into RAM on the condition that the EEPROM i2c address is known and the physical SELFBOOT pin on the DSP is tied high.

There are some rules you need to follow in order for the script to work as intended:
* The DSP in the SigmaStudio project __must__ be called *IC 1*
* The i2c EEPROM in the SigmaStudio project (if present) __must__ be called *IC 2*
* DO NOT edit any of the exported files. The script depends on lines and spaces from the exported files. Editing any of the files may cause the script to malfunction.
  
  
### Exporting a SigmaStudio project
This guide assumes you created your project based on the __Tutorial_0__ project file, which is a blank project with a few pre-configured settings.
* Start out by adding all the blocks you want in your project
* Locate your SigmaStudio project folder. Open this and create a folder called **export**
* **(Skip this step if not using EEPROM)** Open the Hardware configuration tab and add an external EEPROM device by draging the E2PROM symbol into the work space and connect it to USBi's second wire connector. Make sure the EEPROM size and address matches the actual address of the EEPROM. **The EEPROM module has to be named IC_2!**
* Compile the project by clicking the **Link Compile Download** button ![Link Compile Download](https://i.imgur.com/mINhSw0.png)
* Click the diskette icon and store the files in the **export** folder you just created ![Diskette icon](https://i.imgur.com/kLyUKFu.png)
* In the Hardware Configuration tab, right click on the SigmaDSP (called IC_1) and select the option "Write Latest Compilation to EEPROM. Don't worry if you get any connection error messages, we're only looking for the generated files.
* Open the project folder again and move the subfolder called [project name]_IC_2 into **export**.
* Copy the script (DSP_parameter_generator.ps1 or .sh) into the Arduino project folder

Now the project files are ready to be handled by the DSP parameter generator script. Continue reading for more information on how this is done.

### Executing the parameter generator script
As already mentioned, this script has to be placed in the Arduino project folder. It will search for exported project files in the same folder or in any subdirectory. A recommended folder structure for your Arduino project looks something like this:

```
My_Arduino_DSP_Project/
  ↳ My_Arduino_DSP_Project.ino
  ↳ DSP_parameter_generator.sh
  ↳ DSP_parameter_generator.ps1
  ↳ [SS_projectName]/
    ↳ [SS_projectName].dspproj
    ↳ export
      ↳ [SS_ProjectName]_IC_1_PARAM.h
      ↳ [SS_ProjectName]_IC_1.h
      ↳ [SS_ProjectName]_IC_2.h
      ↳ E2Prom.Hex                    
```

#### Windows
Windows users may use the *DSP_parameter_generator.ps1* PowerShell script. Simply double click the DSP_parameter_generator.ps1 file and it will output a header file in the same directory as the script is located. If you're not able to execute the scrit it may be because you'll have to enable execution of unsigned Powershell scripts. You can do this by seaching for Powershell, right clicking on it and select "Run as administrator". Then paste and run the following line:  
`set-executionpolicy remotesigned`  
Close the PowerShell window. You should now be able to run the script by double clicking it.

#### Mac/Linux
\*NIX users may use the *DSP_parameter_generator.sh* script, which is built using bash and awk. Simply double click the DSP_parameter_generator.h file and it will output a header file in the same directory as the script is located. If you can't execute the script it may be because the file itself isn't executable. Open your terminal, cd into the directory where the script is located. Paste and run the following line:  
`chmod +x DSP_parameter_generator.sh`  
Close the terminal window. You should now be able to run the script by double clicking it.


