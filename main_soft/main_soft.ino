#define BUTTON_PIN 12
#define TRIGGER 8
#define ECHO 7
#define LEFT_BACKWARD 11
#define LEFT_FORWARD 10
#define RIGHT_BACKWARD 3
#define RIGHT_FORWARD 9

int distance=0;
bool active=false;
short robotState = 0; //0 - stop; 1- move Forward; 2 - move Backward; 3 - turn Right; 4 - turn Left;
void setup()
{
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(ECHO, INPUT);
  pinMode(TRIGGER, OUTPUT);
  pinMode(LEFT_BACKWARD, OUTPUT);
  pinMode(LEFT_FORWARD, OUTPUT);
  pinMode(RIGHT_BACKWARD, OUTPUT);
  pinMode(RIGHT_FORWARD, OUTPUT);
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

void wait()
{
  while(digitalRead(BUTTON_PIN)==0);
  delay(5000);
}

void loop()
{
  //Serial.println(digitalRead(ECHO));
  if(digitalRead(BUTTON_PIN)==0 && !active)
  {
    wait();
    active=!active;
  }
  if(active){
    distance=sensor();
    Serial.println(distance);

    // less than 75 cm
    if(distance < 75){
      Serial.println(2);
      if(!(robotState == 1)){
        stopMovement();
        moveForward();
        robotState = 1;
      }
    } 
    else {   
        if(!(robotState == 3)){
            stopMovement();
            rotateRight();
            robotState = 3;
        }
    }
  }
  else {
      stopMovement();
  }
}

void stopMovement()
{
  analogWrite(LEFT_FORWARD, 0);
  analogWrite(RIGHT_FORWARD, 0);
  analogWrite(LEFT_BACKWARD, 0);
  analogWrite(RIGHT_FORWARD, 0);
}

void moveForward()
{
  analogWrite(LEFT_FORWARD, 128);
  analogWrite(RIGHT_FORWARD, 128);
}

void rotateRight()
{
  analogWrite(LEFT_FORWARD, 128);
  analogWrite(RIGHT_BACKWARD, 128);
}

void rotateLeft()
{
  analogWrite(LEFT_BACKWARD, 128);
  analogWrite(RIGHT_FORWARD, 128);
}