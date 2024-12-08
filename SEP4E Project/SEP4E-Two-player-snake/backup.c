/*
 * game.c
 *
 * Created: 26-05-2018 15:03:07
 *  Author: amavin
 */ 
 #include <FreeRTOS.h>
 #include <stdlib.h>
 #include <stdbool.h>
 #include <task.h>
 #include "game.h"
 #include "serialcom.h"

static snake_t* snakes[2];
static bool run_game = false;
static uint8_t food_location = 7;
uint8_t* dir = 1;
int* xC=5;
int* yC=5;
void snake_change_direction(Player_t player, Direction_t direction)
{
	switch(snakes[player]->direction)
	{
		case D_UP:
		case D_DOWN:
				snakes[player]->direction = direction;
				
				
			break;
		
		
			

	}
	uint8_t new_location;
	//new_location = move_player_step(player, snakes, food_location);
	if((new_location = move_player_step(player, snakes, food_location)) != UINT8_MAX)
	{
		move_to_location(snakes[player],  new_location, direction);
	}
}

void restart_game()
{
	if(!run_game)
	{
		run_game = true;
		com_send_string("Game is (re)started!");
		init_game(snakes);
	
	}
}
void end_game()
{
	run_game = false;
}

uint8_t xy_to_pixel_id(uint8_t x, uint8_t y)
{
	return x + y * SCREEN_DIMENSION_X;
}

void com_send_string(char *str)
{
	send_bytes(str, strlen(str)+1);
}


void game_renderer_task(void *pvParameters)
{
	#if (configUSE_APPLICATION_TASK_TAG == 1)
	// Set task no to be used for tracing with R2R-Network
	//vTaskSetApplicationTaskTag( NULL, ( void * ) 3 );
	#endif

	uint16_t* frame_buf = pvParameters;
	//food_location=70;
	for(uint8_t i = 0; i < 2; i++)
	{
		snakes[i] = snake_new_instance();
		if(snakes[i] == NULL)
		{
			snakes[i] = NULL;
			//end game?
		}
	}

	TickType_t game_renderer_task_lastwake = xTaskGetTickCount();
	

	clear_screen(frame_buf);
	init_game(snakes);
	
	draw_game(frame_buf, snakes, food_location);
	while(1)
	{
		UBaseType_t stackUsage = uxTaskGetStackHighWaterMark(NULL);
		//Set task period
		vTaskDelayUntil(&game_renderer_task_lastwake, GAME_RENDERER_TASK_PERIOD);
		//Action
		if(run_game)
		{
			clear_screen(frame_buf);
			for(Player_t player = 0; player < 2; player++)
			{
				uint8_t new_location;
				if((new_location = move_player_step(player, snakes, food_location)) != UINT8_MAX)
				{
					//move_to_location(snakes[player],  new_location);
					//if(true)
					//{
						//ball colide
					//*dir = 2;
						
						/*switch(*dir)
						{
							case 1:
							*dir = 2;
							
							break;
							case 2:
							*dir = 1;
							
							break;
						}*/
						
					//}
					
					if(food_location %14==0)
					{
						//ball colide
						/*switch(*dir)
						{
							//case NULL:
							*dir = 1;
							case 1:
							*dir = 2;
							
							break;
							case 2:
							*dir = 1;
							
							break;
						}*/
						if(*dir==1)
						*dir=2;
						else
						*dir=1;
					}
					
					//if(PINC==D_UP)
					//if((score1/2>2)
					//{
						// if score1>2 com_send_string 
						// if score 2>2 com_send_strign
					//}
					generate_new_food(&food_location,dir);
					move_to_location(snakes[player], new_location);
					}
				
				
			}
			
			draw_game(frame_buf, snakes, food_location);
		}
	}
}

void move_to_location(snake_t *snake, uint8_t location ,            /*remove or fix it*/    Direction_t dir)
{
	
	for(int8_t i = (snake->length - 2); i>= 0; i--)//
	{
	//if(dir= D_UP)
	//if(!location<0)
	snake->pixels[i+1] = snake->pixels[i];
	//if(dir=D_DOWN)//
	//snake->pixels[i-1] = snake->pixels[i];//
	}
	snake->pixels[0] = location;
}
void generate_new_food(uint8_t *location, uint8_t* dir)
{
	
	//if(x < 1)
	//score++
	//if(x>13)
	//score++
	
	uint8_t x = *location;
	int id= xy_to_pixel_id(9,0);
	if(*dir==1)
	{
	x = x+1;
	//com_send_string(x+"place");
	}
	else  
	{
	 x = x-1;
	//com_send_string(x+"place");
	}
	x %= SCREEN_DIMENSION_X * SCREEN_DIMENSION_Y;
	*location = x;
}
bool check_collisions_for_player(Player_t player, snake_t **snakes, uint8_t* location)
{
	uint8_t loc = *location;
	//uint8_t loc = food_location;
	for(uint8_t i = 1; i < snakes[player]->length; i++)
		if(snakes[player]->pixels[i] == loc)
			return true;

	for(uint8_t i = 0; i < snakes[1 - player]->length; i++)
		if(snakes[1 - player]->pixels[i] == loc)
			return true;

	return false;
}
snake_t* snake_new_instance()
{
	snake_t *ret = pvPortMalloc(sizeof *ret);
	if(ret == NULL)
		return ret;
	ret->steps = 0;
	ret->direction = 0;
	ret->movespeed = 0;
	ret->length = 0;
	ret->max_length = 20; 
	for(uint8_t i = 0; i < ret->max_length; i++)
		ret->pixels[i] = 0;
	return ret;
}
void init_game(snake_t **snakes)
{
	snakes[PL_ONE]->direction = D_UP;
	snakes[PL_ONE]->length = 3;
	snakes[PL_ONE]->movespeed = 1;
	snakes[PL_ONE]->pixels[0] = xy_to_pixel_id(1,2);
	snakes[PL_ONE]->pixels[1] = xy_to_pixel_id(1, 1);

	snakes[PL_TWO]->direction = D_DOWN;
	snakes[PL_TWO]->length = 3;
	snakes[PL_TWO]->movespeed = 1;
	snakes[PL_TWO]->pixels[0] = xy_to_pixel_id(12, 9);
	snakes[PL_TWO]->pixels[1] = xy_to_pixel_id(12, 8);
}

void clear_screen(uint16_t* framebuffer)
{
	for(uint8_t x = 0; x < SCREEN_DIMENSION_X; x++)
		framebuffer[x] = 0;
}
uint8_t move_player_step(Player_t player, snake_t **snakes, uint8_t food_location)
{
	snakes[player]->steps += snakes[player]->movespeed;
	uint8_t steps;
	if((steps = snakes[player]->steps / STEPS_PER_PIXEL))
	{
		snakes[player]->steps -= steps * STEPS_PER_PIXEL;
		return get_new_position(snakes[player]->pixels[0], snakes[player]->direction, steps);
	}
	return UINT8_MAX;
}
uint8_t get_new_position(uint8_t origin_pixelid, Direction_t direction, uint8_t steps)
{
	//uint8_t x = x_offset(origin_pixelid);
	switch(direction)
	{
		case D_DOWN:
		case D_UP:
			return mod(origin_pixelid + (direction - J_OFFSET) * steps * SCREEN_DIMENSION_X, SCREEN_DIMENSION_X * SCREEN_DIMENSION_Y);
		break;
	}
}
uint8_t y_offset(uint8_t pixel_id)
{
	return pixel_id / SCREEN_DIMENSION_X;
}
uint8_t x_offset(uint8_t pixel_id)
{
	return pixel_id % SCREEN_DIMENSION_X;
}
void draw_game(uint16_t* framebuffer, snake_t **snakes, uint8_t food_location)
{
	framebuffer[x_offset(food_location)] |=  _BV((SCREEN_DIMENSION_Y - (y_offset(food_location)+1)));
	for(Player_t player = 0; player < 2; player++)
	{
		for(uint8_t i = 0; i < snakes[player]->length; i++)
		{
			framebuffer[x_offset(snakes[player]->pixels[i])] |= _BV((SCREEN_DIMENSION_Y - (y_offset(snakes[player]->pixels[i])+1)));
		}
	}
}

int16_t mod(int16_t a, int16_t b)
{
	int16_t r = a % b;
	return r < 0 ? r + b : r;
}