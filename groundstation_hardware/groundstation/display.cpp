#include "display.h"
#include "Local.h"

#include "SPI.h"
#include "ILI9341_t3.h"
#include "font_Arial.h"
#include <Wire.h>
#include <Adafruit_FT6206.h>
#include <EEPROM.h>




Adafruit_FT6206 ts = Adafruit_FT6206();
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);


static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];

static void screens_create();

static void tiles_create(lv_obj_t * parent);
static void header_create(lv_obj_t * parent);
static void settings_create(lv_obj_t * parent);
static void window1_create(lv_obj_t * parent);
static void window2_create(lv_obj_t * parent);
static void window3_create(lv_obj_t * parent);
static void window4_create(lv_obj_t * parent);
static void window5_create(lv_obj_t * parent);
static void window6_create(lv_obj_t * parent);

static lv_style_t style_small;
static lv_style_t style_normal;

static lv_obj_t * header;
static lv_obj_t * main;

static lv_obj_t * tileview;

static lv_obj_t * label_time;
static lv_obj_t * label_packages;

static lv_obj_t * window2_label_error;
static lv_obj_t * window2_label_packages;
static lv_obj_t * window2_label_packages_bad;
static lv_obj_t * window2_label_rf_active;

static lv_obj_t * image_compass;

static lv_obj_t * spinbox;

static lv_obj_t * chart;
static lv_chart_series_t * s1;

static lv_obj_t * window2_bar1;
static lv_obj_t * window2_bar2;
static lv_obj_t * window2_bar3;
static lv_obj_t * window2_bar4;
static lv_obj_t * window2_bar5;

static int window = 0;
static int dark = 0;

static Display* dis;

Display::Display(Local &local_ref) : local (local_ref){
  dis = this;
}

LV_IMG_DECLARE(compass_image);

void init_display(){

  lv_init();
    
  tft.begin(); /* TFT init */
  tft.setRotation(1); /* Landscape orientation */
  tft.fillScreen(0);

  ts.begin(40);

  set_brightness();

  lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

  /*Initialize the display*/
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = 320;
  disp_drv.ver_res = 240;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the input device driver*/
  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);             /*Descriptor of a input device driver*/
  indev_drv.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
  indev_drv.read_cb = my_touchpad_read;      /*Set your driver function*/
  lv_indev_drv_register(&indev_drv);         /*Finally register the driver*/
  dark = EEPROM.read(0);
  if (dark) {
    LV_THEME_DEFAULT_INIT(lv_theme_get_color_primary(), lv_theme_get_color_primary(),
    LV_THEME_MATERIAL_FLAG_DARK,
    lv_theme_get_font_small(), lv_theme_get_font_normal(), lv_theme_get_font_subtitle(), lv_theme_get_font_title());
  } else {
    LV_THEME_DEFAULT_INIT(lv_theme_get_color_primary(), lv_theme_get_color_primary(),
    LV_THEME_MATERIAL_FLAG_LIGHT,
    lv_theme_get_font_small(), lv_theme_get_font_normal(), lv_theme_get_font_subtitle(), lv_theme_get_font_title());
  }

  lv_task_create(window_refresher_task, 100, LV_TASK_PRIO_LOW, NULL);
  
  screens_create();  
}

void update_display(){
  lv_task_handler();
}

void set_brightness (int h) {
   const uint8_t gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

  if (h == -1) h = dis->local.brightness;
  
  analogWrite(BL_PIN,gamma8[map(h,0,100,25,255)]);
  if (dis->local.brightness != h) dis->local.set_brightness(h);
}

void update_values(int value){
  
}

static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.writeRect(area->x1, area->y1, w, h, (uint16_t *)color_p);

    lv_disp_flush_ready(disp); /* tell lvgl that flushing is done */
}

static bool my_touchpad_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    static int oldTouchX = 0;
    static int oldTouchY = 0;
    uint16_t touchX, touchY;
    if (ts.touched())
    {   
        // Retrieve a point  
        TS_Point p = ts.getPoint(); 
        touchX = p.y;         // Rotate the co-ordinates
        touchY = p.x;
        touchX = map(touchX,0,320,320,0);
  
         if ((touchX + 5 >= oldTouchX) || (touchX - 5 <= oldTouchX) || (touchY+5 >= oldTouchY) || (touchY-5 <= oldTouchY))
         {            
              oldTouchY = touchY;
              oldTouchX = touchX;
              data->state = LV_INDEV_STATE_PR; 
              data->point.x = touchX;
              data->point.y = touchY;
         
         }
    }else
    {
        data->point.x = oldTouchX;
        data->point.y = oldTouchY;
        data->state =LV_INDEV_STATE_REL;
     }
           
    return 0;
}

//////////////////////
///   REFRESHER    ///
//////////////////////

void window_refresher_task(void * p)
{
  char buf[32];
  snprintf(buf, 32, "%02d:%02d:%02d", dis->local.gps.hour,dis->local.gps.minute,dis->local.gps.second);
  lv_label_set_text(label_time, buf);
    switch(window){
      case 0:
      break;
      case 1:
      break;
      case 2:
        lv_bar_set_value(window2_bar1, dis->local.rf_strength[0], LV_ANIM_ON);
        lv_bar_set_value(window2_bar2, dis->local.rf_strength[1], LV_ANIM_ON);
        lv_bar_set_value(window2_bar3, dis->local.rf_strength[2], LV_ANIM_ON);
        lv_bar_set_value(window2_bar4, dis->local.rf_strength[3], LV_ANIM_ON);
        lv_bar_set_value(window2_bar5, dis->local.rf_strength[4], LV_ANIM_ON);
        
        
        snprintf(buf, 32, "Error %14.2f%%", 10);
        lv_label_set_text(window2_label_error, buf);
      
        snprintf(buf, 32, "Received%12d", 10);
        lv_label_set_text(window2_label_packages, buf);
      
        snprintf(buf, 32, "Corrupt%15d", 10);
        lv_label_set_text(window2_label_packages_bad, buf);
      break;
      case 3: 
        lv_img_set_angle(image_compass, (3600-(int)dis->local.magneto.heading*10));
      break;
    }
}

//////////////////////
/// EVENT HANDLERS ///
//////////////////////

/// HEADER ///

static void home_event(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
      if (window!=0){
        lv_obj_del(main);
        window = 0;
      }else {
        lv_tileview_set_tile_act(tileview, 0, 0, LV_ANIM_ON);
      }    
    }
}

static void setting_event(lv_obj_t * obj, lv_event_t event){
  if (event == LV_EVENT_CLICKED) {
    if(window==0){
      window = -1;
      main = lv_page_create(lv_scr_act(), NULL);
      lv_obj_set_size(main, 320, 200);
      lv_obj_align(main, NULL, LV_ALIGN_CENTER, 0, 20);
      lv_obj_set_click(main, false);
      settings_create(main);     
    }
  }
}

/// TILES ///

inline void window_button_press(){
  main = lv_page_create(lv_scr_act(), NULL);
  lv_obj_set_size(main, 320, 200);
  lv_obj_align(main, NULL, LV_ALIGN_CENTER, 0, 20);
  lv_obj_set_click(main, false);  
}

static void window1_event(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
          window_button_press();
          window = 1;
          window1_create(main);
    }
}

static void window2_event(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
          window_button_press();
          window = 2;
          window2_create(main);
    }
}

static void window3_event(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
          window_button_press();
          window = 3;
          window3_create(main);
    }
}

static void window4_event(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
          window_button_press();
          window = 4;
          window4_create(main);
    }
}

static void window5_event(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
          window_button_press();
          window = 5;
          window5_create(main);
    }
}

static void window6_event(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
          window_button_press();
          window = 6;
          window6_create(main);
    }
}

/// SETTINGS ///

static void slider_event_cb(lv_obj_t * slider, lv_event_t e)
{
  if(e == LV_EVENT_VALUE_CHANGED) {
      if(lv_slider_get_type(slider) == LV_SLIDER_TYPE_NORMAL) {
          static char buf[16];
          lv_snprintf(buf, sizeof(buf), "%d", lv_slider_get_value(slider));
          lv_obj_set_style_local_value_str(slider, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, buf);
          set_brightness(lv_slider_get_value(slider));
      } 
  }
}


static void color_chg_event_cb(lv_obj_t * sw, lv_event_t e)
{
    if(e == LV_EVENT_VALUE_CHANGED) {
        uint32_t flag = LV_THEME_MATERIAL_FLAG_LIGHT;
        if(lv_switch_get_state(sw)) {
          flag = LV_THEME_MATERIAL_FLAG_DARK;
          dark = 1;
        } else dark = 0;
        
        EEPROM.write(0,dark);
        LV_THEME_DEFAULT_INIT(lv_theme_get_color_primary(), lv_theme_get_color_primary(),
                flag,
                lv_theme_get_font_small(), lv_theme_get_font_normal(), lv_theme_get_font_subtitle(), lv_theme_get_font_title());
    }
}

static void lv_spinbox_increment_event_cb(lv_obj_t * btn, lv_event_t e)
{
    if(e == LV_EVENT_SHORT_CLICKED || e == LV_EVENT_LONG_PRESSED_REPEAT) {
        dis->local.increment_time_zone();
        lv_spinbox_increment(spinbox);
    }
}

static void lv_spinbox_decrement_event_cb(lv_obj_t * btn, lv_event_t e)
{
    if(e == LV_EVENT_SHORT_CLICKED || e == LV_EVENT_LONG_PRESSED_REPEAT) {
        dis->local.decrement_time_zone();
        lv_spinbox_decrement(spinbox);
    }
}

/// APP1 ///


/// APP2 ///


/// APP3 ///


/// APP4 ///


/// APP5 ///


/// APP6 ///


/// APP7 ///

//////////////////////
///    SCREENS     ///
//////////////////////

void screens_create(){

    lv_style_init(&style_small);
    lv_style_set_value_align(&style_small, LV_STATE_DEFAULT, LV_ALIGN_OUT_TOP_LEFT);
    lv_style_set_value_ofs_y(&style_small, LV_STATE_DEFAULT, - LV_DPX(10));
    lv_style_set_margin_top(&style_small, LV_STATE_DEFAULT, LV_DPX(30));
    lv_style_set_text_font(&style_small, LV_STATE_DEFAULT, &lv_font_montserrat_12);

    lv_style_init(&style_normal);
    lv_style_set_value_align(&style_normal, LV_STATE_DEFAULT, LV_ALIGN_OUT_TOP_LEFT);
    lv_style_set_value_ofs_y(&style_normal, LV_STATE_DEFAULT, - LV_DPX(10));
    lv_style_set_margin_top(&style_normal, LV_STATE_DEFAULT, LV_DPX(30));
    lv_style_set_text_font(&style_normal, LV_STATE_DEFAULT, &lv_font_montserrat_16);
    
    
    tiles_create(lv_scr_act());
    
    header = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(header, 320, 40);
    lv_obj_align(header, NULL, LV_ALIGN_CENTER, 0, -100);
    lv_obj_set_click(header, false);
    header_create(lv_scr_act());
}

/// HEADER ///

static void header_create(lv_obj_t * parent)
{
    lv_obj_t * btn;
    lv_obj_t * img;
    
    btn = lv_btn_create(parent, NULL);
    lv_theme_apply(btn, LV_THEME_WIN_BTN);
    lv_obj_set_event_cb(btn, setting_event);
    lv_obj_set_size(btn, 100, 40);
    lv_obj_set_pos(btn,110,0);
    label_time = lv_label_create(btn, NULL);
    lv_label_set_text(label_time, "dd:mm:ss");
    lv_obj_align(label_time, NULL, LV_ALIGN_CENTER, 0, 0);

    btn = lv_btn_create(parent, NULL);
    lv_theme_apply(btn, LV_THEME_WIN_BTN);
    lv_obj_set_size(btn, 40, 40);
    lv_obj_set_pos(btn,0,0);
    lv_obj_set_event_cb(btn, home_event);
    img = lv_img_create(btn, NULL);
    lv_obj_set_click(img, false);
    lv_img_set_src(img, LV_SYMBOL_HOME);

    btn = lv_btn_create(parent, NULL);
    lv_theme_apply(btn, LV_THEME_WIN_BTN);
    lv_obj_set_size(btn, 40, 40);
    lv_obj_set_pos(btn,280,0);
    lv_obj_set_click(btn, false);
    img = lv_img_create(btn, NULL);
    lv_img_set_src(img, LV_SYMBOL_BATTERY_FULL);

    btn = lv_btn_create(parent, NULL);
    lv_theme_apply(btn, LV_THEME_WIN_BTN);
    lv_obj_set_size(btn, 40, 40);
    lv_obj_set_pos(btn,250,0);
    lv_obj_set_click(btn, false);
    img = lv_img_create(btn, NULL);
    lv_img_set_src(img, LV_SYMBOL_GPS);

    btn = lv_btn_create(parent, NULL);
    lv_theme_apply(btn, LV_THEME_WIN_BTN);
    lv_obj_set_size(btn, 40, 40);
    lv_obj_set_pos(btn,220,0);
    lv_obj_set_click(btn, false);
    img = lv_img_create(btn, NULL);
    lv_img_set_src(img, LV_SYMBOL_FILE);
}

/// TILES ///

static void tiles_create(lv_obj_t * parent)
{
    static lv_point_t valid_pos[] = {{0,0}, {1, 0}, {2,0}, {3,0}};
    tileview = lv_tileview_create(parent, NULL);
    lv_tileview_set_valid_positions(tileview, valid_pos, 4);
    lv_tileview_set_edge_flash(tileview, false);
    lv_tileview_set_anim_time(tileview, 10);

    //Tile 1
    lv_obj_t * tile1 = lv_obj_create(tileview, NULL);
    lv_obj_set_size(tile1, lv_obj_get_width_fit(tileview), lv_obj_get_height_fit(tileview)-40);
    lv_obj_set_pos(tile1, 0, 40);
    lv_tileview_add_element(tileview, tile1);

    lv_obj_t * btn = lv_obj_create(tile1, NULL);
    lv_obj_set_size(btn, 80,80);
    lv_obj_set_pos(btn, 20, 10);
    lv_obj_t * label = lv_label_create(btn, NULL);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(label, "Btn");
    lv_obj_set_event_cb(btn, window1_event);

    btn = lv_obj_create(tile1, NULL);
    lv_obj_set_size(btn, 80,80);
    lv_obj_set_pos(btn, 120, 10);
    label = lv_label_create(btn, NULL);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(label, "Btn");
    lv_obj_set_event_cb(btn, window2_event);

    btn = lv_obj_create(tile1, NULL);
    lv_obj_set_size(btn, 80,80);
    lv_obj_set_pos(btn, 220, 10);
    label = lv_label_create(btn, NULL);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(label, "Btn");
    lv_obj_set_event_cb(btn, window3_event);

    btn = lv_obj_create(tile1, NULL);
    lv_obj_set_size(btn, 80,80);
    lv_obj_set_pos(btn, 20, 100);
    label = lv_label_create(btn, NULL);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(label, "Btn");
    lv_obj_set_event_cb(btn, window4_event);

    btn = lv_obj_create(tile1, NULL);
    lv_obj_set_size(btn, 80,80);
    lv_obj_set_pos(btn, 120, 100);
    label = lv_label_create(btn, NULL);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(label, "Btn");
    lv_obj_set_event_cb(btn, window5_event);

    btn = lv_obj_create(tile1, NULL);
    lv_obj_set_size(btn, 80,80);
    lv_obj_set_pos(btn, 220, 100);
    label = lv_label_create(btn, NULL);
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(label, "Btn");
    lv_obj_set_event_cb(btn, window6_event);
    
    //Tile 2
    lv_obj_t * tile2 = lv_obj_create(tileview, NULL);
    lv_obj_set_size(tile2, lv_obj_get_width_fit(tileview), lv_obj_get_height_fit(tileview)-40);
    lv_obj_set_pos(tile2, lv_obj_get_width_fit(tileview), 40);
    lv_tileview_add_element(tileview, tile2);

    label = lv_label_create(tile2, NULL);
    lv_label_set_text(label, "Tile 2");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);

    
    lv_obj_t * tile3 = lv_obj_create(tileview, NULL);
    lv_obj_set_size(tile3, lv_obj_get_width_fit(tileview), lv_obj_get_height_fit(tileview)-40);
    lv_obj_set_pos(tile3, 2*lv_obj_get_width_fit(tileview), 40);
    lv_tileview_add_element(tileview, tile3);

    label = lv_label_create(tile3, NULL);
    lv_label_set_text(label, "Tile 3");
    lv_obj_align(label, NULL, LV_ALIGN_CENTER, 0, 0);
   

    /*Tile3: a button*/
    lv_obj_t * tile4 = lv_obj_create(tileview, tile1);
    lv_obj_set_size(tile4, lv_obj_get_width_fit(tileview), lv_obj_get_height_fit(tileview)-40);
    lv_obj_set_pos(tile4, 3*lv_obj_get_width_fit(tileview), 40);
    lv_tileview_add_element(tileview, tile4);

    btn = lv_btn_create(tile4, NULL);
    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_tileview_add_element(tileview, btn);
    label = lv_label_create(btn, NULL);
    lv_label_set_text(label, "No scroll up");
}

/// SETTINGS ///

static void settings_create(lv_obj_t * parent)
{
    lv_obj_t * h; // container
    lv_obj_t * label; //label
    lv_obj_t * slider; //slider
    lv_obj_t * sw;  //switch

    
    lv_page_set_scrl_layout(parent, LV_LAYOUT_PRETTY_TOP); // Automatisch nächste Zeile generieren wenn kein Platz mehr, alles oben fixiert

    lv_disp_size_t disp_size = lv_disp_get_size_category(NULL);
    lv_coord_t grid_w = lv_page_get_width_grid(parent, 1, 1); // Anzahl Vertikale Fenster, Grösse Fenster 
    

    h = lv_cont_create(parent, NULL);
    lv_cont_set_layout(h, LV_LAYOUT_PRETTY_TOP);
    //lv_obj_add_style(h, LV_CONT_PART_MAIN, &style_box);
    lv_obj_set_drag_parent(h, true);

    lv_cont_set_fit2(h, LV_FIT_NONE, LV_FIT_TIGHT); // Horizontal nicht scrollen / Vertikal scrollen möglich
    lv_obj_set_width(h, grid_w); //Breite vom lokalen Menu

    lv_coord_t fit_w = lv_obj_get_width_fit(h);
    slider = lv_slider_create(h, NULL);
    lv_slider_set_value(slider, EEPROM.read(1), LV_ANIM_OFF);
    lv_obj_set_event_cb(slider, slider_event_cb);
    lv_obj_set_width_margin(slider, fit_w);

    /*Use the knobs style value the display the current value in focused state*/
    lv_obj_set_style_local_margin_top(slider, LV_SLIDER_PART_BG, LV_STATE_DEFAULT, LV_DPX(25));
    lv_obj_set_style_local_value_font(slider, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, lv_theme_get_font_small());
    lv_obj_set_style_local_value_ofs_y(slider, LV_SLIDER_PART_KNOB, LV_STATE_FOCUSED, - LV_DPX(25));
    lv_obj_set_style_local_value_opa(slider, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_value_opa(slider, LV_SLIDER_PART_KNOB, LV_STATE_FOCUSED, LV_OPA_COVER);
    lv_obj_set_style_local_transition_time(slider, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, 300);
    lv_obj_set_style_local_transition_prop_5(slider, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, LV_STYLE_VALUE_OFS_Y);
    lv_obj_set_style_local_transition_prop_6(slider, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, LV_STYLE_VALUE_OPA);

    lv_obj_set_click(h, false);

    label = lv_label_create(h, NULL);
    sw = lv_switch_create(h, NULL);
    if (dark) lv_switch_on(sw, LV_ANIM_OFF);
    lv_obj_set_event_cb(sw,color_chg_event_cb);
    lv_label_set_text(label ,"Dark Mode");

    label = lv_label_create(h, NULL);
    lv_switch_create(h, NULL);
    lv_label_set_text(label ,"Always Log to SD"); 

    label = lv_label_create(h, NULL);
    lv_switch_create(h, NULL);
    lv_label_set_text(label ,"Data Reconstruction");
      
    label = lv_label_create(h, NULL);
    lv_label_set_text(label ,"Time GMT");

    static lv_style_t style_no_border;
    lv_style_init(&style_no_border);
    
    lv_obj_t * con = lv_obj_create(h, NULL);
    lv_obj_set_size(con, 140,40);
    lv_obj_set_click(con, false);
    
    
    lv_style_set_border_opa(&style_no_border, LV_STATE_DEFAULT, LV_OPA_0);
    lv_obj_add_style(con,LV_CONT_PART_MAIN,&style_no_border);
    lv_obj_t * btn = lv_btn_create(con, NULL);
    spinbox = lv_spinbox_create(con, NULL);
    
    lv_spinbox_set_range(spinbox, -12,12);
    lv_spinbox_set_digit_format(spinbox, 2, 0);
    lv_obj_set_width(spinbox, 50);
    lv_obj_align(spinbox, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_coord_t a = lv_obj_get_height(spinbox);
    
    lv_obj_set_size(btn, a, a);
    lv_obj_align(btn, spinbox, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_theme_apply(btn, LV_THEME_SPINBOX_BTN);
    lv_spinbox_set_value(spinbox, dis->local.time_zone);
    lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_PLUS);
    lv_obj_set_event_cb(btn, lv_spinbox_increment_event_cb);

    btn = lv_btn_create(con, btn);
    lv_obj_align(btn, spinbox, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_set_event_cb(btn, lv_spinbox_decrement_event_cb);
    lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_SYMBOL_MINUS);
}

/// APP1 ///

static void window1_create(lv_obj_t * parent)
{
  lv_page_set_scrl_layout(parent, LV_LAYOUT_PRETTY_TOP); // Automatisch nächste Zeile generieren wenn kein Platz mehr, alles oben fixiert

  lv_disp_size_t disp_size = lv_disp_get_size_category(NULL);
  lv_coord_t grid_w = lv_page_get_width_grid(parent, 1, 1); // Anzahl Vertikale Fenster, Grösse Fenster

  lv_obj_t * h = lv_cont_create(parent, NULL);
  lv_cont_set_layout(h, LV_LAYOUT_PRETTY_TOP);
  lv_obj_add_style(h, LV_CONT_PART_MAIN, &style_normal);
  lv_obj_set_drag_parent(h, true);

  lv_cont_set_fit2(h, LV_FIT_NONE, LV_FIT_TIGHT); // Horizontal nicht scrollen / Vertikal scrollen möglich
  lv_obj_set_width(h, grid_w); //Breite vom lokalen Menu
  lv_obj_set_style_local_value_str(h, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "Status");

  char buf[32];
  lv_obj_t * label;
  snprintf(buf, 32, "Address 0x%08X", *parent);
  label = lv_label_create(h, NULL);
  lv_label_set_text(label, buf);
  
}

/// APP2 ///

static void window2_create(lv_obj_t * parent)
{
  lv_page_set_scrl_layout(parent, LV_LAYOUT_PRETTY_TOP); // Automatisch nächste Zeile generieren wenn kein Platz mehr, alles oben fixiert

  lv_disp_size_t disp_size = lv_disp_get_size_category(NULL);
  lv_coord_t grid_w = lv_page_get_width_grid(parent, 2, 1); // Anzahl Vertikale Fenster, Grösse Fenster 
  

  lv_obj_t * h = lv_cont_create(parent, NULL);
  lv_cont_set_layout(h, LV_LAYOUT_PRETTY_TOP);
  lv_obj_add_style(h, LV_CONT_PART_MAIN, &style_normal);
  lv_obj_set_drag_parent(h, true);

  lv_cont_set_fit2(h, LV_FIT_NONE, LV_FIT_TIGHT); // Horizontal nicht scrollen / Vertikal scrollen möglich
  lv_obj_set_width(h, grid_w); //Breite vom lokalen Menu
  lv_obj_set_style_local_value_str(h, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "RF Meter");

  lv_obj_t * label;
   
  window2_bar1 = lv_bar_create(h, NULL);
  lv_obj_set_size(window2_bar1, 100, 10);
  lv_bar_set_anim_time(window2_bar1, 500);
  lv_bar_set_value(window2_bar1, 100, LV_ANIM_ON);

  window2_bar2 = lv_bar_create(h, NULL);
  lv_obj_set_size(window2_bar2, 100, 10);
  lv_bar_set_anim_time(window2_bar2, 500);
  lv_bar_set_value(window2_bar2, 100, LV_ANIM_ON);

  window2_bar3 = lv_bar_create(h, NULL);
  lv_obj_set_size(window2_bar3, 100, 10);
  lv_bar_set_anim_time(window2_bar3, 500);
  lv_bar_set_value(window2_bar3, 100, LV_ANIM_ON);

  window2_bar4 = lv_bar_create(h, NULL);
  lv_obj_set_size(window2_bar4, 100, 10);
  lv_bar_set_anim_time(window2_bar4, 500);
  lv_bar_set_value(window2_bar4, 100, LV_ANIM_ON);

  window2_bar5 = lv_bar_create(h, NULL);
  lv_obj_set_size(window2_bar5, 100, 10);
  lv_bar_set_anim_time(window2_bar5, 500);
  lv_bar_set_value(window2_bar5, 100, LV_ANIM_ON);
   
  h = lv_cont_create(parent, NULL);
  lv_cont_set_layout(h, LV_LAYOUT_GRID);
  
  lv_obj_set_drag_parent(h, true);

  lv_cont_set_fit2(h, LV_FIT_NONE, LV_FIT_TIGHT); // Horizontal nicht scrollen / Vertikal scrollen möglich
  lv_obj_set_width(h, grid_w); //Breite vom lokalen Menu
  lv_obj_set_style_local_value_str(h, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, "Packages");

  lv_obj_add_style(h, LV_CONT_PART_MAIN, &style_small);

  
  label = lv_label_create(h, NULL);
  lv_label_set_text(label, "Logging");

  lv_obj_t * img = lv_img_create(h, NULL);
  lv_obj_set_click(img, false);
  lv_img_set_src(img, LV_SYMBOL_OK);
  
  window2_label_error = lv_label_create(h, NULL);
  lv_label_set_text(window2_label_error, "");

  window2_label_packages = lv_label_create(h, NULL);
  lv_label_set_text(window2_label_packages, "");

  window2_label_packages_bad = lv_label_create(h, NULL);
  lv_label_set_text(window2_label_packages_bad, "");
}

/// APP3 ///

static void window3_create(lv_obj_t * parent)
{
  
  image_compass = lv_img_create(parent, NULL);
  lv_img_set_src(image_compass, &compass_image);
  lv_obj_align(image_compass, NULL, LV_ALIGN_CENTER, 0, 0);

}

/// APP4 ///

static void window4_create(lv_obj_t * parent)
{
  lv_page_set_scrl_layout(parent, LV_LAYOUT_PRETTY_TOP);

  lv_disp_size_t disp_size = lv_disp_get_size_category(NULL);

  lv_coord_t grid_h_chart = lv_page_get_height_grid(parent, 1, 1);
  lv_coord_t grid_w_chart = lv_page_get_width_grid(parent, 1, 1);

  chart = lv_chart_create(parent, NULL);
  //lv_obj_add_style(chart, LV_CHART_PART_BG, &style_box);
  if(disp_size <= LV_DISP_SIZE_SMALL) {
      lv_obj_set_style_local_text_font(chart, LV_CHART_PART_SERIES_BG, LV_STATE_DEFAULT, lv_theme_get_font_small());
  }
  lv_obj_set_drag_parent(chart, true);
  lv_obj_set_width_margin(chart, grid_w_chart);
  lv_obj_set_height_margin(chart, grid_h_chart);
  lv_chart_set_div_line_count(chart, 3, 0);
  lv_chart_set_point_count(chart, 50);
  lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
  s1 = lv_chart_add_series(chart, LV_THEME_DEFAULT_COLOR_PRIMARY);
}

/// APP5 ///

static void window5_create(lv_obj_t * parent)
{
  
}

/// APP6 ///

static void window6_create(lv_obj_t * parent)
{

}

/// APP7 ///
