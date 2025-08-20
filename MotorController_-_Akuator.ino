#include <Arduino.h>

// Pin-definitioner
const int dacPin = 25;        // DAC pin til aktuatorens kontrolsignal
const int feedbackPin = 34;   // Analog pin til feedback-signalet
const float maxVoltage = 3.15; // Maksimal spænding fra ESP32 DAC
const float minVoltage = 0.0;  // Minimal spænding fra ESP32 DAC

// Nye værdier
const float minLength = 92.0;  // Minimal slaglængde i mm
const float maxLength = 279.0; // Maksimal slaglængde i mm
const float tolerance = 2.0;   // Acceptabel fejlmargin i mm

float desiredLength = 92.0; // Standard ønsket længde
unsigned long lastTempCheck = 0; // Variabel til tidsmåling for temperatur
const unsigned long tempInterval = 30000; // Tidsinterval for temperaturmåling (30 sekunder)

void setup() {
  Serial.begin(9600); // Initialiser seriel kommunikation med Node-RED
  pinMode(dacPin, OUTPUT);  // Initialiser DAC-pinnen som output
  pinMode(feedbackPin, INPUT); // Konfigurer feedback-pinnen som input
}

void reportCPUTemperature() {
  // Funktion til at læse og rapportere CPU-temperatur
  float temp = temperatureRead(); // Intern ESP32 temperaturmåling
  Serial.print("CPU Temperature: ");
  Serial.print(temp);
  Serial.println(" °C");
}

void loop() {
  // Tjek, om der er data fra Node-RED
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n'); // Læs indtil linjeskift
    desiredLength = input.toFloat(); // Konverter input til en float
  }

  // Begræns ønsket længde til gyldigt interval
  desiredLength = constrain(desiredLength, minLength, maxLength);

  // Variabel til kontrolspænding
  float controlVoltage;

  // Beregn nødvendig spænding baseret på det korrekte interval
  if (desiredLength <= 137) {
    controlVoltage = (desiredLength - 92.0) / 56.25;
  } else if (desiredLength <= 186) {
    controlVoltage = (desiredLength - 88.0) / 61.25;
  } else if (desiredLength <= 234) {
    controlVoltage = (desiredLength - 90.0) / 60.0;
  } else {
    controlVoltage = (desiredLength - 90.0) / 60.0;
  }

  // Begræns kontrolspændingen til DAC's maksimale og minimale værdier
  controlVoltage = constrain(controlVoltage, minVoltage, maxVoltage);

  // Konverter kontrolspændingen til DAC-værdi (0-255 skala)
  int dacValue = (controlVoltage / maxVoltage) * 255;

  // Send DAC-værdien til aktuatoren
  dacWrite(dacPin, dacValue);

  delay(1000); // Opdater hvert 1000 ms

  // Feedback-delen
  // Læs feedback-signalet
  int feedbackValue = analogRead(feedbackPin);
  float feedbackLength = (feedbackValue / 4095.0) * (maxLength - minLength) + minLength;

  // Debugging: Udskriv feedback-længden
  Serial.print("Feedback Length: ");
  Serial.print(feedbackLength);
  Serial.println(" mm");

  // Valider, om positionen er nået
  if (abs(feedbackLength - desiredLength) > tolerance) {
    Serial.println("Position not reached, adjusting...");
    // Juster signalet igen
    controlVoltage = (desiredLength - minLength) / (maxLength - minLength) * maxVoltage;
    dacValue = (controlVoltage / maxVoltage) * 255;
    dacWrite(dacPin, dacValue);
  } else {
    Serial.println("Position reached successfully.");
  }

  // Overvågning af CPU-temperatur
  if (millis() - lastTempCheck >= tempInterval) { // Hvert 30. sekund
    reportCPUTemperature();
    lastTempCheck = millis(); // Opdater tidsmåling
  }

  delay(1000); // Vent før næste iteration
}
