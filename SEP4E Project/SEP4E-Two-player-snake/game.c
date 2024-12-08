 #include <FreeRTOS.h>
 #include <stdlib.h>
 #include <stdbool.h>
 #include <task.h>
 #include "game.h"
 #include "serialcom.h"

static bat_t* bats[2];
static bool run_game = false;
static uint8_t ball_location = 21;
uint8_t *dir = 2;
uint8_t* scoreone=0;
uint8_t* scoretwo=0;
int z=0;
int up = 0;
int timer = 0;
bool score=false;
int* bata[3];
int* batb[3];
void bat_move(Player_t player, Direction_t direction)
{
	
	switch(direction)
	{ //interrupt messing the exec time. no mutex
		case D_UP:
		if( (player==PL_ONE&& bats[player]->pixels[0]<127 )|| (player==PL_TWO && bats[player]->pixels[0]<138)){
		bats[player]->pixels[0] += 14;
		bats[player]->pixels[1] +=14;
		bats[player]->pixels[2] +=14;
		}
		
		break;
		case D_DOWN:
		if((player==PL_ONE&& bats[player]->pixels[2]>1)||(player==PL_TWO&& bats[player]->pixels[2]>12)){
		bats[player]->pixels[0] -= 14;
		bats[player]->pixels[1] -=14;
		bats[player]->pixels[2] -=14;}
		break;
	}

}

void restart_game()
{
	if(!run_game)
	{	score=false;
		scoreone = 0;
		scoretwo=0;
		run_game = true;
		com_send_string("Game is restarted!");
		init_game(bats);
	    
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
	vTaskSetApplicationTaskTag( NULL, ( void * ) 3 );
	#endif

	uint16_t* frame_buf = pvParameters;

	for(uint8_t i = 0; i < 2; i++)
	{
		bats[i] = bat_new_instance();
	}

	TickType_t game_renderer_task_lastwake = xTaskGetTickCount();
	

	clear_screen(frame_buf);			//clearing from the screen from bootup display.
	init_game(bats);
	
	draw_game(frame_buf, bats, ball_location);
	
	while(1)
	{
		UBaseType_t stackUsage = uxTaskGetStackHighWaterMark(NULL);
		//Set task period
		vTaskDelayUntil(&game_renderer_task_lastwake, GAME_RENDERER_TASK_PERIOD);
		//Action5
		
		if(score==true){timer++;}
		if(timer%123==0&&run_game==false&&score==true){run_game=true; score=false;}
		if(timer==356){timer=0;}
			
		if(run_game)
		{
			clear_screen(frame_buf);
			//remove this for
			for(int i = 0; i < 2; i++)
			{
				
			if(ball_location%13==ball_location/13-1)			//first player scores 
			{
				
				scoreone++;
				ball_location=21;
				dir=2;
				//hide_game(frame_buf, bats);
				com_send_string("Player 1 scored!");
				draw_scores(frame_buf, scoreone, scoretwo);
				run_game=false;
				score=true;
				
					} //score
				
				else if
				((ball_location-12)%14==0 && ball_location==bats[PL_TWO]->pixels[0]	)
				{
					dir=2;
					up=1;
				}
					else if
					((ball_location-12)%14==0 && ball_location==bats[PL_TWO]->pixels[1]	)
					{
						dir=2;
						up=2;
					}
					//
					else if
					((ball_location-12)%14==0 && ball_location==bats[PL_TWO]->pixels[2]	)
					{
						dir=2;
						up=0;
					}
					
				if(ball_location%14==0)							//second player scores.
					{
				scoretwo++;
				dir=1;	
				ball_location = 21;
				//hide_game(frame_buf, bats);
				com_send_string("Player  scored!");
				draw_scores(frame_buf, scoreone, scoretwo);
				run_game=false;
				score=true;
				
					} //SCORE
				else if((ball_location-1)%14==0 && ball_location==bats[PL_ONE]->pixels[0])
				{
				
					dir=1;
					up=1;
				}
			
				//
				else if((ball_location-1)%14==0 && ball_location==bats[PL_ONE]->pixels[1])
						{	
							dir=1;
							up=2;	
						}
			
				//
					else if((ball_location-1)%14==0 && ball_location==bats[PL_ONE]->pixels[2])
					{
						dir=1;
						up=0;
					}
				
				if(z%27==0)				//z is the ball period.
					{
					move_ball(&ball_location,dir);
					}
				z++;
				if(z==270)
				z=0;
				
				if(scoreone>=2||scoretwo>=2)
					{
						if(scoreone>=2)
						com_send_string("player 1 won");
						if(scoretwo>=2)
						com_send_string("player 2 won");
						 end_game();
					}
				
			}
			
			draw_game(frame_buf, bats, ball_location);
		}
	}
}

void move_ball(uint8_t *location, uint8_t* dir)
{
	uint8_t x = *location;
	
	//boolean up 
	if(x>125) up = 0; //bouncing of the ball from the wall
	if(x<13) up = 1;  //bouncing of the ball from the wall
	
	if(dir==1)
	{
		if(up==1) x=x+15;
		else if(up==0) x=x-13; //move down; keep the direction 
		else if(up==2) x=x+1;	//when up=2 move straight
	}
	else  
	{
		if(up==1) x=x+13;
		else if(up==0) x=x-15; //do the maths ;
		else if (up==2) x=x-1;
	}
	x %= SCREEN_DIMENSION_X * SCREEN_DIMENSION_Y;		//makes sure the ball doesn't go out of the screen.
	*location = x;
}

bat_t* bat_new_instance()
{//?
	bat_t *ret = pvPortMalloc(sizeof *ret);
	if(ret == NULL)
		return ret;

	ret->length = 0;

	return ret;
}
void init_game(bat_t **bats)
{
	
	bats[PL_ONE]->length = 3;//
	bats[PL_ONE]->pixels[0] = xy_to_pixel_id(1,2);
	bats[PL_ONE]->pixels[1] = xy_to_pixel_id(1,1);
	bats[PL_ONE]->pixels[2] = xy_to_pixel_id(1, 0);


	bats[PL_TWO]->length = 3;//
	bats[PL_TWO]->pixels[0] = xy_to_pixel_id(12, 9);
	bats[PL_TWO]->pixels[1] = xy_to_pixel_id(12, 8);
	bats[PL_TWO]->pixels[2] = xy_to_pixel_id(12, 7);
}

void clear_screen(uint16_t* framebuffer)
{
	for(uint8_t x = 0; x < SCREEN_DIMENSION_X; x++)
		framebuffer[x] = 0;
}

uint8_t y_offset(uint8_t pixel_id)
{
	return pixel_id / SCREEN_DIMENSION_X;
}
uint8_t x_offset(uint8_t pixel_id)
{
	return pixel_id % SCREEN_DIMENSION_X;
}

void draw_game(uint16_t* framebuffer, bat_t **bats, uint8_t ball_location)
{
	framebuffer[x_offset(ball_location)] |=  _BV((SCREEN_DIMENSION_Y - (y_offset(ball_location)+1)));
	for(Player_t player = 0; player < 2; player++)
	{
		for(uint8_t i = 0; i < bats[player]->length; i++)
		{
			framebuffer[x_offset(bats[player]->pixels[i])] |= _BV((SCREEN_DIMENSION_Y - (y_offset(bats[player]->pixels[i])+1)));
		}
	}
}

void draw_scores(uint16_t* framebuffer, uint8_t score_right, uint8_t score_left)
{			
	if(score_right==1)
	{
		for(uint8_t i = 31; i <=101; i++)
		{
			if((i-3)%14==0)
			framebuffer[x_offset(i)] |= _BV((SCREEN_DIMENSION_Y - (y_offset(i)+1)));
		}
	}
	else if(score_right==2)
		{
			for(uint8_t i = 31; i <=103; i++)
			{
				if((i-3)%14==0)
				framebuffer[x_offset(i)] |= _BV((SCREEN_DIMENSION_Y - (y_offset(i)+1)));
			
				if((i-5)%14==0)
				framebuffer[x_offset(i)] |= _BV((SCREEN_DIMENSION_Y - (y_offset(i)+1)));

			}
		}
		
	
	
		if(score_left==1)
		{
			for(uint8_t i = 37; i <=107; i++)
			{
				if((i-9)%14==0)
				framebuffer[x_offset(i)] |= _BV((SCREEN_DIMENSION_Y - (y_offset(i)+1)));
			}
		}
		else if(score_left==2)
		{
			for(uint8_t i = 37; i <=109; i++)
			{
				if((i-9)%14==0)
				framebuffer[x_offset(i)] |= _BV((SCREEN_DIMENSION_Y - (y_offset(i)+1)));
				if((i-11)%14==0)
				framebuffer[x_offset(i)] |= _BV((SCREEN_DIMENSION_Y - (y_offset(i)+1)));
			}
		}
		
		
		
	
}

	void hide_game(uint16_t* framebuffer, bat_t **bats)
	{
		for(int i=0;i<=2;i++)
			{
				bata[i]=bats[0]->pixels[i];
				batb[i]=bats[0]->pixels[i];
				bats[0]->pixels[i] = 21;
				bats[1]->pixels[i] = 21;
			}
	
			
		
	}
	
	void unhide_game(uint16_t* framebuffer, bat_t **bats)
	{
		for(int i=0;i<=2;i++)
		{
			bats[0]->pixels[i] = bata[i];
			bats[1]->pixels[i] = batb[i];
		}
		
		
		
	}
	