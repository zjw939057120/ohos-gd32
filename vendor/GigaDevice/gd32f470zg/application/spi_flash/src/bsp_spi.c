
#include "bsp_spi.h"
#include <stdio.h>

void w25q64_init_config(void)
{	
	spi_parameter_struct spi_init_struct;
	
	rcu_periph_clock_enable(RCU_GPIOB);  
    rcu_periph_clock_enable(RCU_SPI0);     
    
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_3);
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_4);
    gpio_af_set(GPIOB, GPIO_AF_5, GPIO_PIN_5);
    
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_5);
    
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
    
	
	rcu_periph_clock_enable(RCU_GPIOD);
	
	gpio_mode_set(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_7);
	
	gpio_output_options_set(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
	
	gpio_bit_write(GPIOD, GPIO_PIN_7, SET);

	spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;  	
	spi_init_struct.device_mode          = SPI_MASTER;                	
	spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;        	
	spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;		
	spi_init_struct.nss                  = SPI_NSS_SOFT;              	
	spi_init_struct.prescale             = SPI_PSC_2;                 	
	spi_init_struct.endian               = SPI_ENDIAN_MSB;            	
	
	spi_init(SPI0, &spi_init_struct);
	
	spi_enable(SPI0);

}


uint8_t spi_read_write_byte(uint8_t dat)
{
	
	while(RESET == spi_i2s_flag_get(SPI0,  SPI_FLAG_TBE) ){};
	
         spi_i2s_data_transmit(SPI0, dat);
	
	while(RESET == spi_i2s_flag_get(SPI0,  SPI_FLAG_RBNE) ){};
	
         return spi_i2s_data_receive(SPI0);
}


uint16_t W25Q64_readID(void)
{
	uint16_t  temp = 0;	  	
	
	gpio_bit_write(GPIOD, GPIO_PIN_7, RESET);
		
	spi_read_write_byte(0x90);

	spi_read_write_byte(0x00); 	    
	spi_read_write_byte(0x00); 	    
	spi_read_write_byte(0x00); 		

	temp |= spi_read_write_byte(0xFF)<<8;  
	temp |= spi_read_write_byte(0xFF);	
	
	gpio_bit_write(GPIOD, GPIO_PIN_7, SET);	
	return temp;
}

void W25Q64_wait_busy(void)   
{   
    unsigned char byte = 0;
    do
     { 
        gpio_bit_write(GPIOD, GPIO_PIN_7, RESET);                              
        spi_read_write_byte(0x05);                 
        byte = spi_read_write_byte(0Xff);       
        gpio_bit_write(GPIOD, GPIO_PIN_7, SET);         
     }while( ( byte & 0x01 ) == 1 );  
}  

void W25Q64_write_enable(void)   
{
    gpio_bit_write(GPIOD, GPIO_PIN_7, RESET);                           
    spi_read_write_byte(0x06);                  
    gpio_bit_write(GPIOD, GPIO_PIN_7, SET); 
}                            	      

void W25Q64_erase_sector(uint32_t addr)   
{
	addr *= 4096;
	W25Q64_write_enable(); 
	W25Q64_wait_busy();     
	gpio_bit_write(GPIOD, GPIO_PIN_7, RESET);                                        
	spi_read_write_byte(0x20);        	
	spi_read_write_byte((uint8_t)((addr)>>16));      
	spi_read_write_byte((uint8_t)((addr)>>8));   
	spi_read_write_byte((uint8_t)addr);  
	gpio_bit_write(GPIOD, GPIO_PIN_7, SET);                  
	                   	      	 
	W25Q64_wait_busy();   
}         				                      	      


void W25Q64_write(uint8_t* buffer, uint32_t addr, uint16_t numbyte)
{    
    unsigned int i = 0;
    W25Q64_erase_sector(addr/4096);
    W25Q64_write_enable();    
    W25Q64_wait_busy(); 
    
    gpio_bit_write(GPIOD, GPIO_PIN_7, RESET);                                        
    spi_read_write_byte(0x02);                    
    spi_read_write_byte((uint8_t)((addr)>>16));     
    spi_read_write_byte((uint8_t)((addr)>>8));   
    spi_read_write_byte((uint8_t)addr);   
    for(i=0;i<numbyte;i++)
    {
        spi_read_write_byte(buffer[i]);  
    }
    gpio_bit_write(GPIOD, GPIO_PIN_7, SET);
    W25Q64_wait_busy();    
}



void W25Q64_read(uint8_t* buffer,uint32_t read_addr,uint16_t read_length)   
{ 
	uint16_t i;   		
	gpio_bit_write(GPIOD, GPIO_PIN_7, RESET);            
	spi_read_write_byte(0x03);                           
	spi_read_write_byte((uint8_t)((read_addr)>>16));           
	spi_read_write_byte((uint8_t)((read_addr)>>8));   
	spi_read_write_byte((uint8_t)read_addr);   
	for(i=0;i<read_length;i++)
	{ 
		buffer[i]= spi_read_write_byte(0XFF);  
	}
	gpio_bit_write(GPIOD, GPIO_PIN_7, SET);  		    	      
} 

