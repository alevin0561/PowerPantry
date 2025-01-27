#include "driver/gpio.h"

// Define pin numbers for motor control
#define EN1   GPIO_NUM_8
#define STEP1 GPIO_NUM_9
#define DIR1  GPIO_NUM_10
#define EN2   GPIO_NUM_11
#define STEP2 GPIO_NUM_12
#define DIR2  GPIO_NUM_13

// Define constants for motor control
#define STEP_LIMIT 16000
#define MIN_STEP_DELAY 50
#define MAX_STEP_DELAY 200
#define ACCEL_DELAY 1000 // update acceleration at 1kHz
#define START_POS1 -8000
#define START_POS2 8000

void setup()
{
  // Set pin modes for motor control
  gpio_set_direction(STEP1, GPIO_MODE_OUTPUT);
  gpio_set_direction(DIR1, GPIO_MODE_OUTPUT);
  gpio_set_direction(EN1, GPIO_MODE_OUTPUT);
  gpio_set_direction(STEP2, GPIO_MODE_OUTPUT);
  gpio_set_direction(DIR2, GPIO_MODE_OUTPUT);
  gpio_set_direction(EN2, GPIO_MODE_OUTPUT);

  // Enable motors
  gpio_set_level(EN1, 0);
  gpio_set_level(EN2, 0);
  Serial.begin(9600);
}

void step_motors(int n1, int n2)
{
  long step_delay = MAX_STEP_DELAY;
  long t_prev = 0;
  long t_now = 0;

  // Set direction of motors
  gpio_set_level(DIR1, n1 > 0);
  gpio_set_level(DIR2, n2 > 0);

  n1 = abs(n1);
  n2 = abs(n2);
  while(n1 || n2){
    // Step motors
    gpio_set_level(STEP1, n1);
    gpio_set_level(STEP2, n2);
    delayMicroseconds(step_delay);
    gpio_set_level(STEP1, 0);
    gpio_set_level(STEP2, 0);
    delayMicroseconds(step_delay);

    // Decrement step counters
    if(n1 > 0)
      n1--;
    if(n2 > 0)
      n2--;

    // Accelerate if necessary
    t_now = micros();
    if((t_now-t_prev) > ACCEL_DELAY){
      t_prev = t_now;
      if (step_delay > MIN_STEP_DELAY)
        step_delay--;
    }
  }
}

// Initialize positions
int pos1 = START_POS1;
int pos2 = START_POS2;
String data = "0,0";

void loop()
{
  // Print current data
  Serial.println(data);

  // Read new data from serial input
  data = Serial.readStringUntil('\n');
  int comma_idx = data.indexOf(',');
  if (comma_idx > 0) {
    // Parse X and Y values from input
    String x_str = data.substring(0, comma_idx);
    String y_str = data.substring(comma_idx + 1);

    // Map and constrain Y value
    int y = map(y_str.toInt(), 340, 100, 0, 8000);
    y = constrain(y, 0, 8000);

    // Map and constrain X value
    int x = map(x_str.toInt(), 410, 640, 0, 8000);
    x = constrain(x, 0, 8000);

    // Calculate target positions
    int target1 = y - x;
    int target2 = x + y;
    step_motors(target1-pos1, target2-pos2);
    pos1 = target1;
    pos2 = target2;
  }

  // Perform scanning motion
  for (int z = 0; z < 8000; z += 1000) {
    for (int x = 0; x < 8000; x += 1000) {
      int target1 = z - x;
      int target2 = x + z;
      step_motors(target1-pos1, target2-pos2);
      pos1 = target1;
      pos2 = target2;
    }
    for (int x = 8000; x > 0; x -= 1000) {
      int target1 = z - x;
      int target2 = x + z;
      step_motors(target1-pos1, target2-pos2);
      pos1 = target1;
      pos2 = target2;
    }
  }

  // Return home after scanning
  step_motors(START_POS1-pos1, START_POS2-pos2);
  gpio_set_level(EN1, 1);
  gpio_set_level(EN2, 1);
  delay(1000000000);
}
