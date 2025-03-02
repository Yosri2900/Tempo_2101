#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>
const uint8_t ledPin = 13;
const uint8_t movementLeftPin = 12;
const uint8_t movementRightPin = 9;
// const char* trackNames[] = {
//     "Track 1: Song A", 
//     "Track 2: Song B",
//     "Track 3: Song C",
//     "Track 4: Song D",
//     "Track 5: Song E",
//     "Track 6: Song F",
//     "Track 7: Song G",
// };

const int xPin = A0;
int potPin = A1; // Potentiometer output connected to analog pin 3
// Set the LCD address to 0x3F for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x3F, 16, 2);
// MP3 Player
DFRobotDFPlayerMini mp3;
SoftwareSerial mySerial(10, 11);  // RX, TX

int currentTrack = 1;
int buttonState = 0;
const int buttonPin = 2;  // the number of the pushbutton pin
unsigned long lastMoveTime = 0;  // Store the last movement time
const int debounceDelay = 300;   // Set the debounce delay (300ms)
int lastVolumeLevel = -1;

unsigned long lastButtonPress = 0;
const int buttonDebounceDelay = 300;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(movementLeftPin, OUTPUT);
  pinMode(movementRightPin, OUTPUT);
  lcd.begin();
  lcd.backlight();
  if (!mp3.begin(mySerial)) {
      Serial.println("MP3 Module Not Found!");
      while (true);
  }
  Serial.println("MP3 Module Ready!");
  mp3.volume(15);
  delay(1000);

  updateLCD();
}

void loop() {
  int xVal = analogRead(xPin);
  int potVal = analogRead(potPin);
  buttonState = digitalRead(buttonPin);
  // Map potentiometer value (0-1023) to volume range (0-30)
  int volumeLevel = map(potVal, 0, 1023, 0, 30);
  if (volumeLevel != lastVolumeLevel) {
    mp3.volume(volumeLevel);
    lastVolumeLevel = volumeLevel;
    updateLCD();
  }

  // Serial.print("X = ");
  // Serial.println(xVal);
  // Serial.print("Mapped Volume = ");
  // Serial.println(volumeLevel);

  int totalTracks = mp3.readFileCounts();
  Serial.print("Total Tracks = ");
  Serial.println(totalTracks);
  Serial.print("currentTrack= ");
  Serial.println(currentTrack);
  
  unsigned long currentTime = millis();
  if (xVal >= 1000 && (currentTime - lastMoveTime >= debounceDelay)) {  
    // Detect right movement
    digitalWrite(movementRightPin, HIGH);
    delay(250);
    digitalWrite(movementRightPin, LOW);
    currentTrack = (currentTrack + 1) % totalTracks;
    if (currentTrack == 0) {
      currentTrack = 1;
    }
    lastMoveTime = currentTime;
  } else if (xVal <= 300 && (currentTime - lastMoveTime >= debounceDelay)) {  
    // Detect left movement
    digitalWrite(movementLeftPin, HIGH);
    delay(250);
    digitalWrite(movementLeftPin, LOW);
    currentTrack = (currentTrack - 1) % totalTracks;
    if (currentTrack == 0) {
      currentTrack = totalTracks;
    }  
    lastMoveTime = currentTime;
  }

  // Button Press to Play Track
  if (buttonState == HIGH && (millis() - lastButtonPress > buttonDebounceDelay)) {
    digitalWrite(ledPin, HIGH);
    int state = mp3.readState();  // Get current player state
    delay(250);                        // Wait for 500ms
    digitalWrite(ledPin, LOW);   // Turn LED OFF
    Serial.print("Playing track: ");
    Serial.println(currentTrack);
    mp3.play(currentTrack);
    updateLCD();
    lastButtonPress = millis();

  }

}

// Function to update LCD display with current LED color
void updateLCD() {
  lcd.clear();
  
  // Display Track Name
  lcd.setCursor(0, 0);
  //lcd.print(trackNames[currentTrack - 1]); PRINTING SONG NAME IS IMPOSSIBLE...
  lcd.print("Track: ");
  lcd.print(currentTrack);

  // Display Volume Level Always
  lcd.setCursor(0, 1);
  lcd.print("Volume: ");
  lcd.print(lastVolumeLevel);
}