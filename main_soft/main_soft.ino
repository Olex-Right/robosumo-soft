#define BUTTON_PIN 12
#define TRIGGER 10
#define ECHO 11
#define LEFT_BACKWARD 3
#define LEFT_FORWARD 4
#define RIGHT_BACKWARD 5
#define RIGHT_FORWARD 6

int distance=0;

void setup()
{
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(ECHO, INPUT);
  pinMode(TRIGGER, OUTPUT);
}

int sensor()
{
  digitalWrite(TRIGGER,1);
  delay(10);
  digitalWrite(TRIGGER,0);
  while(digitalRead(ECHO)==0);
  // begin and time in mikro seconds
  unsigned long begin=micros();
  unsigned long time;
  while(micros()-begin<=11660 && digitalRead(ECHO)==1)
  {
    time=micros()-begin;
  }
  if(time>=11660)return -1;
    // return distance in cm; 343 - speed of sound; / 2 - sound goes forward and backward
    return (343 * (time/10000.0)) / 2;
  return 0;
}

void loop()
{
  //Serial.println(digitalRead(ECHO));
  if(digitalRead(BUTTON_PIN)==0)
  {
    distance=sensor();
    Serial.println(distance);

    // less than 100 cm
    if(distance < 100){
      // ensure other pins are 0
      digitalWrite(LEFT_BACKWARD, 0);
      digitalWrite(RIGHT_BACKWARD, 0);

      // go forward
      digitalWrite(LEFT_FORWARD, 1);
      digitalWrite(RIGHT_FORWARD, 1);
    } else {
      // ensure other pins are 0
      digitalWrite(LEFT_BACKWARD, 0);
      digitalWrite(RIGHT_FORWARD, 0);

      // rotate to the right
      digitalWrite(LEFT_FORWARD, 1);
      digitalWrite(RIGHT_BACKWARD, 1);
    }
  }
  delay(100);
}