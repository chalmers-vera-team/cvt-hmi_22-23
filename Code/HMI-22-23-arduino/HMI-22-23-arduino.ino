

// C++ code
//

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);


//#include <LiquidCrystal.h>  (library not required)


/////////// variables /////////////
const byte buttonPin = 2;
bool buttonPressed = LOW;
int screenToDisplay = 0;         // counter for the number of button presses
bool buttonState = HIGH;      // current state of the button
bool lastButtonState = HIGH;  // previous state of the button
int lapCounter = 0;              // counter for number of button holds
uint32_t currentLapTime;
uint32_t totalTime;  // currentLapTime
uint8_t sec;
uint8_t min;
uint32_t buttonPressDownTime;  // currentLapTime of button press (used for comparison with currentLapTime of button release )
bool buttonPressDownTimeStarted = LOW;
uint32_t lastLapTime = 0;      // last laptime
uint32_t totalLapTime = 0;     // Sum of all laptimes excluding current
uint32_t Avg;                  // Average currentLapTime (millis)
uint32_t startCompensationTime;
uint32_t buttonActionIndex = 0;
double diameter = 0.47;                         // (m)
const float wheelc = 3.14159265359 * diameter;  // m
const byte wheelPin = 3;
// uint32_t wheelRotations = 0;
uint32_t magnetRotation = 0;
bool wheelState = LOW;  // current state of the button
bool lastWheelState = LOW;
float velocity;
uint32_t RPT = 0;
float distance = 0;
float RPM = 0;


void (*resetFunc)(void) = 0;  //declare reset function at address 0




void setup() 
{
  //Serial.begin(9600); (only required for USB data transfer to computers, redundant in this case)
  lcd.init();
  lcd.clear();
  lcd.backlight();
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(wheelPin, INPUT);

  attachInterrupt(digitalPinToInterrupt(wheelPin), onWheelHallTrigger, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPin), onButtonTrigger, CHANGE);
  screenToDisplay = 0;
  delay(1000);
}


void loop() 
{
  currentLapTime = millis() - totalLapTime;  // add totalTime - totlaptime = current laptime
  totalTime = millis();                      //add timer
  Avg = ((totalLapTime - startCompensationTime) / lapCounter);

  if (buttonPressed) 
  {
    if (screenToDisplay == 0)
    {
      startCompensationTime = currentLapTime;
      totalLapTime = startCompensationTime; // The total laptime is displaced with the start compensation time
      screenToDisplay++;
      lcd.clear();
    }

    if (buttonActionIndex == 3) 
    {
      resetFunc();  //call reset
    }

    if (buttonActionIndex == 2)
    {
      lapCounter++;
      lastLapTime = currentLapTime;
      currentLapTime = 0;
      totalLapTime = totalLapTime + lastLapTime;
    }

    if (buttonActionIndex == 1) 
    {
      // if the current state is LOW then the button
      // went from off to on:
      screenToDisplay++;  // add one to counter
      lcd.clear();
      if (screenToDisplay > 3)  // if counter over 3 reset the counter to 1
      {
        screenToDisplay = 1;
      }
    }

    buttonPressed = LOW;
  }

  if (buttonPressDownTimeStarted)
  {
    if (totalTime - buttonPressDownTime > 10000)
    {
      lcd.setCursor(15, 0);
      lcd.print(3);
    }
    else if (totalTime - buttonPressDownTime > 3000)
    {
      lcd.setCursor(15, 0);      
      lcd.print("C");
    }
    else if (totalTime - buttonPressDownTime > 500)
    {
      lcd.setCursor(15, 0);
      lcd.print(2);
    }
    else
    {
      lcd.setCursor(15, 0);
      lcd.print(1);
    }
  }


  /////////// Distance and Velocity
  if (wheelState == HIGH) 
  {

    wheelState = LOW;
    
    if (magnetRotation % 4 == 0)
    {
      velocity = 3.6 * wheelc / ((double) (totalTime - RPT) / 1000); // RPT = rotation previous currentLapTime 
      RPM = 1 / ((double) (totalTime - RPT) / 1000 / 60);
      distance = magnetRotation * wheelc / 4;

      RPT = totalTime;    
    }
    
    magnetRotation++;
  }



  ///////// Print based on screenToDisplay ///////
  switch (screenToDisplay)  // choose what to display based on screenToDisplay value
  {
      //////////////    case 0: starting screen, looping infinitely   ///////////////////
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("Starts on");
      lcd.setCursor(0, 1);
      lcd.print("buttonpress");
      break;
      //////////////    case 5: currently empty    ///////////////////
      //case 5:
      //lcd.setCursor(0,0); lcd.print("Sensor Page 5");
      //break;
      //////////////    case 4: currently empty    ///////////////////
      //case 4:
      //lcd.setCursor(0,0); lcd.print("Sensor Page 4");
      //break;
      //////////////    case 3: velocity, distance, RPM    ///////////////////
    case 3:
      lcd.setCursor(0, 0);
      lcd.print("V=");
      lcd.setCursor(2, 0);
      lcd.print(velocity);
      lcd.setCursor(0, 1);
      lcd.print("D= ");
      lcd.setCursor(2, 1);
      lcd.print(distance);
      lcd.setCursor(7, 1);
      lcd.print("RPM=");
      lcd.setCursor(11, 1);
      lcd.print(RPM);






      break;
      /////////
      //////////////    case 2: Avg lap currentLapTime    ///////////////////
      /////////////////// layout /////////////////////
      //////////////Avg=xx:xx:x    B//////////////////   Avg = Average lap currentLapTime
      //////////////                //////////////////   B = Action of button
      ////////////////////////////////////////////////

      /////////////////Case 2, average lap currentLapTime and tot currentLapTime spent on track /////////////////////////
    case 2:
      lcd.setCursor(6, 0);
      lcd.print(":");
      lcd.setCursor(0, 0);
      lcd.print("Avg=");
      lcd.setCursor(9, 0);
      lcd.print(":");




      if (lapCounter > 0) {
        lcd.setCursor(10, 0);
        lcd.print((Avg % 1000) / 100);

        if (Avg / 60000 < 10) {
          lcd.setCursor(4, 0);
          lcd.print(0);
          lcd.setCursor(5, 0);
          lcd.print(Avg / 60000);
        } else {
          lcd.setCursor(4, 0);
          lcd.print(Avg / 60000);
        }

        if ((Avg / 1000) % 60 < 10) {
          lcd.setCursor(7, 0);
          lcd.print(0);
          lcd.setCursor(8, 0);
          lcd.print((Avg / 1000) % 60);
        } else {
          lcd.setCursor(7, 0);
          lcd.print((Avg / 1000) % 60);
        }
      } else {
        lcd.setCursor(4, 0);
        lcd.print(0);
        lcd.setCursor(5, 0);
        lcd.print(0);
        lcd.setCursor(7, 0);
        lcd.print(0);
        lcd.setCursor(8, 0);
        lcd.print(0);
        lcd.setCursor(10, 0);
        lcd.print(0);
      }

      lcd.setCursor(6, 1);
      lcd.print(":");
      lcd.setCursor(0, 1);
      lcd.print("Tot=");
      //lcd.setCursor(9,1); lcd.print(":");


      ////// currentLapTime spent on track/since first button press //////

      if (lapCounter >= 0) {
        //lcd.setCursor(10,1); lcd.print(((totalTime-startCompensationTime)%1000)/100);

        if ((totalTime - startCompensationTime) / 60000 < 10) {
          lcd.setCursor(4, 1);
          lcd.print(0);
          lcd.setCursor(5, 1);
          lcd.print((totalTime - startCompensationTime) / 60000);
        } else {
          lcd.setCursor(4, 1);
          lcd.print((totalTime - startCompensationTime) / 60000);
        }

        if (((totalTime - startCompensationTime) / 1000) % 60 < 10) {
          lcd.setCursor(7, 1);
          lcd.print(0);
          lcd.setCursor(8, 1);
          lcd.print(((totalTime - startCompensationTime) / 1000) % 60);
        } else {
          lcd.setCursor(7, 1);
          lcd.print(((totalTime - startCompensationTime) / 1000) % 60);
        }
      } else {
        lcd.setCursor(4, 1);
        lcd.print(0);
        lcd.setCursor(5, 1);
        lcd.print(0);
        lcd.setCursor(7, 1);
        lcd.print(0);
        lcd.setCursor(8, 1);
        lcd.print(0);
        //lcd.setCursor(10,1); lcd.print(0);
      }

      break;
      //////////

      //////////////    case 1, Current Lap currentLapTime, Laps completed, Last Lap currentLapTime    ///////////////////
      //////////////    note that min > 99 breaks display visualization //////////////
      ////////////////////////////////////////////////
      //////////////CL=xx:xx:x     B//////////////////   CL = Current Lap currentLapTime
      //////////////LC=x  LL=xx:xx:x//////////////////   LC = Laps Completed, LL = Last Lap currentLapTime
      ////////////////////////////////////////////////   B = Action of button

      ////////////////////Case 1//////////////////////
    case 1:
      sec = (currentLapTime / 1000) % 60;
      min = (currentLapTime / 60000);
      lcd.setCursor(11, 1);
      lcd.print(":");  //
      lcd.setCursor(0, 0);
      lcd.print("CL=");
      lcd.setCursor(5, 0);
      lcd.print(":");
      lcd.setCursor(3, 1);
      lcd.print(lapCounter);
      lcd.setCursor(0, 1);
      lcd.print("LC=");
      lcd.setCursor(6, 1);
      lcd.print("LL=");
      //lcd.setCursor(8,0); lcd.print(":");
      lcd.setCursor(14, 1);
      lcd.print(":");



      ///////////display last lap minutes correctly///////////////

      if (lastLapTime / 60000 < 10) {
        lcd.setCursor(9, 1);
        lcd.print(0);
        lcd.setCursor(10, 1);
        lcd.print(lastLapTime / 60000);
      } else {
        lcd.setCursor(9, 1);
        lcd.print(lastLapTime / 60000);
      }

      ///////////display last lap seconds correctly//////////
      if ((lastLapTime / 1000) % 60 < 10) {
        lcd.setCursor(12, 1);
        lcd.print(0);
        lcd.setCursor(13, 1);
        lcd.print((lastLapTime / 1000) % 60);
      } else {
        lcd.setCursor(12, 1);
        lcd.print((lastLapTime / 1000) % 60);
      }

      ////////////display last lap milliseconds correctly //////////////

      lcd.setCursor(15, 1);
      lcd.print((lastLapTime % 1000 / 100));


      ///////////  Current lap minute calculation and display    /////////////////////
      if (min < 10) {
        lcd.setCursor(3, 0);
        lcd.print(0);
        lcd.setCursor(4, 0);
        lcd.print(min);
      } else {
        lcd.setCursor(3, 0);
        lcd.print(min);
      }
      ///////////  Current lap seconds calculation and display  /////////////////////
      if (sec < 10) {
        lcd.setCursor(6, 0);
        lcd.print(0);
        lcd.setCursor(7, 0);
        lcd.print(sec);
      } else {
        lcd.setCursor(6, 0);
        lcd.print(sec);
      }

      /////////// display current lap miliseconds correctly ///////////////

      //lcd.setCursor(9,0); lcd.print((currentLapTime%1000/100));

      break;
  }
}

//*/

void onButtonTrigger() 
{
  if(digitalRead(buttonPin) == HIGH)
  {    
    buttonPressed = HIGH;
    if (totalTime - buttonPressDownTime > 10000) buttonActionIndex = 3;
    else if (totalTime - buttonPressDownTime > 3000) buttonActionIndex = 0;
    else if (totalTime - buttonPressDownTime > 500) buttonActionIndex = 2;
    else buttonActionIndex = 1;
    
    buttonPressDownTimeStarted = LOW;
  }
  else
  {
    buttonPressDownTime = totalTime;
    buttonPressDownTimeStarted = HIGH;
  }
}

void onWheelHallTrigger() 
{
  wheelState = HIGH;
}
