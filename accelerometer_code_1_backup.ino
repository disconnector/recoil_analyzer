
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_ADT7410.h"
#include <Adafruit_ADXL343.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


// Set the pins used
#define cardSelect 4
#define BUTTON_A  9
#define BUTTON_B  6
#define BUTTON_C  5

File logfile;

// blink out an error code
void error(uint8_t errno) {
  while(1) {
    uint8_t i;
    for (i=0; i<errno; i++) {
      digitalWrite(13, HIGH);
      delay(100);
      digitalWrite(13, LOW);
      delay(100);
    }
    for (i=errno; i<10; i++) {
      delay(200);
    }
  }

}

float tempC, accelX, accelY, accelZ;

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

// Create the ADT7410 temperature sensor object
Adafruit_ADT7410 tempsensor = Adafruit_ADT7410();

// Create the ADXL343 accelerometer sensor object
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);

unsigned long myTime = 0;
unsigned long timeOld = 0;
unsigned long elapseTime = 0;

//button setup
// constants won't change. They're used here to set pin numbers:
const int buttonPin = 9;    // the number of the pushbutton pin
const int ledPin = 13;      // the number of the LED pin

// Variables will change:
int ledState = HIGH;         // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers


void setup()
{
  // start the serial connection
  Serial.begin(115200);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  //display.display();
  //delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  display.display();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("---  McGillicutty ---");
  display.println("---- Engineering ----");
  display.println("");
  display.println("   v.1.1 4/11/2021");
 
  display.setCursor(0,0);
  display.display(); // actually display all of the above
  
delay(2000);

 pinMode(13, OUTPUT);
 // see if the card is present and can be initialized:
  if (!SD.begin(cardSelect)) {
    display.clearDisplay();
    display.display();
    display.println("CARD INIT FAILED");
    display.display();
    Serial.println("Card init. failed!");
    error(2);
  }
  char filename[15];
  strcpy(filename, "/SENSOR00.TXT");
  for (uint8_t i = 0; i < 100; i++) {
    filename[7] = '0' + i/10;
    filename[8] = '0' + i%10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }

  logfile = SD.open(filename, FILE_WRITE);
  if( ! logfile ) {
   // Serial.print("Couldnt create "); 
   // Serial.println(filename);
    display.clearDisplay();
    display.display();
    display.println("NO SD CARD?");
    display.display();
    Serial.println("Card init. failed!");
    error(3);
  }
  Serial.print("Writing to "); 
  Serial.println(filename);

  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
  // Serial.println("Ready!");

    //set up display

 
  display.clearDisplay();
  display.display();

 /* Initialise the ADXL343 */
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL343 ... check your connections */
    Serial.println("Ooops, no ADXL343 detected ... Check your wiring!");
    while(1);
  }

  /* Set the range to whatever is appropriate for your project */
  accel.setRange(ADXL343_RANGE_16_G);

  /* Initialise the ADT7410 */
  if(!tempsensor.begin())
  {
    // Serial.println("Couldn't find ADT7410!");
    while (1);
  }

  // sensor takes 250 ms to get first readings
  delay(250);
  
// set up temp sensor and print to data file
tempC = tempsensor.readTempC();
  // Serial.print("Temperature: "); Serial.print(tempC); Serial.println("C");
  logfile.print("New Run @"); logfile.println(tempC);

// pullup butttons on OLED
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);


//button setup

  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // set initial LED state
  digitalWrite(ledPin, ledState);

}


 


void loop()
{


 // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
      }
    }
  }

  // set the LED:
  digitalWrite(ledPin, ledState);
 
  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;

 //display.setCursor(0,0);
 // display.println("Running . . . ");
 // display.display(); // actually display all of the above

  //set up timer

myTime = millis();
// Serial.print("myTime = ");Serial.print(myTime); Serial.print("timeOld = ");Serial.println(timeOld); 

if (ledState == LOW) {
 
    // Serial.println("PAUSED");
    display.setTextSize(2);
    display.setCursor(0,0);
    display.println("");
    display.println("PAUSED");
    display.setCursor(0,0);
    display.display(); // actually display all of the above
    display.clearDisplay();
    display.display();
}

if (ledState == HIGH) {
    /*display.clearDisplay();
    display.display();
    Serial.println("RUNNING");
    display.setTextSize(2);
    display.setCursor(0,0);
    display.println("");
    display.println("RUNNING");
    display.setCursor(0,0);
    display.display(); // actually display all of the above */
    
     if(myTime - timeOld >= 9) // If its been 100 ms or more...
      {
     
    
       /* Get a new accel. sensor event */
      sensors_event_t event;
      accel.getEvent(&event);
    
      accelX = event.acceleration.x;
      accelY = event.acceleration.y;
      accelZ = event.acceleration.z;
    
      /* Display the results (acceleration is measured in m/s^2) */
      elapseTime = (myTime - timeOld);
      Serial.print(elapseTime);  Serial.print("---");
      //Serial.print(myTime); Serial.print("  "); Serial.print(
      // Serial.print("X: "); Serial.print(accelX); Serial.print("  ");
      // Serial.print("Y: "); Serial.print(accelY); Serial.print("  ");
      // Serial.print("Z: "); Serial.print(accelZ); Serial.print("  ");Serial.println("m/s^2 ");
      
      // Read and print out the temperature
      
    
     logfile.print(myTime); 
     logfile.print(",");
     logfile.print(event.acceleration.x);
     logfile.print(",");
     logfile.print(event.acceleration.y);
     logfile.print(",");
     logfile.println(event.acceleration.z);
    
     logfile.flush();
     
     timeOld = myTime; // Reset timeOld
    
      }

 
    }

}
