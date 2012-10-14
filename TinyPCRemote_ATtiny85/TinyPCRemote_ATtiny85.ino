//----------------------------------------------------------------------------------------------------------------------
// TinyPCRemote 
// By Nathan Chantrell http://nathan.chantrell.net
// Receives infra red codes and emulates a USB keyboard for remote control
//
// Licenced under the Creative Commons Attribution-ShareAlike 3.0 Unported (CC BY-SA 3.0) licence:
// http://creativecommons.org/licenses/by-sa/3.0/
//----------------------------------------------------------------------------------------------------------------------

#include "UsbKeyboard.h" // http://code.google.com/p/vusb-for-arduino/

// IR sensor connected to PB4 = ATtiny85 physical pin 3
#define IRpin_PIN      PINB
#define IRpin          4

#define MAXPULSE 5000 // max IR pulse length, default 5 milliseconds
#define NUMPULSES 100 // max IR pulse pairs to sample
#define RESOLUTION 2 // time between IR measurements

uint16_t pulses[NUMPULSES][2];  // pair is high and low pulse 
uint8_t currentpulse = 0; // index for pulses we're storing

void setup() {

 pinMode(4, INPUT); // Make sure IR pin is set as input

 OSCCAL = 0x9C; // Oscillator callibration value from Tiny Tuner http://goo.gl/J71XN
 
 TIMSK&=!(1<<TOIE0); // Disable timer0 as it can interfere with USB timing
 
}

void loop() {
   
 UsbKeyboard.update(); // needs to be called often

 unsigned long irCode=listenForIR(); // Wait for an IR Code

// Process the pulses to get our code
 for (int i = 0; i < 32; i++) {
  irCode=irCode<<1;
  if((pulses[i][0] * RESOLUTION)>0&&(pulses[i][0] * RESOLUTION)<500) {
   irCode|=0; 
  } else {
   irCode|=1;
  }
 }

// ---------------------------------------------------------------------------------------
// Enter IR codes and keystrokes to send below, see keyboard_commands.txt for list of keys
// ---------------------------------------------------------------------------------------

 if (irCode==3631581239) {               // Single character example, "1"
  UsbKeyboard.sendKeyStroke(KEY_1); 

 } else if (irCode==3631585319) {        // String example with trailing space, "hello "
  UsbKeyboard.sendKeyStroke(KEY_H);
  UsbKeyboard.sendKeyStroke(KEY_E);
  UsbKeyboard.sendKeyStroke(KEY_L);
  UsbKeyboard.sendKeyStroke(KEY_L);
  UsbKeyboard.sendKeyStroke(KEY_O); 
  UsbKeyboard.sendKeyStroke(KEY_SPACE); 

 } else if (irCode==3631589399) {        // String example with enter, "world <enter>"
  UsbKeyboard.sendKeyStroke(KEY_W);
  UsbKeyboard.sendKeyStroke(KEY_O);
  UsbKeyboard.sendKeyStroke(KEY_R);
  UsbKeyboard.sendKeyStroke(KEY_L); 
  UsbKeyboard.sendKeyStroke(KEY_D); 
  UsbKeyboard.sendKeyStroke(KEY_ENTER);
  
 } else if (irCode==3631583789) {        // Modifier key example "^c"
  UsbKeyboard.sendKeyStroke(KEY_C, MOD_CONTROL_LEFT);

 } else if (irCode==3631592969) {        // Raw code example Home key = 74
  UsbKeyboard.sendKeyStroke(74);

 // Duplicate for as many buttons as required

 }

} // loop end

// IR receive code
int listenForIR() {  
  currentpulse = 0;
  while (1) {
   unsigned int highpulse, lowpulse;  // temporary storage timing
   highpulse = lowpulse = 0; // start out with no pulse length 
  
   while (IRpin_PIN & _BV(IRpin)) { // got a high pulse
      UsbKeyboard.update(); // needs to be called often
      highpulse++; 
      delayMicroseconds(RESOLUTION);
      if (((highpulse >= MAXPULSE) && (currentpulse != 0))|| currentpulse == NUMPULSES) {
        return currentpulse; 
      }
   }
   pulses[currentpulse][0] = highpulse;

   while (! (IRpin_PIN & _BV(IRpin))) { // got a low pulse
      UsbKeyboard.update(); // needs to be called often
      lowpulse++; 
      delayMicroseconds(RESOLUTION);
      if (((lowpulse >= MAXPULSE) && (currentpulse != 0))|| currentpulse == NUMPULSES) {
        return currentpulse; 
      }
   }
   pulses[currentpulse][1] = lowpulse;
   currentpulse++;
  }
}


