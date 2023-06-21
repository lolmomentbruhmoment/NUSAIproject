String rcvString = "";
float weight = 0;
float height = 0;
uint8_t question = 0;

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("What is your weight?");
}

void loop() {
  float tempValue = 0;
  float bmi = 0;
  if(Serial.available()) {
    rcvString = Serial.readString();
    tempValue = rcvString.toFloat();

    Serial.print(tempValue);
    if (question == 0) {
        weight = tempValue;
        Serial.println("");
        Serial.print("Weight you entered:");
        Serial.println(weight);

        Serial.println("");
        Serial.println("What is your height?");
        question = 1;
    }
    else {
      height = tempValue;
      Serial.println("");
      Serial.print("Height you entered:");
      Serial.println(height);

      if (height > 0 && weight > 0) {
        bmi = weight / (pow (height,2));
        Serial.println("");
        Serial.print("Your BMI:");
        Serial.print(bmi);
        Serial.print(" & ");
        if (bmi >= 40)
          Serial.println("Morbidly Obese");
        else if (bmi >= 35 && bmi < 40)
          Serial.println("Severly Obese");
        else if (bmi >= 30 && bmi < 35)
          Serial.println("Moderately Obese");
        else if (bmi >= 25 && bmi < 30)
          Serial.println("Overweight");
        else if (bmi >= 18.5 && bmi < 25)
          Serial.println("Normal");
        else if (bmi >= 16 && bmi < 18.5)
          Serial.println("Underweight");
        else if (bmi < 16)
          Serial.println("Severly Underweight");
      }
    }
  }
  else {
    Serial.println("");
    Serial.print("Invalid entry");
  }

  Serial.println("");
  Serial.print("Weight?");
  question = 0;
}
                    
                
