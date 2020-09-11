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



#include <Adafruit_GFX.h>  
#include <Adafruit_SSD1306.h> 

#define enableActivityChecker
#define lastActiveDelay 8000

Adafruit_SSD1306 display;  

#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

//variables---------------------------------------------------------------------
String inputString = "";
boolean stringComplete = false;
boolean activeConn = false;
long lastActiveConn = 0;

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];

char cpuUsage[numChars];
char cpuTemp[numChars];
char ramUsed[numChars];
char ramTotal[numChars];
char gpuUsage[numChars];
char gpuTemp[numChars];
char* ramUsage;
char* gpuTotal;
char* cpuTotal;


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
}  

//main loop----------------------------------------------------------------------------
void loop() 
{
  serialEvent();
  #ifdef enableActivityChecker;
    activityChecker();
  #endif
  lastActiveConn = millis();
  
  if (stringComplete) {
    display.setCursor(25,25);
    display.println("suck my balls mate");
    display.display();
    strcpy(tempChars, receivedChars);
    parseData();
    cpupercent();
    stringComplete = false;
    free(ramUsage);
    free(gpuTotal);
    free(cpuTotal);
  }
  
  inputString = "";
  delay(100);
  
}


//----------------------------serial events, activity checker all copied directly from gnatstats, few parts removed

//serial events
void serialEvent()
{
  static boolean inProgress = false;
  static byte index = 0;
  char startMarker = '*';
  char endMarker = '^';
  char curr;
  
  while (Serial.available() > 0 && stringComplete == false) {
    curr = Serial.read();
    if (inProgress) {
      if (curr != endMarker) {
        receivedChars[index] = curr;
        index++;
      }
      else {
        receivedChars[index] = '\0';
        inProgress = false;
        index = 0;
        stringComplete = true;
      }
    } 
    else if (curr == startMarker) inProgress = true;
  }
}

//direct copy------------------------------------------------------------
void parseData() {
  char* index;

  index = strtok(tempChars, ",");
  strcpy(cpuUsage, index);
  strcat(cpuUsage, "%");

  index = strtok(NULL, ",");
  strcpy(cpuTemp, index);
  strcat(cpuTemp, "C");

  index = strtok(NULL, ",");
  strcpy(ramUsed, index);
  strcat(ramUsed, "/");

  index = strtok(NULL, ",");
  strcpy(ramTotal, index);
  strcat(ramTotal, " GB");

  index = strtok(NULL, ",");
  strcpy(gpuTemp, index);
  strcat(gpuTemp, "C");

  index = strtok(NULL, ",");
  strcpy(gpuUsage, index);
  strcat(gpuUsage, "%, ");

  ramUsage = malloc(strlen(ramUsed) + strlen(ramTotal) + 1);
  memcpy(ramUsage, ramUsed, strlen(ramUsed));
  memcpy(ramUsage+strlen(ramUsed), ramTotal, strlen(ramTotal)+1);

  gpuTotal = malloc(strlen(gpuUsage) + strlen(gpuTemp) + 1);
  memcpy(gpuTotal, gpuUsage, strlen(gpuUsage));
  memcpy(gpuTotal+strlen(gpuUsage), gpuTemp, strlen(gpuTemp) + 1); 
}


//print data: CPU------------------------------------------------------------
void cpupercent()
{
  display.fillRect(20,11,90,16,BLACK);
  display.setFont(&FreeSans9pt7b); 
  display.setTextSize(0);
  display.setCursor(0,15);
  display.println("Load");
  display.setCursor(55,25);
  display.print(cpuUsage);
  display.display();
  
}

void cputemp()
{
  display.fillRect(20,11,90,16,BLACK);
  display.setFont(&FreeSans9pt7b); 
  display.setTextSize(0);
  display.setCursor(0,15);
  display.println("Temp");
  display.setCursor(55,25);
  display.print(cpuTemp);
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
//    display.invertDisplay(0);
    display.clearDisplay();
    display.display();
  }
}
