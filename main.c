/**
 * @file  examples/fpga-blinky/main.c
 *
 * @brief Simple FPGA blinky Application running on S1
 *
 *        Includes basic configuration of the S1 module, and
 *        operations required to boot the FPGA. The FPGA
 *        verilog project can be built by running:
 *           "make build-verilog"
 *        from this folder.
 *
 * @attention Copyright 2022 Silicon Witchery AB
 *
 * Permission to use, copy, modify, and/or distribute this
 * software for any purpose with or without fee is hereby
 * granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#include <math.h>
#include "app_scheduler.h"
#include "app_timer.h"
#include "fpga_binfile.h"
#include "nrf_gpio.h"
#include "nrf52811.h"
#include "nrfx_clock.h"
#include "nrfx_saadc.h"
#include "nrfx_spim.h"
#include "nrfx_twim.h"
#include "s1.h"

APP_TIMER_DEF(fpga_boot_task_id);

typedef enum
{
    STARTED,
    ERASING,
    FLASHING,
    BOOTING,
    DONE
} fpga_boot_state_t;

static fpga_boot_state_t fpga_boot_state = STARTED;
static uint32_t pages_remaining;
static uint32_t page_address = 0x000000;

/**
 * @brief Clock event callback. Not used but required to have.
 */
void clock_event_handler(nrfx_clock_evt_type_t event)
{
    (void)event;
}

/**
 * @brief Timer based state machine for flashing the FPGA
 *        image and booting the FPGA. As some of the flash
 *        operations take a lot of time, using a timer based
 *        state machine avoids the main thread hanging while
 *        waiting for flash operations to complete.
 */
static void fpga_boot_task(void *p_context)
{
    UNUSED_PARAMETER(p_context);
    switch (fpga_boot_state)
    {
    // Configure power and erase the flash
    case STARTED:
        s1_pimc_set_vfpga(true);
        s1_pmic_set_vio(2.8f, false);
        s1_pmic_set_vaux(3.3f);
        s1_fpga_hold_reset();
        s1_flash_wakeup();
        s1_flash_erase_all();
        fpga_boot_state = ERASING;
        LOG("Erasing flash. Takes up to 80 seconds.");
        break;

    // Wait for erase to complete
    case ERASING:
        if (!s1_flash_is_busy())
        {
            pages_remaining = (uint32_t)ceil((float)fpga_binfile_bin_len / 256.0f);
            fpga_boot_state = FLASHING;
            LOG("Flashing %d pages.", (int)pages_remaining);
        }
        break;

    // Flash every page until done
    case FLASHING:
        if (!s1_flash_is_busy())
        {
            s1_flash_page_from_image(page_address, (unsigned char *)&fpga_binfile_bin);
            pages_remaining--;
            page_address += 0x100;
        }

        if (pages_remaining == 0)
        {
            fpga_boot_state = BOOTING;
            s1_fpga_boot();
            LOG("Flashing done.");
            break;
        }
        break;

    // Wait for CDONE pin to go high then stop the task
    case BOOTING:
        if (s1_fpga_is_booted())
        {
            app_timer_stop(fpga_boot_task_id);
            fpga_boot_state = DONE;
            LOG("FPGA started.");
        }
        break;

    // Done. Do nothing more
    case DONE:
        break;
    }
}

/**
 * @brief Main application entry for the fpga-blinky demo.
 */
int main(void)
{
    // Log some stuff about this project
    LOG_CLEAR();
    LOG("S1 FPGA Blinky Demo – Built: %s %s – SDK Version: %s.",
        __DATE__,
        __TIME__,
        __S1_SDK_VERSION__);

    // Initialise S1 base setting
    APP_ERROR_CHECK(s1_init());

    // Initialise LFXO required by the App Timer
    APP_ERROR_CHECK(nrfx_clock_init(clock_event_handler));
    nrfx_clock_lfclk_start();

    // Initialise the App Timer
    APP_ERROR_CHECK(app_timer_init());
    APP_SCHED_INIT(sizeof(uint32_t), 5);

    // Create and start a timer for the FPGA flash/boot task
    APP_ERROR_CHECK(app_timer_create(&fpga_boot_task_id,
                                     APP_TIMER_MODE_REPEATED,
                                     fpga_boot_task));

    APP_ERROR_CHECK(app_timer_start(fpga_boot_task_id,
                                    APP_TIMER_TICKS(5),
                                    NULL));

    // The CPU is free to do nothing in the meanwhile
    for (;;)
    {
        __WFI();
    }
}