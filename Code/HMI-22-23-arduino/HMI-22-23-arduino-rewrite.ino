/*

#define LOW 0x0
#define HIGH 0x1


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

// Button controll variables
const byte buttonPin = 2;           // Input pin for external button
int currentScreenMode = 0;          // Current screen mode is used to track which screen to be displayed.
boolean buttonState = HIGH;         // Current state of the button
boolean lastButtonState = HIGH;     // Previous state of the button

// Time controll variables
uint32_t lapCount = 0;
uint32_t currentLapTime;            // 
uint32_t totalTime;                 // 
int buttonPressdownTime;       // Time returned from millis() when button press was initiated
uint32_t lastLapTime;               //
uint32_t completedLapTimeSum;       // Sum of all completed lap times
uint32_t averageLapTime;            //
uint32_t startCompensationTime;     // Time before the first button press when the program starts (To allow user to delay the start of the race)
uint32_t pressTimeIndex = 0;        // Index for action to be taken by the button press.

// Physical constants and wheel control
uint32_t wheelDiameter = 0.47;      // (m)
double Pi = 3.141592653589793;
const double  wheelCircumference = Pi * wheelDiameter ;   // m 
const byte wheelPin = 3;

uint32_t wheelHallTriggers = 0;     // Number of times the Hall sensor passes a magnet. 4 magnets per revolution
boolean wheelState = LOW;           // current state of the button
boolean lastWheelState = LOW; 
float velocity;
uint32_t lastHallTime = 0;          // Last time the hall sensor passed a magnet
float distance = 0;                 
float ERPM = 0;                     // Engine revolutions per minute


// Declare reset function at address 0, used to reset the arduino through software
void(* resetFunc) (void) = 0;

// Correctly formats and prints time starting at specified x and y positions
// precision 1 gives minutes, 2 also gives seconds, and 3 also gives tenths and hundreds
void lcdTimePrint(uint32_t inp_time, int xpos,int ypos, int precision)
{

  // Minutes
  if (precision > 0)
  {
    if (inp_time/60000 < 10)
    {
    lcd.setCursor(xpos,ypos); lcd.print(0);
    lcd.setCursor(xpos+1,ypos); lcd.print(inp_time/60000);
    }
    else
    {
    lcd.setCursor(xpos,ypos); lcd.print(inp_time/60000);
    }
  }

  // Seconds
  if (precision > 1)
  {
    lcd.setCursor(xpos+2,ypos); lcd.print(":");

    if ((inp_time/1000)%60 < 10)
    {
    lcd.setCursor(xpos+3,ypos); lcd.print(0);
    lcd.setCursor(xpos+4,ypos); lcd.print((inp_time/1000)%60);
    }
    else
    {
    lcd.setCursor(xpos+3,0); lcd.print((inp_time/1000)%60);
    }
  }

  // Tenths and hundreds
  if (precision > 2)
  {
    lcd.setCursor(xpos+5,ypos); lcd.print(":");

    if ((inp_time%1000)/10 < 10)
    {
    lcd.setCursor(xpos+6,ypos); lcd.print(0);
    lcd.setCursor(xpos+7,ypos); lcd.print((inp_time%1000)/10);
    }
    else
    {
    lcd.setCursor(xpos+6,0); lcd.print((inp_time%1000)/10);
    }
  }

}


void setup()
{

  Serial.begin(115200);
  
  // LCD initialization
  lcd.init();
  lcd.clear();
  lcd.backlight();
  
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(wheelPin, INPUT);
}

void loop()
{



  // Waiting for initial interaction from user
  while(startCompensationTime == 0)
  {
    lcd.setCursor(0,0); lcd.print("Press to Start");
    if (digitalRead(buttonPin) == LOW)
    { 
      startCompensationTime = millis();
      completedLapTimeSum = 0;
      lcd.clear();
    }
  }

  // Update systems total operation runtime and evaluate a new laptime
  totalTime = millis() - startCompensationTime;
  currentLapTime = totalTime - completedLapTimeSum;

  if (lapCount != 0) { averageLapTime = ((completedLapTimeSum)/lapCount); }
  
  
  //-------------------------- Should be moved to the lcd updating ----------------------/
  lcd.setCursor(15,0); 
  lcd.print(pressTimeIndex);
  //-------------------------------------------------------------------------------------/

  
  buttonState = digitalRead(buttonPin);

  if(buttonState != lastButtonState)
  {
    if (buttonState == LOW)
    {
      buttonPressdownTime = millis() - startCompensationTime;
    }
    
    else
    {
      
      if (pressTimeIndex == 0)
      {
        currentScreenMode++;          // Switch to next screen layout
        lcd.clear();
        if (currentScreenMode > 5) { currentScreenMode = 1; }   // If currentScreenMode exceeds the number of screen layouts, reset it back to first layout
      }
      else if (pressTimeIndex == 1)
      {
        lapCount++;
        completedLapTimeSum += currentLapTime;
        lastLapTime = currentLapTime;
        currentLapTime = 0;
      }
      else if (pressTimeIndex == 3) resetFunc();

      buttonPressdownTime = 0;
      pressTimeIndex = 0;
    }

    lastButtonState = buttonState;
  }
  else if(buttonState == LOW) calculatePressTimeIndex();



  lcdPrinter();
}


/// Calculates the pressTimeIndex value to descide action. Overwrites pressTimeIndex to largest compliant value
void calculatePressTimeIndex()
{
  if ((totalTime - buttonPressdownTime) < 500) { pressTimeIndex = 0; }    // Changes screen mode to the next mode.
  if ((totalTime - buttonPressdownTime) > 500) { pressTimeIndex = 1; }    // Registers a lap and starts a new one.
  if ((totalTime - buttonPressdownTime) > 3000)  { pressTimeIndex = 2; }    // Does nothing (intended for user error in operation)
  if ((totalTime - buttonPressdownTime) > 10000) { pressTimeIndex = 3; }    // Performs a software reset of the arduino
}

void lcdPrinter()
{
  switch (currentScreenMode) // choose what to display based on buttonPushCounter value
  {

    case 1:

      lcd.setCursor(0,0); lcd.print("CLT");
      lcdTimePrint(currentLapTime, 4, 0, 2);

      lcd.setCursor(0,1); lcd.print("Laps");
      lcd.setCursor(5,1); lcd.print(lapCount);
      break;

    case 2:
      lcd.setCursor(0,0); lcd.print("Avg");
      lcdTimePrint(averageLapTime, 4, 0, 3);
      lcd.setCursor(0,1); lcd.print("LLT");
      lcdTimePrint(lastLapTime, 4, 1, 3);

      break;

    case 3: 
      break;

    case 4:
      break;

    case 5:
      break;
  }
}


*/