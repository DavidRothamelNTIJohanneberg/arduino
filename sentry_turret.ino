#include <Servo.h>

Servo servo_180;
Servo servo_360;

#define servo_180_pin 11
#define servo_360_pin 12

#define echoPin 9 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 8

#define ledPin 7

long duration; // variable for the duration of sound wave travel
long distance;
// vars for servo_radar
bool dirc = true;
int radar_degree = 200;
//vars for radar
int distances[] = {0,0,0,0,0,0,0,0,0};
//Vars for servo_turret
bool check = false;
int last_low = 10000;
int last_low_index = 0;
int last_loop_index = 1000;
int turret_degree = 0;
int degree = 0;

void setup() {

  //Servo
  servo_180.attach(servo_180_pin);
  servo_180.write(180);
  
  servo_360.attach(servo_360_pin);
  servo_360.write(180);
  
  delay(10000); //So I can prep everything
  
  // put your setup code here, to run once:
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT
  pinMode(ledPin, OUTPUT);
  
  Serial.begin(9600); // // Serial Communication is starting with 9600 of baudrate speed
  Serial.println("Ultrasonic Sensor HC-SR04 Test"); // print some text in Serial Monitor
  Serial.println("with Arduino UNO R3");
}

void loop() {
  // put your main code here, to run repeatedly:
  scan(); // Scans in a half circle and adds the results to the distances array
  delay(1000);
  servo_turret(); // Rotates the turret or fires depending on the numbers in the distances array
}

void scan() {
  for (int x = 0; x < 9; x++) {//loops through the functions and therefore scans in a half circle
    servo_radar();
    radar(sensor());
  }
  radar_degree = 200;
  servo_180.write(radar_degree);
}

void radar(int dist) {
  //Adds the distance to the current radar direction
  Serial.print("Radar degree: ");
  Serial.println(radar_degree);
  distances[(radar_degree/20) - 1] = dist;
}

void servo_radar() {
  //Rotates 20 degree per step
    radar_degree = radar_degree - 20;
    servo_180.write(radar_degree);
}

int sensor() {
  digitalWrite(trigPin, LOW);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  return distance;
}

void servo_turret() {
  Serial.print("last_low: ");
  Serial.println(last_low);
  
  check = false;
  last_low_index = 0;
  for (int i = 0; i < 9; i++) {//iterates through all the diffrent distamces and find the lowest
    Serial.print("Index: ");
    Serial.print(i);
    Serial.print(", distance: ");
    Serial.println(distances[i]);
    if (distances[i] == 0) {
      Serial.println("Skipped");
      continue;
    }
    if (distances[i] < last_low) {
      last_low = distances[i];
      last_low_index = i;  
    }
  }

  if (last_low_index != last_loop_index){
    check = true;
    last_loop_index = last_low_index;
  }
  
  if (check) {//If it has found a lower number turn the turret in that direction
    //Will turn the servo_360 some degrees
    //if i is 0 it will turn to the left
    Serial.print("Last_low_index: ");
    Serial.println(last_loop_index);
    degree = map(last_loop_index, 0, 8, -90, 90);
    Serial.print("Degree: ");
    Serial.println(degree);
    turret_degree = turret_degree + degree;
    Serial.print("Turret degree: ");
    Serial.println(turret_degree);
    servo_360.write(turret_degree);
  } else {
    Serial.println("FIRE!!");
    digitalWrite(ledPin, HIGH);
    delay(5000);
    digitalWrite(ledPin, LOW);
    last_low = 10000; //Resets what is the lowest value
  }
}