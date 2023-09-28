#define MIN_PRESS_TIME 25 //minimum duration in milliseconds to consider a button press

class ButtonHandler
{
public:
  ButtonHandler(int buttonPin) {
    _buttonPin=buttonPin;
  }
    
  void begin() {
    pinMode(_buttonPin, INPUT_PULLUP);   // set the digital pin as an input with pullup resistor (important)
  }

  int press_counter() {return _buttonPress_counter;}

  unsigned long last_press_duration() {return _last_press_duration;}

  bool handle() { 
    
    _currentButtonState = digitalRead(_buttonPin);  // check button state's change

    if (_currentButtonState != _previousButtonState)  //if change is detected
    {
      if (_currentButtonState == LOW)  //type of change - from released to pressed
      {
         _previousButtonState = _currentButtonState; 
        _buttonPress_last_time = millis();
      } else {
        _previousButtonState = _currentButtonState; 
        _last_press_duration = millis() - _buttonPress_last_time;
        if (_last_press_duration > MIN_PRESS_TIME) {
          _buttonPress_counter = _buttonPress_counter + 1;
          return (true); 
        }
      }
    }
    return (false);
  }

private:
  int _buttonPin;
  bool _previousButtonState=HIGH;
  bool _currentButtonState=HIGH;
  unsigned long _buttonPress_last_time=0;
  unsigned long _last_press_duration=0;
  int _buttonPress_counter=0;
};