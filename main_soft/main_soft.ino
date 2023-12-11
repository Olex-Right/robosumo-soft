#define BUTTON_PIN 12
#define TRIGGER 8
#define FORWARD_ECHO 7
#define LEFT_ECHO 4
#define RIGHT_ECHO 2
#define LEFT_BACKWARD 11
#define LEFT_FORWARD 10
#define RIGHT_BACKWARD 3
#define RIGHT_FORWARD 9

#define ROTATION_GAIN 60
#define DISTANCE 20

const int blackColor = 300;
const int whiteColor = 700;
const int errorRange = 50;

int forwardDistance = 0;
int leftDistance = 0;
int rightDistance = 0;
bool active = false;
short robotState = 0; // 0 - stop; 1- move Forward; 2 - move Backward; 3 - turn Right; 4 - turn Left;
short previousState = 3;

void setup()
{
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LEFT_ECHO, INPUT);
  pinMode(RIGHT_ECHO, INPUT);
  pinMode(FORWARD_ECHO, INPUT);
  pinMode(TRIGGER, OUTPUT);
  pinMode(LEFT_BACKWARD, OUTPUT);
  pinMode(LEFT_FORWARD, OUTPUT);
  pinMode(RIGHT_BACKWARD, OUTPUT);
  pinMode(RIGHT_FORWARD, OUTPUT);
}

int sensor(int ECHO_PIN)
{
  digitalWrite(TRIGGER, 1);
  delay(10);
  digitalWrite(TRIGGER, 0);
  while (digitalRead(ECHO_PIN) == 0)
    ;
  // begin and time in mikro seconds
  unsigned long begin = micros();
  unsigned long time;
  while (micros() - begin <= 11660 && digitalRead(ECHO_PIN) == 1)
  {
    time = micros() - begin;
  }
  if (time >= 11660)
    return -1;
  // return distance in cm; 343 m/sec - speed of sound; time in mikroSec (1e-6 sec * 1e4 m/sec = 10e-2m = cm);  / 2 - sound goes forward and backward
  return (343 * (time / 10000.0)) / 2;
}

void wait()
{
  while (digitalRead(BUTTON_PIN) == 0)
    ;
  // delay(5000);
}

void loop()
{
  delay(1);
  // wait for button
  if ((digitalRead(BUTTON_PIN) == 0) && !active)
  {
    wait();
    active = !active;
  }

  if (active)
  {
    forwardDistance = sensor(FORWARD_ECHO);
    rightDistance = sensor(RIGHT_ECHO);
    leftDistance = sensor(LEFT_ECHO);
    Serial.println("forwardDistance " + forwardDistance);
    Serial.println("rightDistance " + rightDistance);
    Serial.println("leftDistance " + leftDistance);

    //here check COLOR
    int colorSensors = 0;
    if (!isInRangeColor(2)){
        ++colorSensors;
    }
    if (!isInRangeColor(4)){
        ++colorSensors;
    }
    if (!isInRangeColor(6)){
        ++colorSensors;
    }
    if (colorSensors >= 2){
        stopMovement();
        active = false;
        robotState = 0;
    }

    // less than DISTANCE in cm
    if (forwardDistance < DISTANCE && forwardDistance != -1 && active)
    {
      Serial.println("MOVE Forward");
      if (robotState != 1)
      {
        previousState = robotState ? robotState : 3;
        stopMovement();
        moveForward();
        robotState = 1;
      }
    }
    else if(leftDistance < DISTANCE && leftDistance != -1 && active)
    {
      Serial.println("TURN LEFT");
      if (robotState != 4)
      {
        stopMovement();
        rotateLeft();
        robotState = 4;
      }
    }
    else if(rightDistance < DISTANCE && rightDistance != -1 && active)
    {
      Serial.println("TURN RIGHT");
      if (robotState != 3)
      {
        stopMovement();
        rotateRight();
        robotState = 3;
      }
    }
    else if (active)
    {
      Serial.println("BASIC MOVE");
      if (robotState != 3 && robotState != 4)
      {
        stopMovement();
        previousState == 3 ? rotateRight() : rotateLeft();
        robotState = previousState;
      }
    }
  }
  else
  {
    if (robotState != 0)
    {
      stopMovement();
      robotState = 0;
    }
  }
}

void stopMovement()
{
  digitalWrite(LEFT_FORWARD, LOW);
  digitalWrite(RIGHT_FORWARD, LOW);
  digitalWrite(LEFT_BACKWARD, LOW);
  digitalWrite(RIGHT_BACKWARD, LOW);
}

void moveForward()
{
  digitalWrite(LEFT_FORWARD, HIGH);
  digitalWrite(RIGHT_FORWARD, HIGH);
}

void moveBackward()
{
  digitalWrite(LEFT_BACKWARD, HIGH);
  digitalWrite(RIGHT_BACKWARD, HIGH);
}

void rotateRight()
{
  analogWrite(LEFT_FORWARD, ROTATION_GAIN);
  analogWrite(RIGHT_BACKWARD, ROTATION_GAIN);
}

void rotateLeft()
{
  analogWrite(LEFT_BACKWARD, ROTATION_GAIN);
  analogWrite(RIGHT_FORWARD, ROTATION_GAIN);
}
//check if color is in range by giving it pin, if no -> return false
bool isInRangeColor(int pin){
    int colorValue = analogRead(pin);

    if (colorValue >= (blackColor - errorRange) && colorValue <= (blackColor + errorRange))
    {
      // Detected black color
      Serial.print("Black Color Detected on ");
      Serial.println(pin);
      return true;
    }
    else if (colorValue >= (whiteColor - errorRange) && colorValue <= (whiteColor + errorRange))
    {
      // Detected white color
      Serial.print("White Color Detected on ");
      Serial.println(pin);
      return true;
    }
    else
    {
      // Unknown color
      Serial.print("Unknown Color Detected on ");
      Serial.println(pin);
      return false;
    }
}