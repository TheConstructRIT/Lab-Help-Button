# Lab-Help-Button
The lab help button is used to send a "help" signal from the 
print lab to the main lab.

# Button States
The button system can have the following states:
* Off - default state
* Requesting help - when pressed in the print lab, the light starts flashing in the main lab
* Cancel - when pressed in the print lab, the help signal is canceled
* Acknowledge - when pressed in the main lab, the button and main lab light stop blinking
	* When pressed on either end again, it returns to the off state

# Configuration
The LabHelpButtonCode/LabHelpButtonCode.ino has the following
configurable elements:
* 4x pin configuration (see below)
* `alarmButtonCycles` - how many times the main lab light blinks before buzzing
* `alarmLightCycleTimeMil` - the time in milliseconds between blinking and not blinking
* `buzzerOnTime` - how long the buzzer will be on in milliseconds
* `buttonCooldownTime` - the minimum time in milliseconds before presses can be acknogledged
* 3x singal codes
	* `helpChar` - Sends the "requesting help" state
	* `acknowledgeChar` - Sends the "acknowledge" state
	* `clearChar` - Sends the "cancel" and "off" states

# Pinout
The Arduino by default uses the following configuration:
* Pin 0 (TX) - RX on radio module
* Pin 1 (RX) - TX on radio module
* Pin 13 - light output
	* Configurable with `light`
* Pin 11 - button input
	* Configurable with `button`
* Pin A0 (14) - jump to determine print lab or main lab side
	* Configurable with `sideInd`
	* The jumper wire is used on to indicate the main lab side
* Pin A4 (18) - buzzer
	* Configurable with `buzzer`