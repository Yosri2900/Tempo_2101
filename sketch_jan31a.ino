#include <Wire.h>
#include <LiquidCrystal_I2C.h>
const uint8_t LEDRED = 13;
const uint8_t LEDYELLOW = 12;
// const uint8_t LEDGREEN = 11;
const int ledPins[] = {LEDRED, LEDYELLOW};  // Add more LEDs here if needed
const char* ledColors[] = {"RED", "YELLOW"};  // Corresponding names
const int numLeds = sizeof(ledPins) / sizeof(ledPins[0]);
const int xPin = A0;
int currentLEDIndex = 0; 
int potPin = A1; // Potentiometer output connected to analog pin 3

// Set the LCD address to 0x3F for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x3F, 16, 2);

void setup() {
  for (uint8_t i = 0; i < numLeds; i++) {
      pinMode(ledPins[i], OUTPUT);
      digitalWrite(ledPins[i], LOW);
  }
  digitalWrite(ledPins[currentLEDIndex], HIGH);
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  updateLCD();
  while (!Serial);
}

void loop() {
  int xVal = analogRead(xPin);
  int potVal = potVal = analogRead(potPin);

  Serial.print("X = ");
  Serial.println(xVal);
  Serial.println("potentiometer: ");
  Serial.print(potVal);

  // Detect right movement
  if (xVal >= 1020) {  
      moveLED(1);  // Move right
      delay(300);
  }
  // Detect left movement
  else if (xVal <= 10) {  
      moveLED(-1); // Move left
      delay(300);
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
    lcd.setCursor(0, 0);  
    lcd.print("Current LED:");
    lcd.setCursor(0, 1);
    lcd.print(ledColors[currentLEDIndex]);  // Print color name
}