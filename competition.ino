/* 
 *  Ex_18 - NEC Decoder
 *  Decode incoming infrared Signal on the Robot
 *   
 *  I2C 
 *  --  GP18 (SDA) 
 *  --  GP19 (SCL) 
 *  
 *  LED
 *  -- GP12
 *  
 *  IR_RECV
 *  -- GP6
 *  
 *  NeoPixel
 *  -- GP20
 */

#include "pitches.h"
   #define MotorA_1 8
#define MotorA_2 9
#define MotorB_1 10
#define MotorB_2 11     
#define ONBOARD_LED 12 
#define SPK_OUT   22      



#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include <Adafruit_NeoPixel.h>

#define I2C1_SDA 18
#define I2C1_SCL 19

#define PEXP_I2CADDR 0x23
#define OLED_I2CAADR 0x3C
#define SR04_I2CADDR 0x57
#define OLED_I2CAADR 0x3C

#define NEO_PIXEL 20
#define LED_COUNT 15

#define LED 12
#define IR_RECV 6

#define LS 0 

SSD1306AsciiWire oled(Wire1);

Adafruit_NeoPixel strip(LED_COUNT, NEO_PIXEL, NEO_GRB + NEO_KHZ800);

uint8_t LEDState = LOW;

//Just some variables to hold temporary data
char text[10];
uint8_t  i, nec_state = 0; 
unsigned int command, address;

static unsigned long nec_code;
static boolean nec_ok = false;
static unsigned long timer_value_old;
static unsigned long timer_value;

int ping_mm()
{
    unsigned long distance = 0;
    byte i;   
    byte ds[3];
    long timeChecked;
    
    Wire1.beginTransmission(SR04_I2CADDR);
    Wire1.write(1);          //1 = cmd to start measurement.
    Wire1.endTransmission();

    delay(200); // Delay 200ms

    i = 0;
    Wire1.requestFrom(0x57,3);  //read distance       
    while (Wire1.available())
    {
     ds[i++] = Wire1.read();
    }        
    
    distance = (unsigned long)(ds[0] << 16);
    distance = distance + (unsigned long)(ds[1] << 8);
    distance = (distance + (unsigned long)(ds[2])) / 1000;
    //measured value between 10 mm (1 cm) to 6 meters (600 cm)
    if ((10 <= distance) && (6000 >= distance)) {
        return (int)distance;
    }
    else {
        return -1;
    }
}

void irISR() {
    
    timer_value = micros() - timer_value_old;       // Store elapse timer value in microS
    
    switch(nec_state){
        case 0:   //standby:                                      
            if (timer_value > 67500) {          // Greater than one frame...
                timer_value_old = micros();     // Reset microS Timer
                nec_state = 1;                  // Next state: end of 9ms pulse + LeadingSpace 4.5ms
                i = 0;
            }
        break;

        // Leading Mark = Leading pulse + leading space = 9000 + 4500 = 13500
        // max tolerance = 1000
        case 1:   //startPulse:
            if (timer_value >= (13500 - 1000) && timer_value <= (13500 + 1000)) { //its a Leading Mark
                i = 0;
                timer_value_old = micros();
                nec_state = 2;
            }
            else 
                nec_state = 0;
        break;

        //Bit 0 Mark length = 562.5µs pulse + 562.5µs space = 1125
        //Bit 1 Mark length = 562.5µs pulse + 3 * 562.5µs space = 2250
        //max tolerance = (2250 - 1125)/2 = 562.5 
        case 2:   //receiving:
            if (timer_value < (1125 - 562) || timer_value > (2250 + 562)) nec_state = 0; //error, not a bit mark
            else { // it's M0 or M1
                  nec_code = nec_code << 1; //push a 0 from Right to Left (will be left at 0 if it's M0)
                  if(timer_value >= (2250 - 562)) nec_code |= 0x01; //it's M1, change LSB to 1
                  i++;
                  
                  if (i==32) { //all bits received
                      nec_ok = true;
//                    detachInterrupt(IR_RECV);   // Optional: Disable external interrupt to prevent next incoming signal
                      nec_state = 0;
                      timer_value_old = micros();
                  }
                  else {
                      nec_state = 2; //continue receiving
                      timer_value_old = micros();
                  }
              }
        break;
      
        default:
            nec_state = 0;
        break;
    }
}

void setup() {
    Wire1.setSDA(I2C1_SDA);
    Wire1.setSCL(I2C1_SCL);
    Wire1.begin();  

    strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.show();            // Turn OFF all pixels ASAP
    strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
    randomSeed(analogRead(0));
    
    oled.begin(&Adafruit128x32, OLED_I2CAADR); 
    Serial.begin(115200); //set up serial library baud rate to 115200
    delay(2000);
                    
    pinMode(LED, OUTPUT);
    digitalWrite(LED, LEDState);
    pinMode(IR_RECV, INPUT);
    delay(2000);
    
    oled.setFont(Adafruit5x7);
    oled.clear();
    oled.println("NEC Decoder!");
    
    attachInterrupt(IR_RECV, irISR, FALLING);  //Setting IR detection on FALLING edge.

     pinMode(ONBOARD_LED, OUTPUT);
   pinMode(MotorA_1, OUTPUT);
    pinMode(MotorA_2, OUTPUT);
    pinMode(MotorB_1, OUTPUT);
    pinMode(MotorB_2, OUTPUT);
    digitalWrite(MotorA_1, LOW);
    digitalWrite(MotorA_2, LOW);
    digitalWrite(MotorB_1, LOW);
    digitalWrite(MotorB_2, LOW);
}
void playTone(unsigned int frequency, unsigned int duration){
    tone(SPK_OUT, frequency, duration - 10); 
    delay(duration);
}
void loop() {
  
  int uDistance = ping_mm();
    if (uDistance > 0) {
        Serial.println("Range <->");
        Serial.print("Distance(mm): "); 
        Serial.println(ping_mm());
        delay(500);
    } 



//     while (uDistance < 200){

// motorMoveControl(0,1,0);
//           motorMoveControl(1,0,0);
          
//            delay(1000);

//           motorMoveControl(0,1,40);
//           motorMoveControl(1,0,40);

//           delay(1000);

//           motorMoveControl(0,0,10);
//           motorMoveControl(1,0,10);

//     }
    if(nec_ok) {                                     // If a good NEC message is received
        nec_ok = false;                             // Reset decoding process
        
        LEDState = (LEDState == LOW) ? HIGH: LOW;   // Toggle LED to show new IR result
        digitalWrite(LED, LEDState);

        oled.clear();
        //oled.println("NEC IR Received:");
        
        oled.print("Addr: ");
        address = nec_code >> 16;
        command = (nec_code & 0xFFFF) >> 8;         // Remove inverted Bits
        sprintf(text, "%04X", address);
        oled.println(text);                             // Display address in hex format

        oled.print("Cmd: ");
        sprintf(text, "%02X", command);
        oled.println(text);        

// oled.print("Light:");
//         oled.println(LS);
        
       if (uDistance < 200){

playTone(NOTE_E1, 1000);
playTone(NOTE_C4, 500);
playTone(NOTE_E2, 1000);
playTone(NOTE_E3, 700);

 oled.print ("There is an obstruction"); 
          motorMoveControl(0,1,0);
          motorMoveControl(1,0,0);
          
           delay(1000);

          motorMoveControl(0,1,40);
          motorMoveControl(1,0,40);

          delay(1000);

          motorMoveControl(0,0,10);
          motorMoveControl(1,0,10);

           delay(1000);

          motorMoveControl(0,1,0);
          motorMoveControl(1,0,0);

          }
          
          
           if (command == 0x10) {
          oled.print("Turn left");
          motorMoveControl(0,1,20);
           motorMoveControl(1,0,20); 

if (uDistance < 200){

playTone(NOTE_E1, 1000);
playTone(NOTE_C4, 500);
playTone(NOTE_E2, 1000);
playTone(NOTE_E3, 700);

 oled.print ("There is an obstruction"); 
          motorMoveControl(0,1,0);
          motorMoveControl(1,0,0);
          
           delay(1000);

          motorMoveControl(0,1,40);
          motorMoveControl(1,0,40);

          delay(1000);

          motorMoveControl(0,0,10);
          motorMoveControl(1,0,10);

           delay(1000);

          motorMoveControl(0,1,0);
          motorMoveControl(1,0,0);

          }

    // strip.setPixelColor(3, strip.Color(0,0,255));
    // strip.setPixelColor(4, strip.Color(0,0,255));
    // strip.setPixelColor(5, strip.Color(0,0,255));
    // strip.setPixelColor(6, strip.Color(0,0,255));
    // strip.setPixelColor(7, strip.Color(100,100,100));
    // strip.setPixelColor(8, strip.Color(100,100,100));
    // strip.setPixelColor(9, strip.Color(100,100,100));
    // strip.setPixelColor(10, strip.Color(100,100,100));
    // strip.setPixelColor(11, strip.Color(100,100,100));
    // strip.setPixelColor(12, strip.Color(100,100,100));
    // strip.setPixelColor(13, strip.Color(100,100,100));
    // strip.setPixelColor(14, strip.Color(100,100,100));
    //  strip.show();
        }  else if (command == 0x5A){ //right 
          oled.print ("Turn Right"); 
          motorMoveControl(0,0,20);
          motorMoveControl(1,1,20); 



if (uDistance < 200){

playTone(NOTE_E1, 1000);
playTone(NOTE_C4, 500);
playTone(NOTE_E2, 1000);
playTone(NOTE_E3, 700);

 oled.print ("There is an obstruction"); 
          motorMoveControl(0,1,0);
          motorMoveControl(1,0,0);
          
           delay(1000);

          motorMoveControl(0,1,40);
          motorMoveControl(1,0,40);

          delay(1000);

          motorMoveControl(0,0,10);
          motorMoveControl(1,0,10);

           delay(1000);

          motorMoveControl(0,1,0);
          motorMoveControl(1,0,0);

          }
    //  strip.setPixelColor(3, strip.Color(100,100,100));
    // strip.setPixelColor(4, strip.Color(100,100,100));
    // strip.setPixelColor(5, strip.Color(100,100,100));
    // strip.setPixelColor(6, strip.Color(100,100,100));
    // strip.setPixelColor(7, strip.Color(100,100,100));
    // strip.setPixelColor(8, strip.Color(100,100,100));
    // strip.setPixelColor(9, strip.Color(100,100,100));
    // strip.setPixelColor(10, strip.Color(100,100,100));     
    // strip.setPixelColor(11, strip.Color(0,0,255));
    // strip.setPixelColor(12, strip.Color(0,0,255));
    // strip.setPixelColor(13, strip.Color(0,0,255));
    // strip.setPixelColor(14, strip.Color(0,0,255));
    // strip.show();

        }   else if (command == 0x18) { //up 
 oled.print ("Go Forward"); 
          motorMoveControl(0,0,40);
          motorMoveControl(1,0,40);


if (uDistance < 200){

// playTone(NOTE_E1, 1000);
// playTone(NOTE_C4, 500);
// playTone(NOTE_E2, 1000);
// playTone(NOTE_E3, 700);

 oled.print ("There is an obstruction"); 
          motorMoveControl(0,1,0);
          motorMoveControl(1,0,0);
          
           delay(1000);

          motorMoveControl(0,1,40);
          motorMoveControl(1,0,40);

          delay(1000);

          motorMoveControl(0,0,10);
          motorMoveControl(1,0,10);

           delay(1000);

          motorMoveControl(0,1,0);
          motorMoveControl(1,0,0);

          }
// strip.setPixelColor(3, strip.Color(100,100,100));
//     strip.setPixelColor(4, strip.Color(100,100,100));
//     strip.setPixelColor(5, strip.Color(100,100,100));
//     strip.setPixelColor(6, strip.Color(100,100,100));
//     strip.setPixelColor(7, strip.Color(0,255,0));
//     strip.setPixelColor(8, strip.Color(0,255,0));
//     strip.setPixelColor(9, strip.Color(0,255,0));
//     strip.setPixelColor(10, strip.Color(0,255,0));
//      strip.setPixelColor(11, strip.Color(100,100,100));
//     strip.setPixelColor(12, strip.Color(100,100,100));
//     strip.setPixelColor(13, strip.Color(100,100,100));
//     strip.setPixelColor(14, strip.Color(100,100,100));
//      strip.show();

        }   else if (command == 0x4A) {//down 
 oled.print ("Go back"); 
 motorMoveControl(0,1,20);



if (uDistance < 200){

playTone(NOTE_E1, 1000);
playTone(NOTE_C4, 500);
playTone(NOTE_E2, 1000);
playTone(NOTE_E3, 700);

 oled.print ("There is an obstruction"); 
          motorMoveControl(0,1,0);
          motorMoveControl(1,0,0);
          
           delay(1000);

          motorMoveControl(0,1,40);
          motorMoveControl(1,0,40);

          delay(1000);

          motorMoveControl(0,0,10);
          motorMoveControl(1,0,10);

           delay(1000);

          motorMoveControl(0,1,0);
          motorMoveControl(1,0,0);

          }
// strip.setPixelColor(3, strip.Color(100,100,100));
//     strip.setPixelColor(4, strip.Color(100,100,100));
//     strip.setPixelColor(5, strip.Color(100,100,100));
//     strip.setPixelColor(6, strip.Color(100,100,100));
//  strip.setPixelColor(7, strip.Color(255,0,0));
//     strip.setPixelColor(8, strip.Color(255,0,0));
//     strip.setPixelColor(9, strip.Color(255,0,0));
//     strip.setPixelColor(10, strip.Color(255,0,0));
// strip.setPixelColor(11, strip.Color(100,100,100));
//     strip.setPixelColor(12, strip.Color(100,100,100));
//     strip.setPixelColor(13, strip.Color(100,100,100));
//     strip.setPixelColor(14, strip.Color(100,100,100));
//      strip.show();

          motorMoveControl(1,1,20);
        } else if (command == 0x38) { //stop
 oled.print ("Stop"); 
 motorMoveControl(0,1,0);
          motorMoveControl(1,0,0);

    // strip.setPixelColor(3, strip.Color(100,100,100));
    // strip.setPixelColor(4, strip.Color(100,100,100));
    // strip.setPixelColor(5, strip.Color(100,100,100));
    // strip.setPixelColor(6, strip.Color(100,100,100));
    // strip.setPixelColor(7, strip.Color(100,100,100));
    // strip.setPixelColor(8, strip.Color(100,100,100));
    // strip.setPixelColor(9, strip.Color(100,100,100));
    // strip.setPixelColor(10, strip.Color(100,100,100));
    // strip.setPixelColor(11, strip.Color(100,100,100));
    // strip.setPixelColor(12, strip.Color(100,100,100));
    // strip.setPixelColor(13, strip.Color(100,100,100));
    // strip.setPixelColor(14, strip.Color(100,100,100));
    // strip.show();

if (uDistance < 200){

playTone(NOTE_E1, 1000);
playTone(NOTE_C4, 500);
playTone(NOTE_E2, 1000);
playTone(NOTE_E3, 700);

 oled.print ("There is an obstruction"); 
          motorMoveControl(0,0,0);
          motorMoveControl(1,0,0);
          
           delay(1000);

          motorMoveControl(0,1,40);
          motorMoveControl(1,0,40);

          delay(1000);

          motorMoveControl(0,0,10);
          motorMoveControl(1,0,10);

           delay(1000);

          motorMoveControl(0,1,0);
          motorMoveControl(1,0,0);

          }


        }
//         } else if (uDistance < 200){

// playTone(NOTE_C1, 1500);
// playTone(NOTE_C4, 500);
// playTone(NOTE_C1, 1500);
// playTone(NOTE_C4, 500);

//  oled.print ("There is an obstruction"); 
//           motorMoveControl(0,1,0);
//           motorMoveControl(1,0,0);
          
//            delay(1000);

//           motorMoveControl(0,1,40);
//           motorMoveControl(1,0,40);

//           delay(1000);

//           motorMoveControl(0,0,10);
//           motorMoveControl(1,0,10);

//            delay(1000);

//           motorMoveControl(0,1,0);
//           motorMoveControl(1,0,0);

//           }

//      attachInterrupt(IR_RECV, irISR, FALLING);  //Setting IR detection on FALLING edge.
     
  //   strip.setPixelColor(0, strip.Color(255,0,0));
  //   strip.setPixelColor(1, strip.Color(0,255,0));
  //  strip.setPixelColor(2, strip.Color(0,0,255));
  //   strip.setPixelColor(3, strip.Color(255,0,0));
  //   strip.setPixelColor(4, strip.Color(0,255,0));
  //   strip.setPixelColor(5, strip.Color(0,0,255));
  //   strip.setPixelColor(6, strip.Color(255,0,0));
  //   strip.setPixelColor(7, strip.Color(0,255,0));
  //   strip.setPixelColor(8, strip.Color(0,0,255));
  //   strip.setPixelColor(9, strip.Color(255,0,0));
  //   strip.setPixelColor(10, strip.Color(0,255,0));
  //   strip.setPixelColor(11, strip.Color(0,0,255));
  //   strip.setPixelColor(12, strip.Color(255,0,0));
  //   strip.setPixelColor(13, strip.Color(0,255,0));
  //   strip.setPixelColor(14, strip.Color(0,0,255));
  //   strip.show();
    
        }}

void motorMoveControl(unsigned short motor, unsigned short direction, unsigned short speed) {    
            analogWrite((motor == 0) ? MotorA_1 : MotorB_1, (direction == 0) ? 0 : ((speed > 0) && (speed < 100)? speed +150 : 0));
            analogWrite((motor == 0) ? MotorA_2 : MotorB_2, (direction == 1) ? 0 : ((speed > 0) && (speed < 100)? speed +150 : 0));
 }
