#define BLYNK_TEMPLATE_ID "TMPL3k3GmSKjT"
#define BLYNK_TEMPLATE_NAME "ECOFLOW"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "RmBPIZcHdrYJcwwAQcEcVlk06TN30HyQ";  // Your Blynk authentication token
char ssid[] = "HK";   // Your WiFi SSID
char pass[] = "aaaaaaaa";  // Your WiFi password


#define AMMONIA_PIN D4  // Pin for ammonia sensor
#define AMMONIA_RELAY_PIN D5  // Pin for ammonia relay
#define MQ135_PIN A0   // Analog pin for MQ135 sensor
#define AMMONIA_VIRTUAL_PIN V2  // Virtual pin for ammonia value
#define MOTION_PIN D2           // Pin for motion sensor
#define LDR_PIN D8              // Pin for LDR
#define MOTION_RELAY_PIN D1    // Pin for motion relay
#define LDR_RELAY_PIN D0        // Pin for LDR relay

bool motionDetected = false;
bool isNight = false;
bool ammoniaRelayState = true;

// Calibration parameters (example values, adjust based on your calibration)
float slope = 0.01;  // Slope of the calibration curve
float intercept = 0.5;  // Intercept of the calibration curve

// Threshold for activating the relay (adjust as needed)
float ammoniaThreshold = 4;  // Example threshold of 0.5% ammonia concentration

BlynkTimer timer;

void setup() {
  pinMode(MOTION_PIN, INPUT);
  pinMode(AMMONIA_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(LDR_RELAY_PIN, OUTPUT);
  pinMode(MOTION_RELAY_PIN, OUTPUT);
  pinMode(AMMONIA_RELAY_PIN, OUTPUT);
  
  digitalWrite(MOTION_RELAY_PIN, HIGH);
  digitalWrite(LDR_RELAY_PIN, LOW);
  digitalWrite(AMMONIA_RELAY_PIN, HIGH);
  
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  
  timer.setInterval(3000L, checkSensors);
}

void loop() {
  Blynk.run();
  timer.run();
}

void checkSensors() {
  
  int ammoniaValue = digitalRead(AMMONIA_PIN);
  int mq135Value = analogRead(MQ135_PIN);
  int motionValue = digitalRead(MOTION_PIN);
  int ldrValue = digitalRead(LDR_PIN);
  

    // Check motion sensor
  if (motionValue == HIGH) {
    motionDetected = true;
  } else {
    motionDetected = false;
  }

  // Check LDR for night or day
  if (ldrValue == LOW) {
    isNight = true;
    digitalWrite(LDR_RELAY_PIN, HIGH);  // Turn on LDR relay during the night
  } else {
    isNight = false;
    // Control LDR relay based on motion detection during the day
    if (motionDetected) {
      digitalWrite(LDR_RELAY_PIN, HIGH);  // Turn on LDR relay
    } else {
      digitalWrite(LDR_RELAY_PIN, LOW);   // Turn off LDR relay
    }
  }

  // Control motion relay based on motion detection
  if (motionDetected) {
    digitalWrite(MOTION_RELAY_PIN, HIGH);  // Turn on motion relay
    Blynk.virtualWrite(V1, 1);   // Update Blynk app
  } else {
    digitalWrite(MOTION_RELAY_PIN, LOW);   // Turn off motion relay
    Blynk.virtualWrite(V1, 0);   // Update Blynk app
  }

   // Convert analog reading to ammonia concentration using calibration parameters
  float ammoniaConcentration = slope * mq135Value + intercept;
  
  // Send ammonia concentration to Blynk app
  Blynk.virtualWrite(AMMONIA_VIRTUAL_PIN, ammoniaConcentration);
  
  // Check ammonia level
  if (ammoniaConcentration >= ammoniaThreshold) {
    // Ammonia level above threshold
    if (!ammoniaRelayState) {
      ammoniaRelayState = true;
      digitalWrite(AMMONIA_RELAY_PIN, LOW); // Turn on the ammonia relay
    }
  } else {
    // Ammonia level below threshold
    if (ammoniaRelayState) {
      ammoniaRelayState = false;
      digitalWrite(AMMONIA_RELAY_PIN, HIGH); // Turn on the ammonia relay
    }
}}