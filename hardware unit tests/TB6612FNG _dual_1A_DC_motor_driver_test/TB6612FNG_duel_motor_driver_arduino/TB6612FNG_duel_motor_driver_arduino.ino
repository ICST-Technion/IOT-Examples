//motor A connected between A01 and A02
//motor B connected between B01 and B02
int STBY = 10; //standby
//Motor A
int PWMA = 3; //Speed control
int AIN1 = 9; //Direction
int AIN2 = 8; //Direction
//Motor B
int PWMB = 5; //Speed control
int BIN1 = 11; //Direction
int BIN2 = 12; //Direction
void setup(){
pinMode(STBY, OUTPUT);
pinMode(PWMA, OUTPUT);
pinMode(AIN1, OUTPUT);
pinMode(AIN2, OUTPUT);
pinMode(PWMB, OUTPUT);
pinMode(BIN1, OUTPUT);
pinMode(BIN2, OUTPUT);
}
void loop(){
  //disable standby to make the motors run
  digitalWrite(STBY,HIGH);
  //set motor A and motor B speed, 0-255 255 being the fastest
  analogWrite(PWMA,255);
  analogWrite(PWMB,127);
  //set motor A direction
  digitalWrite(AIN1,LOW);
  digitalWrite(AIN1,HIGH);
  //set motor B direction
  digitalWrite(BIN1,LOW);
  digitalWrite(BIN1,HIGH);
  delay(50000);//the two motors will spin in opposite direction for 5 seconds. motor A spins at full speed while motor b spins at half speed
  //enable standby to make the motors stop spinning
  digitalWrite(STBY,LOW);
  delay(1000); //the two motors will stop spinning for 1 second

}