int pin_red_led = 29;
int pin_green_led = 30;

void setup() {
  // put your setup code here, to run once:
  pinMode(pin_red_led, OUTPUT);
  pinMode(pin_green_led,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(pin_red_led,HIGH);
  delay(1000);
  digitalWrite(pin_red_led,LOW);

  delay(1000);
  digitalWrite(pin_green_led,HIGH);
  delay(1000);
  digitalWrite(pin_green_led,LOW);
  
}
