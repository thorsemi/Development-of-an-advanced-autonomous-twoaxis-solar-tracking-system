const int dirPin = 8;     // DIR+ (Orange) - Retningssignal
const int stepPin = 10;    // PLS+ (Rød) - Pulssignal
const int enablePin = 12;  // ENA+ (Grøn) - Aktiver signal
const int feedbackPin = 2; // Feedback signal fra encoder (digital pin)

// Konfiguration
const float microstepAngle = 0.0045;   // Vinkel pr. mikrosteg (output) i grader
const float minPosition = 0.0;        // Minimum vinkel (startpunkt)
const float maxPosition = 180.0;      // Maksimum vinkel (slutpunkt)
const int encoderResolution = 1024;  // Encoderens pulser pr. omdrejning
const float degreesPerPulse = 360.0 / encoderResolution; // Grader pr. encoder-puls

volatile int pulseCount = 0; // Variabel til at tælle pulser
float currentPosition = 0.0; // Aktuel position i grader

void setup() {
  // Konfigurer pins som output og input
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(feedbackPin, INPUT);

  attachInterrupt(digitalPinToInterrupt(feedbackPin), countPulses, RISING); // Interrupt for feedback

  // Standardindstillinger
  digitalWrite(enablePin, HIGH); // Deaktiver driveren som standard
  digitalWrite(dirPin, HIGH);    // Standardretning

  // Start seriel kommunikation
  Serial.begin(9600);
  Serial.println("Send vinkler som positive eller negative værdier (eks. '20' eller '-15'). S for stop.");
}

void countPulses() {
  pulseCount++;
}

void updatePositionFromFeedback() {
  currentPosition = pulseCount * degreesPerPulse; // Beregn position baseret på pulser
  Serial.print("Feedback opdateret position: ");
  Serial.print(currentPosition);
  Serial.println(" grader.");
}

void rotateMotor(float angle) {
  // Opdater position fra feedback
  updatePositionFromFeedback();

  // Bestem retning ud fra fortegn
  bool direction = angle > 0;

  // Tjek om bevægelsen er inden for grænserne
  float newPosition = currentPosition + angle;
  if (newPosition < minPosition || newPosition > maxPosition) {
    Serial.println("Bevægelse blokeret: Overskrider sikkerhedsgrænse!");
    return; // Stop bevægelsen
  }

  // Aktivér driveren og sæt retning
  digitalWrite(enablePin, LOW);
  digitalWrite(dirPin, direction ? HIGH : LOW);

  // Beregn antal pulser
  int pulses = abs(angle) / microstepAngle;

  // Kør motoren
  for (int i = 0; i < pulses; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1700);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1700);
  }

  digitalWrite(enablePin, HIGH); // Deaktivér driveren

  // Opdater position efter rotation
  updatePositionFromFeedback();
}

void loop() {
  // Læs data fra bruger
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n'); 

    if (input == "S") {                  // Stop
      Serial.println("Motor stoppet.");
      digitalWrite(enablePin, HIGH);     // Deaktivér driveren
    } else {
      float angle = input.toFloat();     // Konverter input til vinkel
      if (angle != 0) {
        Serial.print("Rotér med ");
        Serial.print(angle);
        Serial.println(" grader.");
        rotateMotor(angle);
      } else {
        Serial.println("Ugyldig kommando. Send en gyldig vinkel eller 'S' for stop.");
      }
    }
  }
}
