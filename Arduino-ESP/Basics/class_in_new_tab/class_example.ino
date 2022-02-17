#include "Flasher.h"

Flasher led1(13, 100);


void setup()
{
  led1.OffTime = 500;
  led1.OnTime = 200;
}

void loop() {
  led1.Update();
}
