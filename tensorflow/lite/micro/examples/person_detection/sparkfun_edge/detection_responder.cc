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

#include "tensorflow/lite/micro/examples/person_detection/detection_responder.h"

#include "am_bsp.h"  // NOLINT

// Constant indicating the difference required for a decision for the person
// or no-person case.
const int16_t CONFIDENCE_THRESHOLD = 32;

// Turns off all LEDs (except the blue one).
void ResetLeds() {
  am_devices_led_off(am_bsp_psLEDs, AM_BSP_LED_YELLOW);
  am_devices_led_off(am_bsp_psLEDs, AM_BSP_LED_GREEN);
  am_devices_led_off(am_bsp_psLEDs, AM_BSP_LED_RED);
}

// Prints out a debug debug header using the two original scores.
void PrintDebugHeader(tflite::ErrorReporter* error_reporter,
                        uint8_t person_score, uint8_t no_person_score) {
  TF_LITE_REPORT_ERROR(error_reporter, "===================================");
  TF_LITE_REPORT_ERROR(error_reporter, "Person score: %d No person score: %d",
                       person_score, no_person_score);
}

// Differentiate between the three possible cases using the confidence threshold:
// If the difference between the two scores is higher than the threshold,
// turn the respective LED on (green for person, yellow for no person).
// If the difference is lower than the confidence threshold, turn on the red LED.
//
// Besides the turning on an LED, the function also prints out debug messages,
// indicating the concrete values using for the computation.
void ProcessPrediction(tflite::ErrorReporter* error_reporter, int16_t prediction) {
  if (prediction > CONFIDENCE_THRESHOLD) {
    TF_LITE_REPORT_ERROR(error_reporter, "Person identified: Flash green LED");
    am_devices_led_on(am_bsp_psLEDs, AM_BSP_LED_GREEN);
  } else if (prediction < -CONFIDENCE_THRESHOLD) {
    TF_LITE_REPORT_ERROR(error_reporter, "No Person identified: Flash yellow LED");
    am_devices_led_on(am_bsp_psLEDs, AM_BSP_LED_YELLOW);
  } else {
    TF_LITE_REPORT_ERROR(error_reporter, "Identification ambigious: Flash red LED");
    am_devices_led_on(am_bsp_psLEDs, AM_BSP_LED_RED);
  }
}

// Create an absolute value from the prediction for debug logging.
int16_t GetAbsolutePrediction(int16_t prediction) {
  return prediction >= 0 ? prediction : -prediction;
}

// Print a debug footer with the confidence value and the confidence threshold.
void PrintDebugFooter(tflite::ErrorReporter* error_reporter, int16_t prediction) {
  int16_t absolute_prediction = GetAbsolutePrediction(prediction);

  TF_LITE_REPORT_ERROR(error_reporter, "Confidence value: %d", absolute_prediction);
  TF_LITE_REPORT_ERROR(error_reporter, "Confidence threshold: %d", CONFIDENCE_THRESHOLD);
}

// This implementation will light up LEDs on the board in response to the
// inference results.
void RespondToDetection(tflite::ErrorReporter* error_reporter,
                        uint8_t person_score, uint8_t no_person_score) {
  static bool is_initialized = false;
  if (!is_initialized) {
    // Setup LED's as outputs.  Leave red LED alone since that's an error
    // indicator for sparkfun_edge in image_provider.
    am_devices_led_init((am_bsp_psLEDs + AM_BSP_LED_BLUE));
    am_devices_led_init((am_bsp_psLEDs + AM_BSP_LED_GREEN));
    am_devices_led_init((am_bsp_psLEDs + AM_BSP_LED_YELLOW));
    is_initialized = true;
  }

  // Toggle the blue LED every time an inference is performed.
  am_devices_led_toggle(am_bsp_psLEDs, AM_BSP_LED_BLUE);

  ResetLeds();

  PrintDebugHeader(error_reporter, person_score, no_person_score);

  // Create a value indicating which of the two classes has the higher confidence level.
  //
  // Positive values indicate a higher confidence for the person case, negative values
  // a higher confidence for the no-person case.
  int16_t prediction = person_score - no_person_score;

  ProcessPrediction(error_reporter, prediction);
  PrintDebugFooter(error_reporter, prediction);
}

#endif  // ARDUINO_EXCLUDE_CODE
