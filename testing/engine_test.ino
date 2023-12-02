#define BUTTON_PIN 12
#define LEFT_BACKWARD 3
#define LEFT_FORWARD 4
#define RIGHT_BACKWARD 5
#define RIGHT_FORWARD 6

int buttonState = 0;
int lastButtonState = 0;
int motorState = 0;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;  // Хз яка саме затримка буде найкращою

void setup() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LEFT_FORWARD, OUTPUT);
    pinMode(LEFT_BACKWARD, OUTPUT);
    pinMode(RIGHT_FORWARD, OUTPUT);
    pinMode(RIGHT_BACKWARD, OUTPUT);
    Serial.begin(9600);  
}

void loop() {
    int reading = digitalRead(BUTTON_PIN);

    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != buttonState) {
            buttonState = reading;

            if (buttonState == HIGH) {
                motorState = (motorState + 1) % 3;
                updateMotors();
            }
        }
    }

    lastButtonState = reading;
}

void updateMotors() {
    digitalWrite(LEFT_FORWARD, LOW);
    digitalWrite(LEFT_BACKWARD, LOW);
    digitalWrite(RIGHT_FORWARD, LOW);
    digitalWrite(RIGHT_BACKWARD, LOW);

    // Як на мене, то це краще реалізувати через свічкейс, але можу написати і варіант через іф стейтменти
    switch (motorState) {
    case 1:  // Обидва движка вперед
        digitalWrite(LEFT_FORWARD, HIGH);
        digitalWrite(RIGHT_FORWARD, HIGH);
        break;

    case 2:  // Обидва движка назад
        digitalWrite(LEFT_BACKWARD, HIGH);
        digitalWrite(RIGHT_BACKWARD, HIGH);
        break;

        // case 0: ініціалізується вже в функції, зупинка всіх моторів
    default:
        break;
    }
}
