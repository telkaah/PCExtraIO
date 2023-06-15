# PCExtraIO
![alt text](https://github.com/telkaah/PCExtraIO/blob/main/projectImg.png)
![image](https://github.com/telkaah/PCExtraIO/assets/24934957/51ed5a2f-0393-4d28-a09b-0cba90aaf75a)
Arduino, Python and Autohotkey software to create a macro key box with screen

## Partlist:
* Arduino Pro Micro
* Oled SSD1306 128x32px
* Rotary encoder with pushbutton
* 3xSwitches

## ArudinoCode
* Handle input from switches and rotary encoder and send information as keystrokes to pc
* Handles input from PythonSystemMonitor and displays on 

## PythonSystemMonitor
Sends information to screen

## Autohotkey 
* Script to change audio output device
* Uses, ctrl+alt+shift+A or B to run scripts from arduino
* To match your setup down 1 and 3 should batch the position in the audio output list. 
* If you only have 2 devices these should be 1 and 2
