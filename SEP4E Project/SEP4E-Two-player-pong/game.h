#ifndef GAME_H_
#define GAME_H_
#include <stdint.h>
#include <stdbool.h>
#define GAME_RENDERER_TASK_PERIOD 25
#define SCREEN_DIMENSION_X 14
#define SCREEN_DIMENSION_Y 10

typedef enum
{
	D_DOWN = 1,
	D_UP = 2,  //3
	
} Direction_t;
typedef enum
{
	PL_ONE = 0,
	PL_TWO = 1,
} Player_t;
typedef struct bat
{
	uint8_t length;//
	uint8_t pixels[3];
} bat_t;

void restart_game();
void bat_move(Player_t player, Direction_t direction);
uint8_t xy_to_pixel_id(uint8_t x, uint8_t y);
void game_renderer_task(void *pvParameters);
bat_t* bat_new_instance();//?
void init_game(bat_t **snakes);
void clear_screen(uint16_t* framebuffer);
uint8_t y_offset(uint8_t pixel_id);//?
uint8_t x_offset(uint8_t pixel_id);//?
void draw_game(uint16_t* framebuffer,bat_t **snakes, uint8_t food_location);
#endif /* GAME_H_ */