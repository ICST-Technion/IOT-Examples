class Flasher
{
    // Class Member Variables,  These are initialized at startup
    int ledPin;      // the number of the LED pin
    // These maintain the current state
    int ledState;                 // ledState used to set the LED
    unsigned long previousMillis;   // will store last time LED was updated

    // Constructor - creates a Flasher and initializes the member variables and state
  public:
    long OnTime;     // milliseconds of on-time
    long OffTime;    // milliseconds of off-time
    Flasher(int pin, long off)
    {
      ledPin = pin;
      pinMode(ledPin, OUTPUT);


      OffTime = off;

      ledState = LOW;
      previousMillis = 0;
    }

    void Update()
    {
      // check to see if it's time to change the state of the LED
      unsigned long currentMillis = millis();

      if ((ledState == HIGH) && (currentMillis - previousMillis >= OnTime))
      {
        ledState = LOW;  // Turn it off
        previousMillis = currentMillis;  // Remember the time
        digitalWrite(ledPin, ledState);  // Update the actual LED
      }
      else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime))
      {
        ledState = HIGH;  // turn it on
        previousMillis = currentMillis;   // Remember the time
        digitalWrite(ledPin, ledState);   // Update the actual LED
      }
    }
};
