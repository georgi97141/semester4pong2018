
#include <avr/sfr_defs.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
//napishi v dokumentaciqta che koda e reusenat da ne vi ebat za plagiatsvo 
// FfreeRTOS Includes
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <queue.h>
#include <semphr.h>

#include "src/board/board.h"

#include "serialcom.h"
#include "joystick.h"
#include "game.h"


// frame_buf contains a bit pattern for each column in the display
uint16_t frame_buf[14] = {1023, 1+256+512, 28+512, 62, 126, 254, 508, 254, 126, 62, 28 + 512, 1+256+512 , 1023, 0 };

//-----------------------------------------
void vApplicationIdleHook( void )
{
	//
}

// Prepare shift register setting SER = 1
void prepare_shiftregister()
{
	// Set SER to 1
	PORTD |= _BV(PORTD2);
}

// clock shift-register
void clock_shift_register_and_prepare_for_next_col()
{
	// one SCK pulse
	PORTD |= _BV(PORTD5);
	PORTD &= ~_BV(PORTD5);
	
	// one RCK pulse
	PORTD |= _BV(PORTD4);
	PORTD &= ~_BV(PORTD4);
	
	// Set SER to 0 - for next column
	PORTD &= ~_BV(PORTD2);
}

// Load column value for column to show
void load_col_value(uint16_t col_value)
{
	PORTA = ~(col_value & 0xFF);
	
	// Manipulate only with PB0 and PB1
	PORTB |= 0x03;
	PORTB &= ~((col_value >> 8) & 0x03);
}

//-----------------------------------------
void handle_display(void)
{
	static uint8_t col = 0;
	
	if (col == 0)
	{
		prepare_shiftregister();
	}
	
	load_col_value(frame_buf[col]);
	
	clock_shift_register_and_prepare_for_next_col();
	
	// count column up - prepare for next
	col++;
	if (col > 13)
	{
		col = 0;
	}
}

void vApplicationStackOverflowHook( TaskHandle_t xTask,signed char *pcTaskName )
{
	

}

void vApplicationMallocFailedHook( void )
{

}

//-----------------------------------------
int main(void)
{	
	cli();
	//
	init_board();
	
	// Shift register Enable output (G=0)
	PORTD &= ~_BV(PORTD6);
	
	//Tasks 
	
	BaseType_t t1 = xTaskCreate(serial_com_task, (const char *)"serial_com", 250, (void *)NULL, tskIDLE_PRIORITY+3, NULL);
	BaseType_t t2 = xTaskCreate(joystick_task, (const char *)"joystick", configMINIMAL_STACK_SIZE + 50, (void *)NULL, tskIDLE_PRIORITY+2, NULL);
	BaseType_t t3 = xTaskCreate(game_renderer_task, (const char *)"game_rend", configMINIMAL_STACK_SIZE+50, frame_buf, tskIDLE_PRIORITY+4, NULL);

	// Start the display handler timer
	init_display_timer(handle_display);
	
	sei();
	
	//Start the scheduler
	vTaskStartScheduler();
	
	//Should never reach here
	while (1)
	{

	}
}


