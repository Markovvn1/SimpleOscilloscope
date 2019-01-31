volatile char last;

ISR(PCINT0_vect)
{
  cli();
  char cur = PINB & 0b00111111;
  if (cur != last)
  {
    last = cur;
    Serial.print(cur);
  }

  sei();
}

void setup()
{
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  Serial.begin(2000000);

  last = PINB & 0b00111111;
  Serial.print(last);
  PCMSK0 = 0b00111111;
  PCICR = 0b00000001;
}

void loop()
{
 
}
