#include <Servo.h>

int pinServo = 9;
int pos = 0;
Servo myservo; //create Servo object

void strumUp(){
  for (pos = 0; pos <= 80; pos += 1) { // goes from 0 degrees to 180 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      Serial.println(myservo.read());
      delay(20);                       // waits 15 ms for the servo to reach the position
    }
}

void strumDown(){
  for (pos = 80; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      Serial.println(myservo.read());
      delay(20);                       // waits 15 ms for the servo to reach the position
    }
}

void setup() {
  // pinMode(pinServo, OUTPUT);
  pinMode(pinB,INPUT);
  myservo.attach(pinServo);
  Serial.begin(115200);
}

void loop() { 
  Serial.println(analogRead(pinB));
  // delay(500);
  Serial.println("Up");
  strumUp();
  // delay(500);
  Serial.println("Down");
  Serial.println(pos);
  strumDown();

}

