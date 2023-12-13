// Constants for pin assignments
#define BUTTON_PIN 12
#define TRIGGER 8
#define FORWARD_ECHO 6
#define LEFT_ECHO 4
#define RIGHT_ECHO 2
#define BACKWARD_ECHO 1  // 1 ???
#define LEFT_BACKWARD 11
#define LEFT_FORWARD 10
#define RIGHT_BACKWARD 3
#define RIGHT_FORWARD 9

// Constants for robot operation
#define ROTATION_GAIN 60
#define DISTANCE 20

// Color sensor thresholds
const int blackColor = 300;
const int whiteColor = 700;
const int errorRange = 50;

const bool readColor = true;

// Distance measurements
int forwardDistance = 0;
int leftDistance = 0;
int rightDistance = 0;
int backwardDistance = 0;

// Robot state variables
bool isActive = false;
enum RobotState {
    STOP = 0,
    FORWARD,
    BACKWARD,
    TURN_RIGHT,
    TURN_LEFT
};

RobotState robotState = STOP;
RobotState previousState = TURN_RIGHT;

void setup() {
    Serial.begin(9600);
    setupPinModes();
}

void setupPinModes() {
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

// Main loop for robot operation
void loop() {
    delay(1);
    handleButtonPress();

    if (isActive) {
        measureDistances();
        checkColorSensors();
        handleMovement();
    }
    else {
        ensureStopped();
    }
}

// Handle button press to activate/deactivate robot
void handleButtonPress() {
    if (digitalRead(BUTTON_PIN) == LOW && !isActive) {
        waitUntilButtonRelease();
        isActive = !isActive;
    }
}

void waitUntilButtonRelease() {
    while (digitalRead(BUTTON_PIN) == LOW) {
    }
}

// Measure distances from all directions
void measureDistances() {
    forwardDistance = measureDistance(FORWARD_ECHO);
    rightDistance = measureDistance(RIGHT_ECHO);
    leftDistance = measureDistance(LEFT_ECHO);
    backwardDistance = measureDistance(BACKWARD_ECHO);
    logDistances();
}

void logDistances() {
    Serial.println("Forward Distance: " + String(forwardDistance));
    Serial.println("Right Distance: " + String(rightDistance));
    Serial.println("Left Distance: " + String(leftDistance));
}

// Measure distance using ultrasonic sensor
int measureDistance(int echoPin) {
    digitalWrite(TRIGGER, HIGH);
    delay(10);
    digitalWrite(TRIGGER, LOW);

    while (digitalRead(echoPin) == LOW) {}

    unsigned long startTime = micros();
    unsigned long duration;

    while (micros() - startTime <= 11660 && digitalRead(echoPin) == HIGH) {
        duration = micros() - startTime;
    }

    if (duration >= 11660) {
        return -1; // Out of range
    }

    return (343 * (duration / 10000.0)) / 2; // calc distance in cm
}

// Check color sensors and stop if needed
void checkColorSensors() {
    int colorSensorCount = 0;
    if (!isColorInRange(2)) colorSensorCount++;
    if (!isColorInRange(4)) colorSensorCount++;
    if (!isColorInRange(6)) colorSensorCount++;

    if (colorSensorCount >= 2) {
        stopMovement();
        isActive = false;
        robotState = 0;
    }
}

// Check if the color on a given pin is within range
bool isColorInRange(int pin) {
    int colorValue = analogRead(pin);
    bool isBlack = (colorValue >= blackColor - errorRange) && (colorValue <= blackColor + errorRange);
    bool isWhite = (colorValue >= whiteColor - errorRange) && (colorValue <= whiteColor + errorRange);

    if (isBlack || isWhite) {
        logColorDetection(pin, isBlack ? "Black" : "White");
        return true;
    }
    else {
        logColorDetection(pin, "Unknown");
        return false;
    }
}

// Log color detection to Serial
void logColorDetection(int pin, const String& color) {
    Serial.print(color + " Color Detected on Pin ");
    Serial.println(pin);
}

// Determine the desired state based on sensor readings
void determineState() {
    Serial.println("Determinine state:");

    if (forwardDistance < DISTANCE && forwardDistance != -1 && isActive) {
        robotState = FORWARD;
        Serial.println("State set to FORWARD");
    }
    else if (leftDistance < DISTANCE && leftDistance != -1 && isActive) {
        robotState = TURN_LEFT;
        Serial.println("State set to TURN_LEFT");
    }
    else if (rightDistance < DISTANCE && rightDistance != -1 && isActive) {
        robotState = TURN_RIGHT;
        Serial.println("State set to TURN_RIGHT");
    }
    else if (backwardDistance < DISTANCE && backwardDistance != -1 && isActive) {
        robotState = BACKWARD;
        Serial.println("State set to BACKWARD");
    }
    else if (isActive) {
        robotState = previousState;
        Serial.print("Continuing previous state: ");
        Serial.println(robotState);
    }
    else {
        robotState = STOP;
        Serial.println("State set to STOP");
    }
}

// Handle robot movement based on the determined state
void handleMovement() {
    Serial.println("Handle movement:");
    determineState();
    switch (robotState) {
    case FORWARD:
        Serial.println("moveForward()");
        moveForward();
        break;
    case TURN_LEFT:
        Serial.println("turnLeft()");
        turnLeft();
        break;
    case TURN_RIGHT:
        Serial.println("turnRight()");
        turnRight();
        break;
    case BACKWARD:
        Serial.println("moveBackward()");
        moveBackward();
        break;
    case STOP:
        Serial.println("stopMovement()");
        stopMovement();
        break;
    default:
        Serial.println("continuePreviousMovement()");
        continuePreviousMovement();
    }
}

// Continue previous movement (turn left/right)
void continuePreviousMovement() {
    if (robotState != TURN_LEFT && robotState != TURN_RIGHT) {
        stopMovement();
        if (previousState == TURN_RIGHT) {
            turnRight();
        }
        else {
            turnLeft();
        }
        robotState = previousState;
    }
}

// Ensure robot is stopped if inactive
void ensureStopped() {
    if (robotState != 0) {
        stopMovement();
        robotState = STOP;
    }
}

void stopMovement() {
    digitalWrite(LEFT_FORWARD, LOW);
    digitalWrite(RIGHT_FORWARD, LOW);
    digitalWrite(LEFT_BACKWARD, LOW);
    digitalWrite(RIGHT_BACKWARD, LOW);
    Serial.println("STOPPING");
}

void moveForward() {
    if (robotState != FORWARD) {
        Serial.println("Moving Forward");
        stopMovement();
        digitalWrite(LEFT_FORWARD, HIGH);
        digitalWrite(RIGHT_FORWARD, HIGH);
        robotState = FORWARD;
    }
}

void moveBackward() {
    if (robotState != BACKWARD) {
        Serial.println("Moving Backward");
        stopMovement();
        digitalWrite(LEFT_BACKWARD, HIGH);
        digitalWrite(RIGHT_BACKWARD, HIGH);
        robotState = BACKWARD;
    }
}

void turnRight() {
    if (robotState != TURN_RIGHT) {
        Serial.println("Turning Right");
        stopMovement();
        analogWrite(LEFT_FORWARD, ROTATION_GAIN);
        analogWrite(RIGHT_BACKWARD, ROTATION_GAIN);
        robotState = TURN_RIGHT;
        previousState = TURN_RIGHT;
    }
}

void turnLeft() {
    if (robotState != TURN_LEFT) {
        Serial.println("Turning Left");
        stopMovement();
        analogWrite(LEFT_BACKWARD, ROTATION_GAIN);
        analogWrite(RIGHT_FORWARD, ROTATION_GAIN);
        robotState = TURN_LEFT;
        previousState = TURN_LEFT;
    }
}
