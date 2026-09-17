#include <Servo.h>

// ======================================================
// PINLER
// ======================================================

// PWM pinleri:
// 3,5,6,11 -> Servo ile çakışmaz

const uint8_t IN1 = 3;    // Sağ Motor
const uint8_t IN2 = 5;

const uint8_t IN3 = 6;    // Sol Motor
const uint8_t IN4 = 11;

const uint8_t SERVO_PIN = 10;

// ======================================================
// AYARLAR
// ======================================================

const int MAX_SPEED  = 75;
const int ACCEL_STEP = 1;

const int SERVO_STEP = 1;

const uint16_t LOOP_DELAY = 7;

// ======================================================
// MOTOR DURUM
// ======================================================

int currentSpeedL = 0;
int currentSpeedR = 0;

int targetSpeedL = 0;
int targetSpeedR = 0;

// ======================================================
// SERVO DURUM
// ======================================================

int currentServoPos = 90;
int targetServoPos  = 90;

Servo myServo;

// ======================================================
// SERIAL BUFFER
// ======================================================

#define CMD_BUFFER 32

char commandBuffer[CMD_BUFFER];
uint8_t commandIndex = 0;

// ======================================================
// SETUP
// ======================================================

void setup()
{
    Serial.begin(115200);

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    stopMotors();

    myServo.attach(SERVO_PIN);

    myServo.write(currentServoPos);

    Serial.println("SYSTEM READY");
}

// ======================================================
// LOOP
// ======================================================

void loop()
{
    readSerialCommands();

    updateMovement();

    delay(LOOP_DELAY);
}

// ======================================================
// SERIAL KOMUT OKUMA
// ======================================================

void readSerialCommands()
{
    while (Serial.available())
    {
        char c = Serial.read();

        // Satır sonu -> komutu işle
        if (c == '\n' || c == '\r')
        {
            if (commandIndex > 0)
            {
                commandBuffer[commandIndex] = '\0';

                parseCommand(commandBuffer);

                commandIndex = 0;
            }
        }
        else
        {
            // Buffer taşma koruması
            if (commandIndex < CMD_BUFFER - 1)
            {
                commandBuffer[commandIndex++] = c;
            }
        }
    }
}

// ======================================================
// KOMUT AYRIŞTIRMA
// ======================================================

void parseCommand(char* cmd)
{
    Serial.print("CMD: ");
    Serial.println(cmd);

    // --------------------------------------------------
    // HAREKET
    // --------------------------------------------------

    if (strcmp(cmd, "Forward") == 0)
    {
        targetSpeedL =  MAX_SPEED+10;
        targetSpeedR =  MAX_SPEED;
    }

    else if (strcmp(cmd, "Backward") == 0)
    {
        targetSpeedL = -MAX_SPEED;
        targetSpeedR = -MAX_SPEED-10;
    }

    else if (strcmp(cmd, "Left") == 0)
    {
        targetSpeedL = -MAX_SPEED;
        targetSpeedR =  MAX_SPEED;
    }

    else if (strcmp(cmd, "Right") == 0)
    {
        targetSpeedL =  MAX_SPEED;
        targetSpeedR = -MAX_SPEED;
    }

    else if (strcmp(cmd, "Stop") == 0)
    {
        targetSpeedL = 0;
        targetSpeedR = 0;
    }

    // --------------------------------------------------
    // SERVO
    // servo=120
    // --------------------------------------------------

    else if (strncmp(cmd, "servo=", 6) == 0)
    {
        int pos = atoi(&cmd[6]);

        pos = constrain(pos, 0, 180);
 
        targetServoPos = 180 - pos;

        Serial.print("SERVO -> ");
        Serial.println(targetServoPos);
    }
}

// ======================================================
// YUMUŞAK HAREKET
// ======================================================

void updateMovement()
{
    // -------------------------
    // SOL MOTOR
    // -------------------------

    if (currentSpeedL < targetSpeedL)
    {
        currentSpeedL += ACCEL_STEP;

        if (currentSpeedL > targetSpeedL)
            currentSpeedL = targetSpeedL;
    }

    else if (currentSpeedL > targetSpeedL)
    {
        currentSpeedL -= ACCEL_STEP;

        if (currentSpeedL < targetSpeedL)
            currentSpeedL = targetSpeedL;
    }

    // -------------------------
    // SAĞ MOTOR
    // -------------------------

    if (currentSpeedR < targetSpeedR)
    {
        currentSpeedR += ACCEL_STEP;

        if (currentSpeedR > targetSpeedR)
            currentSpeedR = targetSpeedR;
    }

    else if (currentSpeedR > targetSpeedR)
    {
        currentSpeedR -= ACCEL_STEP;

        if (currentSpeedR < targetSpeedR)
            currentSpeedR = targetSpeedR;
    }

    // PWM limit koruması

    currentSpeedL = constrain(currentSpeedL, -255, 255);
    currentSpeedR = constrain(currentSpeedR, -255, 255);

    // -------------------------
    // SERVO
    // -------------------------

    if (currentServoPos < targetServoPos)
    {
        currentServoPos += SERVO_STEP;

        if (currentServoPos > targetServoPos)
            currentServoPos = targetServoPos;
    }

    else if (currentServoPos > targetServoPos)
    {
        currentServoPos -= SERVO_STEP;

        if (currentServoPos < targetServoPos)
            currentServoPos = targetServoPos;
    }

    // -------------------------
    // ÇIKIŞLAR
    // -------------------------

    applyMotors(currentSpeedL, currentSpeedR);

    myServo.write(currentServoPos);
}

// ======================================================
// MOTOR SÜRÜŞ
// ======================================================

void applyMotors(int speedL, int speedR)
{
    // -------------------------
    // SOL MOTOR
    // -------------------------

    if (speedL >= 0)
    {
        analogWrite(IN3, speedL);
        analogWrite(IN4, 0);
    }
    else
    {
        analogWrite(IN3, 0);
        analogWrite(IN4, abs(speedL));
    }

    // -------------------------
    // SAĞ MOTOR
    // -------------------------

    if (speedR >= 0)
    {
        analogWrite(IN1, speedR);
        analogWrite(IN2, 0);
    }
    else
    {
        analogWrite(IN1, 0);
        analogWrite(IN2, abs(speedR));
    }
}

// ======================================================
// ACİL STOP
// ======================================================

void stopMotors()
{
    analogWrite(IN1, 0);
    analogWrite(IN2, 0);

    analogWrite(IN3, 0);
    analogWrite(IN4, 0);
}
