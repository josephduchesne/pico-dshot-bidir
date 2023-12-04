#include "dshot/data.h"

// todo: ifdef out all te arduino stuff
#include <Arduino.h>
// todo: move to cpp file
#include "dshot/utilities.h"

namespace DShot {

class Decoder {
public:
    Decoder(unsigned int poles = 14) : poles(poles) {}

private:
    unsigned int poles;

static constexpr const uint8_t gcr_raw_bitlengths[] = {
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
  /*14 consecutive bits */ 5,  // 
  /*15 consecutive bits */ 5,  // 
  /*16 consecutive bits */ 5,  // 
  // more than 10 consecutive samples, means 4 0 or 1 in a row is invalid in GCR
};

static constexpr const uint32_t gcr_raw_set_bits[] = {
  /* 0 consecutive bits */ 0b00000,  // not a valid lookup 
  /* 1 consecutive bits */ 0b00001,  // 
  /* 2 consecutive bits */ 0b00011,  // 
  /* 3 consecutive bits */ 0b00111,  // 
  /* 4 consecutive bits */ 0b01111,  // 
  /* 5 consecutive bits */ 0b11111,  // 
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


public: 
// todo: refacor this whole thing to be multiple functions
bool decodeTelemetry(const uint64_t& raw_telemetry, DShot::Telemetry& telemetry) {
    uint16_t result = 0;
    uint32_t gcr_result = 0;
    telemetry.reads++;

    // telemetry data must start with a 0, so if the first bit is high, we don't have any data
    if (raw_telemetry & 0x8000000000000000) {
    //Serial.println(" Tel starts with invalid 1");
    telemetry.errors++;
    return false;  // return invalid telemetry reading
    }

    //    print_bin(raw_telemetry); Serial.print(" ");

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
        if (consecutive > 16) {
        Serial.println(" Invalid consecutive count of 16");
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

    // todo: properly handle zero eRPM value (min representable eRPM period)
    if (result & 0x100) {  // eRPM value, not extended telemetry. todo: reduce magic numbers here
        telemetry.eRPM_period_us = (uint32_t)(result) & (uint32_t)0b111111111;  // period base is 9 bits above 4 bit CRC
        telemetry.eRPM_period_us = telemetry.eRPM_period_us << (result >> 9);  // period base is the left shift amount stored in the 3 MSB
        // Serial.print(" period: ");
        // Serial.println(telemetry.eRPM_period_us);
        telemetry.rpm = ONE_MINUTE_OF_MICROSECONDS/(telemetry.eRPM_period_us*poles/2);  // /2 due to pole pairs, not poles
        // Serial.print(" rpm: ");
        // Serial.println(telemetry.rpm);
    } else { // extended telemetry
        uint8_t telemetry_type = (result >> 8);
        uint8_t telemetry_value = (result & 0xFF); // bottom 8 bits
        switch(telemetry_type){
            case DSHOT_EXT_TELEMETRY_TEMPERATURE:
            // Serial.print(" T.Temp: "); Serial.print(telemetry_value); Serial.println("C");
            telemetry.temperature_C = telemetry_value;
            break;
            case DSHOT_EXT_TELEMETRY_VOLTAGE:
            telemetry.volts_cV = (uint16_t)telemetry_value*25;  // comes in as 0.25V/bit
            // Serial.print(" T.Voltage: "); Serial.print((float)telemetry_value/4.0f, 2); Serial.println("V");
            break;
            case DSHOT_EXT_TELEMETRY_CURRENT:
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

};  // end class DShot::Decoder
}  // end namespace DShot