#include <Servo.h>
// These are the values to either move forwards or backwards.
// The values must be fine tuned to be the same forwards and backwards
#define ForwardFast 119
#define ForwardSlow 110
#define Stop 90
#define ReverseSlow 70
#define ReverseFast 60
// Time in ms for the Rover to turn. Must be fine tuned
#define TurnTimer 750
Servo myservoS1;
Servo myservoS2;
void setup () {
myservoS1.attach(9);
myservoS2.attach(11);

}
void turnRight() {
// Stop, and Turn
myservoS1.write(ForwardFast);
myservoS2.write(ReverseFast);
delay(TurnTimer);
}
void turnLeft() {
// Stop, and Turn
myservoS1.write(ForwardFast);
myservoS2.write(ReverseFast);
delay(TurnTimer);
}

void loop() {
/*
This program makes the robot drive in Circles by going forwards,
stopping, turning, and then repeating
*/
myservoS1.write(90);
myservoS2.write(90);
delay(1000);
// Drive Forwards and Stop.
myservoS1.write(ForwardFast);
myservoS2.write(ForwardFast);
delay(1000);
myservoS1.write(90);
myservoS2.write(90);
delay(1000);
turnRight();
}
