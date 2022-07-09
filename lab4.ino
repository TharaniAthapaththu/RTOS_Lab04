/* include library */
#include <WiFi.h>
#include <ESP32Servo.h>

uint8_t pos = 90;
#define SERVO_PIN 26 // ESP32 pin GIOP26 connected to servo motor
Servo servoMotor;

static const BaseType_t pro_cpu = 0;
static const BaseType_t app_cpu = 1;

TaskHandle_t Task_A;
TaskHandle_t Task_B;


WiFiClient client;
WiFiServer server(80);

/* WIFI settings */
const char* ssid = "pasan99";
const char* password = "pasan@1021";
 
/* data received from application */
String  data = "";
 
/* define L298N or L293D motor control pins */
int leftMotorForward = 2;     /* GPIO2(D4) -> IN3   */
int rightMotorForward = 15;   /* GPIO15(D8) -> IN1  */
int leftMotorBackward = 4;    /* GPIO4(D4) -> IN4   */
int rightMotorBackward = 5;  /* GPIO5(D5) -> IN2  */
 
void TaskA(void *pvParameters) {
  
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED);
  
  Serial.println("WiFi connected");
  Serial.println("IP address:\t ");
  Serial.println(WiFi.localIP());   
 
  xTaskCreatePinnedToCore(TaskB, 
                          "Task B", 
                          10000,
                          NULL,
                          0,
                          &Task_B,
                          1);
 
  

   

  vTaskDelete(Task_A);
}

void TaskB(void *pvParameters) {
   
  server.begin();
  client = server.available();
  if (!client) return;
  while (!client.available()) delay(1);
  String request = client.readStringUntil('\r');
  request.remove(0, 5);
  request.remove(request.length() - 9, 9);
  data = request;
 
 
 
  if (data == "forward") {
    Serial.println("forward");
    digitalWrite(leftMotorForward, HIGH);
    digitalWrite(rightMotorForward, HIGH);
    digitalWrite(leftMotorBackward, LOW);
    digitalWrite(rightMotorBackward, LOW);
  }
 
  else if (data == "backward"){
    Serial.println("backward");
    digitalWrite(leftMotorBackward, HIGH);
    digitalWrite(rightMotorBackward, HIGH);
    digitalWrite(leftMotorForward, LOW);
    digitalWrite(rightMotorForward, LOW);
  }
  
  else if (data == "left") {
    Serial.println("left");
    digitalWrite(leftMotorForward, LOW);
    digitalWrite(rightMotorForward, HIGH);
    digitalWrite(rightMotorBackward, LOW);
    digitalWrite(leftMotorBackward, HIGH);
  }
  /* If the incoming data is "right", run the "TurnRight" function */
  else if (data == "right"){
    Serial.println("right");
    digitalWrite(leftMotorForward, HIGH);
    digitalWrite(rightMotorForward, LOW);
    digitalWrite(rightMotorBackward, HIGH);
    digitalWrite(leftMotorBackward, LOW);
  }
  /* If the incoming data is "stop", run the "MotorStop" function */
  else if (data == "stop"){
    Serial.println("stop");
    digitalWrite(leftMotorForward, LOW);
    digitalWrite(leftMotorBackward, LOW);
    digitalWrite(rightMotorForward, LOW);
    digitalWrite(rightMotorBackward, LOW);
  }
  else if(data == "left_rotate"){
    Serial.println("left_rotate");
    for (pos = pos; pos <= 180; pos++) {   
      servoMotor.write(pos);
      delay(15);
    }
  }
  else if(data == "right_rotate"){
    Serial.println("right_rotate");
        for (pos = pos; pos >= 0; pos--) {
           servoMotor.write(pos);
           delay(15);
        } 
  }
  
}


void setup()
{
  Serial.begin(115200);
  delay(10);

  pinMode(leftMotorForward, OUTPUT);
  pinMode(rightMotorForward, OUTPUT);
  pinMode(leftMotorBackward, OUTPUT);
  pinMode(rightMotorBackward, OUTPUT);

  servoMotor.attach(SERVO_PIN);
  servoMotor.write(pos);
  
  xTaskCreatePinnedToCore(TaskA, "Task A", 10000, NULL, 1, &Task_A, 0);
   
}
 
void loop()
{
 
}
 
