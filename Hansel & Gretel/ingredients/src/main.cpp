#include <Arduino.h>
#include <NewPing.h>

#define flour_right 2
#define flour_wrong 3
#define sugar_right 4
#define sugar_wrong 5
#define ginger_right 6
#define ginger_wrong 7
#define MAX_DISTANCE 10 //THIS IS IN CENTIMETERS



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
static int calibration_distances[6];
int reveal_relay_pin = 10;
int distance_thresh = 50; //distance threshold in microseconds

void calibrate_distances(){
  Serial.println("Calibrating distances...");
  for (int i = 0; i < 6; i++) {
    // delay(1000); // Wait 1 second between distance readings.
    // Serial.print("Ping: ");
    // Serial.print((float)sonar[i].ping_median(5) * 0.172); // update distance
    // delay(50);

      const int samples = 10;
      unsigned long sum = 0;
      for (int s = 0; s < samples; s++) {
      unsigned int p = sonar[i].ping(MAX_DISTANCE); // returns microseconds
      if (p == 0) p = MAX_DISTANCE * 58; // treat no-echo as max distance (approx 58 µs/cm)
      sum += p;
      delay(50);
      }
      calibration_distances[i] = sum / samples;

    // calibration_distances[i] = (calibration_distances[i] * 0.344 / 2);
  }
  Serial.println("Calibration complete. Distances:");
  for (int i = 0; i < 6; i++) {
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(calibration_distances[i]);
    Serial.println(" us to return");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(door_relay_pin, OUTPUT);
  pinMode(reveal_relay_pin, OUTPUT);
  pinMode(reed_switch, INPUT_PULLUP);
  calibrate_distances();
}

int sample_num = 10;
// put function definitions here:
void updateSonar() {
  for (int i = 0; i < 6; i++) {
    // delay(1000); // Wait 1 second between distance readings.
    // Serial.print("Ping: ");
    // Serial.print((float)sonar[i].ping_median(5) * 0.172); // update distance
    // delay(50);
    // distance[i] = sonar[i].ping_median(sample_num, MAX_DISTANCE);
    const int samples = 10;
    unsigned long sum = 0;
    for (int s = 0; s < samples; s++) {
    unsigned int p = sonar[i].ping(MAX_DISTANCE); // returns microseconds
    if (p == 0) p = MAX_DISTANCE * 58; // treat no-echo as max distance (approx 58 µs/cm)
    sum += p;
    delay(50);
    }
    distance[i] = sum / samples;
    // distance[i] = (distance[i] * 0.344 / 2);
    
    //DISTANCE IS IN MILLIMETERS
    // sonar sensors return 0 if no obstacle is detected
    // change distance to max value if no obstacle is detected
    // if (distance[i] == 0)
    //   distance[i] = MAX_DISTANCE*10; // convert cm to mm
    // if (i == 2 || i == 3) { // only print ginger sensors to reduce serial clutter
    //   Serial.print("Sensor ");
    //   Serial.print(i);
    //   //also print the name of the sensor
    //   switch (i) {
    //     case 0: Serial.print(" (Flour Right)"); break;
    //     case 1: Serial.print(" (Flour Wrong)"); break;
    //     case 2: Serial.print(" (Sugar Right)"); break;
    //     case 3: Serial.print(" (Sugar Wrong)"); break;
    //     case 4: Serial.print(" (Ginger Right)"); break;
    //     case 5: Serial.print(" (Ginger Wrong)"); break;
    //   }
    //   Serial.print(": ");
    //   Serial.print(distance[i]);
    //   Serial.println(" mm");
    // }
    // }
    // delay(50);
  }
}

void checkIngredients() {
  //if right is detecting object and wrong is not, return true. otherwise return false
  bool flour_correct = ((distance[0] >= calibration_distances[0] + distance_thresh || distance[0] <= calibration_distances[0] - distance_thresh)
    && ((calibration_distances[1] - distance_thresh) <= distance[1] 
      && distance[1] <= (calibration_distances[1] + distance_thresh)));
  bool sugar_correct = ((distance[2] >= calibration_distances[2] + distance_thresh || distance[2] <= calibration_distances[2] - distance_thresh)
    && ((calibration_distances[3] - distance_thresh) <= distance[3] 
      && distance[3] <= (calibration_distances[3] + distance_thresh)));
  bool ginger_correct = ((distance[4] >= calibration_distances[4] + distance_thresh || distance[4] <= calibration_distances[4] - distance_thresh)
    && ((calibration_distances[5] - distance_thresh) <= distance[5] 
      && distance[5] <= (calibration_distances[5] + distance_thresh)));


  // if (flour_correct) {
  //   Serial.println("Flour: Correct");
  // }
  // } else {
  //   Serial.println("Flour: Incorrect");
  // }
  Serial.print("Sugar: ");
  Serial.println(sugar_correct);
  Serial.print("Ginger: ");
  Serial.println(ginger_correct);
  Serial.print("Flour: ");
  Serial.println(flour_correct);


  Serial.print("sugar in correct pos: ");
  Serial.println(((distance[2] >= calibration_distances[2] + distance_thresh || distance[2] <= calibration_distances[2] - distance_thresh)));
  Serial.print("sugar not in wrong pos: ");
  Serial.println(((calibration_distances[3] - distance_thresh) <= distance[3] 
      && distance[3] <= (calibration_distances[3] + distance_thresh)));
  Serial.println("sugar correct dist: ");
  Serial.println(distance[2]);
  Serial.println("sugar wrong dist: ");
  Serial.println(distance[3]);
  // } else {
  //   Serial.println("Sugar: Incorrect");
  // }
  // if (ginger_correct) {
  //   Serial.println("Ginger: Correct");
  // }
  // } else {
  //   Serial.println("Ginger: Incorrect");
  // }
  if (flour_correct && sugar_correct && ginger_correct) {
    Serial.println("All ingredients correct! Unlocking door...");
    delay(1000);
    digitalWrite(reveal_relay_pin, HIGH); // activate relay
    delay(5000); // keep relay activated for 5 seconds. currently arbitrary
    digitalWrite(reveal_relay_pin, LOW); // deactivate relay
  }
}




void loop() {
  // put your main code here, to run repeatedly:
  updateSonar();
  checkIngredients();
  // delay(1000);
}
