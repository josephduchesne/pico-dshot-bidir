/**
 * @file example-main.cpp
 * @author Joseph Duchesne <josephgeek@gmail.com
 * @brief 
 * @version 0.9
 * @date 2023-12-04
 * 
 * @copyright Copyright (c) Joseph Duchesne 2023
 * 
 */
#include <Arduino.h>
#include "dshot/esc.h"

// todo: move pole pair to 2nd argument
DShot::ESC dshot(6, pio0, DShot::Type::Bidir, DShot::Speed::DS600, 10);

void setup() {
  Serial.begin(921600);
  dshot.init();
}

// the loop routine runs over and over again forever:
void loop() {
  
  delay(2);
  //Serial.print("dshot: ");
  //print_bin(dshot.setCommand(1046));
  if (millis()< 3000) dshot.setCommand(0);  // 1046 is the example command
  else if (millis() < 4000) dshot.setCommand(13);  // extended telemetry enable
  else dshot.setThrottle(0.25); // https://github.com/betaflight/betaflight/issues/2879
  delay(1);  // wait for dshot PIO to be done
  
  DShot::ESC::processTelemetryQueue();

  DShot::Telemetry& telemetry = dshot.telemetry;
  Serial.printf("%drpm, %dC, %02d.%02dV, %dA %0.3f\n", telemetry.rpm, telemetry.temperature_C, 
                              telemetry.volts_cV/100, telemetry.volts_cV%100, telemetry.amps_A, 
                              (float)telemetry.errors*100.0f/telemetry.reads);

}
