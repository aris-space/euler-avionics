#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <lvgl.h>

#define BL_PIN 9
#define TFT_DC 41
#define TFT_CS 5
#define TFT_RST 4
#define TFT_MISO 12
#define TFT_MOSI 11
#define TFT_SCLK 13

#define dis_width 320
#define dis_height 240

class Local;

class Display {
 public:
  Display(Local &local_ref);

  uint8_t get_brightness();
  Local &local;

 private:
};

extern void init_display();
extern void update_display();

void set_brightness(int h = -1);
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area,
                   lv_color_t *color_p);
bool my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data);
void window_refresher_task(void *p);

#endif
