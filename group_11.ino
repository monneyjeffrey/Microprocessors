#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int potPin = A0;
const int SensorPin = A2;
const int cryBuzzerPin = 9;
const int motorPin = 11;
const int greenLedPin = 2;
const int yellowLedPin = 3;
const int redLedPin = 4;

unsigned long lastLCDUpdate = 0;
bool isAlertActive = false;

int cryLevel = 0;
bool isPlayingLullaby = false;

// Define lullaby melodies using raw frequencies
int lullaby1[] = { 262, 294, 330, 349, 392, 0 }; // C4, D4, E4, F4, G4
int duration1[] = { 300, 300, 300, 300, 500, 0 };

int lullaby2[] = { 392, 349, 330, 294, 262, 0 }; // G4, F4, E4, D4, C4
int duration2[] = { 500, 300, 300, 300, 300, 0 };

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.print("Initializing...");
  delay(1000);
  lcd.clear();

  pinMode(cryBuzzerPin, OUTPUT);
  pinMode(motorPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(SensorPin, INPUT);
}

void loop() {
  cryLevel = map(analogRead(potPin), 0, 1023, 0, 255);

  if (cryLevel >= 200) {
    activateVibration(true);
    playLullaby(lullaby2, duration2);
  } else if (cryLevel >= 100) {
    activateVibration(false);
    playLullaby(lullaby1, duration1);
  } else {
    activateVibration(false);
    stopBuzzer();
  }

  if (millis() - lastLCDUpdate > 300) {
    lastLCDUpdate = millis();
    updateLCDandLEDs();
  }
}

void activateVibration(bool state) {
  digitalWrite(motorPin, state ? HIGH : LOW);
}

void playLullaby(int melody[], int duration[]) {
  if (!isPlayingLullaby) {
    isPlayingLullaby = true;
    for (int i = 0; melody[i] != 0; i++) {
      tone(cryBuzzerPin, melody[i], duration[i]);
      delay(duration[i] * 1.3);
    }
    noTone(cryBuzzerPin);
    isPlayingLullaby = false;
  }
}

void stopBuzzer() {
  noTone(cryBuzzerPin);
  isPlayingLullaby = false;
}

void updateLCDandLEDs() {
  lcd.setCursor(0, 0);
  lcd.print("Cry Level: ");
  lcd.print(cryLevel);
  lcd.print("   ");

  if (cryLevel < 100) {
    setLEDs(HIGH, LOW, LOW);
    lcd.setCursor(0, 1);
    lcd.print("Sleeping  ");
  } else if (cryLevel < 200) {
    setLEDs(LOW, HIGH, LOW);
    lcd.setCursor(0, 1);
    lcd.print("Playing  ");
  } else {
    setLEDs(LOW, LOW, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("Crying  ");
  }
}

void setLEDs(int green, int yellow, int red) {
  digitalWrite(greenLedPin, green);
  digitalWrite(yellowLedPin, yellow);
  digitalWrite(redLedPin, red);
}
