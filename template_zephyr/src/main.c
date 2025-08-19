#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/sys/printk.h>
#include <stdint.h>

#define I2S_DEV_LABEL DT_LABEL(DT_NODELABEL(i2s1))
#define BUFFER_SIZE 128  // bytes per block, adjust for your needs

static uint8_t rx_buffer[BUFFER_SIZE];

void main(void)
{
    const struct device *i2s_dev = device_get_binding(I2S_DEV_LABEL);
    if (!i2s_dev) {
        printk("I2S device not found!\n");
        return;
    }

    struct i2s_config i2s_cfg = {
        .word_size = 16,                        // 16-bit audio
        .channels = 1,                          // mono
        .format = I2S_FMT_DATA_FORMAT_I2S,
        .options = I2S_OPT_FRAME_CLK_MASTER | I2S_OPT_BIT_CLK_MASTER,
        .frame_clk_freq = 16000,                // 16 kHz sample rate
        .block_size = BUFFER_SIZE,
        .timeout = 1000,
    };

    if (i2s_configure(i2s_dev, I2S_DIR_RX, &i2s_cfg) != 0) {
        printk("Failed to configure I2S\n");
        return;
    }

    if (i2s_trigger(i2s_dev, I2S_DIR_RX, I2S_TRIGGER_START) != 0) {
        printk("Failed to start I2S\n");
        return;
    }

    printk("I2S MEMS mic capture started\n");

    while (1) {
        struct i2s_buf rx_buf = {
            .buf = rx_buffer,
            .size = sizeof(rx_buffer),
        };

        int ret = i2s_read(i2s_dev, &rx_buf);
        if (ret < 0) {
            printk("I2S read error: %d\n", ret);
        } else if (ret > 0) {
            int16_t *samples = (int16_t *)rx_buffer;
            int sample_count = ret / sizeof(int16_t);

            for (int i = 0; i < sample_count; i++) {
                printk("%d\n", samples[i]);
            }
        }

        k_sleep(K_MSEC(10));  // throttle loop
    }
}


//-------------------------------------------

// #include <zephyr/kernel.h>
// #include <zephyr/drivers/gpio.h>

// #define SLEEP_TIME_MS   1500

// /* Change this to match your Nucleo H723ZG's LED label */
// #define LED_NODE DT_ALIAS(led0)

// static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

// int main(void)
// {
//     if (!device_is_ready(led.port)) {
//         return 0;
//     }

//     gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);

//     while (1) {
//         gpio_pin_toggle_dt(&led);
//         printk("Hello from Nucleo!\n");
//         k_msleep(SLEEP_TIME_MS);
//     }
// }