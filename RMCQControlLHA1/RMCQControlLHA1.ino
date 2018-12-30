/* RMCQ Control Module A1 Servo Control Unit - Module A1
  By David Lowe 20160330

  See Description for further detail

  Program Details:
*/
#define progName "RMCQ Control and Indication Module LHA1"
#define version "1.0"
#define date "30 Dec 2018"
#define author "DLowe"


#include <Servo.h>
#include <RMCQControl.h>
RMCQControl rmcqControl = RMCQControl();

//Control Bus: Turnout Control LHA1 (TCLHA1)
Servo turnout100;    // create servo object to control a servo
Servo turnout110;
Servo turnout101;
Servo turnout102;

// constants won't change.
//Control Bus: Route Indication LHA1 (RILHA1)
#define inputYardOut 2      //Yard out
#define inputDeparture  3      //Departure
#define inputArrival  4      //Arrival
#define inputMainInner  5      //Main Inner
#define inputMainOuter  6      //Main Outer

// Servo variables
#define startPos  80
#define throwNormal  60
#define throwReverse  30
#define timeBetweenServoChange  20
#define servoRateOfChange  30

// Trunout Normal and Reverse positions for each servo used. Set to each individual Servo and physical setup.
#define TCLHA1100pin  10
#define TCLHA1110pin  11
#define TCLHA1101pin  12
#define TCLHA1102pin  13

//define the micro switch input pins from the expander
#define TILHA100Tpin 8
#define TILHA110Tpin 9
#define TILHA101Tpin 10
#define TILHA102Tpin 11
//define the LED output pins to the expander
#define mainOuterLEDPin 0
#define mainInnerLEDPin 1
#define yardOutLEDPin 2
#define departLEDPin 3
#define arriveLEDPin 4

// Servo variables
// the Turnout is in the Normal state (HIGH). LOW for Reverse.
boolean currentTurnoutState100 = HIGH;
boolean currentTurnoutState110 = HIGH;
boolean currentTurnoutState101 = HIGH;
//int offSet101 = 10;
boolean currentTurnoutState102 = LOW;

// Variables will change:
int inputStateYardOut;             // the current reading from the input Yard out
int lastinputStateYardOut = HIGH;   // the previous reading from the input Yard Out
int inputStateDeparture;             // the current reading from the input Departure
int lastinputStateDeparture = HIGH;   // the previous reading from the input Departure
int inputStateArrival;             // the current reading from the input Arrival
int lastinputStateArrival = HIGH;   // the previous reading from the input Arrival
int inputStateMainInner;             // the current reading from the input Main Inner
int lastinputStateMainInner = HIGH;   // the previous reading from the input Main Inner
int inputStateMainOuter;             // the current reading from the input Main Outer
int lastinputStateMainOuter = HIGH;   // the previous reading from the input Main Outer

int updateIndication = false;   //Added to only allow update of panel LEDs if a change has occured by a user button press

// the following variables athe long's because the time, measured in miliseconds, will quickly become a bigger number Ran can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 30;    // the debounce time; increase if the output flickers

//Added information for MCP23017
//Initalise the required libraries
#include <Wire.h>
#include "Adafruit_MCP23017.h"
Adafruit_MCP23017 mcp;  //Create a instance of the MCP23017 chip


void setup() {
  Serial.begin(9600);
  Serial.println("RMCQ Standard Indentification Information");
  Serial.print("Running; "); Serial.println(progName);
  Serial.print("Version; "); Serial.println(version);
  Serial.print("Last updated; "); Serial.println(date);
  Serial.print("Programer; "); Serial.println(author);

  // Set Servo global variables
  rmcqControl.setUp(startPos, throwNormal, throwReverse, timeBetweenServoChange, servoRateOfChange);

  // set pin configuration
  pinMode(inputYardOut, INPUT_PULLUP);
  pinMode(inputDeparture, INPUT_PULLUP);
  pinMode(inputArrival, INPUT_PULLUP);
  pinMode(inputMainInner, INPUT_PULLUP);
  pinMode(inputMainOuter, INPUT_PULLUP);

  // set initial state for servos
  // Set Point 100
  rmcqControl.center(turnout100, startPos, TCLHA1100pin);
  rmcqControl.changeNormal(LOW, turnout100, TCLHA1100pin);  // Set the Servo to Normal
  currentTurnoutState100 = HIGH;
  // Set Point 110
  rmcqControl.center(turnout110, startPos, TCLHA1110pin);
  rmcqControl.changeNormal(LOW, turnout110, TCLHA1110pin);    // Set the Servo to Normal
  currentTurnoutState110 = HIGH;
  // Set Point 101
  rmcqControl.center(turnout101, startPos, TCLHA1101pin);
  rmcqControl.changeNormal(LOW, turnout101, TCLHA1101pin);    // Set the Servo to Normal
  currentTurnoutState101 = HIGH;
  // Set Point 102
  rmcqControl.center(turnout102, startPos, TCLHA1102pin);
  rmcqControl.changeNormal(LOW, turnout102, TCLHA1102pin);    // Set the Servo to normal
  currentTurnoutState102 = HIGH;

  //Added setup for control panel indication
  mcp.begin();      // use default address 0
  //Set Port A as inputs and apply the 100K internal pullups
  for (uint8_t pin = 8; pin  < 16; pin ++)
  {
    mcp.pinMode(pin, INPUT);
    mcp.pullUp(pin, HIGH);
  }
  //Set Port B as output and turn all the LEDs on (HIGH)
  for (uint8_t pin = 0; pin  < 7; pin ++)
  {
    mcp.pinMode(pin, OUTPUT);
    mcp.digitalWrite(pin, HIGH);
  }
  //Turn the LEDs off
  delay(200);
  for (uint8_t pin = 0; pin  < 7; pin ++)
  {
    mcp.digitalWrite(pin, LOW);
  }

  //Update Panel LEDs for the inital state of the turnouts
  RMCQIndicationA1();
}
//RMCQ library functions
//void RMCQControl::changeNormal(boolean currentState, Servo servoName, int outputPin)
//void RMCQControl::changeReverse(boolean currentState, Servo servoName, int outputPin)

/*    Route         Point
   _____ __________ 102
   _____\__________ 101, 110
          \
   ________\_______ 100
*/

void loop() {
  // Read Yard Out input Pin2
  // On Yard Out: 100: NA, 110: NA, 101: NA, 102: N.
  int inputYardOutState = digitalRead(inputYardOut);
  if (inputYardOutState != lastinputStateYardOut) {
    lastDebounceTime = millis();
    Serial.println("Yard Out activated"); updateIndication = true;
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (inputYardOutState != inputStateYardOut) {
      inputStateYardOut = inputYardOutState;
      if (inputYardOutState == LOW) {
        setYardOut();
      }
    }
  }
  lastinputStateYardOut = inputYardOutState;

  // Read Departure input Pin3
  // On Departure: 100: NA, 110: N, 101: R, 102: R.
  int inputDepartureState = digitalRead(inputDeparture);
  if (inputDepartureState != lastinputStateDeparture) {
    lastDebounceTime = millis();
    Serial.println("Departure activated"); updateIndication = true;
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (inputDepartureState != inputStateDeparture) {
      inputStateDeparture = inputDepartureState;
      if (inputDepartureState == LOW) {
        setDeparture();
      }
    }
  }
  lastinputStateDeparture = inputDepartureState;

  // Read Arrival input Pin4
  // On Arrival: 100: R, 110: R, 101: R, 102: R.
  int inputArrivalState = digitalRead(inputArrival);
  if (inputArrivalState != lastinputStateArrival) {
    lastDebounceTime = millis();
    Serial.println("Arrival activated"); updateIndication = true;
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (inputArrivalState != inputStateArrival) {
      inputStateArrival = inputArrivalState;
      if (inputArrivalState == LOW) {
        setArrival();
      }
    }
  }
  lastinputStateArrival = inputArrivalState;

  // Read Main Inner (Pin5) or Main Outer (Pin6)
  // On Main Inner: 100: N, 110: N, 101: N, 102: N.
  int inputMainInnerState = digitalRead(inputMainInner);
  int inputMainOuterState = digitalRead(inputMainOuter);
  if (inputMainInnerState != lastinputStateMainInner || inputMainOuterState != lastinputStateMainOuter) {
    // Reset the debouncing timer
    lastDebounceTime = millis();
    Serial.println("Main Inner or Outer activated"); updateIndication = true;
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (inputMainInnerState != inputStateMainInner || inputMainOuterState != inputStateMainOuter) {
      inputStateMainInner = inputMainInnerState;
      inputStateMainOuter = inputMainOuterState;
      if (inputMainInnerState == LOW || inputMainOuterState == LOW) {
        setMain();
      }
    }
  }
  lastinputStateMainInner = inputMainInnerState;
  lastinputStateMainOuter = inputMainOuterState;

  //Added routine for control panel indication
  if (updateIndication == true) {
    RMCQIndicationA1();
    //Reset panel update flag
    updateIndication = false;
  }
}



void setYardOut() {// On Yard Out: 100: NA, 110: NA, 101: NA, 102: N.
  // Point 102 to Normal
  rmcqControl.changeNormal(currentTurnoutState102, turnout102, TCLHA1102pin);
  currentTurnoutState102 = HIGH;
}

void setMain() { // On Main Inner: 100: N, 110: N, 101: N, 102: N.
  // Point 100 to Reverse
  rmcqControl.changeNormal(currentTurnoutState100, turnout100, TCLHA1100pin);
  currentTurnoutState100 = HIGH;
  // Point 110 to Normal
  rmcqControl.changeNormal(currentTurnoutState110, turnout110, TCLHA1110pin);
  currentTurnoutState110 = HIGH;
  // Point 101 to Normal
  rmcqControl.changeNormal(currentTurnoutState101, turnout101, TCLHA1101pin);
  currentTurnoutState101 = HIGH;
  // Point 102 to Reverse
  rmcqControl.changeNormal(currentTurnoutState102, turnout102, TCLHA1102pin);
  currentTurnoutState102 = HIGH;
}

void setArrival() {// On Arrival: 100: R, 110: R, 101: R, 102: R.
  // Point 100 to Normal
  rmcqControl.changeReverse(currentTurnoutState100, turnout100, TCLHA1100pin);
  currentTurnoutState100 = LOW;
  // Point 110 to Reverse
  rmcqControl.changeReverse(currentTurnoutState110, turnout110, TCLHA1110pin);
  currentTurnoutState110 = LOW;
  // Point 101 to Reverse
  rmcqControl.changeReverse(currentTurnoutState101, turnout101, TCLHA1101pin);
  //changeReverse101();
  currentTurnoutState101 = LOW;
  // Point 102 to Normal
  rmcqControl.changeReverse(currentTurnoutState102, turnout102, TCLHA1102pin);
  currentTurnoutState102 = LOW;
}

void setDeparture() {// On Departure: 100: NA, 110: N, 101: R, 102: R.
  // Point 110 to Normal
  rmcqControl.changeNormal(currentTurnoutState110, turnout110, TCLHA1110pin);
  currentTurnoutState110 = HIGH;
  // Point 101 to Reverse
  rmcqControl.changeReverse(currentTurnoutState101, turnout101, TCLHA1101pin);
  // changeReverse101();
  currentTurnoutState101 = LOW;
  //Point 102 to Normal
  rmcqControl.changeReverse(currentTurnoutState102, turnout102, TCLHA1102pin);
  currentTurnoutState102 = LOW;
}

void changeReverse101() {
  int x = 1;
  turnout101.attach(TCLHA1101pin);
  if (currentTurnoutState101 == HIGH) {
    for (x = startPos + throwNormal; x >= (startPos - throwReverse - 10); x --) {
      turnout101.write(x);
      delay(servoRateOfChange);
    }
  }
  else {
    // do nothing as OP already Reverse
  }
  turnout101.detach();
  delay(timeBetweenServoChange);
}
