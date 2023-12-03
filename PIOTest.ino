/*
  Fade

  This example shows how to fade the onboard Raspberry Pi Pico LED

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Fade
*/

#include "dshot_encoder.h"

int led = LED_BUILTIN; // the PWM pin the LED is attached to
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

DShotEncoder dshot(6);
bool init_result;

// the setup routine runs once when you press reset:
void setup() {
  // declare pin to be an output:
  pinMode(led, OUTPUT);
  Serial.begin(921600);

  delay(100);

  // gpio_disable_pulls(27);
  init_result = dshot.init();
  
}

void print_bin(uint16_t var) {
  for (uint32_t test = 0x8000; test; test >>= 1) {
    Serial.write(var & test ? '1' : '0');
  }
  Serial.println();
}

void print_bin(uint32_t var) {
  for (uint32_t test = 0x80000000; test; test >>= 1) {
    Serial.write(var & test ? '1' : '0');
  }
  Serial.println();
}

// the loop routine runs over and over again forever:
void loop() {
  
  //if (init_result) Serial.println("Dshot init success");
  //else Serial.println("DShot init failed");
  delay(2);
  //Serial.print("dshot: ");
  //print_bin(dshot.setCommand(1046));
  if (millis()< 3000) dshot.setCommand(0);  // 1046 is the example command
  else dshot.setCommand(1500); // https://github.com/betaflight/betaflight/issues/2879
  delay(1);  // wait for dshot PIO to be done
  uint32_t raw_telemetry[2];
  if (dshot.getTelemetry(raw_telemetry)) {
    Serial.println("Tel");
    Serial.print("b0: ");
    print_bin(raw_telemetry[0]);
    Serial.print("b1: ");
    print_bin(raw_telemetry[1]);
    
  } else {
    // Serial.println("No telemetry :(");
  }

}
