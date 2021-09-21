//Force Mat python script Spring 2021
//worked on by Rhys Cornelius and Shawn Steven


// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
//the above code reduced the time it took to perform digital write

//use the following variables to ensure that all the multiplexers are connected correctly
//row 1 = the 16 pin multiplexer
//row 2 =  the corresponding 8 multiplexers
// column1 = the main column multiplexer
// column2 = the remaining column multiplexers

const int row1Pins[4] = {30, 31, 32, 33}; // S0~40, S1~41, S2~42
const int row2Pins[3] = {36, 37, 38};
const int column1Pins[3] = {26, 27, 28};
const int column2Pins[3] = {22, 23, 24}; 

const int s0 = 30;
const int s1 = 31;
const int s2 = 32;
const int s3 = 33;

float time1 = 0;
float time2 = 0;

//you will need to connect the z output for the row1 multiplexer and set it to HIGH later on setup
const int zOutput = ;

//this should be the z input for column1 multiplexer
const int zInput = A15;

void setup() {
  //more code to make digital write faster  
  // set prescale to 16
  sbi(ADCSRA,ADPS2) ;
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;

  //setting the baudrate  
  Serial.begin(74880);

  // Set up the select pins, as outputs and set them all to low
  for (int i=0; i<3; i++)
  {
    pinMode(row1Pins[i], OUTPUT);
    digitalWrite(row1Pins[i], LOW);

    pinMode(row2Pins[i], OUTPUT);
    digitalWrite(row2Pins[i], LOW);

    pinMode(column1Pins[i], OUTPUT);
    digitalWrite(column1Pins[i], LOW);

    pinMode(column2Pins[i], OUTPUT);
    digitalWrite(column2Pins[i], LOW);
  }
}

void loop() 
{
  //this delay may or may not be required
  delay(100);
  //time1 is equivalent to f1 in the python script, please look there for more info
  time1 = millis();

  // Cycle from pins Y0 to Y15
  for (int row1 = 0; row1 <= 15; row1 ++) {
    selectMuxPin16(row1);

    // Cycle from pins Y0 to Y7
    for (int row2 = 0; row2 <= 7; row2++){
      selectMuxPin8(row2, row2Pins);
      

      // Cycle from pins Y0 to Y7
      for (int column1 = 0; column1 <= 7; column1++){
        selectMuxPin8(column1, column1Pins);
        
        // Cycle from pins Y0 to Y7
        for(int column2 = 0; column2 <= 7; column2++){
          selectMuxPin8(column2, column2Pins);

          float serialOutput = analogRead(zInput);
          Serial.print(serialOutput);
          //the newline may or may not be needed
          Serial.print("\n");

        }
      }
    }
  } 
    //arbitray letter to signify that all the points have been sent to the serial
    Serial.print("M");
    time2 = millis();
    //the following two lines of code were used to determine what took the longest, see f1 & f2 in the python script
    Serial.print(abs(time1 - time2)); 
    Serial.print("\n");
}

// The selectMuxPin function sets the S0, S1, and S2 pins
// accordingly, given a pin from 0-7 and the pins that the S0, S1,
// and S2 are connected to.
void selectMuxPin8(byte pin, const int pinArray[]) {
  if (pin > 7) return; // Exit if pin is out of scope
  for (int i=0; i<3; i++)
  {
    if (pin & (1<<i))
      digitalWrite(pinArray[i], HIGH);
    else
      digitalWrite(pinArray[i], LOW);
  }
}

//function to select any pin on the 16 output multiplexer
void selectMuxPin16(int channel){
  int controlPin[] = {s0, s1, s2, s3};

  int muxChannel[16][4]={
    {0,0,0,0}, //channel 0
    {1,0,0,0}, //channel 1
    {0,1,0,0}, //channel 2
    {1,1,0,0}, //channel 3
    {0,0,1,0}, //channel 4
    {1,0,1,0}, //channel 5
    {0,1,1,0}, //channel 6
    {1,1,1,0}, //channel 7
    {0,0,0,1}, //channel 8
    {1,0,0,1}, //channel 9
    {0,1,0,1}, //channel 10
    {1,1,0,1}, //channel 11
    {0,0,1,1}, //channel 12
    {1,0,1,1}, //channel 13
    {0,1,1,1}, //channel 14
    {1,1,1,1}  //channel 15
  };

  digitalWrite(s0, muxChannel[channel][0]);
  digitalWrite(s1, muxChannel[channel][1]);
  digitalWrite(s2, muxChannel[channel][2]);
  digitalWrite(s3, muxChannel[channel][3]);
  
}
