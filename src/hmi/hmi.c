#include <assert.h>
#include <limits.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <lvgl.h>
#include "hand.h"
#include "keyboard.h"
#include "hmi.h"
#include "mon.h"
#include "analog.h"
#include "flow.h"
#include "speaker.h"
#include "mc_init.h"
#include "spidisp.h"
#include "pwm.h"
#include "cbuf.h"
#include "temperature.h"
#include "ui.h"

#define HMI_EMULATE 0
#define HMI_DEMO 0

#define CBUF_CURRENT_LEN (15)
#define THREAD_STACK_SIZE_LV_TICK_INC (64)   /* Stack size in words, not bytes */
#define THREAD_STACK_SIZE_LV_HANDLER  (1200) /* Stack size in words, not bytes */
#define THREAD_STACK_SIZE_HMI_INFO    (400)   /* Stack size in words, not bytes */

const uint32_t disp_mutex_timeout_ms_k = 100;
static SemaphoreHandle_t disp_mutex = NULL;
TaskHandle_t hmi_info_handle;
static bool mcen = false;
static bool is_calib_enabled = false;
static int32_t max_speed = 0;

static void mcen_color(uint32_t hex_color)
{
    lv_color_t color = lv_color_hex(hex_color);
    lv_style_selector_t style_bar = LV_PART_INDICATOR | LV_STATE_DEFAULT;
    lv_style_selector_t style_text = LV_PART_MAIN | LV_STATE_DEFAULT;
    lv_obj_set_style_bg_color(ui_BarRPM, color, style_bar);
    lv_obj_set_style_text_color(ui_rpmVAR, color, style_text);
    lv_obj_set_style_text_color(ui_rpmUnit, color, style_text);
    lv_obj_set_style_text_color(ui_rpmPercent, color, style_text);
}

static void update_speed(int32_t step)
{
    static int32_t setting = 500;

    if (step != 0)
    {
        setting += step;
        setting = mc_set_speed(setting);
    }

    int32_t actual = mc_get_speed();
    uint32_t percent = actual * 100 / max_speed;
    
    lv_label_set_text_fmt(ui_rpmVAR, "%"LV_PRId32, actual);
    lv_label_set_text_fmt(ui_rpmVARcurrentValue, "%"LV_PRId32, setting);
    lv_label_set_text_fmt(ui_rpmPercent, "(%"LV_PRId32" %%)", percent);
    lv_bar_set_value(ui_BarRPM, actual, LV_ANIM_OFF);

    if(mcen) mcen_color(0x00F641);
    else mcen_color(0xFFFFFF);
}

static void set_uptime(uint32_t ms)
{
    uint32_t seconds = ms / 1000;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    lv_label_set_text_fmt(
        ui_Label_Hour,
        "%02d:%02d:%02d",
        hours,
        minutes % 60,
        seconds % 60);
}

static void emulate(void)
{
    static int32_t setting = 500;
    static int32_t actual = 0;
    static bool enable = false;

    setting += 477;
    actual += 731;

    if (actual % 1000 > 500)
    {
        mcen_color(0x00F641);
    }
    else
    {
        mcen_color(0xFFFFFF);
    }

    uint32_t percent = (actual % max_speed) * 100 / max_speed;
    lv_label_set_text_fmt(ui_rpmVAR, "%"LV_PRId32, actual % max_speed);
    lv_label_set_text_fmt(ui_rpmVARcurrentValue, "%"LV_PRId32, setting % max_speed);
    lv_label_set_text_fmt(ui_rpmPercent, "(%"LV_PRId32" %%)", percent);
    lv_bar_set_value(ui_BarRPM, actual % max_speed, LV_ANIM_OFF);

    static uint16_t fl = 0;
    fl += 57;
    lv_label_set_text_fmt(ui_flowrateVAR, "%"LV_PRId32, fl % 7000);    

    static uint16_t p1 = 0;
    p1 += 43;
    lv_label_set_text_fmt(ui_p1inletVAR, "%"LV_PRId32, p1 % 600);

    static uint16_t p2 = 0;
    p2 += 37;
    lv_label_set_text_fmt(ui_p2outletVAR, "%"LV_PRId32, p2 % 600);

    static uint16_t pd = 0;
    pd += 67;
    lv_label_set_text_fmt(ui_pumpheadVAR, "%"LV_PRId32, pd % 600);

    static uint16_t current = 0;
    current += 23;
    lv_label_set_text_fmt(ui_currentVAR, "%"LV_PRId32, current % 3000);

    static uint16_t power = 0;
    power += 273;
    lv_label_set_text_fmt(ui_PwVAR, "%"LV_PRId32, power % 30000);

    static uint16_t tm = 0;
    tm += 2;
    lv_label_set_text_fmt(ui_tMotorVAR, "%"LV_PRId32, tm % 250);

    static uint16_t t1 = 0;
    t1 += 3;
    lv_label_set_text_fmt(ui_toutVAR, "%"LV_PRId32, t1 % 250);
}

static void demo(void)
{
    mcen_color(0x00F641);
    lv_label_set_text_fmt(ui_rpmVAR, "%"LV_PRId32, 54000);
    lv_label_set_text_fmt(ui_rpmVARcurrentValue, "%"LV_PRId32, 54000);
    lv_label_set_text_fmt(ui_rpmPercent, "(%"LV_PRId32" %%)", 90);
    lv_bar_set_value(ui_BarRPM, 54000 % max_speed, LV_ANIM_OFF);
    lv_label_set_text_fmt(ui_flowrateVAR, "%"LV_PRId32, 3372);
    lv_label_set_text_fmt(ui_p1inletVAR, "%"LV_PRId32, 5);
    lv_label_set_text_fmt(ui_p2outletVAR, "%"LV_PRId32, 242);
    lv_label_set_text_fmt(ui_pumpheadVAR, "%"LV_PRId32, 237);
    lv_label_set_text_fmt(ui_currentVAR, "%"LV_PRId32, 529);
    lv_label_set_text_fmt(ui_PwVAR, "%"LV_PRId32, 12751);
    lv_label_set_text_fmt(ui_tMotorVAR, "%"LV_PRId32, 24104);
    lv_label_set_text_fmt(ui_toutVAR, "32,24");
}

static void lvgl_tick_thread(void* arg)
{
    (void)arg;
    const uint32_t k_delay = 2;

    for (;;)
    {
        lv_tick_inc(k_delay);
        vTaskDelay(k_delay);
    }
}

static void lvgl_handler_thread(void* arg)
{
    (void)arg;
    const uint32_t k_delay = 100;

    /* Must be done while 'tick' is active
     * but before 'timer_handler' first call */
    spidisp_init();
    xTaskNotify(hmi_info_handle, 0, eSetValueWithoutOverwrite);

    for (;;)
    {
        if (xSemaphoreTake(disp_mutex, k_delay) == pdTRUE)
        {
            lv_timer_handler();
            xSemaphoreGive(disp_mutex);
        }

        vTaskDelay(k_delay);
    }
}


static void hmi_info_thread(void* arg)
{
    (void)arg;

    const uint32_t k_delay = 200;
    uint32_t notification = 0;

    max_speed = mc_get_max();

    /* Waiting display initialization */
    xTaskNotifyWait(pdFALSE, ULONG_MAX, &notification, portMAX_DELAY);

    if (xSemaphoreTake(disp_mutex, portMAX_DELAY) == pdTRUE)
    {
        ui_init();
        lv_bar_set_range(ui_BarRPM, 0, max_speed);
        lv_label_set_text(ui_versionVAR, "01.13.93");
        xSemaphoreGive(disp_mutex);
    }

    for (int i = 0; i < SPK_TONE_TOTAL; i++)
    {
        const uint32_t k_gap_ms = 100;
        speaker_set_tone(i, 100);
        vTaskDelay(k_gap_ms);
        speaker_set_tone(i, 0);
        vTaskDelay(k_gap_ms / 4);
    }

    for (uint32_t i = 100; i < 5000; i += 100)
    {
        const uint32_t k_gap_ms = 10;
        speaker_set_freq(i, 100);
        vTaskDelay(k_gap_ms);
    }

    speaker_set_freq(0, 0);

    for (;;)
    {
        int16_t p1 = analog_get(ADC_CH_SEN_PRES_1);
        int16_t p2 = analog_get(ADC_CH_SEN_PRES_2);
        int16_t pd = abs(p1 - p2);

        int32_t vsup = analog_get(ADC_CH_SEN_V_SUP); // mV
        int32_t current = analog_get(ADC_CH_SEN_U_I); // mA
        int32_t power = current * vsup / 1000; // mW
        
        int16_t flow_rot = (int16_t)flow_get_rot();
        int32_t tm = (int32_t)temperature_get(TEMPERATURE_CH_MOT);
        int32_t to = (int32_t)temperature_get(TEMPERATURE_CH_ENV);

        mon_write(MON_RPM, (int)mc_get_speed());
        mon_write(MON_FLOW_ROT, (int)flow_rot);
        mon_write(MON_PD, (int)pd);
        mon_write(MON_P1, (int)p1);
        mon_write(MON_P2, (int)p2);
        mon_write(MON_MC_I, (int)current);
        mon_write(MON_MC_P, (int)power);
        mon_write(MON_U, (int)vsup);
        mon_write(MON_TM, (int)tm);
        mon_write(MON_TO, (int)to);

        if (xSemaphoreTake(disp_mutex, k_delay) == pdTRUE)
        {
#if HMI_EMULATE
            emulate();
#elif HMI_DEMO
            demo();
#else
            update_speed(0);
            lv_label_set_text_fmt(ui_currentVAR, "%"LV_PRId32, current);
            lv_label_set_text_fmt(ui_PwVAR, "%"LV_PRId32, power);
            lv_label_set_text_fmt(ui_tMotorVAR, "%"LV_PRId32, vsup);
            lv_label_set_text_fmt(ui_flowrateVAR, "%"LV_PRId32, flow_rot);

            if(p1 == ANALOG_INVALID_VAL) lv_label_set_text(ui_p1inletVAR, "-");
            else lv_label_set_text_fmt(ui_p1inletVAR, "%"LV_PRId32, p1);

            if(p2 == ANALOG_INVALID_VAL) lv_label_set_text(ui_p2outletVAR, "-");
            else lv_label_set_text_fmt(ui_p2outletVAR, "%"LV_PRId32, p2);

            if(p1 == ANALOG_INVALID_VAL || p2 == ANALOG_INVALID_VAL)
                lv_label_set_text(ui_pumpheadVAR, "-");
            else
                lv_label_set_text_fmt(ui_pumpheadVAR, "%"LV_PRId32, pd);
            
            if(tm == TEMPERATURE_INVALID_VAL && to == TEMPERATURE_INVALID_VAL)
                lv_label_set_text(ui_toutVAR, "-,-");
            else if(tm == TEMPERATURE_INVALID_VAL && to != TEMPERATURE_INVALID_VAL)
                lv_label_set_text_fmt(ui_toutVAR, "-,%"LV_PRId32, to);
            else if(tm != TEMPERATURE_INVALID_VAL && to == TEMPERATURE_INVALID_VAL)
                lv_label_set_text_fmt(ui_toutVAR, "%"LV_PRId32",-", tm);
            else
                lv_label_set_text_fmt(ui_toutVAR, "%"LV_PRId32",%"LV_PRId32, tm, to);
#endif
            set_uptime(xTaskGetTickCount());
            xSemaphoreGive(disp_mutex);
        }

        vTaskDelay(k_delay);
    }
}

void hmi_init(void)
{
    /* LVGL task handler ******************************************************/

    static StackType_t lvgl_handler_stack[THREAD_STACK_SIZE_LV_HANDLER];
    static StaticTask_t lvgl_handler_ctl;

    TaskHandle_t lvgl_handler_handle = xTaskCreateStatic(lvgl_handler_thread,
            "lvgl_handler_thread",
            THREAD_STACK_SIZE_LV_HANDLER,
            NULL,
            tskIDLE_PRIORITY,
            lvgl_handler_stack,
            &lvgl_handler_ctl);

    assert(lvgl_handler_handle != NULL);

    /* LVGL tick incrementer **************************************************/

    static StackType_t lvgl_tick_stack[THREAD_STACK_SIZE_LV_TICK_INC];
    static StaticTask_t lvgl_tick_ctl;

    TaskHandle_t lvgl_tick_handle = xTaskCreateStatic(lvgl_tick_thread,
            "lvgl_tick_thread",
            THREAD_STACK_SIZE_LV_TICK_INC,
            NULL,
            tskIDLE_PRIORITY,
            lvgl_tick_stack,
            &lvgl_tick_ctl);

    assert(lvgl_tick_handle    != NULL);

    /* HMI updater ************************************************************/

    static StackType_t hmi_info_stack[THREAD_STACK_SIZE_HMI_INFO];
    static StaticTask_t hmi_info_ctl;

    hmi_info_handle = xTaskCreateStatic(hmi_info_thread,
            "hmi_info_thread",
            THREAD_STACK_SIZE_HMI_INFO,
            NULL,
            tskIDLE_PRIORITY,
            hmi_info_stack,
            &hmi_info_ctl);

    assert(hmi_info_handle != NULL);

    static StaticSemaphore_t disp_mutex_buf;
    disp_mutex = xSemaphoreCreateMutexStatic(&disp_mutex_buf);
    assert(disp_mutex != NULL);

    /* Keyboard and encoder ***************************************************/
    hand_init();
}

void hmi_kb_cb(uint16_t key, bool state)
{
    const uint8_t level_percent_k = 100;
    const int32_t speed_step_k = 1000;
    static bool is_fan_enabled = true;

    if (state)
    {
        speaker_set_tone(SPK_TONE_MIDDLE, level_percent_k);
    }
    else
    {
        speaker_set_tone(0, 0);
    }

    switch (key)
    {
        case KEYBOARD_CANCEL:
            if(state)
            {
                if(is_fan_enabled)
                {
                    pwm_set(PWMCH_FAN, 0);
                    is_fan_enabled = false;
                }
                else
                {
                    pwm_set(PWMCH_FAN, pwm_level_fan_k);
                    is_fan_enabled = true;
                }
            }
            
            break;

        case KEYBOARD_UP:
            if(state && disp_mutex != NULL)
            {
                if (xSemaphoreTake(disp_mutex, disp_mutex_timeout_ms_k) == pdTRUE)
                {
                    update_speed(speed_step_k);
                    xSemaphoreGive(disp_mutex);
                }
            }
            break;

        case KEYBOARD_MENU:
            if(state)
            {
                if(is_calib_enabled)
                {
                    flow_set_mode(FLOW_MODE_FLOW);
                    is_calib_enabled = false;
                }
                else
                {
                    flow_set_mode(FLOW_MODE_COUNT);
                    is_calib_enabled = true;
                }
            }
            
            break;

        case KEYBOARD_START:
            if (state)
            {
                if (mcen)
                {
                    mcen = false;
                }
                else
                {
                    mcen = true;
                }

                mc_enable(mcen);
            }

            break;

        case KEYBOARD_LEFT:
            if(state)
            {
                mon_write(MON_BTN, -1);
                mon_trig();
            }
            else
            {
                mon_write(MON_BTN, 0);
            }
            
            break;

        case KEYBOARD_DOWN:
            if(state && disp_mutex != NULL)
            {
                if (xSemaphoreTake(disp_mutex, disp_mutex_timeout_ms_k) == pdTRUE)
                {
                    update_speed(-speed_step_k);
                    xSemaphoreGive(disp_mutex);
                }
            }
            
            break;

        case KEYBOARD_RIGHT:
            if(state)
            {
                mon_write(MON_BTN, 1);
                mon_trig();
            }
            else
            {
                mon_write(MON_BTN, 0);
            }
            break;

        default:
            break;
    }
}

void hmi_enc_cb(enum hmi_enc_event ev)
{
    static uint32_t time_prev = 0;

    uint32_t time_diff = xTaskGetTickCount() - time_prev;
    int32_t step = 1 + (500 / time_diff);
    time_prev = xTaskGetTickCount();

    if (xSemaphoreTake(disp_mutex, disp_mutex_timeout_ms_k) == pdTRUE)
    {
        switch (ev)
        {
            case HMI_ENC_CW:
                update_speed(+step);
                break;

            case HMI_ENC_CCW:
                update_speed(-step);
                break;

            default:
                break;
        }

        xSemaphoreGive(disp_mutex);
    }
}
