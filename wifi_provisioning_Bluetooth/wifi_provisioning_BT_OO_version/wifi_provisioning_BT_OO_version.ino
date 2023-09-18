#include <Preferences.h>
#include "wifiCredentialsBT.h"

Preferences preferences;
wifiCerdentialsBluetooth wifiHandler(&preferences);  // Pass the Preferences object as a pointer

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 reboot");
  pinMode(4, INPUT_PULLUP); // pin 4 is the wifi setup button , used only at boot
  if (!preferences.begin("wifi_creds", false)) { Serial.println("Failed to initialize Preferences"); }
  delay(50);
  if (digitalRead(4) == 0) {  // if pin 4 is low during boot, get new WiFi SSID and password using Bluetooth
    Serial.println("Acquire new WiFi credentials via Bluetooth");
    wifiHandler.begin();
  } else {
    Serial.println("Trying to connect to WiFi using saved credentials");
    wifiHandler.init_wifi();
  }
}

void loop() {
  if (!wifiHandler.connectionStatus) {
    wifiHandler.handle(); //non-blocking 
  } else {
    Serial.println("main loop - wifi is connected");
  }
  delay(50);
}