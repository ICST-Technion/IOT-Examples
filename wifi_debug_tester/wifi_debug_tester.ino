
int i = 0;

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  for (i = 0; i <= 9; i++)
  {
    Serial.print("debug test ");
    Serial.println(i);
    delay(500);        // delay in between reads for stability
  }
}
