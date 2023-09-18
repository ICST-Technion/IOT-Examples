#include "BluetoothSerial.h"
#include <WiFi.h>

class wifiCerdentialsBluetooth {
public:
  String client_wifi_ssid;
  String client_wifi_password;
  bool connectionStatus = false;


  wifiCerdentialsBluetooth(Preferences* prefs) {
    preferences = prefs;
  }

  void begin() {
    SerialBT.begin(bluetooth_name);
    Serial.print("To provision SSID PASS Connect using bluetooth to: ");
    Serial.println(bluetooth_name);
  }

  bool init_wifi() {
    String temp_pref_ssid = preferences->getString("pref_ssid");
    String temp_pref_pass = preferences->getString("pref_pass");
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
    connectionStatus = true;
    return true;
  }

  void handle_BT_comm() {

    if (SerialBT.connected() && wifi_stage == NONE) {
      wifi_stage = START;
    }

    if (SerialBT.available() && wifi_stage == WAIT_SSID) {  // data from phone is SSID
      client_wifi_ssid = SerialBT.readString();
      Serial.println(client_wifi_ssid);
      client_wifi_ssid.trim();
      Serial.println(client_wifi_ssid);
      wifi_stage = SSID_ENTERED;
    }

    if (SerialBT.available() && wifi_stage == WAIT_PASS) {  // data from phone is password
      client_wifi_password = SerialBT.readString();
      client_wifi_password.trim();
      wifi_stage = PASS_ENTERED;
    }
  }


  void handle_logic() {
    switch (wifi_stage) {
      case START:
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
        preferences->putString("pref_ssid", client_wifi_ssid);
        preferences->putString("pref_pass", client_wifi_password);
        if (init_wifi()) {  // Connected to WiFi
          wifi_stage = CONNECTED;
          connected_string = "ESP32 IP: ";
          connected_string = connected_string + WiFi.localIP().toString();
          SerialBT.println(connected_string);
          Serial.println(connected_string);
          disconnect_bluetooth();
        } else {
          wifi_stage = LOGIN_FAILED;
        }
        break;

      case LOGIN_FAILED:
        SerialBT.println("Wi-Fi connection failed - rebooting ESP32");
        Serial.println("Wi-Fi connection failed - rebooting ESP32");
        ESP.restart();
        break;
    }
  }

  void disconnect_bluetooth() {
    Serial.println("BT stopping");
    SerialBT.println("Bluetooth disconnecting...");
    SerialBT.flush();
    SerialBT.disconnect();
    SerialBT.end();
    Serial.print("BT stopped. available heap=");
    Serial.println(ESP.getFreeHeap());
    delay(100);
  }

  void handle() {
    handle_BT_comm();
    handle_logic();

  }


private:
  Preferences* preferences;  // Pointer to Preferences object
  BluetoothSerial SerialBT;
  String connected_string;

  const char* pref_ssid = "";
  const char* pref_pass = "";

  const char* bluetooth_name = "myESP32";

  long start_wifi_millis;
  long wifi_timeout = 10000;
  // bool bluetooth_disconnect = false;

  enum wifi_setup_stages { NONE,
                           START,
                           WAIT_SSID,
                           SSID_ENTERED,
                           WAIT_PASS,
                           PASS_ENTERED,
                           WAIT_CONNECT,
                           CONNECTED,
                           LOGIN_FAILED };
  enum wifi_setup_stages wifi_stage = NONE;
};
