/*
  Fade

  This example shows how to fade the onboard Raspberry Pi Pico LED

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Fade
*/
#include <Arduino.h>
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
}

void print_bin(uint64_t var) {
  for (uint64_t test = 0x8000000000000000; test; test >>= 1) {
    Serial.write(var & test ? '1' : '0');
  }
}

void print_hex(uint8_t x) {
  if (x==0) {
    Serial.print("00");
    return;
  } else if (x<16) {
    Serial.print("0");
  }
  Serial.print(x, HEX);
}


static const uint8_t gcr_raw_bitlengths[] = {
  /* 0 consecutive bits */ 0,  // not a valid lookup 
  /* 1 consecutive bits */ 1,  // 
  /* 2 consecutive bits */ 1,  // 
  /* 3 consecutive bits */ 1,  // 
  /* 4 consecutive bits */ 2,  // 
  /* 5 consecutive bits */ 2,  // 
  /* 6 consecutive bits */ 2,  // 
  /* 7 consecutive bits */ 3,  // 
  /* 8 consecutive bits */ 3,  // 
  /* 9 consecutive bits */ 3,  // 
  /*10 consecutive bits */ 3,  // 
  /*11 consecutive bits */ 4,  // Not valid, but sometimes occurs at the end of the string
  /*12 consecutive bits */ 4,  // 
  /*13 consecutive bits */ 4,  // 
  // more than 10 consecutive samples, means 4 0 or 1 in a row is invalid in GCR
};

static const uint32_t gcr_raw_set_bits[] = {
  /* 0 consecutive bits */ 0b0000,  // not a valid lookup 
  /* 1 consecutive bits */ 0b0001,  // 
  /* 2 consecutive bits */ 0b0011,  // 
  /* 3 consecutive bits */ 0b0111,  // 
  /* 4 consecutive bits */ 0b1111,  // 
};

uint8_t decodeGCRNibble(uint8_t gcr) {
  // from https://brushlesswhoop.com/dshot-and-bidirectional-dshot/
  switch(gcr)	{
    case 0x19:
      return 0x0;
    case 0x1B:
      return 0x1;
    case 0x12:
      return 0x2;
    case 0x13:
      return 0x3;
    case 0x1D:
      return 0x4;
    case 0x15:
      return 0x5;
    case 0x16:
      return 0x6;
    case 0x17:
      return 0x7;
    case 0x1A:
      return 0x8;
    case 0x09:
      return 0x9;
    case 0x0A:
      return 0xA;
    case 0x0B:
      return 0xB;
    case 0x1E:
      return 0xC;
    case 0x0D:
      return 0xD;
    case 0x0E:
      return 0xE;
    case 0x0F:
      return 0xF;
    default:
      return 0xFF;
  }
}


// todo: change to bool success + result struct or similar
bool decodeRawTelemetry(const uint64_t& raw_telemetry, DShotTelemetry& telemetry) {
  uint16_t result = 0;
  uint32_t gcr_result = 0;
  telemetry.reads++;

  // telemetry data must start with a 0, so if the first bit is high, we don't have any data
  if (raw_telemetry & 0x8000000000000000) {
    //Serial.println(" Tel starts with invalid 1");
    telemetry.errors++;
    return false;  // return invalid telemetry reading
  }

  int consecutive = 1;  // we always start with the MSB
  int current = 0;
  int bits_found = 0;
  // starting at 2nd bit since we know our data starts with a 0
  // 56 samples @ 0.917us sample rate = 51.33us sampled
  // loop the mask from 2nd MSB to  LSB
  for (uint64_t mask = 0x4000000000000000; mask; mask = (mask >> 1)) {
    if (((raw_telemetry & mask) != 0) != current) {  // if the doesn't match the current string of bits, end the current string and flip current
      // bitshift gcr_result by N, and 
      gcr_result = (gcr_result << gcr_raw_bitlengths[consecutive]);
      // then set set N bits in gcr_result, if current is 1
      if (current) gcr_result |= gcr_raw_set_bits[gcr_raw_bitlengths[consecutive]];
      // count bits_found (for debugging)
      bits_found += gcr_raw_bitlengths[consecutive];
      // invert current, and reset consecutive
      current = !current;
      consecutive = 1;  // first bit found in the string is the one we just processed
    } else {  // otherwise incriment consecutive count
      consecutive++;
      if (consecutive >= 14) {
        Serial.println(" Invalid consecutive count of 14");
        // invalid run length at the current sample rate (outside of gcr_raw_bitlengths table)
        telemetry.errors++;
        return false;
      }
    }
  }

  // outside the loop, we still need to account for the final bits if the string ends with 1s
  // bitshift gcr_result by N, and 
  gcr_result = (gcr_result << gcr_raw_bitlengths[consecutive]);
  // then set set N bits in gcr_result, if current is 1
  if (current) gcr_result |= gcr_raw_set_bits[gcr_raw_bitlengths[consecutive]];
  // count bits_found (for debugging)
  bits_found += gcr_raw_bitlengths[consecutive];

  // GCR data should be 21 bits
  if (bits_found < 21) {
    Serial.print(" Too few bits found: "); Serial.println(bits_found);
    telemetry.errors++;
    return false;
  }

  // chop the GCR data down to just the 21 MSB
  gcr_result = gcr_result >> (bits_found - 21); 

  // convert edge transition GCR back to binary GCR
  gcr_result = (gcr_result ^ (gcr_result >> 1));

  // Serial.print("GCR: "); print_bin(gcr_result);

  // break the 20 bit gcr result into 4 quintets for converting back to regular 16 bit binary
  uint8_t quintets[4] = { (uint8_t)(gcr_result >> 15) & (uint8_t)0b11111, 
                          (uint8_t)(gcr_result >> 10) & (uint8_t)0b11111,
                          (uint8_t)(gcr_result >>  5) & (uint8_t)0b11111,
                          (uint8_t)(gcr_result      ) & (uint8_t)0b11111};
  // for(int i=0;i<4;i++) {
  //   Serial.print(" ");
  //   print_hex(quintets[i]);
  // }

  for(int i=0;i<4;i++) {
    uint8_t nibble = decodeGCRNibble(quintets[i]);
    //Serial.print(" ");
    //print_hex(nibble);
    result = (result << 4) | (nibble & 0b1111);
  }

  // check CRC! (4 LSB in result)
  uint8_t result_crc = result & 0b1111;  //extract the telemetry nibble
  result = (result >> 4);  // and then remove it from the result
  uint8_t calc_crc = (~(result ^ (result >> 4) ^ (result >> 8))) & 0x0F;
  if (result_crc != calc_crc) {
    Serial.print(" Expected CRC: 0x");
    print_hex(result_crc);
    Serial.print(" but computed 0x");
    print_hex(calc_crc);
    Serial.println();
    telemetry.errors++;
    return false;
  }

  if (result & 0x100) {  // eRPM value, not extended telemetry. todo: reduce magic numbers here
    telemetry.eRPM_period_us = (uint32_t)(result) & (uint32_t)0b111111111;  // period base is 9 bits above 4 bit CRC
    telemetry.eRPM_period_us = telemetry.eRPM_period_us << (result >> 9);  // period base is the left shift amount stored in the 3 MSB
    // Serial.print(" period: ");
    // Serial.println(telemetry.eRPM_period_us);
    telemetry.rpm = 60000000/(telemetry.eRPM_period_us*10/2);  // TODO: don't hardcode minute_in_us, and pole pairs here
    // Serial.print(" rpm: ");
    // Serial.println(telemetry.rpm);
  } else { // extended telemetry
    uint8_t telemetry_type = (result >> 8);
    uint8_t telemetry_value = (result & 0xFF); // bottom 8 bits
    switch(telemetry_type){
      case 0x02:
        // Serial.print(" T.Temp: "); Serial.print(telemetry_value); Serial.println("C");
        telemetry.temperature_C = telemetry_value;
        break;
      case 0x04:
        telemetry.volts_cV = (uint16_t)telemetry_value*25;
        // Serial.print(" T.Voltage: "); Serial.print((float)telemetry_value/4.0f, 2); Serial.println("V");
        break;
      case 0x06:
        telemetry.amps_A = telemetry_value;
        // Serial.print(" T.Current: "); Serial.print(telemetry_value); Serial.println("A");
        break;
      default: 
        //Serial.print(" T.Other: "); Serial.print(telemetry_type, HEX); Serial.print(" val: "); Serial.println(telemetry_value, HEX);
        break;
    }
  }

  return true;
}

// the loop routine runs over and over again forever:
DShotTelemetry telemetry = {0};
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
  if (dshot.getTelemetry(raw_telemetry)) {
    //Serial.print("Tel: ");
    //print_bin(raw_telemetry);
    decodeRawTelemetry(raw_telemetry, telemetry);
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
    // Serial.println("No telemetry :(");
  }

}
