// Version 2 uses a counter-based system to make sure there is no overlap between photometry and opto pulses

const float modifier = 1.027;

const byte ptm_pin = 0; // 0
const byte opto_pin = 2; // 2

// *Everything below is operating on a 50 Hz cycle.*
// In each cycle, it goes: pre-spacer (I) -> photometry pulse (II) -> post-spacer (III) -> opto pulse (IV)
// Master cycle time
const unsigned long CYCLETIME = 20 * 1000 * modifier; // in micro secs

// MODE variable
byte MASTER_MODE = 0;

// Photometry
// On for A1 milliseconds every 20 milliseconds (duty cycle = A1/20)
const unsigned long ptm_pulsewidth = 6 * 1000 * modifier; // in micro secs (A1)
const unsigned long ptm_spacer = 2 * 1000 * modifier; // in micro secs. This occurs before and after each photometry pulse. This number should be (10 - A1) / 2;

// Opto (10 ms pulses)
// On for 10 milliseconds every B2 * 20 milliseconds (duty cycle = 10 / B2 / 20)
const unsigned long opto_per = 5; // in the number of 50 Hz cycles (B2). Can be set to 1, 2, 5, 10, 25, 50. Frequency is 50 / B2.

// Train for opto
// Train is on for A3 / (50 / B2) milliseconds every B3 millieseconds (duty cycle = A3 / (50 / B2) / B3)
const byte opto_pulses_per_train = 50; // in the number of pulses (A3). Train length is A3 / (50 / B2).
const unsigned long train_cycletime = 30 * 1000 * modifier; // in milli secs (B3)

// Counters
// Using a counter system instead of mod for trains since arduino cannot handle large number mods.
byte train_counter = 0;
byte opto_counter = 0; 
unsigned long opto_spacer_counter = 0;


// Time variables
unsigned long t0, tnow, t0_train, tnow_train;
unsigned long t_debug;

// opto/photometry switch
bool opto = true;
bool ptm = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(ptm_pin, OUTPUT);
  pinMode(opto_pin, OUTPUT);
 
  // Define t 0
  t0 = micros();
  t0_train = millis();
  
  // Debug
  Serial.begin(9600); 
  t_debug = micros();
}

void loop() {
  t_debug = micros();
  // Figure out where we are in a cycle
  tnow = (micros() - t0) % CYCLETIME; // In micro seconds
  tnow_train = millis() - t0_train; // in milli seconds

  if (tnow <= ptm_spacer){
    // If in the pre-spacer range (I), turn off opto stim if on.
    MASTER_MODE = 1;
  }
  else if (tnow <= (ptm_spacer + ptm_pulsewidth)){
    // If in the photometry range (II), turn on photometry if off.
    // Also advance opto spacer counter.
    MASTER_MODE = 2;
  }
  else if (tnow <= (ptm_spacer * 2 + ptm_pulsewidth)){
    // If in the post-spacer range (III), turn off photometry if on
    MASTER_MODE = 3;
  }
  else{
    // If in the opto range (IV)
    MASTER_MODE = 4;
  }

  switch (MASTER_MODE) {
    
    // If in the pre-spacer range (I), turn off opto stim if on.
    case 1:
      if(opto){
        digitalWrite(opto_pin, LOW);
        opto = false;
      }
      break;

    // If in the photometry range (II), turn on photometry if off.
    // Also advance opto spacer counter.
    case 2:
      if (~ptm){
        digitalWrite(ptm_pin, HIGH);
        ptm  = true;
  
        opto_spacer_counter++;
        Serial.println(opto_spacer_counter);
      }
      break;

    // If in the post-spacer range (III), turn off photometry if on
    case 3:
      if (ptm){
        digitalWrite(ptm_pin, LOW);
        ptm  = false;
        
      }
      break;

    // If in the opto range (IV)
    case 4:
      //  Train (start every X seconds)
      if ((tnow_train / train_cycletime) >= train_counter){
        Serial.print("x");
        Serial.println(opto_spacer_counter);
        // Turn on opto pulse if off and if enough time has passed between opto pulses
        if (~opto && ((opto_spacer_counter % opto_per) == 0)){
          
          digitalWrite(opto_pin, HIGH);
          opto = true;
          opto_counter++;
          
        }
        
        // If enough opto pulses have been delivered, reset things
        if (opto_counter >= opto_pulses_per_train){
          
          // Advance train counter
          train_counter++;
  
          // Reset opto counters
          opto_counter = 0;
        }
      }
  }
  
}
