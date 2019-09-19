//include the fast I/O 2 functions 
// #include "DIO2.h"  

const float modifier = 1.027;

const byte ptm_pin = 0;
const byte opto_pin = 2;


const unsigned long ptm_pulsewidth = 6 * 1000 * modifier; // in micro secs
const unsigned long ptm_cycletime = 20 * 1000 * modifier; // in micro secs

const unsigned long opto_pulsewidth = 10 * modifier; // in milli secs
const unsigned long opto_cycletime = 100 * modifier; // in milli secs

const unsigned long train_length = 5 * 1000 * modifier; // in milli secs
const unsigned long train_cycletime = 20 * 1000 * modifier; // in milli secs


// Time variables
unsigned long t0, tnow, t0_opto, tnow_opto;

// opto switch
bool opto = true;


void setup() {
  // put your setup code here, to run once:
  pinMode(ptm_pin, OUTPUT);
  pinMode(opto_pin, OUTPUT);
 

  // Serial.begin(19200);
  t0 = micros();
  t0_opto = millis();

}

void loop() {
  tnow = micros() - t0;
  tnow_opto = millis() - t0_opto;
  
  //  Photometry
  
  if ((tnow % ptm_cycletime) <= ptm_pulsewidth){
    digitalWrite(ptm_pin, HIGH);
  }
  else{
    digitalWrite(ptm_pin, LOW);
  }
  
  
  //  Train
  if ((tnow_opto % train_cycletime) <= train_length){
    if ((tnow_opto % opto_cycletime) <= opto_pulsewidth){
      opto = true;
      digitalWrite(opto_pin, HIGH);
    }
    else{
      opto = false;
      digitalWrite(opto_pin, LOW);
    }
  }
  else if(opto){
    digitalWrite(opto_pin, LOW);
  }
  
  

}
