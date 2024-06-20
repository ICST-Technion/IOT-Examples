#include "HX711.h"
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;
HX711 scale;

void setup() {
  // put your setup code here, to run once:
Serial.begin(57600);
scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
Serial.println("Before setting up the scale:");
Serial.print("read: \t\t");
Serial.println(scale.read());      // print a raw reading from the ADC

Serial.print("read average: \t\t");
Serial.println(scale.read_average(20));   // print the average of 20 readings from the ADC

Serial.print("get value: \t\t");
Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight (not set yet)

Serial.print("get units: \t\t");
Serial.println(scale.get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided
// by the SCALE parameter (not set yet)
scale.set_scale(-362.8); //!!!!!!!!!!!!!!!!!!!!!!!!insert your calibration factor!!!!!!!!!!!!!!!!!!!!!!!!!!!
scale.tare();               // reset the scale to 0
Serial.print("read: \t\t");
Serial.println(scale.read());                 // print a raw reading from the ADC

Serial.print("read average: \t\t");
Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

Serial.print("get value: \t\t");
Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight, set with tare()

Serial.print("get units: \t\t");
Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
// by the SCALE parameter set with set_scale

}

void loop() {
  // put your main code here, to run repeatedly:
Serial.print("one reading:\t");
Serial.print(scale.get_units(), 1);
Serial.print("\t| average:\t");
Serial.println(scale.get_units(10), 5);
scale.power_down();  // put the ADC in sleep mode
delay(5000);
scale.power_up();
}
