#include <SerialLCD.h>
#include <SoftwareSerial.h>
SerialLCD slcd(11,12);
#include <VarSpeedServo.h> 
VarSpeedServo myservo;
#define BUZZER_PIN 6 
const int ButtonPin = 2;

unsigned long previousMillis = 0;

const long interval = 1000;

unsigned int total_seconds = 0;
unsigned long int standup_time = 86400;
unsigned long int stretch_time = 14400;
bool isstandup_time = true;
bool button_message = false;

int length = 31;
char notes[] = "eefggfedccdeeddeefggfedccdedcc";
int beats[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 4};
int tempo = 120;

void update_lcd(int total_seconds) {
   if (total_seconds % 60 == 0) {
    slcd.setCursor(0, 1);
    slcd.print(total_seconds/60,DEC);
  }
}

void ring_bell() {
   slcd.setCursor(0, 0);
   slcd.print("Stand up time!");
   for (int x = 0; x <= 10; x++) {
    myservo.write(10,250, true);
    myservo.write(70,250, true);
   }
   slcd.setCursor(0, 0);
   slcd.print("Minutes passed: ");
}

void ring_buzzer () {
   slcd.setCursor(0, 0);
   slcd.print("Stretch O'Clock!");
   int start_time = millis();
   for(int i = 0; i < length; i++) {
    if(notes[i] == ' ') {
      delay(beats[i] * tempo);
      } else {
        playNote(notes[i], beats[i] * tempo);
      }
      delay(tempo / 1);
   }
   int end_time = millis();
   int time_passed = (end_time - start_time)/1000;
   total_seconds = total_seconds + time_passed;
   slcd.setCursor(0, 0);
   slcd.print("Minutes passed: ");
}

void playTone(int tone, int duration) {
    for (long i = 0; i < duration * 1000L; i += tone * 2) {
        digitalWrite(BUZZER_PIN, HIGH);
        delayMicroseconds(tone);
        digitalWrite(BUZZER_PIN, LOW);
        delayMicroseconds(tone);
    }
}

void playNote(char note, int duration) {
    char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
    int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };

    for (int i = 0; i < 8; i++) {
        if (names[i] == note) {
            playTone(tones[i], duration);
        }
    }
}

void setup() {
   slcd.begin();
   slcd.print("Minutes passed:");
   update_lcd(total_seconds);
   slcd.backlight();
   pinMode(BUZZER_PIN, OUTPUT);
   myservo.attach(9);
   //Serial.begin(9600);
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    total_seconds = total_seconds + 1;
    update_lcd(total_seconds);    
    previousMillis = currentMillis;
    
    //Serial.print(F("Total seconds "));
    //Serial.println(total_seconds, DEC);
  }

  if (isstandup_time == true) {
    ring_bell();
    isstandup_time = false;
  }

  if (total_seconds == stretch_time) {
    ring_buzzer();
  }
      
  if (total_seconds == standup_time) {
    isstandup_time = true;
    total_seconds = 0;
    slcd.setCursor(0, 1);
    slcd.print(total_seconds/60,DEC);
  }  
  
  if(digitalRead(ButtonPin) == HIGH && button_message == false)
  {
    slcd.backlight();
    slcd.setCursor(0, 0);
    slcd.print("You've got this!");
    slcd.setCursor(0, 1);
    slcd.print("Have a nice day!");
    button_message = true;
  } else if (digitalRead(ButtonPin) == LOW && button_message == true)
  {
    slcd.noBacklight();
    slcd.clear();
    slcd.setCursor(0, 0);
    slcd.print("Minutes passed:");
    slcd.setCursor(0, 1);
    slcd.print(total_seconds/60,DEC);
    button_message = false;
  }
  
}
