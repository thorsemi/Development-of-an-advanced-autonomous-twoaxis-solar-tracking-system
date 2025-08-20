#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <avr/wdt.h> // Watchdog Timer

// Kompas opsætning
Adafruit_HMC5883_Unified compass = Adafruit_HMC5883_Unified(12345);

// GPS opsætning
TinyGPSPlus gps;
SoftwareSerial gpsSerial(3, 4); // GPS TX til D3, GPS RX til D4

unsigned long startTime; // Variabel til at holde starttidspunktet
bool initializationMessageShown = false; // Flag for opstartstekst

void setup() {
  Serial.begin(9600);      // Serial Monitor
  gpsSerial.begin(9600);   // GPS-forbindelse
  Wire.begin();            // I2C-forbindelse til kompasset

  // Watchdog-konfiguration
  if (MCUSR & (1 << WDRF)) {
    MCUSR &= ~(1 << WDRF);
  }
  wdt_enable(WDTO_8S);

  // Initialisering af kompas
  if (!compass.begin()) {
    Serial.println("Kompasset kunne ikke initialiseres.");
    while (1);
  }

  startTime = millis(); // Registrer starttidspunktet
}

void loop() {
  wdt_reset(); // Nulstil Watchdog Timer

  // Tjek om de første 10 sekunder er gået
  if (millis() - startTime < 10000) {
    // Vis meddelelsen "GPS og kompas klar til brug!" én gang, når ventetiden slutter
    if (!initializationMessageShown && millis() - startTime >= 10000) {
      Serial.println("GPS og kompas klar til brug!");
      initializationMessageShown = true;
    }
    return; // Vent uden at sende data
  }

  // Kompasdata
  sensors_event_t event;
  compass.getEvent(&event);
  float correctedX = (event.magnetic.x + 6.73) * 1.0; // Kalibrering af X
  float correctedY = (event.magnetic.y + 44.64) * 1.0; // Kalibrering af Y
  float heading = atan2(correctedY, correctedX) * 180 / PI; // Beregn retning
  if (heading < 0) heading += 360; // Justér til 0-360 grader

  // Udskriv kompasdata
  Serial.print("Kompas: ");
  Serial.print(heading);
  Serial.println("°");

  // GPS-data
  while (gpsSerial.available() > 0) {
    char c = gpsSerial.read();
    gps.encode(c); // Parser NMEA-tegn med TinyGPS++

    // Hent kun latitude og longitude, når nye data er opdateret
    if (gps.location.isUpdated()) {
      float latitude = gps.location.lat();    // Breddegrad
      float longitude = gps.location.lng();  // Længdegrad

      // Udskriv GPS-koordinater
      Serial.print("Latitude: ");
      Serial.println(latitude, 6);  // 6 decimaler for præcision
      Serial.print("Longitude: ");
      Serial.println(longitude, 6); // 6 decimaler for præcision
    }
  }

  delay(1000); // Opdater hvert sekund
}
