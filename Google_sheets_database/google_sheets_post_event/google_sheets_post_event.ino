//Include required libraries
#include "WiFi.h"
#include <HTTPClient.h>

// WiFi credentials
const char* ssid = "Wokwi-GUEST";         // change SSID
const char* password = "";    // change password

//sample web app link -
 
// results google sheet link  - https://docs.google.com/spreadsheets/d/1bLoXfMlVEIFwtIqw9Ddh5lMiISU4WpZzajkjpbW_rZU/

// Google script ID and required credentials
//https://script.google.com/macros/s/AKfycbyR1hQposfn0mVRamEYJjSRv8qTnLfnv7SqjWGYlWZkslWVGaQqsVhzorIpeNFaBlw3/exec
const String GOOGLE_SCRIPT_ID = "AKfycbyR1hQposfn0mVRamEYJjSRv8qTnLfnv7SqjWGYlWZkslWVGaQqsVhzorIpeNFaBlw3";    // change Gscript ID
const int EVENT_OPEN = 1;
const int EVENT_CLOSE = 0;
const String THING_NAME = "door_sensor_1";

String event_string="";
bool last_state=false;
bool current_state=false;

int retry_counter = 0;

void setup() {
  pinMode(4, INPUT_PULLUP);
  delay(100);
  Serial.begin(115200);
  delay(100);
  // connect to WiFi
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected ");
}
void loop() {
  current_state=digitalRead(4);
  if (current_state!=last_state)
  {
    if (current_state)
    {
    event_string = String(EVENT_OPEN);
    Serial.println("***open_event***");
    }
    else
    {
    event_string = String(EVENT_CLOSE);
    Serial.println("***close_event***");
    }
  last_state=current_state;
  post_data();
  }

}

void post_data()
{
  retry_counter=3;
  while (retry_counter>0)
  {
    String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + 
    "/exec?" + THING_NAME + "=" + event_string + 
    "&retries_left=" + String(retry_counter) + 
    "&esp_time=" + String(millis());

    Serial.println("POST data to spreadsheet:");
    Serial.println(urlFinal);
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    //---------------------------------------------------------------------
    //getting response from google sheet
    if (httpCode > 0) {
      String payload;
      payload = http.getString();
      Serial.println("Payload: " + payload);
      http.end();
      retry_counter=0; 
    }
    else
    {
      retry_counter--;
      Serial.println("HTTP GET failed - retries left: " + String(retry_counter));
      delay(2000);
    }
    //---------------------------------------------------------------------
    
  }
  delay(500); 
}
