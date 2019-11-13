
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//####################################################
//              INCLUDING LIBRARIES                  #
//####################################################
#include <TimerOne.h>
#include <Wire.h>              // for I2C
#include <Adafruit_ADS1015.h>  // ADC1115 16 bit
#include "DigitLedDisplay.h"   // uses SPI

//####################################################
//                 DEFINING VARS.                    #
//####################################################
// DigitLedDisplay ld = DigitLedDisplay(7, 6, 5); // DIN, CS, CLK
Adafruit_ADS1115 ads(0x48); // I2C address of ADS1115 [in decimal = 72, in Hex.decimal = 48 --> 0x48]
float Voltage = 0.0;
int16_t adc0;  // we read from the ADC, we have a sixteen bit integer as a result
bool FLAG = true;
//float DIFF_;
bool none_FLAG = true;
float voltage, REF_, mapped_percent; // voltage is for storing averaged voltage, Voltage is global variable for storing current value
int left, right;
// for detecting rate of change
//float CURRENT_VAL = 0;
//float PREV_VAL = 0;
//float RATE = 0;
//unsigned long LAST_TIME = 0;
//bool SKIP_READINGS = true;
//unsigned long dt = 100; // dt in ms
//unsigned long int counter = 0;

// FOR SETTING FLAGS FOR TIMER for checking no. of counts
float MICRON_VAL = 0, DIA_MM = 0;
bool FLAG_1 = false, FLAG_2 = false;
int COUNTER = 0; // global var. to store no. of HIGH's
//####################################################
//           SOFTWARE TIMER INTERRUPT                #
//####################################################

void show()
{
  unsigned long prev_millis = 0; // LOCAL VARIABLE
  int dt = 3000; // 3 sec

  if (((DIA_MM - REF_) * pow(10, 3)) >= 20 && ((DIA_MM - REF_) * pow(10, 3)) <= 50)
  {
    FLAG_1 = true;
    //    Serial.println("FLAG_1 set true");
  }
  else FLAG_1 = false;
  delay(3000);
  if ( (millis() - prev_millis) >= dt ) // after 3-sec
  {
    //    prev_millis = millis();
    if (((DIA_MM - REF_) * pow(10, 3)) >= 20 && ((DIA_MM - REF_) * pow(10, 3)) <= 50)
    {
      FLAG_2 = true;
      //      Serial.println((millis() - prev_millis)/1000);
//      Serial.print("t3.txt=");
//      Serial.print("\"");
//      Serial.print((CURRENT_VAL - PREV_VAL) / dt );
//      Serial.print(counter);
//      Serial.print("\"");
//      Serial.write(0xff);
//      Serial.write(0xff);
//      Serial.write(0xff);
      //      Serial.println("FLAG_2 set true");
    }
    else FLAG_2 = false;

  }

  if (FLAG_1 == true && FLAG_2 == true)
  {
//    digitalWrite(13, HIGH);
      COUNTER += 1;
      Serial.print("t3.txt=");
      Serial.print("\"");
//      Serial.print("HIGH !");
      Serial.print(COUNTER);
//      Serial.print(counter);
      Serial.print("\"");
      Serial.write(0xff);
      Serial.write(0xff);
      Serial.write(0xff);

      
  }
  else if (FLAG_1 == false && FLAG_2 == false)
  {
//    digitalWrite(13, LOW);
      Serial.print("t3.txt=");
      Serial.print("\"");
//      Serial.print("LOW!");
      Serial.print(COUNTER);
      Serial.print("\"");
      Serial.write(0xff);
      Serial.write(0xff);
      Serial.write(0xff);
  }
  else if (FLAG_1 == true && FLAG_2 == false)
  {
//    digitalWrite(13, LOW);/
          Serial.print("t3.txt=");
      Serial.print("\"");
//      Serial.print("LOW!");
      Serial.print(COUNTER);
      Serial.print("\"");
      Serial.write(0xff);
      Serial.write(0xff);
      Serial.write(0xff);
  }
  else if (FLAG_1 == false && FLAG_2 == true)
  {
//    digitalWrite(13, LOW);
      Serial.print("t3.txt=");
      Serial.print("\"");
//      Serial.print("LOW!");
      Serial.print(COUNTER);
      Serial.print("\"");
      Serial.write(0xff);
      Serial.write(0xff);
      Serial.write(0xff);
  }
  if (COUNTER == 3)
  {
    showCalibrationMsg();
    COUNTER = 0;
  }
}

void showCalibrationMsg()
{
  Serial.print("t0.txt=");
  Serial.print("\"");
  Serial.print("Calibrate Again");
  Serial.print("\"");
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);
}
// ####################################################
//                  MAPPING FUNCTION                  #
//     to map MICRON diff. --> int percentage         #
//               for progress bar                     #
// ####################################################
int micron_to_percent(float x) {
  // takes in micron difference as arg. and outputs the percentage (int) value for progress-bar
  float y = ((-100 / 30) * x) + 100;
  return int(y);
}
// ###################################################
//                VOLTAGE ---> MICRON                #
// ###################################################
/**
   @brief Takes a floating value representing "mV" and converts it to "mm"
   @details x must be in mV and calibration was done by averaging 10 values where each value in itself was an average of 100 readings.

   @param x float, mV
   @return float, mm
*/
float mV2mm(float x)
{
  float y = ((10.0 / (-3434.233)) * x) + ((10 / 3434.233) * 3434.050);
  return 10.000 - y;
}
// ####################################################
//                  SPLIT into left & right           #
// ####################################################
/**
   @brief split a floating val. to parts before and after decimal
   @details Takes in a floating value. Then applies subsequently "floor" and "pow" functions to split it

   @param x float
   @return left,right float
*/
char splitLR(float x) {
  //           Takes X as float as input                #
  //        and outputs LEFT and RIGHT as integers
  char left = char(floor(x)); // 2
  char right = char((x - left) * pow(10, 3)); // 2.032 - 2 = 0.032 --> 0.032 * 10e3 --> 32 microns

  return left, right;
}
// ####################################################
//                  50-150 to PERCENTAGE              #
// ####################################################
/**
   @brief Takes in OUT-REF i.e. X as input and outputs corresponding PERCENTAGE val.

   @param x float, difference reading (3dp of the DIFF.)
   @return y int, output percentage 100%-->150 and 0%-->50
*/
//int fifty_150_2_percentage(float x)
//{
//  float y = x - 50.0;
//  constrain(y, 0, 100); // constrain value of y between 0-100 to avoid errors from screen
//  return int(y);
//}
int fifty_150_2_percentage(float x)
{
  float y = -x + 150.0;
  constrain(y, 0, 100); // constrain value of y between 0-100 to avoid errors from screen
  return int(y);
}
// ###################################
//           FUNCTION-average        #
//####################################
/**
   @brief takes average of the number of readings specified
   @details Takes in an integer as arg. which corresponds to the number of reading of which to take AVERAGE

   @param x int, no. of readings to take avr. of
   @return averaged float, averaged reading
*/
float average(int x)
{
  int n = x;
  float vals[n] = {0};

  float sum = 0;
  float averaged = 0;

  for (int i = 0; i < n; i++)
  {
    // Get voltage from sensor
    // THEN
    adc0 = ads.readADC_SingleEnded(0);
    Voltage = (adc0 * 0.1875F);
    vals[i] = Voltage;
  }

  // array is filled with values
  for (int i = 0; i < n; i++) {
    sum = sum + vals[i];
  }

  averaged = (float)sum / n;
  return averaged;
}
// ####################################################
//                      SETUP                         #
// ####################################################
void setup()
{
  // REF_ = 0.000;
  Serial.begin(9600);
//  counter = 1;
  pinMode(4, INPUT); // BUTTON for saving current reading

  ads.begin(); // adc object
  ads.setGain(GAIN_TWOTHIRDS); // 1 bit = 0.1875mV // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)

  //  for counting no. of times while grinding
  pinMode(13, OUTPUT);
  Timer1.initialize(3000000); // micro seconds
  Timer1.attachInterrupt(show);
}


void loop()
{
  // ####################################################
  //              Commented section is for              #
  //                  calibrating at 0mm                #
  //              and extreme (10mm) position           #
  // ####################################################
  //     float vals[10] = {0};
  //     for (int i=0;i<10;i++)
  //      {
  //        vals[i] = average(100);
  //      }
  //      float sum = 0.0;
  //      for (int i=0;i<10;i++)
  //
  //      {
  //        sum = sum + vals[i];
  //      }


  voltage = average(50); // in mV
  //  COUNTING USING RATE OF CHANGE THRESHOLD METHOD
  //  if (millis() - LAST_TIME >= dt)
  //  {
  //    LAST_TIME = millis();
  //    CURRENT_VAL = voltage;
  //if ((CURRENT_VAL - PREV_VAL) / dt > 12.)
  //{
  //      Serial.print("t3.txt=");
  //  Serial.print("\"");
  ////  Serial.print((CURRENT_VAL - PREV_VAL) / dt );
  //Serial.print(counter);
  //  Serial.print("\"");
  //  Serial.write(0xff);
  //  Serial.write(0xff);
  //  Serial.write(0xff);
  //  counter += 1;
  //}
  //    PREV_VAL = CURRENT_VAL;
  //  }

  //     Serial.println(voltage,3);
  float dia_mm = mV2mm(voltage);
  DIA_MM = dia_mm; // for grinding check
  //     Serial.print("Dia. mm");
  //     Serial.println(dia_mm);
  char out[20];
  dtostrf(dia_mm, 6, 3, out);

  // REALTIME VALUE
  Serial.print("t1.txt=");
  Serial.print("\"");
  Serial.print(out);
  Serial.print("\"");
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);

  if (FLAG) {
    // if button is not-pressed,
    Serial.print("t2.txt=");
    //      Serial.print("\"");
    //      Serial.print(left);
    //      Serial.print("\"");
    Serial.print("\"");
    Serial.print("-------");
    Serial.print("\"");
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);


    if (digitalRead(4) == LOW) {
      // When button is PRESSED show "Reference SET" for 1-sec.
      REF_ = dia_mm;
      MICRON_VAL = (dia_mm - REF_) * pow(10, 3);
      FLAG = LOW;

      Serial.print("t2.txt=");
      Serial.print("\"");
      Serial.print("Reference SET");
      Serial.print("\"");
      Serial.write(0xff);
      Serial.write(0xff);
      Serial.write(0xff);

      none_FLAG = LOW;
      delay(1000);
    }
  }
  else if (!FLAG)
    // When flag is LOW, show text on "t2" and "t0"

  {
    // ####################################################
    //              Write REF (saved) value to            #
    //                      t2 field                      #
    // ####################################################
    char dummy[10];
    dtostrf(REF_, 6, 3, dummy); // for example 5.236 mm
    Serial.print("t2.txt=");
    Serial.print("\"");
    Serial.print(dummy);
    Serial.print("\"");
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);

    // ####################################################
    //             if > 200, write +200..                 #
    // ####################################################
    if ((dia_mm - REF_)*pow(10, 3) > +200.0) {
      // ####################################################
      //                      CASE-1                        #
      // ####################################################
      // if dia_mm is greater than 50.0 then show +50....
      //          Serial.println("CASE-1");
      Serial.print("t0.txt=");
      Serial.print("\""); // write value in the QUOTATION MARKS
      Serial.print("+50");
      Serial.print("\"");
      Serial.write(0xff);
      Serial.write(0xff);
      Serial.write(0xff);

      Serial.print("p.val=");
      Serial.print("0");
      Serial.write(0xff);
      Serial.write(0xff);
      Serial.write(0xff);

      Serial.print("p.bco=");
      Serial.print("YELLOW");
      Serial.write(0xff);
      Serial.write(0xff);
      Serial.write(0xff);

      Serial.print("p.pco=");
      Serial.print("BLACK");
      Serial.write(0xff);
      Serial.write(0xff);
      Serial.write(0xff);
    }
    // ####################################################
    //          if diff. < 0.0, write <0                  #
    // ####################################################
    else if ((dia_mm - REF_)*pow(10, 3) < (0.0)) {
      // ####################################################
      //                      CASE-2                        #
      // ####################################################

      if ((dia_mm - REF_)*pow(10, 3) > (-50.0)) {
        //          Serial.println("CASE-2");
        char D[10];
        dtostrf((dia_mm - REF_)*pow(10, 3), 5, 0, D);
        Serial.print("t0.txt=");
        Serial.print("\"");
        Serial.print(D);
        Serial.print("\"");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);


        Serial.print("p.val=");
        Serial.print("100");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);

        Serial.print("p.bco=");
        Serial.print("YELLOW");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);

        Serial.print("p.pco=");
        Serial.print("BLACK");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);
      }

      if ((dia_mm - REF_)*pow(10, 3) <= (-50.0))
      {
        Serial.print("t0.txt=");
        Serial.print("\"");
        Serial.print("-50");
        Serial.print("\"");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);

        Serial.print("p.val=");
        Serial.print("100");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);

        Serial.print("p.bco=");
        Serial.print("YELLOW");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);

        Serial.print("p.pco=");
        Serial.print("BLACK");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);
      }
    }

    // ####################################################
    //              if val (diff) > 0 and < 200           #
    // ####################################################

    else if (((dia_mm - REF_)*pow(10, 3) >= 0) && ((dia_mm - REF_)*pow(10, 3) <= 200))
    {
      if (((dia_mm - REF_)*pow(10, 3) > 50) && ((dia_mm - REF_)*pow(10, 3) <= 200))
      {

        // ####################################################
        //                      CASE-3                        #
        // ####################################################
        //          Serial.println("CASE-3");
        int PRCNT = fifty_150_2_percentage((dia_mm - REF_) * pow(10, 3));
        Serial.print("p.val=");
        //          Serial.print("\"");
        Serial.print(PRCNT);
        //          Serial.print("\"");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);
        // changing foreground colour to YELLOW
        Serial.print("p.pco=");
        Serial.print("BLACK");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);
        // changing background colour to BLACK
        Serial.print("p.bco=");
        Serial.print("YELLOW");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);

        // INDICATE at t0 field that value is greater than "50" micron
        Serial.print("t0.txt=");
        Serial.print("\"");
        Serial.print("+50");
        Serial.print("\"");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);
      }
      else if (((dia_mm - REF_)*pow(10, 3) <= 50))
      {
        // ####################################################
        //                      CASE-4                        #
        // ####################################################
        // PROGRESS BAR should remain at 0 PERCENT when val. is less than 50 microns
        //          Serial.println("CASE-4");
        Serial.print("p.val=");
        Serial.print("100");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);
        // changing foreground colour to BLACK
        Serial.print("p.pco=");
        Serial.print("BLACK");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);
        // changing background colour to YELLOW
        Serial.print("p.bco=");
        Serial.print("YELLOW");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);

        char dummy[10];
        dtostrf((dia_mm - REF_)*pow(10, 3), 4, 0, dummy);
        Serial.print("t0.txt=");
        Serial.print("\"");
        Serial.print(dummy);
        Serial.print("\"");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);

      }
    }
  }
  if (none_FLAG == HIGH)
  {
    Serial.print("t0.txt=");
    Serial.print("\"");
    Serial.print("None");
    Serial.print("\"");
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);

    Serial.print("p.val=");
    Serial.print("100");
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);

    Serial.print("p.bco=");
    Serial.print("YELLOW");
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);

    Serial.print("p.pco=");
    Serial.print("BLACK");
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);

  }


}


// ####################################################
//                  micron to percentage              #
// ####################################################
int get_percentage(float x)
{

  float y = x + 50;
  return y;
}
