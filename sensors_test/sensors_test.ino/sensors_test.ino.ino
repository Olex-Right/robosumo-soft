#define BUTTON_PIN 12
#define TRIGGER 8
#define LEFT_ECHO 7
#define RIGHT_ECHO 2  
#define LEFT_BACKWARD 11
#define LEFT_FORWARD 10
#define RIGHT_BACKWARD 3
#define RIGHT_FORWARD 9

#define ROTATION_GAIN 60
#define DISTANCE 75

const int blackColor = 300;
const int whiteColor = 700;
const int errorRange = 50;

int leftDistance = 0;
int rightDistance = 0;
bool active = false;
short robotState = 0;  // 0 - stop; 1- move Forward; 2 - move Backward; 3 - turn Right; 4 - turn Left;

void setup()
{
    Serial.begin(9600);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LEFT_ECHO, INPUT);
    pinMode(RIGHT_ECHO, INPUT); 
    pinMode(TRIGGER, OUTPUT);
    pinMode(LEFT_BACKWARD, OUTPUT);
    pinMode(LEFT_FORWARD, OUTPUT);
    pinMode(RIGHT_BACKWARD, OUTPUT);
    pinMode(RIGHT_FORWARD, OUTPUT);
}

int sensor(int echoPin)
{
    digitalWrite(TRIGGER, HIGH);
    delay(10);
    digitalWrite(TRIGGER, LOW);

    unsigned long duration = pulseIn(echoPin, HIGH);

    if (duration == 0)
        return -1;

    return (duration * 0.0343) / 2;
}

void wait()
{
    while (digitalRead(BUTTON_PIN) == HIGH)
        delay(10);
    delay(50); 
}

void loop()
{
    if ((digitalRead(BUTTON_PIN) == 0) && !active)
    {
        wait();
        active = !active;
    }

    if (active)
    {
        leftDistance = sensor(LEFT_ECHO);
        rightDistance = sensor(RIGHT_ECHO);

        Serial.print("Left Distance: ");
        Serial.println(leftDistance);
        Serial.print("Right Distance: ");
        Serial.println(rightDistance);

        
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