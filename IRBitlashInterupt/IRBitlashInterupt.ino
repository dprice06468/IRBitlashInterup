#include <IRremote.h>
//#include <IRremoteInt.h>
#include "bitlash.h"

String iractionstr = "function iraction {d9=1;delay(2000);d9=0;}";
String irfunctionstr = "function irfunction {iraction;}";
String toggle9str = "function toggle9 {d9=!d9;}";
String toggle8str = "function toggle8 {d8=!d8;}";
String sbactionstr = "function sbaction {run toggle8, 1000;}";

int pinLED = 9;
int recvPin = 5;        //PIN receiving signal from IR
boolean funcsLoaded = false;
boolean runmode = true;
boolean sbRunning = false;

IRrecv irrecv(recvPin);
decode_results results;

//-------------------------------------------------------------------
numvar ping(void) {
  return TCNT1; // return the value of Timer 1
}

//-------------------------------------------------------------------
numvar start(void) {
  runmode = true;
}

//-------------------------------------------------------------------
numvar standby(void) {
  runmode = false;
}

//----------------------------------------------------------------------
void setup() {
  // initialize bitlash and set primary serial port baud
  // print startup banner and run the startup macro
  initBitlash(57600);

  irrecv.enableIRIn();
  
  // initialize output pins.
  pinMode(pinLED, OUTPUT);

  // add custom function to handle ping
  addBitlashFunction("ping", (bitlash_function) ping);
  addBitlashFunction("start", (bitlash_function) start);
  addBitlashFunction("standby", (bitlash_function) standby);
}

//----------------------------------------------------------------------
boolean checkIR() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    if (results.value == 0x219E10EF) {
      irrecv.resume(); 
      return true;
    }
    // Receive the next value
    irrecv.resume(); 
  }
  return false;
}

//----------------------------------------------------------------------
void loop() {
  runBitlash();

  if (!funcsLoaded)
  {
    funcsLoaded = true;
    doCommand("rm *");
    doCommand(&iractionstr[0u]);
    doCommand(&sbactionstr[0u]);
    doCommand(&irfunctionstr[0u]);
    doCommand(&toggle8str[0u]);
    doCommand(&toggle9str[0u]);
  }

  if (runmode) {
    if (!sbRunning) {doCommand("sbaction;"); sbRunning = true;}
    if (checkIR()) doCommand("irfunction;");
  } else {
    if (sbRunning) {doCommand("stop 0;"); sbRunning = false;}
  }
}

