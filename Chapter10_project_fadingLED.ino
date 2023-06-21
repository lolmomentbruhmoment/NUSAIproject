/* 
 *  Ex_10 - AnalogOut
 *  Simple Triangle breathing LED on the TomatoCube Pico MainBoard
 *  LED 
 *  -- GP12
 *  
 */

#define ONBOARD_LED 12    // LED GP12

float smoothness_pts = 1000;
int x = 0;

float gammaC = 0.14;
float betaC = 0.5;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(ONBOARD_LED, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
    if (x >= smoothness_pts)
        x = 0;
    //Simple triangle breathing LED on the PICO-W board
    float pwm_val = 255.0*(1.0 -  abs((2.0*(x/smoothness_pts))-1.0));

    //Circular Wave breathing LED on the PICO-W board
    float pwm_val = 255.0*sqrt(1.0 -  pow(abs((2.0*(x/smoothness_pts))-1.0),2.0));

    //Gausian Functions breathing LED on the PICO-W board
    float pwm_val = 255.0*(exp(-(pow(((x/smoothness_pts)-betaC)/gammaC,2.0))/2.0));

    analogWrite(ONBOARD_LED,int(pwm_val));
    x += 1;
    delay(5);                    
}
                    
                
