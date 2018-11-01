#include <Driver.h>

Driver X("A4988");
Driver Y("A4988");

void setup() {
  Serial.begin(9600);

  X.set_step_and_dir_pins(4, 3);
  Y.set_step_and_dir_pins(6, 7);

  X.start();
  Y.start();

  X.set_speed(50);    // Trabalhar com velocidade de 80 SPS
  Y.set_speed(100);
  
  X.set_diameter(29.6); // Measured diameter
  Y.set_diameter(12.85);
  
  X.set_resolution(1);
  Y.set_resolution(1);

  delay(5000);
}

int dist = 10;

void loop() {


  move_together(X, 100, Y, 50, 'r');
  delay(1000);

  move_together(X, -100, Y, -50, 'r');
  delay(1000);
  
  // Goes 4 centimeters absolute with second
  //A4988.move('a', 40);
  //delay(1000);

  // Move both together
  //move_together(DRV8825, -20, A4988, -40, 'r');
  //delay(1000);

  //delay(10000);
}
