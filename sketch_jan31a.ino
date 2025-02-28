#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>
const uint8_t LEDRED = 13;
const uint8_t LEDYELLOW = 12;
const uint8_t LEDGREEN = 11;
const int ledPins[] = {LEDRED, LEDYELLOW};  // Add more LEDs here if needed
const char* ledColors[] = {"RED", "YELLOW"};  // Corresponding names
const int numLeds = sizeof(ledPins) / sizeof(ledPins[0]);
const char* trackNames[] = {
    "Track 1: Song A", 
    "Track 2: Song B",
    "Track 3: Song C" 
};

const int xPin = A0;
int currentLEDIndex = 0; 
int potPin = A1; // Potentiometer output connected to analog pin 3
const int buttonPin = 2;
// Set the LCD address to 0x3F for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x3F, 16, 2);
// MP3 Player
DFRobotDFPlayerMini mp3;
SoftwareSerial mySerial(10, 11);  // RX, TX

int trackCount;
int currentTrack = 1;
int buttonState = 0;
unsigned long lastMoveTime = 0;  // Store the last movement time
const int debounceDelay = 300;   // Set the debounce delay (300ms)
int lastVolumeLevel = -1;


void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  for (uint8_t i = 0; i < numLeds; i++) {
      pinMode(ledPins[i], OUTPUT);
      digitalWrite(ledPins[i], LOW);
  }
  digitalWrite(ledPins[currentLEDIndex], HIGH);
  pinMode(buttonPin, INPUT);
  lcd.begin();
  lcd.backlight();
  updateLCD();
  Serial.print("Number of tracks=");
  Serial.println(trackCount);
  if (!mp3.begin(mySerial)) {
      Serial.println("MP3 Module Not Found!");
      while (true);
  }
  Serial.println("MP3 Module Ready!");
  mp3.volume(15);
  delay(1000);

  // Get total tracks
  trackCount = mp3.readFileCounts();
  Serial.print("Total Tracks on SD Card: ");
  Serial.println(trackCount);
  updateLCD();
}

void loop() {
  int xVal = analogRead(xPin);
  int potVal = analogRead(potPin);
  // Map potentiometer value (0-1023) to volume range (0-30)
  int volumeLevel = map(potVal, 0, 1023, 0, 30);
  if (volumeLevel != lastVolumeLevel) {
    mp3.volume(volumeLevel);
    lastVolumeLevel = volumeLevel;
    updateLCD();
  }

  Serial.print("X = ");
  Serial.println(xVal);
  Serial.print("Mapped Volume = ");
  Serial.println(volumeLevel);

  int totalTracks = mp3.readFileCounts();
  Serial.print("Total Tracks: ");
  Serial.println(totalTracks);


  // Detect right movement
  unsigned long currentTime = millis();
  if (xVal >= 1000 && (currentTime - lastMoveTime >= debounceDelay)) {  
      moveLED(1);  // Move right
      currentTrack++;
      if (currentTrack > trackCount) {
        currentTrack = 1;
      }
      Serial.print("Playing previous track: ");
      Serial.println(currentTrack);
      mp3.play(currentTrack);
      updateLCD();
      lastMoveTime = currentTime;
      mp3.play(1);
  }
  // Detect left movement
  else if (xVal <= 300 && (currentTime - lastMoveTime >= debounceDelay)) {  
      moveLED(-1); // Move left
        currentTrack--;
        if (currentTrack < 1) {
            currentTrack = trackCount;
        }
        Serial.print("Playing previous track: ");
        Serial.println(currentTrack);
        mp3.play(currentTrack);
        updateLCD();
        lastMoveTime = currentTime;
  }
}

// Function to move LED left (-1) or right (+1)
void moveLED(int direction) {
    int newLED = currentLEDIndex + direction;
    
    // Check if newLED is within range
    if (newLED >= 0 && newLED < numLeds) {
        digitalWrite(ledPins[currentLEDIndex], LOW);   // Turn off current LED
        currentLEDIndex = newLED;                      // Update current LED index
        digitalWrite(ledPins[currentLEDIndex], HIGH);  // Turn on new LED
        updateLCD();
    }
}

// Function to update LCD display with current LED color
void updateLCD() {
  lcd.clear();
  
  // Display Track Name
  lcd.setCursor(0, 0);
  if (trackCount > 0 && currentTrack >= 1 && currentTrack <= trackCount) {
    Serial.print(trackNames[currentTrack - 1]);
    Serial.print(currentTrack);
    
      lcd.print(trackNames[currentTrack - 1]);
  }

  // Display Volume Level Always
  lcd.setCursor(0, 1);
  lcd.print("Volume: ");
  lcd.print(lastVolumeLevel);
}