#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL3Hj5cjXZR"
#define BLYNK_TEMPLATE_NAME "MOTOR"
#define BLYNK_AUTH_TOKEN "Qr304jztD7tDruWL4dx-76qeuWs_w-So"
#define trigPin 23
#define echoPin 22
#define ldrPin 36
#define Bulb_G1 21
#define Bulb_G2 19
#define Bulb_Gate 1
#define Bulb_Stair 3
#define Start 33
#define Stop 25
#define Buzz 15
#define FloatCut 5
#define Connect 17

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>


char ssid[] = "Adminashu";
char pass[] = "7pg(3ta2v#n56;uc2!x";
// Define variables:
long duration;
const unsigned int numReadings = 20;
int distanceVals[numReadings];
int percentVals[5];
unsigned int i = 0;
unsigned int z = 0;
unsigned int level = 0;
unsigned int percent = 0;
unsigned int sensorConnect = 0;
unsigned int Status = 0;
unsigned int Auto = 0;
unsigned int Tank = 5;
unsigned int Run = 0;
unsigned int fCut = 0;
unsigned int Break = 0;
unsigned int error = 0;
unsigned int tankHieght = 122;

int currentLargest;
int currentPercent;
BlynkTimer timer;

TaskHandle_t Task1;
BLYNK_CONNECTED() {
  Blynk.syncAll();
}
void myTimerEvent() {
  Blynk.virtualWrite(V1, level);
  Blynk.virtualWrite(V0, Status);
  if (Status == 1 && Run == 1) {
    Blynk.virtualWrite(V3, 0);  // Run triger off;
    Run = 0;
  }
  if (Status == 0 && Break == 1) {
    Blynk.virtualWrite(V4, 0);  // Break triger off;
    Break = 0;
  }
}
void runMotor() {
  for (int z = 0; z < 10; z++) {
    digitalWrite(Buzz, HIGH);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    digitalWrite(Buzz, LOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  if (Status == 0) {
    digitalWrite(Start, HIGH);
    delay(500);
    digitalWrite(Start, LOW);
    Serial.println("Motor Start");
  }
}
void stopMotor() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(Buzz, HIGH);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    digitalWrite(Buzz, LOW);
    vTaskDelay(400 / portTICK_PERIOD_MS);
  }
  if (Status == 1) {
    digitalWrite(Stop, HIGH);
    delay(1000);
    digitalWrite(Stop, LOW);
    Serial.println("Motor Stop");
  }
}
int getTank(int level) {
  int tank = 0;
  if (level = 100) {
    tank = 6;
  } else if (level >= 92) {
    tank = 5;
  } else if (level >= 75) {
    tank = 4;
  } else if (level >= 50) {
    tank = 3;
  } else if (level >= 25) {
    tank = 2;
  } else if (level >= 10) {
    tank = 1;
  } else if (level >= 1) {
    tank = 0;
    if (Auto == 0) {
      digitalWrite(Buzz, HIGH);
      vTaskDelay(500 / portTICK_PERIOD_MS);
      digitalWrite(Buzz, LOW);
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
  }
  return tank;
}
int getLevel() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(25);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceVals[i] = duration * 0.034 / 2;
  i++;
  if (i >= numReadings) {
    i = 0;

    // if (distanceVals[10] == distanceVals[1]) {
    //   error = 1;
    //   digitalWrite(2, HIGH);
    // } else {
    //   error = 0;
    //   digitalWrite(2, LOW);
    // }
  }
  for (int j = 0; j < numReadings; j++) {
    currentLargest = j == 0 ? max(distanceVals[j], distanceVals[j + 1]) : max(distanceVals[j], currentLargest);
  }
  percent = tankHieght < currentLargest ? 0 : tankHieght - currentLargest;
  percentVals[z] = map(percent, 0, tankHieght - 20, 0, 100);
  z++;
  if (z >= 5) {
    z = 0;
  }
  for (int k = 0; k < 5; k++) {
    currentPercent = k == 0 ? min(percentVals[k], percentVals[k + 1]) : min(percentVals[k], currentPercent);
  }

  delay(50);

  Serial.print("percent = ");
  Serial.println(currentPercent);
  return currentPercent;
}


BLYNK_WRITE(V3) {
  int pinValue3 = param.asInt();
  Run = pinValue3;
}
BLYNK_WRITE(V4) {
  int pinValue4 = param.asInt();
  Break = pinValue4;
}
BLYNK_WRITE(V5) {
  int pinValue5 = param.asInt();
  Auto = pinValue5;
}
BLYNK_WRITE(V6) {
  int pinValue6 = param.asInt();
  digitalWrite(Bulb_G1, pinValue6);
}
BLYNK_WRITE(V7) {
  int pinValue7 = param.asInt();
  digitalWrite(Bulb_G2, pinValue7);
}
BLYNK_WRITE(V8) {
  int pinValue8 = param.asInt();
  digitalWrite(Bulb_Stair, pinValue8);
}
BLYNK_WRITE(V9) {
  int pinValue9 = param.asInt();
  digitalWrite(Bulb_Gate, pinValue9);
}


void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ldrPin, INPUT);
  pinMode(FloatCut, INPUT);
  pinMode(Connect, INPUT);
  pinMode(Bulb_G1, OUTPUT);
  pinMode(Bulb_G2, OUTPUT);
  pinMode(Bulb_Gate, OUTPUT);
  pinMode(Bulb_Stair, OUTPUT);
  pinMode(Start, OUTPUT);
  pinMode(Stop, OUTPUT);
  pinMode(Buzz, OUTPUT);

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    Task1code, /* Task function. */
    "Task1",   /* name of task. */
    10000,     /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &Task1,    /* Task handle to keep track of created task */
    0);
}
//Task1code: connect wifi
void Task1code(void* pvParameters) {
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000L, myTimerEvent);

  for (;;) {
    timer.run();
    if (Blynk.connected()) {
      Blynk.run();
    }

    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void loop() {

  sensorConnect = digitalRead(Connect);
  if (sensorConnect == 1) {
    digitalWrite(Buzz, LOW);
    digitalWrite(2, LOW);
  } else {
    digitalWrite(Buzz, HIGH);
    digitalWrite(2, HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    digitalWrite(Buzz, LOW);
    digitalWrite(2, LOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
  level = getLevel();
  Tank = getTank(level);
  fCut = digitalRead(FloatCut);
  Status = analogRead(ldrPin) >= 550 ? 1 : 0;
  // Serial.println(analogRead(ldrPin));
  // Serial.print(Auto);
  // Serial.print(Status);
  // Serial.print(Tank);
  // Serial.print(fCut);
  // Serial.print(error);
  if ((Run == 1 && Status == 0) || (Auto == 1 && Status == 0 && Tank < 2 && error == 0)) {
    digitalWrite(Stop, LOW);
    runMotor();
  }
  if ((Break == 1 && Status == 1) || (Auto == 1 && Status == 1 && Tank == 5) || (Auto == 0 && Status == 1 && Tank == 5 && error == 0)) {
    stopMotor();
  }
  if ((fCut == 0 && Status == 1) || (Auto == 1 && Status == 0)) {
    digitalWrite(Stop, HIGH);
  } else {
    digitalWrite(Stop, LOW);
  }
  if (Auto == 1) {
    digitalWrite(2, HIGH);
  } else {
    digitalWrite(2, LOW);
  }
}
