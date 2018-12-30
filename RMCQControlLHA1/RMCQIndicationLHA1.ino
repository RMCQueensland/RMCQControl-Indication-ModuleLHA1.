/*RMCQ Control Module A1 Feedback Indication Unit - Module A1
  By David Lowe 20160330
  See Description for further detail

  //define the micro switch input pins from the expander
  #define TILHA1100TPin 8
  #define TILHA1110TPin 9
  #define TILHA1101TPin 10
  #define TILHA1102TPin 11
  //define the LED output pins to the expander
  #define mainOuterLEDPin 0
  #define mainInnerLEDPin 1
  #define yardOutLEDPin 2
  #define departLEDPin 3
  #define arriveLEDPin 4

*/

void RMCQIndicationA1() {
  //do a reset of all LEDs after a short delay
  mcp.digitalWrite(mainOuterLEDPin, LOW); mcp.digitalWrite(mainInnerLEDPin, LOW); mcp.digitalWrite(yardOutLEDPin, LOW); mcp.digitalWrite(departLEDPin, LOW); mcp.digitalWrite(arriveLEDPin, LOW);

  //Check for each condition and set the output accordingly
  //for Main Outer
  if (mcp.digitalRead(TILHA102Tpin)) {
    mcp.digitalWrite(mainOuterLEDPin, HIGH);
  }

  //for Main Inner
  if (mcp.digitalRead(TILHA101Tpin) && mcp.digitalRead(TILHA110Tpin)) {
    mcp.digitalWrite(mainInnerLEDPin, HIGH);
  }

  //for Yard Out
  if (mcp.digitalRead(TILHA100Tpin)) {
    mcp.digitalWrite(yardOutLEDPin, HIGH);
  }

  //for Depart
  if (!mcp.digitalRead(TILHA100Tpin) && !mcp.digitalRead(TILHA110Tpin) && mcp.digitalRead(TILHA101Tpin)) {
    mcp.digitalWrite(departLEDPin, HIGH);
  }

  //for Arrive
  if (!mcp.digitalRead(TILHA100Tpin) && !mcp.digitalRead(TILHA110Tpin) && !mcp.digitalRead(TILHA101Tpin) && !mcp.digitalRead(TILHA102Tpin)) {
    mcp.digitalWrite(arriveLEDPin, HIGH);
  }
}
