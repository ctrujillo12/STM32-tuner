#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/sys/printk.h>
#include <stdint.h>
#include <errno.h>

#define I2S_DEV_NODE      DT_NODELABEL(i2s1)
#define FRAME_CLK_FREQ    16000
#define BLOCK_SIZE        128      /* bytes */
#define NUM_RX_BLOCKS     4        /* number of DMA buffers */
#define TIMEOUT_MS        1000

/* RX buffer pool (DMA-safe, aligned) */
static uint8_t rx_pool[NUM_RX_BLOCKS][BLOCK_SIZE] __aligned(4);

void main(void)
{
    const struct device *i2s = DEVICE_DT_GET(I2S_DEV_NODE);

    if (!device_is_ready(i2s)) {
        printk("I2S device not ready!\n");
        return;
    }

    struct i2s_config cfg = {
        .word_size      = 16,
        .channels       = 1,
        .format         = I2S_FMT_DATA_FORMAT_I2S,
        .options        = I2S_OPT_FRAME_CLK_MASTER | I2S_OPT_BIT_CLK_MASTER,
        .frame_clk_freq = FRAME_CLK_FREQ,
        .block_size     = BLOCK_SIZE,
        .timeout        = TIMEOUT_MS,
    };

    if (i2s_configure(i2s, I2S_DIR_RX, &cfg) != 0) {
        printk("I2S RX configure failed\n");
        return;
    }

    /* Prequeue empty buffers for DMA to fill */
    for (int i = 0; i < NUM_RX_BLOCKS; i++) {
        int r = i2s_buf_write(i2s, rx_pool[i], BLOCK_SIZE);
        if (r < 0) {
            printk("Prequeue %d failed: %d\n", i, r);
            return;
        }
    }

    if (i2s_trigger(i2s, I2S_DIR_RX, I2S_TRIGGER_START) != 0) {
        printk("I2S RX start failed\n");
        return;
    }

    printk("I2S capture started (16 kHz, 16-bit, mono)\n");

    while (1) {
        void *mem_block = NULL;
        size_t size = 0;

        int ret = i2s_read(i2s, &mem_block, &size);
        if (ret < 0) {
            if (ret == -EAGAIN) {
                k_sleep(K_MSEC(1));
                continue;
            }
            printk("i2s_read error: %d\n", ret);
            break;
        }

        if (mem_block && size) {
            /* Process samples */
            int16_t *samples = (int16_t *)mem_block;
            int count = size / sizeof(int16_t);

            /* Print first few samples */
            for (int i = 0; i < MIN(count, 8); i++) {
                printk("%d\n", samples[i]);
            }

            /* Return buffer back to driver */
            int rr = i2s_buf_write(i2s, mem_block, size);
            if (rr < 0) {
                printk("Requeue failed: %d\n", rr);
                break;
            }
        }

        k_sleep(K_MSEC(10));
    }

    (void)i2s_trigger(i2s, I2S_DIR_RX, I2S_TRIGGER_STOP);
}
