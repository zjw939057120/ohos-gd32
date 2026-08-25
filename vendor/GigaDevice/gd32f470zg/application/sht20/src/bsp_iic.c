
#include "bsp_iic.h"
#include "systick.h"
#include "stdio.h"
#include <ohos_init.h>
#include "bsp_iic.h"
#include "los_task.h"
#include <unistd.h>

void delay_us(uint32_t count)
{
   usleep(count+10);
}
void iic_gpio_config(void)
{
	
	rcu_periph_clock_enable(RCU_SCL);
	rcu_periph_clock_enable(RCU_SDA);
		
	
	gpio_mode_set(PORT_SCL,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_SCL);
	
	gpio_output_options_set(PORT_SCL,GPIO_OTYPE_OD,GPIO_OSPEED_50MHZ,GPIO_SCL);
		
	
	gpio_mode_set(PORT_SDA,GPIO_MODE_OUTPUT,GPIO_PUPD_PULLUP,GPIO_SDA);
	
	gpio_output_options_set(PORT_SDA,GPIO_OTYPE_OD,GPIO_OSPEED_50MHZ,GPIO_SDA);
}


void IIC_Start(void)
{
	SDA_OUT();
	SDA(1);
	SCL(1); 
	delay_us(5);
	SDA(0);
	delay_us(5);
	SCL(0);
	delay_us(5);               
}


void IIC_Stop(void)
{
	SDA_OUT();
	SCL(0);
	SDA(0);
	SCL(1);
	delay_us(5);
	SDA(1);
	delay_us(5);
}


void IIC_Send_Nack(void)
{
	SDA_OUT();
	SCL(0);
	SDA(0);
	SDA(1);
	SCL(1);
	delay_us(5);
	SCL(0);
	SDA(0);
}


void IIC_Send_Ack(void)
{
	SDA_OUT();
	SCL(0);
	SDA(1);
	SDA(0);
	SCL(1);
	delay_us(5);
	SCL(0);
	SDA(1);
}


unsigned char I2C_WaitAck(void)
{
	char ack = 0;
	unsigned char ack_flag = 10;
	SCL(0);
	SDA(1);
	SDA_IN();
	delay_us(5);
	SCL(1);
    delay_us(5);

	while( (SDA_GET()==1)  &&  ( ack_flag ) )
	{
		ack_flag--;
		delay_us(5);
	}
	
	
	if( ack_flag <= 0 )
	{
		IIC_Stop();
		return 1;
	}
	else
	{
		SCL(0);
		SDA_OUT();
	}
	return ack;
}


void IIC_Send_Byte(uint8_t dat)
{
	int i = 0;
	SDA_OUT();
	SCL(0);
		
	for( i = 0; i < 8; i++ )
	{
		SDA( (dat & 0x80) >> 7 );
		delay_us(1);
		SCL(1);
		delay_us(5);
		SCL(0);
		delay_us(5);
		dat<<=1;
	}	
}


unsigned char IIC_Read_Byte(void)
{
	unsigned char i,receive=0;
	SDA_IN();
	for(i=0;i<8;i++ )
	{
		SCL(0);
		delay_us(5);
		SCL(1);
		delay_us(5);
		receive<<=1;
		if( SDA_GET() )
		{	
			receive |= 1;   
		} 
		}
	SCL(0); 
	return receive;
}


float SHT20_receive_data(unsigned char num)
{
	uint8_t data_msb = 0;
	uint8_t data_lsb = 0;
	uint16_t dat = 0;
	float temp = 0;
	IIC_Start();
	IIC_Send_Byte(0X80);
	if( I2C_WaitAck() == 1 ) printf("receive fail -1\r\n");
	IIC_Send_Byte(num);
	if( I2C_WaitAck() == 1 ) printf("receive fail -2\r\n");
	do
	{
		delay_us(10);
		IIC_Start();
		IIC_Send_Byte(0X81);
	}
	while( I2C_WaitAck() == 1 );
	data_msb = IIC_Read_Byte();
	IIC_Send_Ack();
	data_lsb = IIC_Read_Byte();
	IIC_Send_Ack();
	IIC_Read_Byte();
	IIC_Send_Nack();
	IIC_Stop();
	dat = data_msb<<8;
	dat = dat | data_lsb;
	dat &= ~(1<<1);
	if( num == 0xf3 )
	{
		temp = ( dat / 65536.0 ) * 175.72 - 46.85;
	}
	if( num == 0xf5)
	{
		temp = ( dat / 65536.0 ) * 125 - 6;
	}
	return temp;
}


void iic_hardware_config(void)
{
    
    rcu_periph_clock_enable(RCU_SCL);
    rcu_periph_clock_enable(RCU_SDA);

    
    gpio_af_set(PORT_SCL, GPIO_AF_4, GPIO_SCL);
    
    gpio_af_set(PORT_SDA, GPIO_AF_4, GPIO_SDA);
    
    
    gpio_mode_set(PORT_SCL, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_SCL);
    gpio_output_options_set(PORT_SCL, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_SCL);
   
    
    gpio_mode_set(PORT_SDA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_SDA);
    gpio_output_options_set(PORT_SDA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_SDA);

	
	rcu_periph_clock_enable(RCU_I2C0);

	
	i2c_deinit(I2C0);

	
	i2c_clock_config(I2C0, 400000, I2C_DTCY_2);

	
	 i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0X80);

	
	i2c_enable(I2C0);

	
	i2c_ack_config(I2C0, I2C_ACK_ENABLE);

}


float iic_hardware_get_data(unsigned int num)
{
	unsigned int timeout = 0;
	unsigned char data_H = 0;
	unsigned char data_L = 0;
	unsigned int dat = 0;
	
	i2c_start_on_bus(I2C0);
	while( !i2c_flag_get(I2C0,I2C_FLAG_SBSEND) );
	
	i2c_master_addressing(I2C0, 0x80, I2C_TRANSMITTER);
	while( !i2c_flag_get(I2C0,I2C_FLAG_ADDSEND) );
	i2c_flag_clear(I2C0,I2C_FLAG_ADDSEND);
	
	
	while( !i2c_flag_get(I2C0,I2C_FLAG_TBE) );
	i2c_data_transmit (I2C0, 0xf3);
	while( !i2c_flag_get(I2C0,I2C_FLAG_BTC) );
	
	
	i2c_stop_on_bus(I2C0);

	
	again:
	
	i2c_start_on_bus(I2C0);
	while( !i2c_flag_get(I2C0,I2C_FLAG_SBSEND) );
	
	i2c_master_addressing(I2C0, 0x80, I2C_RECEIVER);
	while( !i2c_flag_get(I2C0,I2C_FLAG_ADDSEND) )
	{
		delay_1ms(1);
		timeout++;
		if( timeout > 10 )
		{
			timeout = 0;
			goto again;
		}
	}
	i2c_flag_clear(I2C0,I2C_FLAG_ADDSEND);


	i2c_ack_config(I2C0, I2C_ACK_ENABLE);
	while( !i2c_flag_get(I2C0,I2C_FLAG_RBNE) );
	data_H = i2c_data_receive (I2C0);
	
	
	i2c_ack_config(I2C0, I2C_ACK_DISABLE);
	while( !i2c_flag_get(I2C0,I2C_FLAG_RBNE) );
	data_L = i2c_data_receive (I2C0);
	
	
	i2c_stop_on_bus(I2C0);
	
	dat = data_H<<8;
	dat = dat | data_L;
	dat &= ~(1<<1);
	
	if( num == 0xf3 )
	{
		return ( ( dat / 65536.0 ) * 175.72 - 46.85);
	}
	if( num == 0xf5 )
	{
		return ( ( dat / 65536.0 ) *125 - 6);
	}
	return 0;
}
