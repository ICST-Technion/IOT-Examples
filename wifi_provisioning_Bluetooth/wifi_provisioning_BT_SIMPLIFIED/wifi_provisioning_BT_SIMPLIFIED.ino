// Based on https://github.com/robotzero1/esp32-bluewifi/blob/master/ESP32WebcamBluetoothWifi.ino


#include <WiFi.h>
#include <Preferences.h>
#include "BluetoothSerial.h"

String network_string;
String connected_string;

const char* pref_ssid = "";
const char* pref_pass = "";
String client_wifi_ssid;
String client_wifi_password;

const char* bluetooth_name = "myESP32";

long start_wifi_millis;
long wifi_timeout = 10000;
// bool bluetooth_disconnect = false;

enum wifi_setup_stages { NONE,
                         SCAN_START,
                         WAIT_SSID,
                         SSID_ENTERED,
                         WAIT_PASS,
                         PASS_ENTERED,
                         WAIT_CONNECT,
                         LOGIN_FAILED };
enum wifi_setup_stages wifi_stage = NONE;

BluetoothSerial SerialBT;
Preferences preferences;



void setup() {
  Serial.begin(115200);
  Serial.println("Booting...");

  preferences.begin("wifi_details", false);

  pinMode(4, INPUT_PULLUP);
  delay(50);
  if (digitalRead(4) == 0) {  // setup button
    SerialBT.register_callback(callback);
    SerialBT.begin(bluetooth_name);
    Serial.print("To provision SSID PASS Connect using bluetooth to ");
    Serial.println(bluetooth_name);
  } else {
    init_wifi();
  }
}

bool init_wifi() {
  String temp_pref_ssid = preferences.getString("pref_ssid");
  String temp_pref_pass = preferences.getString("pref_pass");
  pref_ssid = temp_pref_ssid.c_str();
  pref_pass = temp_pref_pass.c_str();

  Serial.print("connecting to SSID ");
  Serial.print(pref_ssid);
  Serial.print(" PASS: ");
  Serial.println(pref_pass);

  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);

  start_wifi_millis = millis();
  WiFi.begin(pref_ssid, pref_pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - start_wifi_millis > wifi_timeout) {
      WiFi.disconnect(true, true);
      Serial.print("Wifi connection fail!");
      return false;
    }
  }
  Serial.print("Connected!");
  return true;
}

void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t* param) {

  if (event == ESP_SPP_SRV_OPEN_EVT) {
    wifi_stage = SCAN_START;
  }

  if (event == ESP_SPP_DATA_IND_EVT && wifi_stage == WAIT_SSID) {  // data from phone is SSID
    client_wifi_ssid = SerialBT.readString();
    client_wifi_ssid.trim();
    wifi_stage = SSID_ENTERED;
  }

  if (event == ESP_SPP_DATA_IND_EVT && wifi_stage == WAIT_PASS) {  // data from phone is password
    client_wifi_password = SerialBT.readString();
    client_wifi_password.trim();
    wifi_stage = PASS_ENTERED;
  }
}


void disconnect_bluetooth() {
  delay(1000);
  Serial.println("BT stopping");
  SerialBT.println("Bluetooth disconnecting...");
  delay(1000);
  SerialBT.flush();
  SerialBT.disconnect();
  SerialBT.end();
  Serial.print("BT stopped. available heap=");
  Serial.println(ESP.getFreeHeap());
  delay(1000);
}

void handle_BT_provisioning() {

  switch (wifi_stage) {
    case SCAN_START:
      Serial.println("Bluetooth communication with client");
      SerialBT.println("Please enter the NAME for your Wi-Fi");
      wifi_stage = WAIT_SSID;
      break;

    case SSID_ENTERED:
      SerialBT.println("Please enter your Wi-Fi password");
      Serial.println("Please enter your Wi-Fi password");
      wifi_stage = WAIT_PASS;
      break;

    case PASS_ENTERED:
      SerialBT.println("Please wait for Wi-Fi connection...");
      Serial.println("Please wait for Wi_Fi connection...");
      wifi_stage = WAIT_CONNECT;
      preferences.putString("pref_ssid", client_wifi_ssid);
      preferences.putString("pref_pass", client_wifi_password);
      if (init_wifi()) {  // Connected to WiFi
        connected_string = "ESP32 IP: ";
        connected_string = connected_string + WiFi.localIP().toString();
        SerialBT.println(connected_string);
        Serial.println(connected_string);
        disconnect_bluetooth();
      } else {  // try again
        SerialBT.println("Wi-Fi connection failed");
        Serial.println("Wi-Fi connection failed");
      }
      break;

    case LOGIN_FAILED:
      SerialBT.println("Wi-Fi connection failed");
      Serial.println("Wi-Fi connection failed");
      delay(2000);
      break;
  }
}

void loop() {
  handle_BT_provisioning();
  delay(100);
}