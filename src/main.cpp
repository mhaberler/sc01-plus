//*********************************************************************************************************/
//  WT32-SC01-PLUS template for platform.io
//  created by Frits Jan / productbakery on 11 oktober 2022
//
//
// When working with the squareline editor from lvgl, set the project in
// squareline to:
// - Arduino, with TFT_eSPI (which we cannot use, but will replace with
// LovyanGFX in this main.cpp file)
// - 480 x 320, 16 bit display
//
// Export the template project AND export the UI Files
// You will get a project directory with two directories inside, 'ui' and
// 'libraries' From the libraries directory, copy the lv_conf.h to this projects
// /src/ directory (overwrite the old one) From the ui directory, copy all files
// to this projects src/ui/ directory (you can empty the ui directory first if
// needed) The ui.ino file can/should be deleted because this main.cpp files
// takes over.
//
//*********************************************************************************************************/

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

// SD CARD - SPI PINS
#define SDSPI_HOST_ID SPI3_HOST
#define SD_MISO GPIO_NUM_38
#define SD_MOSI GPIO_NUM_40
#define SD_SCLK GPIO_NUM_39
#define SD_CS GPIO_NUM_41

#define MICRO_SD_IO 41

#include "SD.h"
#include "USB.h"
#include "USBMSC.h"

// SETUP LGFX PARAMETERS FOR WT32-SC01-PLUS
class LGFX : public lgfx::LGFX_Device {

  lgfx::Panel_ST7796 _panel_instance;
  lgfx::Bus_Parallel8 _bus_instance;
  lgfx::Light_PWM _light_instance;
  lgfx::Touch_FT5x06
      _touch_instance; // FT5206, FT5306, FT5406, FT6206, FT6236, FT6336, FT6436

public:
  LGFX(void) {
    {
      auto cfg = _bus_instance.config();

      cfg.freq_write = 20000000;
      cfg.pin_wr = 47; // pin number connecting WR
      cfg.pin_rd = -1; // pin number connecting RD
      cfg.pin_rs = 0;  // Pin number connecting RS(D/C)
      cfg.pin_d0 = 9;  // pin number connecting D0
      cfg.pin_d1 = 46; // pin number connecting D1
      cfg.pin_d2 = 3;  // pin number connecting D2
      cfg.pin_d3 = 8;  // pin number connecting D3
      cfg.pin_d4 = 18; // pin number connecting D4
      cfg.pin_d5 = 17; // pin number connecting D5
      cfg.pin_d6 = 16; // pin number connecting D6
      cfg.pin_d7 = 15; // pin number connecting D7
      // cfg.i2s_port = I2S_NUM_0; // (I2S_NUM_0 or I2S_NUM_1)

      _bus_instance.config(cfg);              // Apply the settings to the bus.
      _panel_instance.setBus(&_bus_instance); // Sets the bus to the panel.
    }

    { // Set display panel control.
      auto cfg = _panel_instance
                     .config(); // Get the structure for display panel settings.

      cfg.pin_cs = -1;   // Pin number to which CS is connected (-1 = disable)
      cfg.pin_rst = 4;   // pin number where RST is connected (-1 = disable)
      cfg.pin_busy = -1; // pin number to which BUSY is connected (-1 = disable)

      // * The following setting values ​​are set to general default values
      // ​​for each panel, and the pin number (-1 = disable) to which BUSY
      // is connected, so please try commenting out any unknown items.

      cfg.memory_width = 320;  // Maximum width supported by driver IC
      cfg.memory_height = 480; // Maximum height supported by driver IC
      cfg.panel_width = 320;   // actual displayable width
      cfg.panel_height = 480;  // actual displayable height
      cfg.offset_x = 0;        // Panel offset in X direction
      cfg.offset_y = 0;        // Panel offset in Y direction
      cfg.offset_rotation = 0; // was 2
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = true; // was false
      cfg.invert = true;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = true; // was false something to do with SD?

      _panel_instance.config(cfg);
    }

    { // Set backlight control. (delete if not necessary)
      auto cfg =
          _light_instance
              .config(); // Get the structure for backlight configuration.

      cfg.pin_bl = 45;     // pin number to which the backlight is connected
      cfg.invert = false;  // true to invert backlight brightness
      cfg.freq = 44100;    // backlight PWM frequency
      cfg.pwm_channel = 0; // PWM channel number to use (7??)

      _light_instance.config(cfg);
      _panel_instance.setLight(
          &_light_instance); // Sets the backlight to the panel.
    }

    //*
    { // Configure settings for touch screen control. (delete if not necessary)
      auto cfg = _touch_instance.config();

      cfg.x_min =
          0; // Minimum X value (raw value) obtained from the touchscreen
      cfg.x_max =
          319; // Maximum X value (raw value) obtained from the touchscreen
      cfg.y_min = 0; // Minimum Y value obtained from touchscreen (raw value)
      cfg.y_max =
          479; // Maximum Y value (raw value) obtained from the touchscreen
      cfg.pin_int = 7; // pin number to which INT is connected
      cfg.bus_shared =
          false; // set true if you are using the same bus as the screen
      cfg.offset_rotation = 0;

      // For I2C connection
      cfg.i2c_port = 0;    // Select I2C to use (0 or 1)
      cfg.i2c_addr = 0x38; // I2C device address number
      cfg.pin_sda = 6;     // pin number where SDA is connected
      cfg.pin_scl = 5;     // pin number to which SCL is connected
      cfg.freq = 400000;   // set I2C clock

      _touch_instance.config(cfg);
      _panel_instance.setTouch(
          &_touch_instance); // Set the touchscreen to the panel.
    }
    //*/
    setPanel(&_panel_instance); // Sets the panel to use.
  }
};

#include "ui/ui.h" // this is the ui generated with lvgl / squareline editor
#include <lvgl.h>

LGFX tft;

#define screenWidth 480
#define screenHeight 320

// lv debugging can be set in lv_conf.h
#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char *buf) {
  log_i(buf);
  Serial.flush();
}
#endif

// create buffer for display
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area,
                   lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
  tft.endWrite();
  lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
void my_touch_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
  uint16_t touchX, touchY;
  bool touched = tft.getTouch(&touchX, &touchY);
  if (!touched) {
    data->state = LV_INDEV_STATE_REL;
  } else {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = touchX;
    data->point.y = touchY;

#if DEBUG_TOUCH != 0
    Serial.print("Data x ");
    log_i(touchX);
    Serial.print("Data y ");
    log_i(touchY);
#endif
  }
}

//************************************************************************************
//  SETUP AND LOOP
//************************************************************************************

void readdir(const char *dir) {
  File root = SD.open(dir);
  while (true) {
    File entry = root.openNextFile();
    if (!entry) {
      break;
    }
    log_i("entry:  %s", entry.name());
    entry.close();
  }
}

USBCDC CDC;

USBMSC MSC;
static uint32_t DISK_SECTOR_COUNT = 0;
static uint16_t DISK_SECTOR_SIZE = 0;

static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t *buffer,
                       uint32_t bufsize) {
  // log_i("MSC WRITE: lba: %lu, offset: %lu, bufsize: %lu\n", lba,
  // offset, bufsize);
  uint32_t secSize = SD.sectorSize();
  if (!secSize) {
    return 0; // disk error
  }
  for (int x = 0; x < bufsize / secSize; x++) {
    uint8_t blkbuffer[secSize];
    memcpy(blkbuffer, (uint8_t *)buffer + secSize * x, secSize);
    if (!SD.writeRAW(blkbuffer, lba + x)) {
      return 0;
    }
  }
  return bufsize;
}

static int32_t onRead(uint32_t lba, uint32_t offset, void *buffer,
                      uint32_t bufsize) {
  // log_i("MSC READ: lba: %lu, offset: %lu, bufsize: %lu\n", lba,
  // offset, bufsize);
  uint32_t secSize = SD.sectorSize();
  if (!secSize) {
    return false; // disk error
  }
  for (int x = 0; x < bufsize / secSize; x++) {
    if (!SD.readRAW((uint8_t *)buffer + (x * secSize), lba + x)) {
      return 0; // outside of volume boundary
    }
  }
  return bufsize;
}

static bool onStartStop(uint8_t power_condition, bool start, bool load_eject) {
  log_i("MSC START/STOP: power: %u, start: %u, eject: %u\n", power_condition,
        start, load_eject);
  return true;
}

static void usbEventCallback(void *arg, esp_event_base_t event_base,
                             int32_t event_id, void *event_data) {
  if (event_base == ARDUINO_USB_EVENTS) {
    arduino_usb_event_data_t *data = (arduino_usb_event_data_t *)event_data;
    switch (event_id) {
    case ARDUINO_USB_STARTED_EVENT:
      log_i("USB PLUGGED");
      break;
    case ARDUINO_USB_STOPPED_EVENT:
      log_i("USB UNPLUGGED");
      break;
    case ARDUINO_USB_SUSPEND_EVENT:
      log_i("USB SUSPENDED: remote_wakeup_en: %u\n",
            data->suspend.remote_wakeup_en);
      break;
    case ARDUINO_USB_RESUME_EVENT:
      log_i("USB RESUMED");
      break;

    default:
      break;
    }
  }
}

void setup() {
  Serial.begin(115200);

  SPI.begin(SD_SCLK, SD_MISO, SD_MOSI);
  SD.begin(SD_CS, SPI, 25000000);
  log_i("SD numSectors %u sectorSize %u", SD.numSectors(), SD.sectorSize());
  readdir("/");

  USB.onEvent(usbEventCallback);
  MSC.vendorID("ESP32");      // max 8 chars
  MSC.productID("USB_MSC");   // max 16 chars
  MSC.productRevision("1.0"); // max 4 chars
  MSC.onStartStop(onStartStop);
  MSC.onRead(onRead);
  MSC.onWrite(onWrite);

  MSC.mediaPresent(true);
  // MSC.isWritable(true);  // true if writable, false if read-only

  DISK_SECTOR_COUNT = SD.numSectors();
  DISK_SECTOR_SIZE = SD.sectorSize();

  MSC.begin(DISK_SECTOR_COUNT, DISK_SECTOR_SIZE);
  // CDC.begin();
  // CDC.setDebugOutput(true);
  USB.begin();

  tft.begin();
  tft.setRotation(3);
  tft.setBrightness(255);

  lv_init();

#if LV_USE_LOG != 0
  lv_log_register_print_cb(
      my_print); /* register print function for debugging */
#endif

  lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touch_read;
  lv_indev_drv_register(&indev_drv);

  // start the UI
  ui_init();
}

void loop() {
  lv_timer_handler();
  delay(5);
}