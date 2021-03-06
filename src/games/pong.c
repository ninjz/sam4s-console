#include "game.h"

#include "controllers/ps_controller.h"
#include "games/pong.h"

short p1_x;
short p1_y;
short p1_yPrev;

short p2_x;
short p2_y;
short p2_yPrev;


short ball_xPrev;
short ball_yPrev;
short ball_x;
short ball_y;
short ball_xdir;
short ball_ydir;

unsigned char p1_score;
unsigned char p2_score;

char server;
char is_served;

char game_state;
char menu_drawn;

void pong_setup(void){
	
	if(p1_score == 10 || p2_score == 10){
		game_state = 2;
		menu_drawn= 0;
		p1_score = p2_score = 0;
	}
	
	p1_x = 30;
	p1_y = SCREEN_HEIGHT/2 - PADDLE_LEN/2;
	p1_yPrev = p1_y;
	
	p2_x = SCREEN_WIDTH - 30;
	p2_y = SCREEN_HEIGHT/2 - PADDLE_LEN/2;
	p2_yPrev = p2_y;

	ball_x = p1_x + 1;
	ball_y = p1_y + PADDLE_LEN/2;

	ball_xPrev = ball_x;
	ball_yPrev = ball_y;
	
	server = 1;
	is_served = 0;

	vga_clear_screen(COLOR_MAGENTA);
	vga_draw_character(156, 15, COLOR_GREEN, p1_score + 48);
	vga_draw_character(164, 15, COLOR_WHITE, '|');
	vga_draw_character(172, 15, COLOR_GREEN, p2_score + 48);

	//BORDER DRAW
	vga_draw_line(0, 0, SCREEN_WIDTH, 0, BORDER_COLOR);
	vga_draw_line(0, 0, 0, SCREEN_HEIGHT, BORDER_COLOR);
	vga_draw_line(SCREEN_WIDTH-1, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT, BORDER_COLOR);
	vga_draw_line(0, SCREEN_HEIGHT-1, SCREEN_WIDTH, SCREEN_HEIGHT-1, BORDER_COLOR);
}

void pong_main(void){
	
	game_state = 0;
	p1_score = p2_score = 0;
	menu_drawn = 0;

	char screen_flash = 0;
	int run_game = 1;
	
	unsigned char ball_color = 0;
	
	while(run_game){
		
		if(controller_is_down(0, ps_select)){
			run_game = 0;
		}
		
		if(game_state == 0){
			screen_flash++;
			vga_draw_string(SCREEN_WIDTH/2 - 16, SCREEN_HEIGHT/2 - 10, COLOR_WHITE, "PONG", 4);
			vga_draw_string(SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2, screen_flash, "Press Start", 11);
			
			if(menu_drawn == 0){
				vga_clear_screen(COLOR_BLACK);
				menu_drawn = 1;
				sleep_frames(45);
			}else if(controller_is_down(0, ps_start)){ // game start from 0
				pong_setup();
				p1_score = p2_score = 0;
				server = 1;
				game_state = 1;
				sleep_frames(45);
			}
			
		}else if(game_state == 1){
			ball_xPrev = ball_x;
			ball_yPrev = ball_y;
			p1_yPrev = p1_y;
			p2_yPrev = p2_y;

			if(controller_is_down(0, ps_start)){
				game_state = 0;
				menu_drawn = 0;
			}
			
			//PLayer 1 controls
			if(controller_is_down(0, ps_dpad_up)){
				p1_y--;

			}else if(controller_is_down(0, ps_dpad_down)){
				p1_y++;
			}

			//Player 2 controls
			if(controller_is_down(1, ps_dpad_up)){
				p2_y--;

			}else if(controller_is_down(1, ps_dpad_down)){
				p2_y++;
			}

			if(is_served == 0 && server == 1 && controller_is_down(0, ps_x)){
				if(controller_is_down(0, ps_dpad_down)){
					ball_xdir = 1;
					ball_ydir = 1;
				} else if(controller_is_down(0, ps_dpad_up)){
					ball_xdir = 1;
					ball_ydir = -1;
				} else {
					ball_xdir = 1;
					ball_ydir = 0;
				}

				is_served = 1;
			} else if(is_served == 0 && server == 2 && controller_is_down(1, ps_x)){
				if(controller_is_down(1, ps_dpad_down)){
					ball_xdir = -1;
					ball_ydir = 1;
				} else if(controller_is_down(1, ps_dpad_up)){
					ball_xdir = -1;
					ball_ydir = -1;
				} else {
					ball_xdir = -1;
					ball_ydir = 0;
				}

				is_served = 1;
			}



			//BORDER check.
			if(vga_get_pixel(p1_x, p1_y) == BORDER_COLOR){
				p1_y = p1_yPrev;
			}else if(vga_get_pixel(p1_x, p1_y+PADDLE_LEN) == BORDER_COLOR){
				p1_y = p1_yPrev;
			}
		
			if(vga_get_pixel(p2_x, p2_y) == BORDER_COLOR){
				p2_y = p2_yPrev;
			}else if(vga_get_pixel(p2_x, p2_y+PADDLE_LEN) == BORDER_COLOR){
				p2_y = p2_yPrev;
			}




			if(is_served){
				//ball is moving.
				ball_x += ball_xdir;
				ball_y += ball_ydir;

				//BALL COLLISION
				if(ball_x <= 2){
					p2_score++;
					is_served = 0;
					//next server is the winner.
					server = 2;
					pong_setup();
				} else if(ball_x >= SCREEN_WIDTH - 2){
					p1_score++;
					is_served = 0;
					server = 1;
					pong_setup();
				}

				//hit border
				if(vga_get_pixel(ball_x, ball_y) == BORDER_COLOR){				
					if(ball_xdir == 1 && ball_ydir == 1){
						ball_y = SCREEN_HEIGHT - 2;
						ball_xdir = 1;
						ball_ydir = -1;
					} else if(ball_xdir == -1 && ball_ydir == 1){
						ball_y = SCREEN_HEIGHT - 2;
						ball_xdir = -1;
						ball_ydir = -1;
					} else if(ball_xdir == 1 && ball_ydir == -1){
						ball_y = 2;
						ball_xdir = 1;
						ball_ydir = 1;
					} else if(ball_xdir == -1 && ball_ydir == -1){
						ball_y = 2;
						ball_xdir = -1;
						ball_ydir = 1;
					}


				}

				// ball collide with player paddles
				if(vga_get_pixel(ball_x, ball_y) == PLAYER_ONE){
					ball_x = p1_x + 1;
					ball_xdir = 1;
					if(ball_ydir == -1){
						if(controller_is_down(0, ps_dpad_down)){
							ball_ydir = 1;
						} else if(controller_is_down(0, ps_dpad_up)){
							ball_ydir = -1;
						} else{
							ball_ydir = -1;
						}

					} else if(ball_ydir == 1){
						if(controller_is_down(0, ps_dpad_down)){
							ball_ydir = 1;
						} else if(controller_is_down(0, ps_dpad_up)){
							ball_ydir = -1;
						} else{
							ball_ydir = 1;
						}
					} else{
						if(controller_is_down(0, ps_dpad_down)){
							ball_ydir = 1;
						} else if(controller_is_down(0, ps_dpad_up)){
							ball_ydir = -1;
						} else{
							ball_ydir = 0;
						}
						
					}

				} else if(vga_get_pixel(ball_x, ball_y) == PLAYER_TWO){
					ball_x = p2_x - 1;
					ball_xdir = -1;
					if(ball_ydir == -1){
						if(controller_is_down(1, ps_dpad_down)){
							ball_ydir = -1;
						} else if(controller_is_down(1, ps_dpad_up)){
							ball_ydir = 1;
						} else{
							ball_ydir = -1;
						}

					} else if(ball_ydir == 1){
						if(controller_is_down(1, ps_dpad_down)){
							ball_ydir = 1;
						} else if(controller_is_down(1, ps_dpad_up)){
							ball_ydir = -1;
						} else{
							ball_ydir = 1;
						}
					} else{
						if(controller_is_down(1, ps_dpad_down)){
							ball_ydir = 1;
						} else if(controller_is_down(1, ps_dpad_up)){
							ball_ydir = -1;
						} else{
							ball_ydir = 0;
						}
					}
				}

			} else {
				if(server == 1){
					ball_x = p1_x + 1;
					ball_y = p1_y + PADDLE_LEN/2;
				} else { // server == 2
					ball_xPrev = ball_x;
					ball_yPrev = ball_y;

					ball_x = p2_x - 1;
					ball_y = p2_y + PADDLE_LEN/2;
				}
			}
		

			//DRAW PLAYERS
			vga_draw_line(p1_x, p1_yPrev, p1_x, p1_yPrev + PADDLE_LEN, COLOR_MAGENTA);  // draw current position black first
			vga_draw_line(p1_x, p1_y, p1_x, p1_y + PADDLE_LEN, PLAYER_ONE);			  // draw new position

			vga_draw_line(p2_x, p2_yPrev, p2_x, p2_yPrev + PADDLE_LEN, COLOR_MAGENTA);  // draw current position black first
			vga_draw_line(p2_x, p2_y, p2_x, p2_y + PADDLE_LEN, PLAYER_TWO);			  // draw new position

			//DRAW BALL
			vga_set_pixel(ball_xPrev, ball_yPrev, COLOR_BLACK);
			ball_color++;
			vga_set_pixel(ball_x, ball_y, ball_color);

		}else{
			if(menu_drawn == 0){
				if(p1_score == 10){
					vga_draw_string(SCREEN_WIDTH/2 - 32, SCREEN_HEIGHT/2, PLAYER_ONE, "PLAYER ONE WINS", 15);
				}else{
					vga_draw_string(SCREEN_WIDTH/2 - 44, SCREEN_HEIGHT/2, PLAYER_TWO, "PLAYER TWO WINS", 15);
				}
				
				menu_drawn = 1;
				sleep_frames(60);
			}else if(controller_is_down(0, ps_start)){
				game_state = 0;
				menu_drawn = 0;
			}
			
		}
		
		sleep_frame();
	}
}