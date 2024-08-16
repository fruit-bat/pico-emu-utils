/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2021, Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "bsp/board.h"
#include "tusb.h"
#include "hid_host_joy.h"
#include "hid_host_mouse.h"
#include "hid_host_info.h"

//--------------------------------------------------------------------+
// MACRO TYPEDEF CONSTANT ENUM DECLARATION
//--------------------------------------------------------------------+

// If your host terminal support ansi escape code such as TeraTerm
// it can be use to simulate mouse cursor movement within terminal
#define USE_ANSI_ESCAPE   0

#define MAX_REPORT  4

void process_kbd_report(hid_keyboard_report_t const *report, hid_keyboard_report_t const *prev_report);
void process_kbd_mount(uint8_t dev_addr, uint8_t instance);
void process_kbd_unmount(uint8_t dev_addr, uint8_t instance);
void process_mouse_mount(uint8_t dev_addr, uint8_t instance);
void process_mouse_unmount(uint8_t dev_addr, uint8_t instance);
void process_mouse_report(hid_mouse_report_t const * report);

static void process_generic_report(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len);
//--------------------------------------------------------------------+
// HID Keyboard
//--------------------------------------------------------------------+

static void __not_in_flash_func(_process_kbd_report)(hid_keyboard_report_t const *report)
{
  static hid_keyboard_report_t prev_report = { 0, 0, {0} }; // previous report to check key released
  process_kbd_report(report, &prev_report);
  prev_report = *report;
}

void __not_in_flash_func(handle_kbd_report)(tusb_hid_host_info_t* info, const uint8_t* report, uint8_t report_length, uint8_t report_id)
{
  TU_LOG1("HID receive keyboard report\r\n");
  _process_kbd_report((hid_keyboard_report_t*)report);
}

void handle_keyboard_unmount(tusb_hid_host_info_t* info) {
  TU_LOG1("HID keyboard unmount\n");
  // Free up keybouard definitions
  process_kbd_unmount(info->key.elements.dev_addr, info->key.elements.instance);
}

//--------------------------------------------------------------------+
// HID Mouse
//--------------------------------------------------------------------+

void handle_mouse_unmount(tusb_hid_host_info_t* info) {
  TU_LOG1("HID mouse unmount\n");
  // Free up mouse definitions
  tuh_hid_free_simple_mice_for_instance(info->key.elements.dev_addr, info->key.elements.instance);

  process_mouse_unmount(info->key.elements.dev_addr, info->key.elements.instance);
}

void __not_in_flash_func(handle_mouse_report)(tusb_hid_host_info_t* info, const uint8_t* report, uint8_t report_length, uint8_t report_id)
{
  TU_LOG1("HID receive mouse report\r\n");
  tusb_hid_simple_mouse_t* simple_mouse = tuh_hid_get_simple_mouse(
    info->key.elements.dev_addr, 
    info->key.elements.instance, 
    report_id);
    
  if (simple_mouse != NULL) {
    tusb_hid_simple_mouse_process_report(simple_mouse, report, report_length);
    hid_mouse_report_t report;
    report.x = simple_mouse->values.x1;
    report.y = simple_mouse->values.y1;
    report.wheel = simple_mouse->values.w1;
    report.buttons = simple_mouse->values.buttons;
    process_mouse_report(&report);
  }
}

//--------------------------------------------------------------------+
// HID Joystick/Gamepad
//--------------------------------------------------------------------+

void __not_in_flash_func(handle_joystick_report)(tusb_hid_host_info_t* info, const uint8_t* report, uint8_t report_length, uint8_t report_id)
{ 
  TU_LOG1("HID receive joystick report\r\n");
  tusb_hid_simple_joystick_t* simple_joystick = tuh_hid_get_simple_joystick(
    info->key.elements.dev_addr, 
    info->key.elements.instance, 
    report_id);
    
  if (simple_joystick != NULL) {
    tusb_hid_simple_joystick_process_report(simple_joystick, report, report_length);
  }
}

void handle_joystick_unmount(tusb_hid_host_info_t* info) {
  TU_LOG1("HID joystick unmount\n");
  // Free up joystick definitions
  tuh_hid_free_simple_joysticks_for_instance(info->key.elements.dev_addr, info->key.elements.instance);
}

void handle_gamepad_report(tusb_hid_host_info_t* info, const uint8_t* report, uint8_t report_length, uint8_t report_id)
{
  TU_LOG1("HID receive gamepad report ");
  for(int i = 0; i < report_length; ++i) {
    printf("%02x", report[i]);
  }
  printf("\r\n");
}

//--------------------------------------------------------------------+
// TinyUSB Callbacks
//--------------------------------------------------------------------+

// Invoked when device with hid interface is mounted
// Report descriptor is also available for use. tuh_hid_parse_report_descriptor()
// can be used to parse common/simple enough descriptor.
// Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE, it will be skipped
// therefore report_desc = NULL, desc_len = 0
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len)
{
  printf("HID device address = %d, instance = %d is mounted\r\n", dev_addr, instance);

  printf("HID Report Description \r\n");
  for(int i = 0; i < desc_len; ++i) printf("%02X ", desc_report[i]);
  printf("\r\n");

  // Interface protocol (hid_interface_protocol_enum_t)
  const char* protocol_str[] = { "None", "Keyboard", "Mouse" };
  uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

  printf("HID Interface Protocol = %s\r\n", protocol_str[itf_protocol]);

  // By default host stack will use activate boot protocol on supported interface.
  // Therefore for this simple example, we only need to parse generic report descriptor (with built-in parser)
  if ( itf_protocol == HID_ITF_PROTOCOL_NONE || itf_protocol == HID_USAGE_DESKTOP_MOUSE)
  {
    tuh_hid_report_info_t reports[MAX_REPORT];
    uint8_t report_count = tuh_hid_parse_report_descriptor(reports, MAX_REPORT, desc_report, desc_len);
    printf("HID has %u reports \r\n", report_count);
    
    for (uint8_t i = 0; i < report_count; ++i) {
      tuh_hid_report_info_t *report = &reports[i];
      bool has_report_id = report_count > 1 || (report[0].report_id > 0);
      
      printf("HID report usage_page=%d, usage=%d, has_report_id=%d dev_addr=%d instance=%d\n", report->usage_page, report->usage, has_report_id, dev_addr, instance);
      
      if (report->usage_page == HID_USAGE_PAGE_DESKTOP)
      {
        switch (report->usage)
        {
          case HID_USAGE_DESKTOP_KEYBOARD: {
            printf("HID receive keyboard report description dev_addr=%d instance=%d\r\n", dev_addr, instance);
            tuh_hid_allocate_info(dev_addr, instance, has_report_id, &handle_kbd_report, handle_keyboard_unmount);
            process_kbd_mount(dev_addr, instance);
            break;
          }
          case HID_USAGE_DESKTOP_GAMEPAD:
          case HID_USAGE_DESKTOP_JOYSTICK: {
            printf("HID receive joystick report description dev_addr=%d instance=%d\r\n", dev_addr, instance);
            if(tuh_hid_allocate_info(dev_addr, instance, has_report_id, &handle_joystick_report, handle_joystick_unmount)) {
              tuh_hid_joystick_parse_report_descriptor(desc_report, desc_len, dev_addr, instance);
            }
            break;
          }
          case HID_USAGE_DESKTOP_MOUSE: {
            printf("HID receive mouse report description dev_addr=%d instance=%d\r\n", dev_addr, instance);
            if(tuh_hid_allocate_info(dev_addr, instance, has_report_id, &handle_mouse_report, handle_mouse_unmount)) {
              tuh_hid_mouse_parse_report_descriptor(desc_report, desc_len, dev_addr, instance);
              process_mouse_mount(dev_addr, instance);
            }
            break;
          }

          default: {
            TU_LOG1("HID usage unknown usage:%d\r\n", report->usage);
            break;
          }
        }
      }
    }  
  }
  else if ( itf_protocol == HID_ITF_PROTOCOL_KEYBOARD )
  {
     tuh_hid_allocate_info(dev_addr, instance, false, handle_kbd_report, handle_keyboard_unmount);
     process_kbd_mount(dev_addr, instance);
  }
  else if ( itf_protocol == HID_ITF_PROTOCOL_MOUSE )
  {
    tuh_hid_allocate_info(dev_addr, instance, false, handle_mouse_report, handle_mouse_unmount);
    process_mouse_mount(dev_addr, instance);
  }
  
  // request to receive report
  // tuh_hid_report_received_cb() will be invoked when report is available
  if ( !tuh_hid_receive_report(dev_addr, instance) )
  {
    printf("Error: cannot request to receive report\r\n");
  }
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance)
{
  printf("HID device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
  
  // Invoke unmount functions adn free up host info structure 
  tuh_hid_free_info(dev_addr, instance);
}

// Invoked when received report from device via interrupt endpoint
void __not_in_flash_func(tuh_hid_report_received_cb)(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len)
{
  uint8_t const itf_protocol = tuh_hid_interface_protocol(dev_addr, instance);

  switch (itf_protocol)
  {
    // case HID_ITF_PROTOCOL_KEYBOARD:
    //   TU_LOG2("HID receive boot keyboard report\r\n");
    //   _process_kbd_report((hid_keyboard_report_t*)report);
    // break;

    // case HID_ITF_PROTOCOL_MOUSE:
    //   TU_LOG2("HID receive boot mouse report\r\n");
    //   process_mouse_report( (hid_mouse_report_t const*) report );
    // break;

    default:
      TU_LOG2("HID receive generic report\r\n");
      // Generic report requires matching ReportID and contents with previous parsed report info
      process_generic_report(dev_addr, instance, report, len);
    break;
  }

  // continue to request to receive report
  if ( !tuh_hid_receive_report(dev_addr, instance) )
  {
    printf("Error: cannot request to receive report\r\n");
  }
}

//--------------------------------------------------------------------+
// Generic Report
//--------------------------------------------------------------------+
static void __not_in_flash_func(process_generic_report)(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len)
{
  tusb_hid_host_info_t* info = tuh_hid_get_info(dev_addr, instance);
  
  if (info == NULL)
  {
    printf("Couldn't find the host report info for dev_addr=%d instance=%d\r\n", dev_addr, instance);
    return;
  }
  
  uint8_t rpt_id = 0;

  if (info->has_report_id) {
    rpt_id = report[0];
    report++;
    len--;       
  }
  
  info->handler(info, report, len, rpt_id);
}

//--------------------------------------------------------------------+
// XINPUT Gamepad
//--------------------------------------------------------------------+
#ifdef USB_JOYSTICK
usbh_class_driver_t const* usbh_app_driver_get_cb(uint8_t* driver_count) {
    *driver_count = 1;
    return &usbh_xinput_driver;
}

static inline void update_joystate_xinput(uint16_t wButtons, int16_t sThumbLX, int16_t sThumbLY, int16_t sThumbRX, int16_t sThumbRY, uint8_t bLeftTrigger, uint8_t bRightTrigger) {
    uint8_t dpad = wButtons & 0xf;
    if (!dpad) {
        joystate_struct.joy1_x = ((int32_t)sThumbLX + 32768) >> 8;
        joystate_struct.joy1_y = ((-(int32_t)sThumbLY) + 32767) >> 8;
    } else {
        joystate_struct.joy1_x = (dpad & XINPUT_GAMEPAD_DPAD_RIGHT) ? 255 : ((dpad & XINPUT_GAMEPAD_DPAD_LEFT) ? 0 : 127);
        joystate_struct.joy1_y = (dpad & XINPUT_GAMEPAD_DPAD_DOWN) ? 255 : ((dpad & XINPUT_GAMEPAD_DPAD_UP) ? 0 : 127);
    }
    // Analog triggers are mapped to up/down on joystick 1 to emulate throttle/brake for driving games
    if (bLeftTrigger) {
        joystate_struct.joy1_y = 127u + (bLeftTrigger >> 1);
    } else if (bRightTrigger) {
        joystate_struct.joy1_y = 127u - (bRightTrigger >> 1);
    }
    joystate_struct.joy2_x = ((int32_t)sThumbRX + 32768) >> 8;
    joystate_struct.joy2_y = ((-(int32_t)sThumbRY) + 32767) >> 8;
    joystate_struct.button_mask = (~(wButtons >> 12)) << 4;
    /* printf("%04x %04x\n", wButtons, joystate_struct.button_mask); */
}

void tuh_xinput_report_received_cb(uint8_t dev_addr, uint8_t instance, xinputh_interface_t const* xid_itf, uint16_t len)
{
    const xinput_gamepad_t *p = &xid_itf->pad;
    /*
    const char* type_str;
    switch (xid_itf->type)
    {
        case 1: type_str = "Xbox One";          break;
        case 2: type_str = "Xbox 360 Wireless"; break;
        case 3: type_str = "Xbox 360 Wired";    break;
        case 4: type_str = "Xbox OG";           break;
        default: type_str = "Unknown";
    }
    */

    if (xid_itf->connected && xid_itf->new_pad_data) {
        /*
        printf("[%02x, %02x], Type: %s, Buttons %04x, LT: %02x RT: %02x, LX: %d, LY: %d, RX: %d, RY: %d\n",
             dev_addr, instance, type_str, p->wButtons, p->bLeftTrigger, p->bRightTrigger, p->sThumbLX, p->sThumbLY, p->sThumbRX, p->sThumbRY);
        */
        update_joystate_xinput(p->wButtons, p->sThumbLX, p->sThumbLY, p->sThumbRX, p->sThumbRY, p->bLeftTrigger, p->bRightTrigger);
    }
    tuh_xinput_receive_report(dev_addr, instance);
}

void tuh_xinput_mount_cb(uint8_t dev_addr, uint8_t instance, const xinputh_interface_t *xinput_itf)
{
    printf("XINPUT MOUNTED %02x %d\n", dev_addr, instance);
    // If this is a Xbox 360 Wireless controller we need to wait for a connection packet
    // on the in pipe before setting LEDs etc. So just start getting data until a controller is connected.
    if (xinput_itf->type == XBOX360_WIRELESS && xinput_itf->connected == false) {
        tuh_xinput_receive_report(dev_addr, instance);
        return;
    } else if (xinput_itf->type == XBOX360_WIRED) {
        /*
         * Some third-party Xbox 360-style controllers require this message to finish initialization.
         * Idea taken from Linux drivers/input/joystick/xpad.c
         */
        uint8_t dummy[20];
        tusb_control_request_t const request =
        {
            .bmRequestType_bit =
            {
                .recipient = TUSB_REQ_RCPT_INTERFACE,
                .type      = TUSB_REQ_TYPE_VENDOR,
                .direction = TUSB_DIR_IN
            },
            .bRequest = tu_htole16(0x01),
            .wValue   = tu_htole16(0x100),
            .wIndex   = tu_htole16(0x00),
            .wLength  = 20
        };
        tuh_xfer_t xfer =
        {
            .daddr       = dev_addr,
            .ep_addr     = 0,
            .setup       = &request,
            .buffer      = dummy,
            .complete_cb = NULL,
            .user_data   = 0
        };
        tuh_control_xfer(&xfer);
    }
    tuh_xinput_set_led(dev_addr, instance, 0, true);
    tuh_xinput_set_led(dev_addr, instance, 1, true);
    tuh_xinput_set_rumble(dev_addr, instance, 0, 0, true);
    tuh_xinput_receive_report(dev_addr, instance);
}

void tuh_xinput_umount_cb(uint8_t dev_addr, uint8_t instance)
{
    printf("XINPUT UNMOUNTED %02x %d\n", dev_addr, instance);
}
#endif