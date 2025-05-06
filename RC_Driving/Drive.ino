#include <Wire.h>
#include <Servo.h>
#include "Adafruit_VL53L0X.h"

// Distance sensor setup
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
const int numSamples = 10;
int distances[numSamples];

// Drive motors
Servo motorLeft;
Servo motorRight;

// Gun servos
Servo reloadServo;
Servo shootServo;

// Motor constants
const int STOP = 1500;
const int FULL_FORWARD = 1100;
const int FULL_REVERSE = 1900;
const int SLOW_FORWARD = 1400;
const int SLOW_TURN_SPEED = 100;
const int TURN_DURATION = 600;

// Gun servo constants
const int servoReloadPin = 6;
const int servoShootPin = 5;
const int reloadRetractedPos = 190;
const int reloadExtendedPos = 60;
const int shootIdlePos = 20;
const int shootActivePos = 50;

// Control variables
unsigned long lastCommandTime = 0;
const unsigned long commandTimeout = 2000;
String input = "";
bool autonomousMode = true;
bool shootingInProgress = false;

void setup() {
  Serial.begin(115200);
  
  // Initialize drive motors
  motorLeft.attach(9);
  motorRight.attach(11);
  motorLeft.writeMicroseconds(STOP);
  motorRight.writeMicroseconds(STOP);
  
  // Gun servos
  shootServo.write(shootIdlePos);
  
  // Distance sensor
  Wire.begin();
  if (!lox.begin()) {
    Serial.println("Failed to boot VL53L0X");
    while (1); 
  }
  lox.startRangeContinuous();
  
  Serial.println("Robot initialized and ready");
}

void loop() {
  // Check for serial commands
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      processCommand(input);
      input = "";
      lastCommandTime = millis();
    } else {
      input += c;
    }
  }
  
  // Run autonomous mode if active and we haven't received commands recently
  if (autonomousMode && millis() - lastCommandTime > commandTimeout && !shootingInProgress) {
    autonomousRoam();
  }
}

void autonomousRoam() {
  int medianDistance = getMedianDistance();
  
  if (isClose(medianDistance)) {
    Serial.println("Wall detected, turning left.");
    turnLeft90();
  } else {
    motorLeft.writeMicroseconds(SLOW_FORWARD);
    motorRight.writeMicroseconds(SLOW_FORWARD);
  }
  delay(50);
}

int getMedianDistance() {
  for (int i = 0; i < numSamples; i++) {
    while (!lox.isRangeComplete()) {
      delay(1);
    }
    distances[i] = lox.readRange();
  }
  sortArray(distances, numSamples);
  return distances[numSamples / 2];
}

bool isClose(int distance) {
  return (distance > 120 && distance < 330);
}

void turnLeft90() {
  motorLeft.writeMicroseconds(STOP - SLOW_TURN_SPEED);
  motorRight.writeMicroseconds(STOP + SLOW_TURN_SPEED);
  delay(TURN_DURATION);
  motorLeft.writeMicroseconds(STOP);
  motorRight.writeMicroseconds(STOP);
}

void processCommand(String cmd) {
  cmd.trim();
  
  if (cmd == "a") {
    Serial.println("Autonomous mode enabled.");
    autonomousMode = true;
  } 
  else if (cmd == "s") {
    Serial.println("Stopping robot.");
    motorLeft.writeMicroseconds(STOP);
    motorRight.writeMicroseconds(STOP);
    autonomousMode = false;
  } 
  else if (cmd == "FIRE") {
    Serial.println("Firing dart gun!");
    autonomousMode = false;  // Disable autonomous mode during shooting
    fireGun();
  }
  else if (cmd.startsWith("SL") || cmd.startsWith("SR")) {
    int turnTime = cmd.substring(2).toInt();
    if (turnTime > 0) {
      if (cmd.startsWith("SL")) {
        Serial.println("Slow left turn.");
        motorLeft.writeMicroseconds(STOP - SLOW_TURN_SPEED);
        motorRight.writeMicroseconds(STOP + SLOW_TURN_SPEED);
      } else {
        Serial.println("Slow right turn.");
        motorLeft.writeMicroseconds(STOP + SLOW_TURN_SPEED);
        motorRight.writeMicroseconds(STOP - SLOW_TURN_SPEED);
      }
      delay(turnTime);
      motorLeft.writeMicroseconds(STOP);
      motorRight.writeMicroseconds(STOP);
    }
  }
}

void fireGun() {
  // Set flag to true
  shootingInProgress = true;
  
  // Stop the robot first
  motorLeft.writeMicroseconds(STOP);
  motorRight.writeMicroseconds(STOP);
  
  // Attach servos
  reloadServo.attach(servoReloadPin);
  shootServo.attach(servoShootPin);
  
  // Spin motors up
  for (int pos = shootIdlePos; pos <= shootActivePos; pos += 2) {
    shootServo.write(pos);
    delay(10);
  }
  
  // Push bullet position
  for (int pos = reloadRetractedPos; pos >= reloadExtendedPos; pos -= 5) {
    reloadServo.write(pos);
    delay(20);
  }
  
  // Return to retracted position
  for (int pos = reloadExtendedPos; pos <= reloadRetractedPos; pos += 1) {
    reloadServo.write(pos);
    delay(15);
  }
  
  // Stop spinning motors
  for (int pos = shootActivePos; pos >= shootIdlePos; pos -= 2) {
    shootServo.write(pos);
    delay(15);
  }
  
  // Final positioning
  shootServo.write(30);
  delay(200);
  reloadServo.write(160); // Move back to retracted position for next bullet
  delay(700);
  
  // Detach servos
  reloadServo.detach();
  shootServo.detach();
  
  // Clear the shooting flag
  shootingInProgress = false;
  
  Serial.println("Firing sequence completed");
}

void sortArray(int arr[], int size) {
  for (int i = 0; i < size - 1; i++) {
    for (int j = 0; j < size - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        int temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
      }
    }
  }
}