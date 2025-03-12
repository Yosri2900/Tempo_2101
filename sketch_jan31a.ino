#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <LowPower.h>
const uint8_t ledPin = 13;

const int xPin = A0;
int yPin = A1;
// Set the LCD address to 0x3F for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x3F, 16, 2);
// MP3 Player
DFRobotDFPlayerMini mp3;
SoftwareSerial mySerial(10, 11);  // TX, RX

int currentTrack = 1;
int buttonState = 0;
const int buttonPin = 2;  // the number of the pushbutton pin
unsigned long lastMoveTime = 0;  // Store the last movement time
const int debounceDelay = 300;   // Set the debounce delay (300ms)
int lastVolumeLevel = 15;  // Default volume at startup

unsigned long lastButtonPress = 0;
const int buttonDebounceDelay = 300;
int lastPlayedTrack = 0;
bool isDeviceOn = true;  // Start with LCD & MP3 ON
bool buttonPressed = false;  // Flag to check if button is held down

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  lcd.begin();
  lcd.backlight();
  if (!mp3.begin(mySerial)) {
      // Serial.println("MP3 Module Not Found!");
      while (true);
  }
  // Serial.println("MP3 Module Ready!");
  mp3.volume(lastVolumeLevel);
  delay(1000);

  updateLCD();
}

void loop() {

  int buttonState = digitalRead(buttonPin);
  unsigned long currentTime = millis();

  if (buttonState == LOW && !buttonPressed && (currentTime - lastButtonPress > buttonDebounceDelay)) {
      buttonPressed = true;  // Mark button as held
      isDeviceOn = !isDeviceOn;  // Toggle power state
      lastButtonPress = currentTime;
      if (!isDeviceOn) {              
          lcd.noBacklight();
          lcd.noDisplay();
          digitalWrite(ledPin, LOW);
          mp3.pause();
      } else {
          lcd.display();
          lcd.backlight();
          digitalWrite(ledPin, HIGH);
          mp3.start();
      }
  }
  if (buttonState == HIGH) {
      buttonPressed = false;
  }
  if (!isDeviceOn) return;

  int xVal = analogRead(xPin);
  int yVal = analogRead(yPin);

  // Serial.print("X = ");
  // Serial.println(xVal);
  // Serial.print("Mapped Volume = ");
  // Serial.println(volumeLevel);

  int totalTracks = mp3.readFileCounts();
  // Serial.print("Total Tracks = ");
  // Serial.println(totalTracks);

  static unsigned long lastVolumeChangeTime = 0;

  if (yVal >= 1000 && (currentTime - lastVolumeChangeTime >= debounceDelay)) {  
      if (lastVolumeLevel < 30) {  
          lastVolumeLevel++;
          mp3.volume(lastVolumeLevel);  // Apply volume change here
          updateLCD();
      }
      lastVolumeChangeTime = currentTime;
  } 
  else if (yVal <= 300 && (currentTime - lastVolumeChangeTime >= debounceDelay)) {  
      if (lastVolumeLevel > 0) {  
          lastVolumeLevel--;
          mp3.volume(lastVolumeLevel);  // Apply volume change here
          updateLCD();
      }
      lastVolumeChangeTime = currentTime;
  }
  
  if (xVal >= 1000 && (currentTime - lastMoveTime >= debounceDelay)) {  
    // Detect right movement
    currentTrack = (currentTrack % totalTracks) + 1;
    updateLCD();
    lastMoveTime = currentTime;
  } else if (xVal <= 300 && (currentTime - lastMoveTime >= debounceDelay)) {  
    // Detect left movement
    currentTrack = (currentTrack - 1);
    if (currentTrack < 1) {
      currentTrack = totalTracks;
    }
    updateLCD();
    lastMoveTime = currentTime;
  }

  if (currentTrack != lastPlayedTrack) {
      // Serial.print("Playing track: ");
      // Serial.println(currentTrack);
      mp3.play(currentTrack);
      lastPlayedTrack = currentTrack;
  }
}

// Function to update LCD display with current track# and volume
void updateLCD() {
  lcd.clear();
  
  // Display Track Name
  lcd.setCursor(0, 0);
  lcd.print("Track: ");
  lcd.print(currentTrack);

  // Display Volume Level Always
  lcd.setCursor(0, 1);
  lcd.print("Volume: ");
  lcd.print(lastVolumeLevel);
}