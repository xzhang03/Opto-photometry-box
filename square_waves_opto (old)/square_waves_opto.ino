//include the fast I/O 2 functions 
// #include "DIO2.h"  

const float modifier = 1.027;

const byte ptm_pin = 0;
const byte opto_pin = 2;

// Photometry
// On for A seconds every B seconds (duty cycle = A/B)
const unsigned long ptm_pulsewidth = 6 * 1000 * modifier; // in micro secs (A)
const unsigned long ptm_cycletime = 20 * 1000 * modifier; // in micro secs (B)

// Opto
// On for A seconds every B seconds (duty cycle = A/B)
const unsigned long opto_pulsewidth = 10 * modifier; // in milli secs (A)
const unsigned long opto_cycletime = 100 * modifier; // in milli secs (A)

// Train for opto
// Train is on for A seconds every B seconds (duty cycle = A/B)
unsigned long train_length = 5 * 1000 * modifier; // in milli secs (A)
unsigned long train_cycletime = 30 * 1000 * modifier; // in milli secs (B)

// Using a counter system instead of mod for trains since arduino cannot handle large number mods.
byte traincounter = 0; // A downscaling factor to assist the mod function


// Time variables
unsigned long t0, tnow, t0_opto, tnow_opto, t_train;

// opto switch
bool opto = true;
bool train_start = false;

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
  if ((tnow_opto / train_cycletime) > traincounter){
    
    // Timestamp when the train starts
    if(~train_start){
      t_train = tnow_opto;
      train_start = true;
    }

    // If the train length is right
    if ((tnow_opto - t_train) <= train_length){
      // Actual stim
      if ((tnow_opto % opto_cycletime) <= opto_pulsewidth){
        opto = true;
        digitalWrite(opto_pin, HIGH);
      }
      else{
        opto = false;
        digitalWrite(opto_pin, LOW);
      }
    }
    // After the train is over. propogate train number and flag train as off
    else{
      train_start = false;
      traincounter++;
      
      // Keep LED off otherwise
      if (opto){
        digitalWrite(opto_pin, LOW);
      }
    }
  }

  
  

}
