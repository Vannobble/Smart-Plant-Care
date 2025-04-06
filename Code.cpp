#define BLYNK_TEMPLATE_ID "TMPL6AJGKV2R5"
#define BLYNK_TEMPLATE_NAME "Test"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFiClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

#define SOIL_SENSOR_PIN 33   // GPIO33 for Soil Sensor
#define RELAY_PIN 4          // GPIO4 for Relay
#define BUTTON_PIN 13        // GPIO13 for Physical Button
#define DHT_PIN 16           // GPIO16 for DHT22
#define LDR_PIN 35           // GPIO35 for LDR Module
#define VPIN_SOIL V3         // Virtual Pin for Soil Moisture
#define VPIN_TEMP V0         // Virtual Pin for Temperature
#define VPIN_HUMIDITY V1     // Virtual Pin for Humidity
#define VPIN_BUTTON V12      // Virtual Pin for Button
#define VPIN_LDR V2          // Virtual Pin for LDR

// Initialize LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHT_PIN, DHT22);
BlynkTimer timer;

// Enter your Auth token
char auth[] = "avf-HWqC3swINrmfBsGWFUju5R187O0y";

char ssid[] = "iyah";
char pass[] = "dosson1234";

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  lcd.begin();
  lcd.backlight();

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  dht.begin();

  lcd.setCursor(1, 0);
  lcd.print("Initializing...");
  for (int i = 0; i <= 15; i++) {
    lcd.setCursor(i, 1);
    lcd.print(".");
    delay(200);
  }
  lcd.clear();

  timer.setInterval(1000L, readSoilMoisture);
  timer.setInterval(1000L, readDHTSensor);
  timer.setInterval(500L, checkPhysicalButton);
  timer.setInterval(1000L, readLDRSensor); // Tambahkan interval untuk membaca LDR
}

void readSoilMoisture() {
  int soilValue = analogRead(SOIL_SENSOR_PIN);
  soilValue = map(soilValue, 0, 4095, 0, 100);
  soilValue = (soilValue - 100) * -1;

  Blynk.virtualWrite(VPIN_SOIL, soilValue);
  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(soilValue);
  lcd.print(" ");
}

void readDHTSensor() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(VPIN_TEMP, temperature);
  Blynk.virtualWrite(VPIN_HUMIDITY, humidity);

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature);
  lcd.print("C ");
  lcd.print("H:");
  lcd.print(humidity);
  lcd.print("%");
}

void readLDRSensor() {
  int ldrValue = analogRead(LDR_PIN);
  ldrValue = map(ldrValue, 0, 4095, 0, 100);
  ldrValue = (ldrValue - 100) * -1;

  Blynk.virtualWrite(VPIN_LDR, ldrValue);
  lcd.setCursor(5, 1); // Menampilkan LDR di baris 1, kolom 5
  lcd.print("L:");
  lcd.print(ldrValue);
  lcd.print("% ");
}

BLYNK_WRITE(VPIN_BUTTON) {
  int buttonState = param.asInt();
  if (buttonState == 1) { // Tombol ditekan
    digitalWrite(RELAY_PIN, LOW); // Motor ON
  } else { // Tombol dilepas
    digitalWrite(RELAY_PIN, HIGH); // Motor OFF
  }
}

void checkPhysicalButton() {
  // Jika tetap ingin tombol fisik digunakan, logika tambahan dapat ditulis di sini.
}

void loop() {
  Blynk.run();
  timer.run();
  delay(200);
  if (digitalRead(RELAY_PIN) == LOW) {
    lcd.setCursor(11, 1);
    lcd.print("W:ON ");
  } else {
    lcd.setCursor(11, 1);
    lcd.print("W:OFF");
  }
}
