const uint8_t LEDRED = 13;
const uint8_t LEDYELLOW = 12;
const uint8_t LEDGREEN = 11;

void setup() {
  pinMode(LEDRED, OUTPUT);
  pinMode(LEDYELLOW, OUTPUT);
  pinMode(LEDGREEN, OUTPUT);
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Board x!!!");
}


void loop() {
  digitalWrite(LEDYELLOW, HIGH);  
  digitalWrite(LEDRED, HIGH);  
  delay(500);             
  digitalWrite(LEDYELLOW, LOW);   
  digitalWrite(LEDRED, LOW);   
  delay(500);
}