// This is the simple version.

const float modifier = 1.006;
const byte ptm_pin = 0; // 0
const byte opto_pin = 2; // 2

// Everything operates in 20 ms cycles (50 Hz)
// Order to things: buffer -> photometry -> buffer -> opto pulses
const int ptm_width = 6000 * modifier; // in micro secs
const int ptm_buffer_pre = 3500 * modifier; // in micro secs
const int ptm_buffer_post = 500 * modifier; // in micro secs
const int opto_width = 10000 * modifier; // in micro secs

// Opto pulse settings
const byte opto_per = 5; // Number of photometry pulses per opto pulse (A). Can be: 1, 2, 5, 10, 25, 50.
const byte train_length = 50; // Number of opto pulses per train (B). Duration is  B / (50 / A).
const long train_cycle = 30 * 50; // First number is in seconds. How often does the train come on.

// Switch
bool opto = false;
bool train = false;

// Counters
byte pmt_counter_for_opto = 0;
byte opto_counter = 0;
long counter_for_train = 0; // Number of cycles

// Debug variables
// unsigned long t0;


void setup() {
  // put your setup code here, to run once:
  pinMode(ptm_pin, OUTPUT);
  pinMode(opto_pin, OUTPUT);  

  // Serial.begin(9600);
}

void loop() {
  // t0 = micros();

  // Advance counters
  pmt_counter_for_opto++;
  counter_for_train++;

  // Beginning of each train cycle. Turn train mode on.
  if (counter_for_train == 1){
    train = true;
  }

  // Turn on opto mode
  if (train){
    if (pmt_counter_for_opto==1){
      opto = true;
      opto_counter++;
    }
  }
  
  // Pre-buffer
  delayMicroseconds(ptm_buffer_pre);

  // Turn on and off photometry LED
  digitalWrite(ptm_pin, HIGH);
  delayMicroseconds(ptm_width);
  digitalWrite(ptm_pin, LOW);
  delayMicroseconds(ptm_buffer_post);

  // Turn on opto LED
  if (opto){ 
    digitalWrite(opto_pin, HIGH);  
  }
  else {
    digitalWrite(opto_pin, LOW);  
  }

  // Turn off opto LED
  delayMicroseconds(opto_width);
  digitalWrite(opto_pin, LOW);

  // Flag LED as off
  opto = false;

  // reset pmt counter
  if (pmt_counter_for_opto >= opto_per){
    pmt_counter_for_opto = 0;
  }
  
  // reset opto counter
  if (opto_counter >= train_length){
    
    train = false;
    opto_counter = 0;
  }

  // Serial.println(counter_for_train);
  // reset train counter
  if (counter_for_train >= train_cycle){
    counter_for_train= 0;
  }
  
  // Serial.println(micros() - t0);
  
}
