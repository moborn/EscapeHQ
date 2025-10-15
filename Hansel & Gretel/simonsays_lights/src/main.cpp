#include <Arduino.h>

#define length(array) int(sizeof(array)/sizeof(array[0]))

int R_1 = 13;
int G_1 = 12;
int B_1 = 11;
int R_2 = 10;
int G_2 = 9;
int B_2 = 8;
int R_3 = 7;
int G_3 = 6;
int B_3 = 5;
int R_4 = 4;
int G_4 = 3;
int B_4 = 2;

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
int rpi_control = 0;

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
      digitalWrite(R_1 - (led-1)*3, colors[led-1][0]);
      digitalWrite(G_1 - (led-1)*3, colors[led-1][1]);
      digitalWrite(B_1 - (led-1)*3, colors[led-1][2]);
      delay(easy_rate);
      digitalWrite(R_1 - (led-1)*3, 0);
      digitalWrite(G_1 - (led-1)*3, 0);
      digitalWrite(B_1 - (led-1)*3, 0);
      delay(easy_rate);
    }
    delay(2000);
  }

  else if (digitalRead(rpi_control) == HIGH) {
    for (int i = 0; i < length(hard_order); i++) {
      int led = hard_order[i];
      digitalWrite(R_1 - (led-1)*3, colors[led-1][0]);
      digitalWrite(G_1 - (led-1)*3, colors[led-1][1]);
      digitalWrite(B_1 - (led-1)*3, colors[led-1][2]);
      delay(hard_rate);
      digitalWrite(R_1 - (led-1)*3, 0);
      digitalWrite(G_1 - (led-1)*3, 0);
      digitalWrite(B_1 - (led-1)*3, 0);
      delay(hard_rate);
    }
    delay(2000);
  }
}
