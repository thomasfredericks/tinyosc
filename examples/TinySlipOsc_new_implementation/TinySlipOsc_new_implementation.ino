#include <TinyOsc.h>

TinySlipOsc osc;

void setup() {
  
  Serial.begin(57600);

  osc.begin(Serial);

}

void loop() {

  osc.writeMessage( "/ms",  "i",   millis()  );


  delay(1000);

}
