#pypcmonitor

Program that turns openhardwaremonitor data on windows into serial output for an arduino to read. This program is specifically made for the SSD1306 module, and won't look proper on other oled displays that aren't the same resolution.

**Usage**
_________
Upload the sketch to your arduino, and run the python file in administrator mode so that you can access CPU temperature.

**Limitations**
________________
Currently only runs on windows, as it relies on the openhardwaremonitorlib.dll


**Future Adjustments**
______________________
*Add GPU screen to arduino code, either by uploading a seperate program or pressing a button to change windows
*Add Linux support, using psutil rather than a .dll
*Add instructions for windows users to run program on startup
