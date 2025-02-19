#include <LiquidCrystal.h>
#include <Servo.h>

#define BUZZER_PIN 8
#define R 5 // Red LED
#define G 6 // Green LED
#define Y 7 // Yellow LED
int pinServo = A5; // can be any pin number
int pos = 0;
Servo myservo; //create Servo object

// Note frequencies
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047


// Durations 
#define W 800   // Whole note
#define dH 600  // dotted Half note
#define H 400   // Half note
#define Q 200   // Quarter note
#define E 100   // Eighth note
#define S 50    // Sixteenth note

int pinButton = 3;
int buttonState = 0;
int cursor = 0;

LiquidCrystal lcd( 7, 8, 9, 10, 11, 12);

const char* menu[] = {"Its a small world", "The Muppets - theme song", "Over the rainbow"};
const int menuSize = sizeof(menu)/sizeof(menu[0]);


void setup() {
  // music related
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(Y, OUTPUT);

  // strum related
  myservo.attach(pinServo);

  // display related
  pinMode(pinButton, INPUT);
  lcd.begin(16,2);
  lcd.clear();

  displayMenu();
  Serial.begin(115200);

  displayMenu();
}

void loop() {
  // put your main code here, to run repeatedly:
  buttonState = digitalRead(pinButton);

  if (buttonState > 0.5){
    Serial.println("Button Pressed");
    scrollMenu();
    // actuate();  // UNCOMMENT WHEN ACTUATE FUNCTION IS SETUP
  }
  
  delay(200);

}

void displayMenu(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("> ");
  lcd.print(menu[cursor]);
  // lcd.autoscroll();
  // lcd.scrollDisplayLeft();
}

void scrollMenu(){
  cursor = (cursor + 1) % menuSize;
  displayMenu();
}

void playSong(int melody[], int durations[], int lights[], int length) {
  for (int i = 0; i < length; i++) {
    if (melody[i] == 0) {
      
      delay(durations[i]); // Rest
    } else {
      digitalWrite(lights[i], HIGH);  // turn the LED on (HIGH is the voltage level)
      //delay(durations[i]);                      // wait for note
      
      strumUp();

      tone(BUZZER_PIN, melody[i], durations[i]);
      delay(durations[i] * 1.3); // Small gap between notes

      strumDown();

      digitalWrite(lights[i], LOW);   // turn the LED off by making the voltage LOW
     
      noTone(BUZZER_PIN);
    }
  }
  delay(500);
  //blink all lights
  digitalWrite(R, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(Y, HIGH);
  delay(500);
  digitalWrite(R, LOW); 
  digitalWrite(G, LOW); 
  digitalWrite(Y, LOW); 

}

// Muppets Theme (Double Tempo)
void muppets() {
  int melody[] = {
    NOTE_C6, NOTE_C6, NOTE_A5, NOTE_B5, NOTE_A5, NOTE_B5, NOTE_G5, 0,
    NOTE_C6, NOTE_C6, NOTE_A5, NOTE_B5, NOTE_A5, NOTE_G5, NOTE_G5, 0,
    NOTE_E5, NOTE_E5, NOTE_G5, NOTE_F5, NOTE_E5, NOTE_F5, NOTE_C6, NOTE_C5, NOTE_D5, NOTE_E5, 0,
    NOTE_E5, 0, NOTE_E5, 0, NOTE_D5, NOTE_D5, 0, NOTE_C5
  };

  int durations[] = {
    H, H, H, H, Q, H, H, Q,
    H, H, H, H, Q, Q, W, Q,
    H, H, H, H, Q, Q, Q, Q, Q, H, Q,
    Q, Q, Q, E,E,E, Q, W
  };

  int lights[] = {
    R, G, Y, R, G, Y, R, G,
    Y, R, G, Y, R, G, Y, R,
    G, Y, R, G, Y, R, G, Y, R, G, Y,
    R, G, Y, R, G, Y, R, G


  };

  playSong(melody, durations, lights, sizeof(melody) / sizeof(melody[0]));
}

// Over the Rainbow
void overTheRainbow() {
  int melody[] = {
    NOTE_C5, NOTE_C6, NOTE_B5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6, NOTE_C5, NOTE_A5, NOTE_G5, 0,
    NOTE_A4, NOTE_F5, NOTE_E5, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_D5, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_C5
  };

  int durations[] = {
    W, W, H, Q, Q, H, H, W, W, W, H,
    W, W, H, Q, Q, H, H, H, Q, Q, H, H, W
  };

  int lights[] = {
    R, G, Y, R, G, Y, R, G, Y, R, G,
    Y, R, G, Y, R, G, Y, R, G, Y, R, G, Y

  };

  playSong(melody, durations, lights, sizeof(melody) / sizeof(melody[0]));
}

// It's a Small World
void smallWorld() {
  int melody[] = {
    NOTE_C5, NOTE_C5, NOTE_E5, NOTE_C5, NOTE_D5, NOTE_D5, NOTE_D5, 0,
    NOTE_D5, NOTE_D5, NOTE_F5, NOTE_D5, NOTE_E5, NOTE_E5, NOTE_E5, 0,
    NOTE_E5, NOTE_E5, NOTE_G5, NOTE_E5, NOTE_F5, NOTE_F5, NOTE_F5,
    NOTE_E5, NOTE_D5, NOTE_G4, NOTE_B4, NOTE_C5, 0
  };

  int durations[] = {
    dH,Q,H,H,dH,Q,dH,Q,
    dH,Q,H,H,dH,Q,dH,Q,
    dH,Q,H,H,dH,Q,dH,
    Q,Q,W,W,W,W
  
  };

  int lights[] = {
    R, G, Y, R, G, Y, R, G, Y,
    R, G, Y, R, G, Y, R, G, Y,
    R, G, Y, R, G, Y, R, G, Y, R, G, Y
  };

  playSong(melody, durations, lights, sizeof(melody) / sizeof(melody[0]));
}

void strumUp(){
  for (pos = 0; pos <= 40; pos += 1) { // goes from 0 degrees to 180 degrees
      myservo.write(pos);             
    }
    delay(80);
}

void strumDown(){
  for (pos = 40; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      myservo.write(pos);              
    }
    delay(80);
}

void actuate(){ // for now this is not called anywhere, it's just a template for Evan to add his code.
  // IF TRIGGERED
  // DO THIS ... 
  if (cursor == 0){
      muppets();
    }
    else if (cursor == 1){
      overTheRainbow();
    } 
    else if (cursor == 2){
      smallWorld();
    }
}
