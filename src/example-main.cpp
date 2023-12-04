/*
  Fade

  This example shows how to fade the onboard Raspberry Pi Pico LED

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Fade
*/
#include <Arduino.h>
#include "dshot/esc.h"

// todo: move pole pair to 2nd argument
DShot::ESC dshot(6, pio0, DShot::Type::Bidir, DShot::Speed::DS300, 10);

void setup() {
  Serial.begin(921600);

  dshot.init();
}

// the loop routine runs over and over again forever:
// todo: remove telemetry from here, include in dshot class
DShot::Telemetry telemetry = {0};
void loop() {
  
  
  //if (init_result) Serial.println("Dshot init success");
  //else Serial.println("DShot init failed");
  delay(2);
  //Serial.print("dshot: ");
  //print_bin(dshot.setCommand(1046));
  if (millis()< 3000) dshot.setCommand(0);  // 1046 is the example command
  else if (millis() < 4000) dshot.setCommand(13);  // extended telemetry enable
  else dshot.setCommand(1500); // https://github.com/betaflight/betaflight/issues/2879
  delay(1);  // wait for dshot PIO to be done
  uint64_t raw_telemetry;
  if (dshot.getRawTelemetry(raw_telemetry)) {
    //Serial.print("Tel: ");
    //print_bin(raw_telemetry);
    dshot.decodeTelemetry(raw_telemetry, telemetry);
    Serial.print(telemetry.rpm);
    Serial.print(", ");
    Serial.print(telemetry.temperature_C);
    Serial.print(", ");
    Serial.print(telemetry.volts_cV/100); Serial.print("."); Serial.print(telemetry.volts_cV%100);
    Serial.print(", ");
    Serial.print(telemetry.amps_A);
    Serial.print(", ");
    Serial.print((float)telemetry.errors*100.0f/telemetry.reads, 3); Serial.println("");
    if (telemetry.reads % 1000 == 0) {
      telemetry.errors = 0;
      telemetry.reads = 0;
    }
  } else {
    Serial.println("No telemetry :(");
  }

}
