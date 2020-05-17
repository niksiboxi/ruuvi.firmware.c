#include "app_config.h"
#include "app_sensor.h"
#include "ruuvi_boards.h"
#include "ruuvi_driver_enabled_modules.h"
#include "ruuvi_driver_sensor_test.h"
#include "run_integration_tests.h"
#include "ruuvi_interface_communication_ble_advertising_test.h"
#include "ruuvi_interface_communication_ble_advertising.h"
#include "ruuvi_interface_communication_nfc_test.h"
#include "ruuvi_interface_communication_radio_test.h"
#include "ruuvi_interface_gpio_test.h"
#include "ruuvi_interface_gpio_interrupt_test.h"
#include "ruuvi_interface_flash_test.h"
#include "ruuvi_interface_i2c.h"
#include "ruuvi_interface_log.h"
#include "ruuvi_interface_power_test.h"
#include "ruuvi_interface_rtc.h"
#include "ruuvi_interface_scheduler_test.h"
#include "ruuvi_interface_spi.h"
#include "ruuvi_interface_timer_test.h"
#include "ruuvi_interface_watchdog.h"
#include "ruuvi_interface_yield.h"
#include "ruuvi_task_sensor.h"

/**
 * @addtogroup integration_test
 */

/** @{ */

/**
 * @file run_integration_test.h
 * @author Otso Jousimaa <otso@ojousima.net>
 * @date 2020-02-03
 * @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
 */

void on_integration_test_wdt (void)
{
}

static inline void LOG (const char * const msg)
{
    ri_log (RI_LOG_LEVEL_INFO, msg);
}

/** @brief Print test open JSON to console */
void integration_test_start (void)
{
    ri_watchdog_init (APP_WDT_INTERVAL_MS, &on_integration_test_wdt);
    ri_log_init (APP_LOG_LEVEL);
    ri_yield_init();
    LOG ("{\r\n\"firmware\":\"" APP_FW_NAME "\",\r\n");
    LOG ("\"board\":\"" RB_MODEL_STRING "\",\r\n");
    LOG ("\"ruuvi.boards.c\":\"" RUUVI_BOARDS_SEMVER "\",\r\n");
    LOG ("\"ruuvi.drivers.c\":\"" RUUVI_DRIVERS_SEMVER "\",\r\n");
}

/** @brief Print test close JSON to console */
void integration_test_stop (void)
{
    LOG ("}");
}

static void integration_test_power (void)
{
    ri_power_regulators_t regs = {0};
#   if (RB_DCDC_INTERNAL_INSTALLED)
    regs.DCDC_INTERNAL = 1;
#endif
#   if (RB_DCDC_HV_INSTALLED)
    regs.DCDC_HV = 1;
#endif
    ri_power_run_integration_test (&LOG, regs);
}

static void integration_test_sensors (void)
{
    rt_sensor_ctx_t ** p_sensors;
    size_t num_sensors = 0;
    LOG ("\"sensors\": {\r\n");
    app_sensor_init();
    app_sensor_ctx_get (&p_sensors, &num_sensors);

    for (size_t ii = 0; ii < num_sensors; ii++)
    {
        rd_sensor_run_integration_test (&LOG, p_sensors[ii]);

        if (ii < (num_sensors - 1))
        {
            LOG (",\r\n");
        }
        else
        {
            LOG ("\r\n");
        }
    }

    app_sensor_uninit();
    // TODO: Uninit I2C
    (void) ri_rtc_uninit();
    rd_sensor_timestamp_function_set (NULL);
    LOG ("},\r\n");
}

/** @brief Run integration tests*/
void integration_tests_run (void)
{
    integration_test_start();
    ri_flash_run_integration_test (&LOG);
    integration_test_power();
    ri_timer_integration_test_run (&LOG);
    ri_scheduler_run_integration_test (&LOG);
    ri_communication_radio_run_integration_test (&LOG);
    ri_communication_ble_advertising_run_integration_test (&LOG, RI_RADIO_BLE_1MBPS);
    ri_communication_ble_advertising_run_integration_test (&LOG, RI_RADIO_BLE_2MBPS);
#ifdef S140
    ri_communication_ble_advertising_run_integration_test (&LOG, RI_RADIO_BLE_125KBPS);
#endif
#if defined(RB_GPIO_TEST_INPUT) && defined(RB_GPIO_TEST_OUTPUT)
    ri_gpio_run_integration_test (&LOG, RB_GPIO_TEST_INPUT, RB_GPIO_TEST_OUTPUT);
    ri_gpio_interrupt_run_integration_test (&LOG, RB_GPIO_TEST_INPUT, RB_GPIO_TEST_OUTPUT);
    //ri_gpio_pwm_run_integration_test (&LOG, RB_GPIO_TEST_INPUT, RB_GPIO_TEST_OUTPUT);
#endif
    integration_test_sensors();
#if RB_NFC_INTERNAL_INSTALLED
    ri_communication_nfc_run_integration_test (&LOG);
#endif
    integration_test_stop();
}

/*@}*/