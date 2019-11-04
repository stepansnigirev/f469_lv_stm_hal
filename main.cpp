#include "mbed.h"
#include "lvgl.h"
#include "lv_stm_hal/lv_stm_hal.h"

DigitalOut led(LED4);
Serial pc(SERIAL_TX, SERIAL_RX, 115200);

#define PADDING 30
#define BTN_HEIGHT 80

int main(){
    led = 0;
    wait(0.1);
    printf("Start\r\n");

    lv_init();
    tft_init();
    touchpad_init();

    printf("Init complete\r\n");
    // ms_tick.attach_us(onMillisecondTicker, 1000);

    /* define theme */
    lv_theme_t * th = lv_theme_material_init(210, NULL);
    lv_theme_set_current(th);

    // gui_styles_create();

    // create screen
    lv_obj_t * scr = lv_cont_create(NULL, NULL);
    lv_disp_load_scr(scr);

    /* loading calibration file */
    // int err = gui_calibration_load();
    // if(err < 0){
    //     logit("gui", "calibration required");
    //     // if file is not found
    //     if(err == -3){
    //         /* calibration screen and a callback when done */
    //         tpcal_create(gui_calibration_save);
    //     }
    // }

    // button
    lv_obj_t * obj = lv_btn_create(scr, NULL);
    // lv_obj_set_event_cb(obj, callback);
    lv_obj_set_width(obj, LV_HOR_RES-2*PADDING);
    lv_obj_set_height(obj, BTN_HEIGHT);
    // button label
    lv_obj_t * lbl = lv_label_create(obj, NULL);
    lv_label_set_text(lbl, "Hello world");
    lv_label_set_align(lbl, LV_LABEL_ALIGN_CENTER);
    // alignment
    lv_obj_align(obj, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_set_y(obj, 700);

    while(1){
        HAL_Delay(1);
        lv_tick_inc(1);
        lv_task_handler();
    }
}

#if 0
#include "stm32469i_discovery_lcd.h"
#include "stm32469i_discovery_ts.h"

using namespace std;

DigitalOut led(LED4);

#define HIGH 0
#define LOW  1

Serial pc(SERIAL_TX, SERIAL_RX, 115200);


#define COLOR_BG            0xff192432
#define COLOR_DIM           0xff7f8fa4
#define COLOR_WHITE         0xffffffff
#define COLOR_BTN           0xff506072
#define COLOR_BTN_TOUCHED   0xff405062
#define COLOR_TRANSPARENT   0x00000000

#define SCREEN_WIDTH  480
#define SCREEN_HEIGHT 800

void draw_bitmap(const uint8_t * arr, size_t len, size_t w, uint16_t x0, uint16_t y0, uint16_t scale, bool fill, uint16_t stroke){
    for(int i=0; i<len; i++){
        uint16_t x = (i % w) * 8;
        uint16_t y = i/w;
        for(int j=0; j<8; j++){
            if((arr[i] >> (7-j)) & 0x01){
                if(fill){
                    BSP_LCD_FillRect(x0+x*scale+j*scale-stroke, y0+y*scale-stroke, scale+2*stroke, scale+2*stroke);
                }else{
                    BSP_LCD_DrawRect(x0+x*scale+j*scale-stroke, y0+y*scale-stroke, scale+2*stroke, scale+2*stroke);
                }
            }
        }
    }
}

void draw_logo(uint16_t x0, uint16_t y0, uint16_t scale){
    static const uint8_t specter_r[] = {
        0b00000111, 0b10000000,
        0b00011111, 0b11100000,
        0b00111111, 0b11110000,
        0b01111111, 0b11111000,
        0b01111111, 0b11111000,
        0b01111111, 0b11111000,
        0b11111111, 0b11111100,
        0b11111111, 0b11111100,
        0b11111111, 0b11111100,
        0b11111111, 0b11111100,
        0b11111111, 0b11111100,
        0b11011100, 0b11101100,
        0b10001100, 0b11000100,
    };
    static const uint8_t specter_w[] = {
        0, 0,
        0, 0,
        0, 0,
        0b00001100, 0b00110000,
        0b00011110, 0b01111000,
        0b00011110, 0b01111000,
        0b00011110, 0b01111000,
        0b00001100, 0b00110000,
    };
    static const uint8_t specter_b[] = {
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0b00000110, 0b00011000,
        0b00000110, 0b00011000,
    };
    BSP_LCD_SetTextColor(0xFF000000);
    draw_bitmap(specter_r, sizeof(specter_r), 2, x0, y0, scale, true, (scale+1)/2);
    BSP_LCD_SetTextColor(0xFFD0021B);
    draw_bitmap(specter_r, sizeof(specter_r), 2, x0, y0, scale, true, 0);
    BSP_LCD_SetTextColor(0xFF9C182D);
    draw_bitmap(specter_r, sizeof(specter_r), 2, x0, y0, scale, false, 0);
    BSP_LCD_SetTextColor(0xFFFFFFFF);
    draw_bitmap(specter_w, sizeof(specter_w), 2, x0, y0, scale, true, 0);
    BSP_LCD_SetTextColor(0xFFC2C2C2);
    draw_bitmap(specter_w, sizeof(specter_w), 2, x0, y0, scale, false, 0);
    BSP_LCD_SetTextColor(0xFF4A90E2);
    draw_bitmap(specter_b, sizeof(specter_b), 2, x0, y0, scale, true, 0);
    BSP_LCD_SetTextColor(0xFF356FB2);
    draw_bitmap(specter_b, sizeof(specter_b), 2, x0, y0, scale, false, 0);
}

TS_StateTypeDef touch_data;
uint8_t was_touching = 0;

int main(){
    BSP_TS_Init(480, 800);
    BSP_LCD_Init();
    BSP_LCD_InitEx(LCD_ORIENTATION_PORTRAIT);
    BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER_BACKGROUND, LCD_FB_START_ADDRESS);
    BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER_BACKGROUND);
    BSP_LCD_Clear(COLOR_BG);
    BSP_LCD_SetBackColor(COLOR_BG);

    BSP_LCD_SetFont(&Font20);
    BSP_LCD_SetTextColor(COLOR_DIM);
    BSP_LCD_DisplayStringAt(0, 100, (uint8_t*)"Total balance:", CENTER_MODE);

    BSP_LCD_SetFont(&Font24);
    BSP_LCD_SetTextColor(COLOR_WHITE);
    BSP_LCD_DisplayStringAt(0, 130, (uint8_t*)"3.14159265 tBTC", CENTER_MODE);

    BSP_LCD_SetTextColor(COLOR_BTN);
    BSP_LCD_FillRect(SCREEN_WIDTH/2-100, SCREEN_HEIGHT/2-30, 200, 60);

    BSP_LCD_SetFont(&Font20);
    BSP_LCD_SetTextColor(COLOR_WHITE);
    BSP_LCD_SetBackColor(COLOR_BTN);
    BSP_LCD_DisplayStringAt(0, SCREEN_HEIGHT/2-8, (uint8_t*)"Let's rock!", CENTER_MODE);

    draw_logo(200, 600, 5);

    led = LOW;

    while(1){
        BSP_TS_GetState(&touch_data);
        if(touch_data.touchDetected != was_touching){
            was_touching = touch_data.touchDetected;
            uint32_t color = COLOR_BTN;
            if(touch_data.touchDetected){
                led = HIGH;
                if(
                    (touch_data.touchX[0] > SCREEN_WIDTH/2-100) &
                    (touch_data.touchX[0] < SCREEN_WIDTH/2+100) &
                    (touch_data.touchY[0] > SCREEN_HEIGHT/2-30) &
                    (touch_data.touchY[0] < SCREEN_HEIGHT/2+30)
                ){
                    color = COLOR_BTN_TOUCHED;
                }
            }else{
                led = LOW;                
                color = COLOR_BTN;
            }
            BSP_LCD_SetTextColor(color);
            BSP_LCD_FillRect(SCREEN_WIDTH/2-100, SCREEN_HEIGHT/2-30, 200, 60);
            BSP_LCD_SetTextColor(COLOR_WHITE);
            BSP_LCD_SetBackColor(color);
            BSP_LCD_DisplayStringAt(0, SCREEN_HEIGHT/2-8, (uint8_t*)"Let's rock!", CENTER_MODE);
        }
        HAL_Delay(10);
    }
}
#endif