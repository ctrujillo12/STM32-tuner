// #include <zephyr.h>
// #include <device.h>
// #include <drivers/i2s.h>
// #include <sys/printk.h>
// #include <stdlib.h>  // for abs()

// #define I2S_DEV DT_LABEL(DT_NODELABEL(i2s1))
// #define BUFFER_SIZE 1024  // bytes (adjust as needed)

// static uint8_t rx_buffer[BUFFER_SIZE];
// static const struct device *i2s_dev;

// void main(void)
// {
//     int ret;
//     struct i2s_config i2s_cfg = {
//         .word_size = 16,                        // 16-bit audio
//         .channels = 1,                          // mono mic
//         .format = I2S_FMT_DATA_FORMAT_I2S,      // try LEFT_JUSTIFIED if needed
//         .options = I2S_OPT_FRAME_CLK_MASTER | I2S_OPT_BIT_CLK_MASTER,
//         .frame_clk_freq = 16000,                // 16 kHz sample rate
//         .block_size = 64,                       // bytes per block
//         .timeout = 1000,                        // ms timeout
//     };

//     printk("Starting I2S MEMS mic capture\n");

//     i2s_dev = device_get_binding(I2S_DEV);
//     if (!i2s_dev) {
//         printk("Failed to get I2S device\n");
//         return;
//     }

//     ret = i2s_configure(i2s_dev, I2S_DIR_RX, &i2s_cfg);
//     if (ret) {
//         printk("Failed to configure I2S: %d\n", ret);
//         return;
//     }

//     ret = i2s_trigger(i2s_dev, I2S_DIR_RX, I2S_TRIGGER_START);
//     if (ret) {
//         printk("Failed to start I2S: %d\n", ret);
//         return;
//     }

//     while (1) {
//         for (int i = 0; i < 100; i++) {
//             printk("%d\n", i);
//             k_sleep(K_MSEC(10));
//         }
//         // struct i2s_buf rx_buf = {
//         //     .buf = rx_buffer,
//         //     .len = sizeof(rx_buffer),
//         // };

//         // ret = i2s_read(i2s_dev, &rx_buf, sizeof(rx_buffer));
//         // if (ret < 0) {
//         //     printk("I2S read error: %d\n", ret);
//         // } else {
//         //     // interpret as 16-bit signed samples
//         //     int16_t *samples = (int16_t *)rx_buffer;
//         //     int sample_count = ret / 2; // 2 bytes per sample

//         //     // Print each sample as plain integer (plotter-friendly)
//         //     for (int i = 0; i < sample_count; i++) {
//         //         printk("%d\n", samples[i]);
//         //         k_sleep(K_MSEC(5));  // pause slightly to avoid overwhelming UART
//         //     }
//         // }

//         // // optional small sleep to throttle loop
//         // k_sleep(K_MSEC(10));
//     }
// }
