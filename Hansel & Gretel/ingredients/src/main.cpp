#include <Arduino.h>
#include <NewPing.h>

#define flour_right 2
#define flour_wrong 3
#define sugar_right 4
#define sugar_wrong 5
#define ginger_right 6
#define ginger_wrong 7
#define MAX_DISTANCE 5 //THIS IS IN CENTIMETERS



NewPing sonar[6] = { // array of sonar sensor objects
  NewPing(flour_right, flour_right, MAX_DISTANCE),
  NewPing(flour_wrong, flour_wrong, MAX_DISTANCE),
  NewPing(sugar_right, sugar_right, MAX_DISTANCE),
  NewPing(sugar_wrong, sugar_wrong, MAX_DISTANCE),
  NewPing(ginger_right, ginger_right, MAX_DISTANCE),
  NewPing(ginger_wrong, ginger_wrong, MAX_DISTANCE)
};

int door_relay_pin = 8;
int reed_switch = 9;
int distance[6];
int reveal_relay_pin = 10;

void setup() {
  Serial.begin(115200);
  pinMode(door_relay_pin, OUTPUT);
  pinMode(reveal_relay_pin, OUTPUT);
  pinMode(reed_switch, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  updateSonar();
  checkIngredients();
}

// put function definitions here:
void updateSonar() {
  for (int i = 0; i < 6; i++) {
    distance[i] = sonar[i].ping(MAX_DISTANCE) * 0.172; // update distance
    //DISTANCE IS IN MILLIMETERS
    // sonar sensors return 0 if no obstacle is detected
    // change distance to max value if no obstacle is detected
    if (distance[i] == 0)
      distance[i] = MAX_DISTANCE*10; // convert cm to mm
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(distance[i]);
    Serial.println(" mm");
  }
}

void checkIngredients() {
  //if right is detecting object and wrong is not, return true. otherwise return false
  bool flour_correct = (distance[0] < 25) && (distance[1] >= 30);
  bool sugar_correct = (distance[2] < 25) && (distance[3] >= 30);
  bool ginger_correct = (distance[4] < 25) && (distance[5] >= 30);

  if (flour_correct && sugar_correct && ginger_correct) {
    //check reed switch
    digitalWrite(door_relay_pin, HIGH); //turn on door EM
    if (digitalRead(reed_switch) == HIGH) //reed switch not activated
      return;
    else if (digitalRead(reed_switch) == LOW) //reed switch activated
      delay(1000); // wait for 1 second before activating relay
      if (digitalRead(reed_switch) == HIGH) //reed switch not activated anymore, ie door is opened
        return;
      else if (digitalRead(reed_switch) == LOW) //reed switch still activated
        digitalWrite(reveal_relay_pin, HIGH); // activate relay
        delay(5000); // keep relay activated for 5 seconds. currently arbitrary
        digitalWrite(reveal_relay_pin, LOW); // deactivate relay
        digitalWrite(door_relay_pin, LOW); //turn off door EM
  }
  else {
    digitalWrite(door_relay_pin, LOW); //turn off door EM
  }

}