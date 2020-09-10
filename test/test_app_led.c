#include "unity.h"

#include "app_led.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_error.h"
#include "mock_ruuvi_interface_log.h"
#include "mock_ruuvi_interface_yield.h"
#include "mock_ruuvi_task_led.h"

static const ri_gpio_id_t    m_led_pins[]    = RB_LEDS_LIST;
static const ri_gpio_state_t m_leds_active[] = RB_LEDS_ACTIVE_STATE;

void setUp (void)
{
}

void tearDown (void)
{
}

void test_app_led_init_ok (void)
{
    rt_led_init_ExpectWithArrayAndReturn (m_led_pins, RB_LEDS_NUMBER,
                                          m_leds_active, RB_LEDS_NUMBER,
                                          RB_LEDS_NUMBER, RD_SUCCESS);
    rd_status_t err_code = app_led_init();
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_led_init_twice (void)
{
    rt_led_init_ExpectWithArrayAndReturn (m_led_pins, RB_LEDS_NUMBER,
                                          m_leds_active, RB_LEDS_NUMBER,
                                          RB_LEDS_NUMBER, RD_ERROR_INVALID_STATE);
    rd_status_t err_code = app_led_init();
    TEST_ASSERT (RD_ERROR_INVALID_STATE == err_code);
}

void test_app_led_activate_ok (void)
{
    rt_led_write_ExpectAndReturn (m_led_pins[0], true, RD_SUCCESS);
    rd_status_t err_code = app_led_activate (m_led_pins[0]);
    TEST_ASSERT (RD_SUCCESS == err_code);
}

void test_app_led_deactivate_ok (void)
{
    rt_led_write_ExpectAndReturn (m_led_pins[0], false, RD_SUCCESS);
    rd_status_t err_code = app_led_deactivate (m_led_pins[0]);
    TEST_ASSERT (RD_SUCCESS == err_code);
}