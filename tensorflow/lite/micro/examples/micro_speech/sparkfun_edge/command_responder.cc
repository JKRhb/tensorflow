/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#if defined(ARDUINO) && !defined(ARDUINO_SFE_EDGE)
#define ARDUINO_EXCLUDE_CODE
#endif  // defined(ARDUINO) && !defined(ARDUINO_SFE_EDGE)

#ifndef ARDUINO_EXCLUDE_CODE

#include "tensorflow/lite/micro/examples/micro_speech/command_responder.h"

#include "am_bsp.h"  // NOLINT

// Turns off all of the LEDs on the SparkFun Edge board.
void ResetLeds() {
  am_devices_led_off(am_bsp_psLEDs, AM_BSP_LED_BLUE);
  am_devices_led_off(am_bsp_psLEDs, AM_BSP_LED_RED);
  am_devices_led_off(am_bsp_psLEDs, AM_BSP_LED_YELLOW);
  am_devices_led_off(am_bsp_psLEDs, AM_BSP_LED_GREEN);
}

// Maps the prediction score to an LED color identifier.
//
// The LED identifier is encoded as a 32-bit unsigned
// integer.
//
// Since the possible values of the prediction score range
// from 0 to 255, we devided the number range into four
// quartiles and assigned a color based on the position on
// the SparkFun Edge board. This led us to the following
// mapping:
//
// -   0 –  63 => red
// -  64 – 127 => blue
// - 128 – 191 => green
// - 192 – 255 => yellow
uint32_t DetermineLedColor(uint8_t score) {
  if (score < 64) {
    return AM_BSP_LED_RED;
  }

  if (score < 128) {
    return AM_BSP_LED_BLUE;
  }

  if (score < 192) {
    return AM_BSP_LED_GREEN;
  }

  return AM_BSP_LED_YELLOW;
}

// Turn on LEDs in relation to the prediction score.
//
// The score is mapped to the LEDs in relation to their
// placement on the board, i.e. the red LED represents
// the bottom quartile of the possible prediction scores
// and the yellow LED represents the top quartile.
//
// Since the score is represented by an 8-bit unsigned
// integer, we used the following (inclusive) ranges to
// represent the scores by the four LEDs on the board:
//
// -   0 –  63 => red
// -  64 – 127 => blue
// - 128 – 191 => green
// - 192 – 255 => yellow
//
// See the `DetermineLedColor` function above for the
// concrete implementation.
//
// The appropriate LED will be set, after all LEDs have
// been reset via the `ResetLeds` function.
void VisualizePredictionScore(uint8_t score) {
  ResetLeds();

  uint32_t led_color = DetermineLedColor(score);

  am_devices_led_on(am_bsp_psLEDs, led_color);
}

// This implementation will light up the LEDs on the board in response to
// different commands.
void RespondToCommand(tflite::ErrorReporter* error_reporter,
                      int32_t current_time, const char* found_command,
                      uint8_t score, bool is_new_command) {
  static bool is_initialized = false;
  if (!is_initialized) {
    // Setup LED's as outputs
#ifdef AM_BSP_NUM_LEDS
    am_devices_led_array_init(am_bsp_psLEDs, AM_BSP_NUM_LEDS);
    am_devices_led_array_out(am_bsp_psLEDs, AM_BSP_NUM_LEDS, 0x00000000);
#endif
    is_initialized = true;
  }

  VisualizePredictionScore(score);

  // Kept for debugging purposes.
  if (is_new_command) {
    TF_LITE_REPORT_ERROR(error_reporter, "Heard %s (%d) @%dms", found_command,
                         score, current_time);
  }
}

#endif  // ARDUINO_EXCLUDE_CODE
