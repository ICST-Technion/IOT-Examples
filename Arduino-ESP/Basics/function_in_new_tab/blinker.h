void blinker()
{
  digitalWrite(LED_PIN, HIGH);
  delay(BLINK_RATE);
  digitalWrite(LED_PIN, LOW);
  delay(BLINK_RATE);
}
