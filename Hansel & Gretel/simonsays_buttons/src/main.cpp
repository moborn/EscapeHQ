#include <Arduino.h>
#define length(array) int(sizeof(array)/sizeof(array[0]))

int Rin = 2;
int Gin = 4;
int Bin = 6;
int Yin = 8;
int rpi_control = 0;
int relay = 12;

int easy_order[] = {1,3,2,4};
int hard_order[] = {4,2,3,1,4,2,3};
int colors[4][3] = {
  {255,0,0}, //red
  {0,255,0}, //green
  {0,0,255}, //blue
  {255,255,0} //yellow
};

bool code_correct = false;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(Rin, INPUT_PULLUP);
  pinMode(Gin, INPUT_PULLUP);
  pinMode(Bin, INPUT_PULLUP);
  pinMode(Yin, INPUT_PULLUP);
  pinMode(rpi_control, INPUT_PULLUP);
  pinMode(relay, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(rpi_control) == LOW){
    //easy order
    //rolling storage of code, if digit is correct, store and move to next. If wrong, reset.
    int code[] = {0,0,0,0};
    int index = 0;
    //
    if (code == easy_order){
      digitalWrite(relay, HIGH);
      code_correct = true;
    }
  }

  else if (digitalRead(rpi_control) == HIGH){
    //hard order
    int code[] = {0,0,0,0,0,0,0};
    int index = 0;

    
    if (code == hard_order){
      digitalWrite(relay, HIGH);
      code_correct = true;
    }
  }

}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}