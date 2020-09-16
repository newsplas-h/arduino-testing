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

//python solution finally solved for cpu temperature handling issue, had to find my own workaround. not sure why display isn't producing anything now though... something in the way of instruction
//serialevent not completing the string inside of it, not letting loop run --------- need to review loop and serial event reading current data
//not sure why stringComplete isn't going through, need to look into it more and find out exactly how loops work

//final update: developed own code and own way of going through strings to find data, the majority of difficulty in this project is based in python and not arduino.



#include <Adafruit_GFX.h>  
#include <Adafruit_SSD1306.h> 

#define OLED_RESET -1
#define enableActivityChecker
#define lastActiveDelay 8000

Adafruit_SSD1306 display;  

#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

//for button--------------------------------------------------------------------
const int togglePin = 2;
//set x to 0 so display only starts when program does
int x=1;
//variables---------------------------------------------------------------------
String inputString = "";
boolean stringComplete = false;
boolean activeConn = false;
long lastActiveConn = 0;

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];

String cpuString= "";


//----------------------------------------------------------------------------
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
  //button
  pinMode(togglePin, INPUT_PULLUP);
}  

//main loop----------------------------------------------------------------------------
void loop() 
{
  serialEvent();
  
  #ifdef enableActivityChecker;
    activityChecker();
  #endif
  lastActiveConn = millis();

//main info--------------------
// if button is pressed, add 1 to x. check if even/odd on each loop

  if (digitalRead(togglePin) == 0){
    display.clearDisplay();
    display.display();
    x=(x+1);
  }

  //if even
  if ((x % 2) == 0){
    if (stringComplete) {
      gpudata();
      inputString = "";
      stringComplete = false;
      //Serial.println(x);
    }
  }
  
  else{
    if (stringComplete) {
      cpudata();
      inputString = "";
      stringComplete = false;
      //Serial.println(x);
    }
  }
//end info---------------------
  delay(100);
}


//----------------------------serial events, activity checker all copied directly from gnatstats, few parts removed


void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '/') {
      stringComplete = true;
    }
  }
}

//print data: CPU------------------------------------------------------------
void cpudata() 
{
  display.fillRect(20,5,150,30,BLACK);
  display.setFont(&FreeSans9pt7b); 
  display.setTextSize(0);
  display.setCursor(0,15);
  display.println("CPU");
  display.setCursor(35,30);
  int cpuStringStart = inputString.indexOf("C");
  int cpuStringLimit = inputString.indexOf("|");
  String cpuString = inputString.substring(cpuStringStart + 1, cpuStringLimit);
  display.print(cpuString);
  display.display();
  
}


//print data: GPU-------------------------------------------------------------
void gpudata() 
{
  display.fillRect(20,5,150,30,BLACK);
  display.setFont(&FreeSans9pt7b); 
  display.setTextSize(0);
  display.setCursor(0,15);
  display.println("GPU");
  display.setCursor(35,30);
  int gpuStringStart = inputString.indexOf("G");
  int gpuStringLimit = inputString.indexOf("^");
  String gpuString = inputString.substring(gpuStringStart + 1, gpuStringLimit);
  display.print(gpuString);
  display.display();
  
}


//activity checker, copied directly from gnatstats, removed rotation stuff---------------------
void activityChecker()
{
  if (millis() - lastActiveConn > lastActiveDelay)

    activeConn = false;
  else
    activeConn = true;
  if (!activeConn) {
    display.clearDisplay();
    display.display();
  }
}
