#include <IRremote.h>
//#include <IRremoteInt.h>
#include "bitlash.h"

//strings containing proximity and IR script to be loaded during setup()
String irscriptstr  = "function irscript {if (!g) {d9=0;i=0;r=0;} else if (r) {i++;d9=!d9;if (i==6) {i=0;r=0;}} snooze(1000);}";
String prxscriptstr = "function prxscript {if (r || !g) {d8=0;j=0;} else {j++;d8=!d8;if (j==6) j=0;} snooze(2000);}";

int pinLED = 9;
int recvPin = 5;        //PIN receiving signal from IR
boolean scriptsLoaded = false;  //variable to indicate whether scripts have been loaded

IRrecv irrecv(recvPin);
decode_results results;

//-------------------------------------------------------------------
numvar ping(void) {
  return TCNT1; // return the value of Timer 1
}

//-------------------------------------------------------------------
numvar start(void) {
  assignVar('g'-'a', 1);
  Serial.println("Scripts are running.  Type 'standby' to pause.");
}

//-------------------------------------------------------------------
numvar standby(void) {
  assignVar('g'-'a', 0);
  Serial.println("Scripts are paused. Type 'start' to resume running.");
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
  //add custom function to start the controller
  addBitlashFunction("start", (bitlash_function) start);
  //add customer function to pause the controller
  //this will be used in case of a master scene sequence
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

  if (!scriptsLoaded)
  {
    scriptsLoaded = true;
    //load the proximity and IR scripts
    doCommand("rm *");
    doCommand(&prxscriptstr[0u]);
    doCommand(&irscriptstr[0u]);
    delay(500);

    // start the proximity and IR scripts running in the background.
    doCommand("run prxscript;");
    doCommand("run irscript;");
    delay(500);

//    Serial.println("Controller is running. Type 'start' to start scripts and 'standby' to pause scripts.");
    Serial.println("Commands: 'start' or 'standby'");
  }

  //if the controller is in "go" mode, check for an IR signal
  if (getVar('g'-'a')) {
    if (checkIR()) {assignVar('r'-'a', 1);};
  }
}

