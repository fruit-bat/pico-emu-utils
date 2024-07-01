#ifndef _TUSB_HID_HOST_MOUSE_H_
#define _TUSB_HID_HOST_MOUSE_H_

#include "hid_host_common.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define HID_MAX_MICE 2

// Very simple representation of a mouse to try and map to
// (and this will be quite tricky enough).
typedef struct {
  int32_t x1;
  int32_t y1;
  int32_t w1;
  uint32_t buttons;
} tusb_hid_simple_mouse_values_t;

// TODO could this be the same as for joystick?
typedef union TU_ATTR_PACKED
{
  uint32_t combined;
  struct TU_ATTR_PACKED
  {
    uint8_t instance     :8;
    uint8_t dev_addr     :8;
    uint8_t report_id    :8;
    uint8_t in_use       :8;
  } elements;
} tusb_hid_simple_mouse_key_t;

// Simple joystick definitions and values
typedef struct {
  tusb_hid_simple_mouse_key_t key;
  uint8_t report_length; // requied report length in bytes
  bool has_values;
  tusb_hid_simple_axis_t axis_x1;
  tusb_hid_simple_axis_t axis_y1;
  tusb_hid_simple_axis_t axis_w1;
  tusb_hid_simple_buttons_t buttons;
  tusb_hid_simple_mouse_values_t values;
  uint32_t updated;
} tusb_hid_simple_mouse_t;

#ifdef __cplusplus
}
#endif

#endif /* _TUSB_HID_HOST_MOUSE_H_ */
