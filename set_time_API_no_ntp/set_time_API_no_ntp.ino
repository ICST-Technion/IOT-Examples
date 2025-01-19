#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "ICST";
const char* password = "arduino123";

// Timer variables
unsigned long previousMillis_timeSync = 0;
const unsigned long interval_timeSync = 3600000;  // 1 hour in milliseconds
unsigned long previousMillis_timeDisp = 0;
const unsigned long interval_timeDisp = 1000;  // 1 second in milliseconds

void setupWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

bool syncTimeFromAPI() {
  HTTPClient http;
  http.begin("https://timeapi.io/api/Time/current/zone?timeZone=Asia/Jerusalem");
  
  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    Serial.printf("HTTP GET failed: %d\n", httpCode);
    http.end();
    return false;
  }

  // Get the response payload
  String payload = http.getString();  // Using getString() to get response
  http.end();

  // Allocate JSON document
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, payload);
  
  if (error) {
    Serial.print("JSON parsing failed: ");
    Serial.println(error.c_str());
    return false;
  }

  // Create time structure
  struct timeval tv;
  struct timezone tz;
  
  // Convert time components to epoch
  struct tm timeinfo;
  timeinfo.tm_year = doc["year"].as<int>() - 1900; // Years since 1900
  timeinfo.tm_mon = doc["month"].as<int>() - 1;    // Months since January
  timeinfo.tm_mday = doc["day"].as<int>();
  timeinfo.tm_hour = doc["hour"].as<int>();
  timeinfo.tm_min = doc["minute"].as<int>();
  timeinfo.tm_sec = doc["seconds"].as<int>();
  timeinfo.tm_isdst = doc["dstActive"].as<bool>() ? 1 : 0;

  tv.tv_sec = mktime(&timeinfo);
  tv.tv_usec = doc["milliSeconds"].as<long>() * 1000; // Convert ms to μs
  
  // Set system time
  if (settimeofday(&tv, &tz) != 0) {
    Serial.println("Failed to set system time");
    return false;
  }

  Serial.println("Time synchronized successfully");
  return true;
}

void setup() {
  Serial.begin(9600);
  setupWiFi();
  
  if (syncTimeFromAPI()) {
    Serial.println("syncTimeFromAPI returned success");
  }
}

void loop() {

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis_timeSync >= interval_timeSync) {
    previousMillis_timeSync = currentMillis;
    syncTimeFromAPI();
  }
  if (currentMillis - previousMillis_timeDisp >= interval_timeDisp) {
    previousMillis_timeDisp = currentMillis;
    time_t now;
    time(&now);
    Serial.printf("Current time: %s", ctime(&now));
  }


}