#include "iic.h"

#define IIC_SCL PBout(8)
#define IIC_SDA PBout(9)
#define READ_SDA PBin(9)

#define IIC_CLK RCU_GPIOD
#define IIC_SCL_PORT GPIOD
#define IIC_SCL_PIN GPIO_PIN_9
#define IIC_SDA_PORT GPIOD
#define IIC_SDA_PIN GPIO_PIN_8

/**
 * @brief  iic bus init
 * @return status code
 *         - 0 success
 * @note   SCL is PB8 and SDA is PB9
 */
void iic_init(void)
{
    rcu_periph_clock_enable(IIC_CLK);

    gpio_mode_set(IIC_SCL_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, IIC_SCL_PIN);
    gpio_output_options_set(IIC_SCL_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, IIC_SCL_PIN);

    gpio_bit_set(IIC_SCL_PORT, IIC_SCL_PIN);

    /* set sda high */
    IIC_SDA = 1;

    /* set scl high */
    IIC_SCL = 1;
}

/**
 * @brief  iic bus deinit
 * @return status code
 *         - 0 success
 * @note   none
 */
uint8_t iic_deinit(void)
{
    /* iic gpio deinit */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8 | GPIO_PIN_9);

    return 0;
}

/**
 * @brief iic bus send start
 * @note  none
 */
static void a_iic_start(void)
{
    SDA_OUT();
    IIC_SDA = 1;
    IIC_SCL = 1;
    delay_us(4);
    IIC_SDA = 0;
    delay_us(4);
    IIC_SCL = 0;
}

/**
 * @brief iic bus send stop
 * @note  none
 */
static void a_iic_stop(void)
{
    SDA_OUT();
    IIC_SCL = 0;
    IIC_SDA = 0;
    delay_us(4);
    IIC_SCL = 1;
    delay_us(4);
    IIC_SDA = 1;
    delay_us(4);
}

/**
 * @brief  iic wait ack
 * @return status code
 *         - 0 get ack
 *         - 1 no ack
 * @note   none
 */
static uint8_t a_iic_wait_ack(void)
{
    uint16_t uc_err_time = 0;

    SDA_IN();
    IIC_SDA = 1;
    delay_us(1);
    IIC_SCL = 1;
    delay_us(1);
    while (READ_SDA != 0)
    {
        uc_err_time++;
        if (uc_err_time > 250)
        {
            a_iic_stop();

            return 1;
        }
    }
    IIC_SCL = 0;

    return 0;
}

/**
 * @brief iic bus send ack
 * @note  none
 */
static void a_iic_ack(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 0;
    delay_us(2);
    IIC_SCL = 1;
    delay_us(2);
    IIC_SCL = 0;
}

/**
 * @brief iic bus send nack
 * @note  none
 */
static void a_iic_nack(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 1;
    delay_us(2);
    IIC_SCL = 1;
    delay_us(2);
    IIC_SCL = 0;
}

/**
 * @brief     iic send one byte
 * @param[in] txd sent byte
 * @note      none
 */
static void a_iic_send_byte(uint8_t txd)
{
    uint8_t t;

    SDA_OUT();
    IIC_SCL = 0;
    for (t = 0; t < 8; t++)
    {
        IIC_SDA = (txd & 0x80) >> 7;
        txd <<= 1;
        delay_us(2);
        IIC_SCL = 1;
        delay_us(2);
        IIC_SCL = 0;
        delay_us(2);
    }
}

/**
 * @brief     iic read one byte
 * @param[in] ack sent ack
 * @return    read byte
 * @note      none
 */
static uint8_t a_iic_read_byte(uint8_t ack)
{
    uint8_t i;
    uint8_t receive = 0;

    SDA_IN();
    for (i = 0; i < 8; i++)
    {
        IIC_SCL = 0;
        delay_us(2);
        IIC_SCL = 1;
        receive <<= 1;
        if (READ_SDA != 0)
        {
            receive++;
        }
        delay_us(1);
    }
    if (ack != 0)
    {
        a_iic_ack();
    }
    else
    {
        a_iic_nack();
    }

    return receive;
}

/**
 * @brief     iic bus write command
 * @param[in] addr iic device write address
 * @param[in] *buf pointer to a data buffer
 * @param[in] len length of the data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      addr = device_address_7bits << 1
 */
uint8_t iic_write_cmd(uint8_t addr, uint8_t *buf, uint16_t len)
{
    uint16_t i;

    /* send a start */
    a_iic_start();

    /* send the write addr */
    a_iic_send_byte(addr);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();

        return 1;
    }

    /* write the data */
    for (i = 0; i < len; i++)
    {
        /* send one byte */
        a_iic_send_byte(buf[i]);
        if (a_iic_wait_ack() != 0)
        {
            a_iic_stop();

            return 1;
        }
    }

    /* send a stop */
    a_iic_stop();

    return 0;
}

/**
 * @brief     iic bus write
 * @param[in] addr iic device write address
 * @param[in] reg iic register address
 * @param[in] *buf pointer to a data buffer
 * @param[in] len length of the data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      addr = device_address_7bits << 1
 */
uint8_t iic_write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    uint16_t i;

    /* send a start */
    a_iic_start();

    /* send the write addr */
    a_iic_send_byte(addr);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();

        return 1;
    }

    /* send the reg */
    a_iic_send_byte(reg);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();

        return 1;
    }

    /* write the data */
    for (i = 0; i < len; i++)
    {
        /* send one byte */
        a_iic_send_byte(buf[i]);
        if (a_iic_wait_ack() != 0)
        {
            a_iic_stop();

            return 1;
        }
    }

    /* send a stop */
    a_iic_stop();

    return 0;
}

/**
 * @brief     iic bus write with 16 bits register address
 * @param[in] addr iic device write address
 * @param[in] reg iic register address
 * @param[in] *buf pointer to a data buffer
 * @param[in] len length of the data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      addr = device_address_7bits << 1
 */
uint8_t iic_write_address16(uint8_t addr, uint16_t reg, uint8_t *buf, uint16_t len)
{
    uint16_t i;

    /* send a start */
    a_iic_start();

    /* send the write addr */
    a_iic_send_byte(addr);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();

        return 1;
    }

    /* send the reg high part */
    a_iic_send_byte((reg >> 8) & 0xFF);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();

        return 1;
    }

    /* send the reg low part */
    a_iic_send_byte(reg & 0xFF);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();

        return 1;
    }

    /* write the data */
    for (i = 0; i < len; i++)
    {
        /* send one byte */
        a_iic_send_byte(buf[i]);
        if (a_iic_wait_ack() != 0)
        {
            a_iic_stop();

            return 1;
        }
    }

    /* send a stop */
    a_iic_stop();

    return 0;
}

/**
 * @brief      iic bus read command
 * @param[in]  addr iic device write address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       addr = device_address_7bits << 1
 */
uint8_t iic_read_cmd(uint8_t addr, uint8_t *buf, uint16_t len)
{
    /* send a start */
    a_iic_start();

    /* send the read addr */
    a_iic_send_byte(addr + 1);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();

        return 1;
    }

    /* read the data */
    while (len != 0)
    {
        /* if the last */
        if (len == 1)
        {
            /* send nack */
            *buf = a_iic_read_byte(0);
        }
        else
        {
            /* send ack */
            *buf = a_iic_read_byte(1);
        }
        len--;
        buf++;
    }

    /* send a stop */
    a_iic_stop();

    return 0;
}

/**
 * @brief      iic bus read
 * @param[in]  addr iic device write address
 * @param[in]  reg iic register address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       addr = device_address_7bits << 1
 */
uint8_t iic_read(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    /* send a start */
    a_iic_start();

    /* send the write addr */
    a_iic_send_byte(addr);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();

        return 1;
    }

    /* send the reg */
    a_iic_send_byte(reg);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();

        return 1;
    }

    /* send a start */
    a_iic_start();

    /* send the read addr */
    a_iic_send_byte(addr + 1);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();

        return 1;
    }

    /* read the data */
    while (len != 0)
    {
        /* if the last */
        if (len == 1)
        {
            /* send nack */
            *buf = a_iic_read_byte(0);
        }
        else
        {
            /* send ack */
            *buf = a_iic_read_byte(1);
        }
        len--;
        buf++;
    }

    /* send a stop */
    a_iic_stop();

    return 0;
}

/**
 * @brief      iic bus read with 16 bits register address
 * @param[in]  addr iic device write address
 * @param[in]  reg iic register address
 * @param[out] *buf pointer to a data buffer
 * @param[in]  len length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       addr = device_address_7bits << 1
 */
uint8_t iic_read_address16(uint8_t addr, uint16_t reg, uint8_t *buf, uint16_t len)
{
    /* send a start */
    a_iic_start();

    /* send the write addr */
    a_iic_send_byte(addr);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();

        return 1;
    }

    /* send the reg high part */
    a_iic_send_byte((reg >> 8) & 0xFF);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();

        return 1;
    }

    /* send the reg low part */
    a_iic_send_byte(reg & 0xFF);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();

        return 1;
    }

    /* send a start */
    a_iic_start();

    /* send the read addr */
    a_iic_send_byte(addr + 1);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();

        return 1;
    }

    /* read the data */
    while (len != 0)
    {
        /* if the last */
        if (len == 1)
        {
            /* send nack */
            *buf = a_iic_read_byte(0);
        }
        else
        {
            /* send ack */
            *buf = a_iic_read_byte(1);
        }
        len--;
        buf++;
    }

    /* send a stop */
    a_iic_stop();

    return 0;
}
