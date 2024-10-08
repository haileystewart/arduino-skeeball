int sensorPin1 = A0;  // Simulated sensor pin for target 1
int sensorPin2 = A1;  // Simulated sensor pin for target 2
int ledPin1 = 13;     // Simulated LED for target 1
int ledPin2 = 12;     // Simulated LED for target 2
int buzzerPin = 11;   // Simulated buzzer

void setup() {
  pinMode(sensorPin1, INPUT);  // Initialize sensors
  pinMode(sensorPin2, INPUT);
  pinMode(ledPin1, OUTPUT);    // Initialize LEDs
  pinMode(ledPin2, OUTPUT);
  pinMode(buzzerPin, OUTPUT);  // Initialize buzzer

  Serial.begin(9600);  // Start serial communication with Raspberry Pi
}

void loop() {
  // Simulate sensor detection with random values for testing
  int sensor1State = random(0, 2);  // Simulate ball passing through target 1
  int sensor2State = random(0, 2);  // Simulate ball passing through target 2

  if (sensor1State == 1) {
    digitalWrite(ledPin1, HIGH);   // Light up LED 1
    tone(buzzerPin, 1000, 200);    // Simulate buzzer sound
    Serial.println("Target 1 Hit!");  // Send score update to Raspberry Pi
    delay(1000);
    digitalWrite(ledPin1, LOW);
  }

  if (sensor2State == 1) {
    digitalWrite(ledPin2, HIGH);   // Light up LED 2
    tone(buzzerPin, 1500, 200);    // Simulate buzzer sound
    Serial.println("Target 2 Hit!");  // Send score update to Raspberry Pi
    delay(1000);
    digitalWrite(ledPin2, LOW);
  }

  delay(1000);  // Delay between simulated readings
}
