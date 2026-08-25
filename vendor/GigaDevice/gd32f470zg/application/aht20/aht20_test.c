#include "pinctrl.h"
#include "i2c.h"
#include "osal_debug.h"
#include "soc_osal.h"
#include "aht20.h"
#include "app_init.h"

#define CONFIG_I2C_SCL_MASTER_PIN 15
#define CONFIG_I2C_SDA_MASTER_PIN 16
#define CONFIG_I2C_MASTER_PIN_MODE 2
#define I2C_MASTER_ADDR 0x0
#define I2C_SLAVE1_ADDR 0x38
#define I2C_SET_BANDRATE 400000
#define I2C_TASK_STACK_SIZE 0x1000
#define I2C_TASK_PRIO 17

const unsigned char headSize[] = {64, 64};

void app_i2c_init_pin(void)
{
    uapi_pin_set_mode(CONFIG_I2C_SCL_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);
    uapi_pin_set_mode(CONFIG_I2C_SDA_MASTER_PIN, CONFIG_I2C_MASTER_PIN_MODE);
}

void Aht20TestTask(void)
{
    uint32_t retval = 0;
    float temp = 0.0f;
    float humi = 0.0f;
    static char templine[32] = {0};
    static char humiline[32] = {0};
    uint32_t baudrate = I2C_SET_BANDRATE;
    uint32_t hscode = I2C_MASTER_ADDR;
    app_i2c_init_pin();
    errcode_t ret = uapi_i2c_master_init(1, baudrate, hscode);
    if (ret != 0) {
        printf("i2c init failed, ret = %0x\r\n", ret);
    }

    while (AHT20_Calibrate() != 0) {
        printf("AHT20 sensor init failed!\r\n");
        osal_mdelay(100); // 10ms在判断设备是否复位成功
    }
    while (1) {
        retval = AHT20_StartMeasure();
        printf("AHT20_StartMeasure: %d\r\n", retval);
        retval = AHT20_GetMeasureResult(&temp, &humi);
        if (retval != 0) {
            printf("get humidity data failed!\r\n");
        }

        int ret = sprintf(templine, ": %.2f", temp);
        if (ret < 0) {
            printf("temp failed\r\n");
        }
        
        ret = sprintf(humiline, ": %.2f", humi);
        if (ret < 0) {
            printf("humi failed\r\n");
        }
        printf("temp = %s, humi = %s\r\n", templine, humiline);
    }
}

void Aht20Test(void)
{
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)Aht20TestTask, 0, "PwmTask", I2C_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, I2C_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

app_run(Aht20Test);