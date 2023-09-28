class BlinkingLED {
public:
  BlinkingLED(Adafruit_NeoPixel& pixels, int ledIndex, int blinkInterval) {
    _pixels = &pixels;
    _ledIndex = ledIndex;
    _blinkInterval = blinkInterval;
    _lastUpdateTime = 0;
    _isOn = false;
  }

  void update() {
    unsigned long currentTime = millis();

    if (currentTime - _lastUpdateTime >= _blinkInterval) {
      _lastUpdateTime = currentTime;
      _isOn = !_isOn;

      if (_isOn) {
        _pixels->setPixelColor(_ledIndex, _pixels->Color(255, 0, 0)); 
      } else {
        _pixels->setPixelColor(_ledIndex, _pixels->Color(0, 0, 0)); // Turn off the LED
      }

      _pixels->show();
    }
  }

private:
  Adafruit_NeoPixel* _pixels;
  int _ledIndex;
  int _blinkInterval;
  unsigned long _lastUpdateTime;
  bool _isOn;
};