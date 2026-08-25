

#ifndef __BSP_IIC_H
#define __BSP_IIC_H

#include "gd32f4xx.h"


#define 	RCU_SDA 		RCU_GPIOD
#define 	PORT_SDA 		GPIOD
#define 	GPIO_SDA 		GPIO_PIN_8

#define 	RCU_SCL 		RCU_GPIOD
#define 	PORT_SCL	 	GPIOD
#define 	GPIO_SCL 		GPIO_PIN_9

#define SDA_OUT()	gpio_mode_set(PORT_SDA,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_SDA)

#define SDA_IN()	gpio_mode_set(PORT_SDA,GPIO_MODE_INPUT,GPIO_PUPD_PULLUP,GPIO_SDA)

#define SDA_GET()	gpio_input_bit_get(PORT_SDA,GPIO_SDA) 

#define SDA(x)         	gpio_bit_write(PORT_SDA,GPIO_SDA, (x?SET:RESET) )
#define SCL(x)          	gpio_bit_write(PORT_SCL,GPIO_SCL, (x?SET:RESET)  )

void iic_gpio_config(void);
float SHT20_receive_data(unsigned char num);
void iic_hardware_config(void);
float iic_hardware_get_data(unsigned int num);
#endif
