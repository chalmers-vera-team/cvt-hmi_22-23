

// C++ code
//

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);


//#include <LiquidCrystal.h>  (library not required)


/////////// variables /////////////
const byte buttonPin = 2;
int buttonPushCounter = 0;   // counter for the number of button presses
boolean buttonState = HIGH;         // current state of the button
boolean lastButtonState = HIGH;     // previous state of the button
int buttonHoldCounter = 0;         // counter for number of button holds
uint32_t currentLapTime; 
uint32_t totalTime;         // currentLapTime
uint8_t sec;
uint8_t min;
uint32_t buttonPressDownTime;    // currentLapTime of button press (used for comparison with currentLapTime of button release )
uint32_t lastLapTime = 0;      // last laptime
uint32_t totalLapTime = 0;       // Sum of all laptimes excluding current        
uint32_t Avg;        // Average currentLapTime (millis)
uint32_t startCompensationTime;
uint32_t buttonActionIndex = 0;
uint32_t diameter = 0.47; // (m)
const float  wheelc = 3.14159265359 * diameter ; // m 
const byte wheelpin = 3; 
// uint32_t wheelRotations = 0;
uint32_t magnetRotation = 0;
boolean wheelState = LOW;         // current state of the button
boolean lastWheelState = LOW; 
float velocity; 
uint32_t RPT = 0; 
float distance = 0; 
float RPM = 0;


void(* resetFunc) (void) = 0;//declare reset function at address 0




void setup()
{
  //Serial.begin(9600); (only required for USB data transfer to computers, redundant in this case)
  lcd.init();
  lcd.clear();
  lcd.backlight();
  pinMode(buttonPin, INPUT_PULLUP);
  
}


void loop()
{
  currentLapTime = millis()-totalLapTime; // add totalTime - totlaptime = current laptime
  totalTime = millis(); //add timer
  Avg = ((totalLapTime-startCompensationTime)/buttonHoldCounter); 
  lcd.setCursor(15,0); lcd.print(buttonActionIndex);

  // read the pushbutton input pin:
  buttonState = digitalRead(buttonPin);



////////// Button Functionality //////////
  if (buttonState != lastButtonState)
  {
    if (buttonState == LOW)
    {
      buttonPressDownTime = totalTime;
    }

    if (buttonState == HIGH)
    {
      if (buttonActionIndex == 3)
      {
        resetFunc(); //call reset 
      }
      buttonActionIndex = 0;

      if (500 < totalTime-buttonPressDownTime)
      {
        if (3000 > totalTime-buttonPressDownTime)
        {
          if (buttonPushCounter == 0)
          {
            startCompensationTime = currentLapTime;
            totalLapTime = totalLapTime+startCompensationTime;
            buttonPushCounter++;
            lcd.clear();
            
          }
          else
          {
            buttonHoldCounter++;
            buttonPressDownTime = 0;
            lastLapTime = currentLapTime;
            currentLapTime = 0;
            totalLapTime = totalLapTime + lastLapTime;
          } 
        }   
      }

      if (totalTime-buttonPressDownTime < 500)
      {
        buttonPressDownTime = 0;

        if (buttonPushCounter == 0)
        {
          startCompensationTime = currentLapTime;
          totalLapTime = totalLapTime+startCompensationTime;
        }
        // if the current state is LOW then the button
        // went from off to on:
        buttonPushCounter++;  // add one to counter
        lcd.clear();  
        if (buttonPushCounter > 3) // if counter over 5 reset the counter to 1
        {
          buttonPushCounter = 1;
        } 
      }
    }
    // save the current state as the last state,
    //for next currentLapTime through the loop
    lastButtonState = buttonState;
  }

  if (buttonState == LOW)
  {
    if ((totalTime - buttonPressDownTime) > 500)
    {
      buttonActionIndex = 1;
    }
    if (totalTime - buttonPressDownTime > 3000)
    {
      buttonActionIndex = 2;
    }
    if (totalTime - buttonPressDownTime > 10000)
    {
      buttonActionIndex = 3;
    }
  }




  /////////// Distance and Velocity 
  if (wheelState != lastWheelState)
  {
    if (wheelState == HIGH)
    {
      
      magnetRotation++;
      velocity = wheelc / (4 * (totalTime - RPT) * 1000); // RPT = rotation previous currentLapTime 
      RPM = 1 / ((totalTime - RPT) / 1000 / 60 * 4);
      distance = magnetRotation * wheelc / 4;

      RPT = totalTime;
      
      

    }

    lastWheelState = wheelState;
  }  



///////// Print based on button counter ///////

  Serial.println(buttonPushCounter);
      switch (buttonPushCounter) // choose what to display based on buttonPushCounter value
      {
//////////////    case 0: starting screen, looping infinitely   ///////////////////
        case 0:
          lcd.setCursor(0,0); lcd.print("Starts on");
          lcd.setCursor(0,1); lcd.print ("buttonpress"); 
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
          lcd.setCursor(0,0); lcd.print("V=");
          lcd.setCursor(2,0); lcd.print(velocity);
          lcd.setCursor(0,1); lcd.print("D= ");
          lcd.setCursor(2,1); lcd.print(distance);
          lcd.setCursor(7,1); lcd.print("RPM=");
          lcd.setCursor(11,1); lcd.print(RPM);






          break;
/////////          
//////////////    case 2: Avg lap currentLapTime    ///////////////////
/////////////////// layout /////////////////////   
//////////////Avg=xx:xx:x    B//////////////////   Avg = Average lap currentLapTime
//////////////                //////////////////   B = Action of button
////////////////////////////////////////////////

/////////////////Case 2, average lap currentLapTime and tot currentLapTime spent on track /////////////////////////  
        case 2:
          lcd.setCursor(6,0); lcd.print(":");
          lcd.setCursor(0,0); lcd.print("Avg=");
          lcd.setCursor(9,0); lcd.print(":");
        

          

          if (buttonHoldCounter > 0)
          {
            lcd.setCursor(10,0); lcd.print((Avg%1000)/100);

            if (Avg/60000 < 10)
            {
            lcd.setCursor(4,0);
            lcd.print(0);
            lcd.setCursor(5,0);
            lcd.print(Avg/60000);
            }
            else
            {
            lcd.setCursor(4,0);
            lcd.print(Avg/60000);
            }

            if ((Avg/1000)%60 < 10)
            {
            lcd.setCursor(7,0);
            lcd.print(0);
            lcd.setCursor(8,0);
            lcd.print((Avg/1000)%60);
            }
            else
            {
            lcd.setCursor(7,0);
            lcd.print((Avg/1000)%60);
            }
          }
          else
          {
            lcd.setCursor(4,0); lcd.print(0);
            lcd.setCursor(5,0); lcd.print(0);
            lcd.setCursor(7,0); lcd.print(0);
            lcd.setCursor(8,0); lcd.print(0);
            lcd.setCursor(10,0); lcd.print(0);
          }

          lcd.setCursor(6,1); lcd.print(":");
          lcd.setCursor(0,1); lcd.print("Tot=");
          //lcd.setCursor(9,1); lcd.print(":");
        

          ////// currentLapTime spent on track/since first button press //////

          if (buttonHoldCounter >= 0)
          {
            //lcd.setCursor(10,1); lcd.print(((totalTime-startCompensationTime)%1000)/100);

            if ((totalTime-startCompensationTime)/60000 < 10)
            {
            lcd.setCursor(4,1);
            lcd.print(0);
            lcd.setCursor(5,1);
            lcd.print((totalTime-startCompensationTime)/60000);
            }
            else
            {
            lcd.setCursor(4,1);
            lcd.print((totalTime-startCompensationTime)/60000);
            }

            if (((totalTime-startCompensationTime)/1000)%60 < 10)
            {
            lcd.setCursor(7,1);
            lcd.print(0);
            lcd.setCursor(8,1);
            lcd.print(((totalTime-startCompensationTime)/1000)%60);
            }
            else
            {
            lcd.setCursor(7,1);
            lcd.print(((totalTime-startCompensationTime)/1000)%60);
            }   
          }
          else
          {
            lcd.setCursor(4,1); lcd.print(0);
            lcd.setCursor(5,1); lcd.print(0);
            lcd.setCursor(7,1); lcd.print(0);
            lcd.setCursor(8,1); lcd.print(0);
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
          sec = (currentLapTime/1000)%60; 
          min = (currentLapTime/60000);
          lcd.setCursor(11,1); lcd.print(":"); // 
          lcd.setCursor(0,0); lcd.print("CL=");
          lcd.setCursor(5,0); lcd.print(":");
          lcd.setCursor(3,1); lcd.print(buttonHoldCounter);
          lcd.setCursor(0,1); lcd.print("LC=");
          lcd.setCursor(6,1); lcd.print("LL=");
          //lcd.setCursor(8,0); lcd.print(":");
          lcd.setCursor(14,1); lcd.print(":");

          
          
///////////display last lap minutes correctly///////////////
        
          if (lastLapTime/60000 < 10)
          {
            lcd.setCursor(9,1);
            lcd.print(0);
            lcd.setCursor(10,1);
            lcd.print(lastLapTime/60000);
          }
          else
          {
            lcd.setCursor(9,1);
            lcd.print(lastLapTime/60000);
          }
          
///////////display last lap seconds correctly//////////          
          if ((lastLapTime/1000)%60 < 10)
          {
            lcd.setCursor(12,1);
            lcd.print(0);
            lcd.setCursor(13,1);
            lcd.print((lastLapTime/1000)%60);
          }
          else
          {
            lcd.setCursor(12,1);
            lcd.print((lastLapTime/1000)%60);
          }       

////////////display last lap milliseconds correctly //////////////          

            lcd.setCursor(15,1); lcd.print((lastLapTime%1000/100));


///////////  Current lap minute calculation and display    /////////////////////
          if (min < 10)
          {
            lcd.setCursor(3,0);
            lcd.print(0);
            lcd.setCursor(4,0);
            lcd.print(min);
          }
          else
          {
            lcd.setCursor(3,0);
            lcd.print(min);
          }
///////////  Current lap seconds calculation and display  /////////////////////          
          if (sec < 10)
          {
            lcd.setCursor(6,0);
            lcd.print(0);
            lcd.setCursor(7,0);
            lcd.print(sec);
          }
          else
          {
            lcd.setCursor(6,0);
            lcd.print(sec);
          }
          
/////////// display current lap miliseconds correctly ///////////////

            //lcd.setCursor(9,0); lcd.print((currentLapTime%1000/100));

          break;
              
          
      }
}

//*/
