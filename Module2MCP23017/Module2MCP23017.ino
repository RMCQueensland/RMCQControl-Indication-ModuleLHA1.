 // Module1MCP23017 Created by  Glenn Black 19/10/2018
 /*
 * Program debounce created 21 November, by David A Mellis ,
 * modified  30 August 2011 by Limor Fried
 * modified 28 December 2012 by Mike Walters
 * RMCQControl Module A1 Servo Control created by David Lowe 20160330
 * on 01/09/18
 * LogicIndicatorA1_1 created by David Lowe 30971
 * on 01/09/18
 */

/*Theory of operation
 * The system will start with the set position of each servo in a known initial position .
 * In most cases this will set each turnout  to Closed (C) condition  
 * This is a requirement for the system to be aware of this position
 * On the COMMAND to move the Turnout position, The system will determine if:-
 * A/ The servo needs to be moved
 * ie Checks to see if the command position is the current position 
 * OR
 * If NOT the servo will be moved in to the commanded position
 * 
 * Pin Configuration:
 * OUTPUT
 *T100: Servo output =D10
 *T110: Servo output =D11
 *T101: Servo output =D12
 *T102: Servo output =D13
 *INPUTS
 *YARDOUT pushbutton input =D2
 *DEPARTURE pushbutton input = D3
 *ARRIVAL pushbutton input = D4
 *MAIN INNER pushbuttoninput = D5
 *MAIN OUTER pushbuttoninput =D6
 */
 
 /*
  * Theory of operation MCP23017
  * The program to control the LOGIC INDICATION of the track routes (LED's) with input from the MICRO SWITCHES 
  * mounted on the Turnouts
  * The feedback Signals are Logic LOW when active (turned on)
  * 
  * TRUTH TABLE
  * TURNOUT   100  110  101  102
  * YARD OUT   -    -    -    1
  * DEPARTURE  -    1    0    0
  * ARRIVAL    0    0    0    0
  * MAIN INNER -    1    1    -
  * MAIN OUTER 1    -    -    1
  * 
  * Turnout CLOSED (C) = 1 (LED off)
  * Turnout THROWN  (T) = 0 (LED on)
  * Feedback (Micro switch ) is activated of Turnout  change to THROWN (T)
  * 
  * DESCRIPTION
  * This program uses I/O Port Expander MCP23017 (16 Configerable I/O)
  * Library Adafruit-MCP23017
  * 
  * CONFIGURATION:-
  * Arduino PROMINI (APM)  Analog Pin 5 (i2c clock) to Expander pin #12
  * Arduino (APM) Analg Pin 4 (i2c Data)to Expander Pin #13
  * Expander Pons #15,16,17 to GROUND
  * Expander Pin #9 to 5V Power 
  * Expander#10 to ground common
  * Expander pin#18 through a 10kohm resistor to 5V(Reset pin , active LOW)
  * 
  * 
  * OUTPUTS  (Active LOW,Port B (GPIO B)
  * Expander Pin#1(OP8) connect to  yardOut LED (Port B Bit 0)
  * Expander Pin#2(OP9) connect to mainOuter LED (Port B Bit 1)
  * Expander Pin#3 (OP10) connect to mainInner LED (Port B Bit2)
  * Expander pin#4 (OP11)connect to depart LED (Port B B Bit3)
  * Expander pin#5 (OP12) connects to arrive LED (Port B Bit 4)
  * 
  * INPUTS  (Active LOW, Port A (GPIO A)
  * Expander pin#21 (OP0) to Turnout 100 micro switch (PortA Bit 0)
  * Expander pin#22 (OP 1) to Turnout 110 microswitch (Port A Bit 1)
  * Expander pin #23 (OP2 ) to Turnout 101 microswitch (Port A Bit 2)
  * Expander pin #24 (OP 3) to Turnout 102 microswitch (Port A Bit 3)
  *  
  */
  /*  LOGIC INDICATION
   * The program to control the LOGIC INDICATION of track routes(LED's)based on 
   * the feed back inputs(microswitches mounted on the turnout mount) of turnouts
   * The feedback signals are logic low(taken to GND)when active.
   * The route table below shows the indications  as per the inputs received
   * 
   * Turnout    100   110   101   102
   * Yard out    -     -     -     1  
   * Depart      -     1     0     0
   * Arrive      0     0     0     0
   * Main Inner  -     1     1     -
   * Maon outer  1     -     -     -
   * 
   * The program uses an I/O Port Expander. 
   * Component MCP 23017
   * Adafruit-MCP23017 Libruary
   * Configuration MCP23017
   * Connect pin#12 to the expander to Analog 5 (12c clock) Arduino Pro Mini (APM)
   * Connect pin#13 of the expander to Analog 4  (i2c data) APM
   * Connect pin#15,#16 ,#17 of the expander to Ground(address selection =0)
   * Connect pin #9of the expander to 5V ( power)
   * Connect pin #10 of the expander to ground (common ground)
   * Connect pin #18 through a  10K ohm resistor  to 5V(reset pin active LOW)
   * 
   * INPUTS (active LOW,Port A (GIOA))
   * Connect pin#21 (OP 0) of the expaneder to Turnout 100 micro switch (Port A Bit 0)
   * Connect pin#22 (OP 1) of the expander  to Turnout 110 micro switch (Port A Bit 1)
   * Connect pin#23 (OP 2) of the expander  to Turnout 101 micro switch (Port A Bit 2)
   * Connect pin#23 (OP 3) of the expander  to Turnout 102 micro switch (Port A Bit 3)
   * 
   * OUTPUTS (active LOW , Port B (GPIOB))
   * Connect pin#1 (OP 8) of the expander to yardOut LED (PortB Bit 0) 
   * Connect pin#2 (OP 9) of the expander to mainOuter LED (Port B Bit 1)
   * Connect pin#3 (OP 10) of the expander to mainInner LED (Port B Bit 2)
   * Connect pin#4 (OP 11) of the expander to depart LED (Port B Bit 3)
   * Connect pin#5 (OP 12) of the expander to arrive LED (Port B Bit 4)
   */

  //SCOPE:-

 #include <Servo.h>
 #include <RMCQControl.h>
 #include <Wire.h>
 #include <Adafruit_MCP23017.h>

 //RMCQ Standard Identification 
 #define progName "RMCQ Control and Indication Module LHA1"
 #define version  "0.2alpha"
 #define date "25 Oct 2018"
 #define author "GBlack" 
  


 
 //create a servo object to control servo
 Servo servo100;
 Servo servo110;
 Servo servo101;
 Servo servo102;

 //Constants
 // YARD OUT 
 const int inputYardOut = 2;
 //DEPARTURE
 const int inputDeparture =3;
 // ARRIVAL
 const int inputArrival = 4;
 //MAIN INNER
 const int inputMainInner = 5 ;
 //MAIN OUTER
 const int inputMainOuter = 6;
// Constructor
 RMCQControl rmcqControl = RMCQControl();

 boolean currentServoState;
 
 // Servo variables
 int startPos = 80;
 int throwNormal = 60;
 int throwReverse =30;
 int timeBetweenServoChange = 20;
 int servoRateOfChange =30;

 /*Turnout Normal (Closed), and Reverse (Thrown) position for each servo used.
  * Set to each individual Servo and physical setup
  */
  //Declare & initialize outputs

  int opPin100 =10;
  int opPin110 =11;
  int opPin101 =12;
  int opPin102 =13;

  // Servo variables
  /* The Turnout is in the Normal (Closed) state (HIGH)
   *  Reverse (Thrown) state (LOW)
   */
   boolean currentServoState100 = HIGH;
   boolean currentServoState110 = HIGH;
   boolean currentServoState101= HIGH;
   int offSet=10;
   boolean currentServoState102 =LOW;

   //VARIABLES
   /*YARD OUT  The current reading of the input*/ 
   int inputStateYardOut;
   int lastinputStateYardOut = HIGH ;
   
   /*DEPARTURE  input reading*/
   int inputStateDeparture;
   int lastinputStateDeparture = HIGH;
   
   /*ARRIVAL input reading*/
   int inputStateArrival ;
   int lastinputStateArrival = HIGH;
   
   /*MAIN INNER  input reading*/
   int inputStateMainInner;
   int lastinputStateMainInner = HIGH;
   
   /*MAIN OUTER  input reading*/
   int inputStateMainOuter;
   int lastinputStateMainOuter = HIGH;

   //Time variables primitive Long due to millisec time measurement
   long lastDebounceTime =0;
   //Debounce Time (increase if output flickers
   long debounceDelay =30;
   
     void setYardOut()
    {
      //On Yard Out : 100: NA, 110: NA, 101 :NA , 102: N.
      //Turnout 102 to Normal
      rmcqControl.changeNormal(currentServoState102,servo102,opPin102);
      currentServoState =HIGH;
    }

    void setMain()
    {
      //On MainInner: 100 :N, 110 :N, 101 :N, 102:N,
      //Turn out 100 Normal
      rmcqControl.changeNormal(currentServoState100,servo100,opPin100);
      // change to Normal
      currentServoState100=HIGH;

      //Turnout 110 Normal
      rmcqControl.changeNormal(currentServoState110, servo110,opPin110);
      currentServoState110=HIGH;

      //Turnout 101 Normal
      rmcqControl.changeNormal(currentServoState101, servo101,opPin101);
      currentServoState101=HIGH;

      //Turnout 102 Normal
      rmcqControl.changeNormal(currentServoState102,servo102,opPin102);
      currentServoState102 = HIGH;
      
    }

    void setArrival(){
      //On Arrival : 100 :R, 110: R, 101: R,102: R.
     // Turnout 100 to Thrown 
      rmcqControl.changeReverse(currentServoState100,servo100,opPin100);
     // change to Thrown
      currentServoState100 =LOW;

      //Turnout 110 to Thrown
      rmcqControl.changeReverse(currentServoState110,servo110,opPin110);
      currentServoState110 =LOW;

      //Turnout101 to Throwm
      rmcqControl.changeReverse(currentServoState101,servo101,opPin101);
      currentServoState101 =LOW;

      //Turnout 102 to Thrown
      rmcqControl.changeReverse(currentServoState102,servo102,opPin102);
      currentServoState102 =LOW;
    }

    void setDeparture() 
    {
      // On Departure: 100:NA, 110:N, 101:R, 102: R.
      // Turnout110 to Normal
      rmcqControl.changeNormal(currentServoState110,servo110,opPin110);
      currentServoState110 =HIGH;

      // Turnout101 to Thrown
      rmcqControl.changeReverse(currentServoState101,servo101,opPin101);
      //Change to Thrown
      currentServoState101 =LOW;

      //Turnout 102 to Thrown
      rmcqControl.changeReverse(currentServoState102,servo102,opPin102);
      //change to Thrown
      currentServoState102 =LOW;
      
    }
    
    // LOGIC INDICATION
   /* Define  the micro switch INPUT pins from the expander
    */
    #define t100Pin 0
    #define t110Pin 1
    #define t101Pin 2
    #define t102Pin 3

    // define the LED OUTPUT pins to the expander
    #define mainOuterLEDPin 8
    #define mainInnerLEDPin 9
    #define yardOutLEDPin 10
    #define departLEDPin 11
    #define arriveLEDPin 12

    // Constructor, Create an instance  of the MCP23017 Libruary

    Adafruit_MCP23017 mcp;


    
   void setup() {
  Serial.begin(9600);
  Serial.println ("RMCQ Standard Identification Information");
  Serial.print("Running;  ");
  Serial.println(progName);
  Serial.print("version:  "); 
  Serial.println(version);
  Serial.print("Last Updted: ");
  Serial.println(date);
  Serial.print("Programer:  ");
  Serial.println(author);
  
  //set Servo Global Variables
  rmcqControl.setUp(startPos,throwNormal,throwReverse,timeBetweenServoChange,servoRateOfChange);

  //Set Pin configuration
  pinMode(inputYardOut,INPUT_PULLUP);
  pinMode(inputDeparture,INPUT_PULLUP);
  pinMode(inputArrival,INPUT_PULLUP);
  pinMode(inputMainInner,INPUT_PULLUP);
  pinMode(inputMainOuter,INPUT_PULLUP);

  /* Set initial State of the Servos
   *  Set Turnout  100
   */

   rmcqControl.center(servo100,startPos,opPin100);
   rmcqControl.changeNormal(LOW,servo100,opPin100);
   /*Set to  Normal State*/
   currentServoState100 =HIGH;
   
   /*Set Turnout 110*/
   rmcqControl.center(servo110,startPos,opPin110);
   rmcqControl.changeNormal(LOW,servo110,opPin110);
   //Set to Normal State
   currentServoState100= HIGH;

   //Set Turnout 101
   rmcqControl.center(servo101,startPos,opPin101);
   rmcqControl.changeNormal(LOW,servo101,opPin101);
   //Set to Normal State
   currentServoState101 =HIGH ;

   //Set Turnout 102
   rmcqControl.center(servo102,startPos,opPin102);
   rmcqControl.changeNormal(LOW,servo102,opPin102);
   //Set to Normal State
   currentServoState102 =HIGH;
    
    // LOGIC INDICATION
   

    //Use default address 0
    mcp.begin();
    //SetPort A as INPUTS and apply 100k internal pullups
    for(uint8_t pin = 0; pin < 8; pin++)
    {
      mcp.pinMode(pin, INPUT);
      mcp.pullUp (pin,HIGH);
    }
     //Set Port B as OUTPUTS and turn all the LEDs on (LOW)
     for(uint8_t pin = 8; pin <16; pin++)
     {
      mcp.pinMode(pin, OUTPUT);
      mcp.digitalWrite(pin, LOW);
     }
     //Turn the LEDs off
     delay(200);
     for(uint8_t  pin = 8; pin < 16; pin++)
     {
      mcp.digitalWrite(pin, HIGH);
     }
      }
      

  //RMCQControl Library Functions
  /*void RMCQcontrol:: changeNormal(boolean currentState,Servo Servo servoName, int outputPin)
   */
   /*RMCQcontrol::changeReverse(boolean currentState,Servo servoName ,int outputPin)
    */

    /*Route        Point
   ________ _________102
           \
             \
   __________  ______101,110
              \
                \
   _____________  _____100

   */ 
     

void loop() {
  // Read Yard Out input Pin 2
  // On Yard Out:100 NA,110: NA, 101 :NA.  102: N.
  int inputYardOutState =digitalRead(inputYardOut);
  //Current read of Yard Out State ( NOT EQUAL TO) last assigned Read of Yard Out State
  if (inputYardOutState !=lastinputStateYardOut)
  {
    //reset the Debounce Timer
    lastDebounceTime = millis();
  Serial.println("Yard Out Activated");
  }
  if((millis() - lastDebounceTime) > debounceDelay)
  {
    /*Checks to seeif the commanded position is the current position
     Current read of Yard Out State ( NOT EQUAL TO) previous Read of Yard Out State*/
     
    if (inputYardOutState != inputStateYardOut)
   {
    inputStateYardOut =inputYardOutState;
  if (inputYardOutState==LOW)
    {
       setYardOut();
     }
    }
   }
  lastinputStateYardOut = inputYardOutState;


  //Read Departure input Pin 3
  //On Departure :100 :NA,110: N, 101: R, 102 :R.
   int inputDepartureState = digitalRead(inputDeparture);
   if(inputDepartureState != lastinputStateDeparture)
  {
    //reset the Debounce Timer
    lastDebounceTime =millis();
  }
  Serial.println("Departure Activated");
  
  if ((millis()-lastDebounceTime) > debounceDelay)
  {
    /*Checks to seeif the commanded position is the current position
    Current read of Departure State ( NOT EQUAL TO) previous Read of Departure State*/
    if (inputDepartureState !=inputStateDeparture)
    {
      inputStateDeparture = inputDepartureState;
      if (inputDepartureState == LOW)
      {
        setDeparture();
      }
    }
  }
  lastinputStateDeparture = inputDepartureState;

  //Read Arrival input Pin 4
  //On Arrival: 100:R, 110:R, 101:R,102:R.

  int inputArrivalState =digitalRead(inputArrival); 
  if(inputArrivalState != lastinputStateArrival)
  {
    //reset the Debounce Timer
    lastDebounceTime =millis();
    Serial.println("Arrival Activated");
   }
   if((millis() -lastDebounceTime) > debounceDelay)
   {
    /*Checks to seeif the commanded position is the current position
    Current read of Arrival State ( NOT EQUAL TO) previous Read of Arrival State*/
    
    if(inputArrivalState != inputStateArrival)
    {
      inputStateArrival = inputArrivalState;
      {
        if(inputArrivalState == LOW)
        {
          setArrival();
        }
      }
    }
   }
   lastinputStateArrival = inputArrivalState;   


   //Read mainInner(Pin 5) or mainOuter (pin6)
   //On Main Inner :100:N, 110: N, 101:N, 102: N.
   int inputMainInnerState =digitalRead (inputMainInner);
   int inputMainOuterState = digitalRead(inputMainOuter);
   if(inputMainInnerState != lastinputStateMainInner || inputMainOuterState != lastinputStateMainOuter)
   {
    //reset the debouncing timer
    lastDebounceTime = millis();
    Serial.println ("Main Inner or Outer activated");
    }

    if((millis() -lastDebounceTime)> debounceDelay)
    {
      /*Checks to seeif the commanded position is the current position
      Current read of MainInner /Main Outer State ( NOT EQUAL TO) previous Read of MainInner /MainOuter State*/
      if (inputMainInnerState != inputStateMainInner|| inputStateMainOuter != inputStateMainOuter)
      {
        inputStateMainInner = inputMainInnerState;
        inputStateMainOuter = inputMainOuterState;
        if(inputMainInnerState == LOW || inputMainOuterState ==LOW)
        {
          setMain();
        }
      }
    }
    lastinputStateMainInner = inputMainInnerState;
    lastinputStateMainOuter =inputMainOuterState;

    // LOGIC INDICATION LOOP CODE

    //Check for the condition and set the OUTPUT accordingly

    //Main Outer
    if(mcp.digitalRead(t100Pin)) 
    
    {
      mcp.digitalWrite(mainOuterLEDPin, LOW);
    }

    //Main Inner
    if(mcp.digitalRead(t101Pin) && mcp.digitalRead(t110Pin))
    {
      mcp.digitalWrite(mainInnerLEDPin, LOW);
    }
     // YardOut
     if(mcp.digitalRead(t102Pin))
     {
      mcp.digitalWrite(yardOutLEDPin, LOW);
     }

     //Depart

     if(!mcp.digitalRead(t102Pin) && !mcp.digitalRead(t101Pin) && mcp.digitalRead(t110Pin))
     {
      mcp.digitalWrite(departLEDPin, LOW);
     }

     //Arrive
     if(!mcp.digitalRead(t100Pin) && !mcp.digitalRead(t110Pin) && !mcp.digitalRead(t101Pin) && !mcp.digitalRead(t102Pin))
     {
      mcp.digitalWrite(arriveLEDPin, LOW);
     }
       }
     
      
  

