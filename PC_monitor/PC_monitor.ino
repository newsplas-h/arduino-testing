#include <bitmap.h>
#include <Configuration_Settings.h>


//project scope
//create display LCD that cycles through cpu temperature, load and frequency by pushing a button
//note:: this project is HEAVILY based off of copy/paste code
//attempts to link up to pc temperature reporting: creating a form in vbnet, failed (thought it would be easier due to vb form experience, not as easy as i thought)
//2: use coretemp, program I am already using, to link and send data to serial port
//tried using coretemp from program that someone using arduino had made in 2013, no luck. now using included coretempplugin.h with the coretemp sdk provided on website
//trying gnat-stats
//figuring out how to use gnat stats in own program
//random pixels lit on screen? values not updating == not having display.display();

//ended up finally getting it to work, needed to edit gnat-stats provided sketch a lot. working on adapting that code now.

//finally got the temperature to display, however more than just the temps are being sent over. gotta figure that out.
//also, the display updates need to be adjusted. not sure how the original program updates without a hitch, no visible removal of lines whenever it's running.

//3 days total into the hardwareserialmonitor/gnat stats, took me 2 days to figure out how to display and I spent the last 2 days working on trying to find out why my gpu/cpu displays as well as the temp. I tried a few things however from trying to debug the entire provided program in vs as well as combing the entire arduino sketch, I haven't been able to figure it out
//going to look into a solution using python now.
//python program will be easily cross compatible, as opposed to hardwareserialmonitor which runs off of a lot of c++

#include <Adafruit_GFX.h>  
#include <Adafruit_SSD1306.h> 

#define enableActivityChecker
#define lastActiveDelay 8000

Adafruit_SSD1306 display;  

#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

//variables
String inputString = "";
boolean stringComplete = false;
boolean activeConn = false;
long lastActiveConn = 0;

void setup()  
{                
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  display.clearDisplay(); 
  display.setTextColor(WHITE); 
  display.setRotation(0);  
  display.setTextWrap(false);  
  display.display();
  Serial.begin(9600);
  inputString.reserve(200);
}  

void loop() 
{
  serialEvent();
  #ifdef enableActivityChecker;
    activityChecker();
  #endif

  //display.clearDisplay();
  
  lastActiveConn = millis();
  
  
  functionS();

  inputString = "";
  stringComplete = false;
  
}
//pulling functions here from displaystyle1


void functionS()
{
  display.fillRect(20,11,90,16,BLACK);
  
  int cpuStringStart = inputString.indexOf("C");
  //modified code to limit to 4 maximum characters, at least i think i did -- update, this did nothing at all
  //aiming to remove the instances that are not temperatures
  int cpuDegree = inputString.indexOf("c");
  int cpuStringLimit = inputString.indexOf("|"); 
  
  String cpuString1 = inputString.substring(cpuStringStart + 1, cpuDegree);
  String cpuString2 = inputString.substring(cpuDegree + 1, cpuStringLimit - 1);

  
  display.setFont(&FreeSans9pt7b); 
  display.setTextSize(0);
  display.setCursor(0,15);
  display.println("T");
  display.setCursor(25,25);
  display.print(cpuString1);
  
  #ifdef noDegree
    display.print("C");
  #else
    display.print((char)247);
    display.print("C");
  #endif

//adding all of the other arguments underneath, however I won't print them and see what happens
 
//  String cpuName = "";
//  int cpuNameStart = inputString.indexOf("CPU:");
//  int gpuNameStart = inputString.indexOf("GPU:");
//  int gpuNameEnd = inputString.indexOf("|", gpuNameStart);
//  String gpuName = inputString.substring(gpuNameStart, gpuNameEnd);
//    
//  int cpuCoreClockStart = inputString.indexOf("CHC") + 3;
//  int cpuCoreClockEnd = inputString.indexOf("|", cpuCoreClockStart);
//  String cpuClockString = inputString.substring(cpuCoreClockStart, cpuCoreClockEnd);
//
//  int gpuCoreClockStart = inputString.indexOf("GCC") + 3;
//  int gpuCoreClockEnd = inputString.indexOf("|", gpuCoreClockStart);
//  String gpuCoreClockString = inputString.substring(gpuCoreClockStart, gpuCoreClockEnd);
//
//  int gpuMemClockStart = inputString.indexOf("GMC") + 3;
//  int gpuMemClockEnd = inputString.indexOf("|", gpuMemClockStart);
//  String gpuMemClockString = inputString.substring(gpuMemClockStart, gpuMemClockEnd);
//
//  int gpuShaderClockStart = inputString.indexOf("GSC") + 3;
//  int gpuShaderClockEnd = inputString.indexOf("|", gpuShaderClockStart);
//  String gpuShaderClockString = inputString.substring(gpuShaderClockStart, gpuShaderClockEnd);
//
//  int gpuStringStart = inputString.indexOf("G", cpuStringLimit);
//  int gpuDegree = inputString.indexOf("c", gpuStringStart);
//  int gpuStringLimit = inputString.indexOf("|", gpuStringStart);
//  String gpuString1 = inputString.substring(gpuStringStart + 1, gpuDegree);
//  String gpuString2 = inputString.substring(gpuDegree + 1, gpuStringLimit - 1);

 
  
  display.display();
  
}

//----------------------------serial events, activity checker all copied directly from gnatstats, few parts removed

//serial events
void serialEvent()
{
  
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    Serial.print(Serial.read());
    inputString += inChar;
    if (inChar == '|') {
      stringComplete = true;
      
    }
  }
}


//activity checker, copied directly from gnatstats, removed rotation stuff
void activityChecker()
{
  if (millis() - lastActiveConn > lastActiveDelay)

    activeConn = false;
  else
    activeConn = true;
  if (!activeConn) {
//    display.invertDisplay(0);
    display.clearDisplay();
    display.display();
  }
}
