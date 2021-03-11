#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Servo.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define STATUS_LED 2
#define TRIG 10
#define ECHO 4
#define SPEAKER 3
#define SERVO 9

#define MIN_1 12
#define MIN_2 11
#define MIN_3 8
#define MIN_4 7
#define MSP_1 6
#define MSP_2 5

#define DISTANCES_SIZE 6
#define SCREEN_WIDTH_DISTANCES_RATIO 21
#define SERVO_DISTANCES_RATIO 30

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

Servo servo;

bool displayState = false;
int servoState = 0;

int distances[DISTANCES_SIZE];

void setup()
{
  pinMode(STATUS_LED, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(SPEAKER, OUTPUT);

  pinMode(MIN_1, OUTPUT);
  pinMode(MIN_2, OUTPUT);
  pinMode(MIN_3, OUTPUT);
  pinMode(MIN_4, OUTPUT);
  pinMode(MSP_1, OUTPUT);
  pinMode(MSP_2, OUTPUT);

  digitalWrite(STATUS_LED, HIGH);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();

  servo.attach(SERVO);
  servo.write(90);

  clearDistances();
  delay(5000);

  digitalWrite(STATUS_LED, LOW);
}

void loop()
{
  writeDisplay();
  scan();
  delay(1000);
}

void writeDisplay()
{
  int startPos = 0;
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);

  String text = String((-abs(servoState) + DISTANCES_SIZE)) + ":" + String(servoState);
  display.setCursor(5, 40);
  display.println(text);

  for(int i = 0; i < DISTANCES_SIZE; i++)
  {
    startPos = i * SCREEN_WIDTH_DISTANCES_RATIO;
    display.writeLine(startPos, distances[i], startPos + SCREEN_WIDTH_DISTANCES_RATIO, distances[i], WHITE);
  }

  display.display();
}

void scan()
{
  int distancesIndex = (-abs(servoState) + DISTANCES_SIZE);
  int pos = distancesIndex * SERVO_DISTANCES_RATIO;
  servo.write(pos);

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
 
  long result = pulseIn(ECHO, HIGH);
  result = result / 58;

  if(result <= 50)
    distances[distancesIndex] = (-result + 50);
  else
    distances[distancesIndex] = 0;

  servoState++;

  if(servoState >= DISTANCES_SIZE)
    servoState = -DISTANCES_SIZE; //possible bug with index out of array
}

void motorCallback()
{
   /*if(motorState == 0)
    setMotor(false, false, false, false, 255, 255);

   if(motorState == 1)
    setMotor(true, false, false, false, 255, 255);

   if(motorState == 2)
    setMotor(false, true, false, false, 150, 150);

   if(motorState == 3)
    setMotor(false, false, true, false, 255, 255);

   if(motorState == 4)
    setMotor(false, false, false, true, 150, 150);

   motorState++;

   if(motorState == 5)
    motorState = 0;*/
}

void setMotor(bool in1, bool in2, bool in3, bool in4, int msp1, int msp2)
{
  if(in1)
    digitalWrite(MIN_1, HIGH);
  else
    digitalWrite(MIN_1, LOW);

  if(in2)
    digitalWrite(MIN_2, HIGH);
  else
    digitalWrite(MIN_2, LOW);

  if(in3)
    digitalWrite(MIN_3, HIGH);
  else
    digitalWrite(MIN_3, LOW);

  if(in4)
    digitalWrite(MIN_4, HIGH);
  else
    digitalWrite(MIN_4, LOW);

  analogWrite(MSP_1, msp1);
  analogWrite(MSP_2, msp2);
}

void clearDistances()
{
  for(int i = 0; i < DISTANCES_SIZE; i++)
    distances[i] = 0;
}
