#define ONBOARD_LED 12 //LED is in PIN 12
#define ONBOARD_SW 7   //Onboard switch is in PIN 7

int current_LED = LOW;
byte current_mode = 0;
byte current_Buttonstate = HIGH;
byte previous_Buttonstate = HIGH;

void setup() {
  Serial.begin(115200);
  pinMode(ONBOARD_LED, OUTPUT);
  pinMode(ONBOARD_SW, INPUT);

  digitalWrite(ONBOARD_LED, current_LED);
}

void loop() {
  //button reading portion
  current_Buttonstate = digitalRead(ONBOARD_SW);

  //Transition from HIGH to LOW, button press instance
  if ((current_Buttonstate == LOW) && (previous_Buttonstate == HIGH)){
    current_mode = (current_mode >= 3 ) ? 0: current_mode + 1;
    Serial.print("Button Mode: ");
    Serial.print(current_mode);
  }
  previous_Buttonstate = current_Buttonstate;

  if (current_mode == 0) {
    current_LED = ((millis()/1000) % 2 == 0)? LOW: HIGH;
  }
  else if (current_mode == 1) {
    current_LED = ((millis()/500) % 2 == 0)? LOW: HIGH;
  }
  else if (current_mode == 2) {
    current_LED = ((millis()/100) % 2 == 0)? LOW: HIGH;
  }
  else {
    current_LED = LOW;
  }

  digitalWrite(ONBOARD_LED, current_LED);
}
