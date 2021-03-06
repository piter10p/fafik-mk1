#include <Thread.h>
#include <StaticThreadController.h>

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

Thread diodeThread = Thread();
Thread diodeChangeStateThread = Thread();
Thread distanceMeterThread = Thread();
Thread speakerThread = Thread();
Thread displayThread = Thread();
Thread servoThread = Thread();
Thread motorThread = Thread();

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

Servo servo;

StaticThreadController<7> controller (&diodeThread, &diodeChangeStateThread, &distanceMeterThread,
  &speakerThread, &displayThread, &servoThread, &motorThread);

bool diodeState = false;
bool forcedDiodeState = false;
bool displayState = false;
int servoState = 0;
int motorState = 0;

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

  diodeThread.setInterval(10);
  diodeThread.onRun(diodeCallback);
  
  diodeChangeStateThread.setInterval(1000);
  diodeChangeStateThread.onRun(diodeChangeStateCallback);

  distanceMeterThread.setInterval(10);
  distanceMeterThread.onRun(distanceMeterCallback);

  speakerThread.setInterval(5000);
  speakerThread.onRun(speakerCallback);

  displayThread.setInterval(1000);
  displayThread.onRun(displayCallback);

  servoThread.setInterval(2000);
  servoThread.onRun(servoCallback);

  motorThread.setInterval(1000);
  motorThread.onRun(motorCallback);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();

  servo.attach(SERVO);
}

void loop()
{
  controller.run();
}

void diodeCallback()
{
  if(diodeState || forcedDiodeState)
    digitalWrite(STATUS_LED, HIGH);
  else
    digitalWrite(STATUS_LED, LOW);
}

void diodeChangeStateCallback()
{
  diodeState = !diodeState;
}

void distanceMeterCallback()
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
 
  long result = pulseIn(ECHO, HIGH);
  result = result / 58;

  if(result <= 10)
    forcedDiodeState = true;
  else
    forcedDiodeState = false;
}

void speakerCallback()
{
  tone(SPEAKER, 1000, 10);
}

void displayCallback()
{
  display.clearDisplay();
  display.setTextSize(5);
  display.setTextColor(WHITE); 
  
  if(displayState)
  {
    display.setCursor(5, 10);
    display.println("O  O");
  }
  else
  {
    display.setCursor(5, 10);
    display.println(">  <");
  }

  display.display();
  displayState = !displayState;
}

void servoCallback()
{
  switch(servoState)
  {
    case 0:
      servo.write(0);
      break;

    case 1:
      servo.write(90);
      break;

    case 2:
      servo.write(180);
      break;
  }

  servoState++;

  if(servoState == 3)
    servoState = 0;
}

void motorCallback()
{
   if(motorState == 0)
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
    motorState = 0;
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
