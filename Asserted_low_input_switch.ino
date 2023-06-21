#define ONBOARD_LED 12
#define ONBOARD_SW 7

int current_LED = LOW;

void setup() {
  Serial.begin(115200);
  pinMode(ONBOARD_LED, OUTPUT);
  pinMode(ONBOARD_SW, INPUT_PULLUP);

  digitalWrite(ONBOARD_LED, current_LED);
}

void loop() {
  current_LED = (current_LED == HIGH) ? LOW: HIGH;
  digitalWrite(ONBOARD_LED, current_LED);

  if (digitalRead(ONBOARD_SW)==LOW) {
    delay(100);
  }
  else {
    delay(500);
  }
}
