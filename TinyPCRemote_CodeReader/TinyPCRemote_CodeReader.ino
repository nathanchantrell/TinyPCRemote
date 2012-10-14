//----------------------------------------------------------------------------------------------------------------------
// TinyPCRemote_CodeReader 
// By Nathan Chantrell http://nathan.chantrell.net
// Reads codes from the remote control and outputs to serial monitor
// Enter the resulting codes in TinyPCRemote_ATtiny85.ino
//
// Licenced under the Creative Commons Attribution-ShareAlike 3.0 Unported (CC BY-SA 3.0) licence:
// http://creativecommons.org/licenses/by-sa/3.0/
//----------------------------------------------------------------------------------------------------------------------

// IR sensor connected to Arduino D11
#define IRpin_PIN      PINB
#define IRpin          3

#define MAXPULSE 5000 // max IR pulse length, default 5 milliseconds
#define NUMPULSES 100 // max IR pulse pairs to sample
#define RESOLUTION 2 // time between IR measurements

uint16_t pulses[NUMPULSES][2];  // pair is high and low pulse 
uint8_t currentpulse = 0; // index for pulses we're storing

void setup() {
  
 Serial.begin(9600);
 Serial.println("TinyPCRemote_CodeReader Ready!");

 pinMode(11, INPUT); // Set IR pin as input

 //power pin for ir rx
 pinMode(13, OUTPUT); 
 digitalWrite(13, HIGH);
  
}

void loop() {  

 unsigned int highpulse, lowpulse;  // temporary storage timing
 highpulse = lowpulse = 0; // start out with no pulse length 
  
 while (IRpin_PIN & _BV(IRpin)) { // got a high pulse
    highpulse++;
    delayMicroseconds(RESOLUTION);
    if (((highpulse >= MAXPULSE) && (currentpulse != 0))|| currentpulse == NUMPULSES) {
      currentpulse=0;
      printpulses();
      return;
    }
 }
 pulses[currentpulse][0] = highpulse;
 
 while (! (IRpin_PIN & _BV(IRpin))) { // got a low pulse
    lowpulse++;
    delayMicroseconds(RESOLUTION);
    if (((lowpulse >= MAXPULSE)  && (currentpulse != 0))|| currentpulse == NUMPULSES) {
      currentpulse=0;
      printpulses();
      return;
    }
 }
 pulses[currentpulse][1] = lowpulse;
 currentpulse++;

}

void printpulses() {
  
 unsigned long irCode=0;
  
 for (int i = 0; i < 32; i++) {
   irCode=irCode<<1;
   if((pulses[i][0] * RESOLUTION)>0&&(pulses[i][0] * RESOLUTION)<500) {
    irCode|=0; 
   } else {
    irCode|=1;
   }
 }

 Serial.print("Code for this button: ");  
 Serial.println(irCode);

}

