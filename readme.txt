MSF-Lite Microcontroller Software Framework.
Created by Jan Dolinay, 2014

Purpose
This is software library intended to make it easier to work with supported 
boards (and/or MCUs).
The aim is to provide unified API for common functions. 
There is also Arduino compatibility layer which allows using the Arduino API functions,
as Arduino can be considered the closes thing to common microcontroller API out there now.
 

License
The files located in this package are licensed under GNU LESSER GENERAL PUBLIC LICENSE
unless otherwise noted in the file itself.
Some files in this package may be covered by other licenses, especially the files
in the \vendor directory.  


Directory structure
board 		- code specific for given board (in sub-directory)
common 		- code shared by all boards (platforms)
doc 		- documentation
examples	- complete projects with example programs
platfrom 	- code specific for given platform, for example Kinetis MCUs.
	<platform>	- contains source and headers for given platform
	 	<device> - header with definitions for one device from this platform         	
template	- template files for user-created projects and for extending MSF. 
    cw  - template files for CodeWarrior projects. For example the configuration
        file (msf_config.h) or startup code file modified for MSF.
    src - template files for extending MSF - template for a new module and driver.    			
vendor		- code provided by the MCU vendor, for example CMSIS code.


Supported boards
Freescale FRDM-KL25Z