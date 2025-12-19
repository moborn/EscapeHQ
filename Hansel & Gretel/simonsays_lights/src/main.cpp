#include <Arduino.h>

#define length(array) int(sizeof(array)/sizeof(array[0]))


int red_pin = 11;
int blue_pin = 5;
int green_pin = 9;
int yellow_G_pin = 3;
int yellow_R_pin = 4;


float desired_red = 2;
int Vp = 5;
int red_out = (desired_red / Vp) * 255;
int blue_green_out = (3.5 / Vp) * 255;

int easy_order[] = {1,3,2,4};
int hard_order[] = {4,2,3,1,4,2,3};
int colors[4][3] = {
  {255,0,0}, //red
  {0,255,0}, //green
  {0,0,255}, //blue
  {255,255,0} //yellow
};

int easy_rate = 1000; //milliseconds
int hard_rate = 250;
int rpi_control = 10;

void setup() {
  Serial.begin(9600);
  for (int i = 2; i <= 13; i++) {
    pinMode(i, OUTPUT);
  }
  pinMode(rpi_control, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(rpi_control) == LOW) {
    for (int i = 0; i < length(easy_order); i++) {
      int led = easy_order[i];
      //write to pins based on colour
      if (led == 1) {
        digitalWrite(red_pin, red_out);
        Serial.println("red");
      }
      else if (led == 2) {
        digitalWrite(green_pin, blue_green_out);
        Serial.println("green");
      }
      else if (led == 3) {
        digitalWrite(blue_pin, blue_green_out);
        Serial.println("blue");
      }
      else if (led == 4) {
        digitalWrite(yellow_R_pin, red_out);
        Serial.println(colors[i][0]);
        digitalWrite(yellow_G_pin, blue_green_out);
        Serial.println(colors[i][1]);
        Serial.println("yellow");

      }

      delay(easy_rate);
      digitalWrite(red_pin, 0);
      digitalWrite(green_pin, 0);
      digitalWrite(blue_pin, 0);
      digitalWrite(yellow_R_pin, 0);
      digitalWrite(yellow_G_pin, 0);
      delay(easy_rate);
    }
    delay(2000);
  }

  else if (digitalRead(rpi_control) == HIGH) {
    for (int i = 0; i < length(hard_order); i++) {
      int led = hard_order[i];
      if (led == 1) {
        digitalWrite(red_pin, red_out);
        Serial.println("red");
      }
      else if (led == 2) {
        digitalWrite(green_pin, blue_green_out);
        Serial.println("green");
      }
      else if (led == 3) {
        digitalWrite(blue_pin, blue_green_out);
        Serial.println("blue");
      }
      else if (led == 4) {
        digitalWrite(yellow_R_pin, red_out);
        Serial.println(colors[i][0]);
        digitalWrite(yellow_G_pin, blue_green_out);
        Serial.println(colors[i][1]);
        Serial.println("yellow");

      }
      delay(hard_rate);
      digitalWrite(red_pin, 0);
      digitalWrite(green_pin, 0);
      digitalWrite(blue_pin, 0);
      digitalWrite(yellow_R_pin, 0);
      digitalWrite(yellow_G_pin, 0);
      delay(hard_rate);
    }
    delay(2000);
  }
}
