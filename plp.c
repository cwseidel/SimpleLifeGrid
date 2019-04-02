// Code to drive the 32x32 LED arrays available from SKPANG 
// http://skpang.co.uk/catalog/rgb-led-panel-32x32-p-1329.html
// Based on code from https://github.com/hzeller/rpi-rgb-led-matrix
// Which contains the following copyright notice:
// Code is (c) Henner Zeller h.zeller@acm.org, and I grant you the
// permission to do whatever you want with it :)

// This code is (c) Peter Onion (Peter.Onion@btinternet.com), and I too grant you the
// permission to do whatever you want with it, as long as this header block
// is retained in any code you may distribute that uses or is based on this code.
// Life Simulator game is (c) of Ferran Fabregas (ferri.fc@gmail.com) 
// gcc -o PanelLifeProject PanelLifeProject.c -lm -lpthread


// from c-example
#include "led-matrix-c.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
//


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>

#include <sys/mman.h>
#include <termios.h>
#include <signal.h>
#include <math.h>
#include <time.h>


#define PLANTS_LIFE_EXPECTANCY 255
#define PLANTS_RANDOM_BORN_CHANCES 1000 // high is less chances
#define PLANTS_RANDOM_NEARBORN_CHANCES 100
#define PLANTS_RANDOM_DIE_CHANCES 2
#define PLANTS_ENERGY_BASE_PER_CYCLE 10

#define SPECIE1_LIFE_EXPECTANCY 200
#define SPECIE1_RANDOM_BORN_CHANCES 10000
#define SPECIE1_RANDOM_NEARBORN_CHANCES 100
#define SPECIE1_RANDOM_DIE_CHANCES 2
#define SPECIE1_ENERGY_BASE 10
#define SPECIE1_ENERGY_NEEDED_PER_CYCLE 2
#define SPECIE1_MAX_ENERGY_RECOLECTED_PER_CYCLE 10
#define SPECIE1_ENERGY_TO_REPLICATE 15

#define SPECIE2_LIFE_EXPECTANCY 180
#define SPECIE2_RANDOM_BORN_CHANCES 10000
#define SPECIE2_RANDOM_NEARBORN_CHANCES 100
#define SPECIE2_RANDOM_DIE_CHANCES 2
#define SPECIE2_ENERGY_BASE 10
#define SPECIE2_ENERGY_NEEDED_PER_CYCLE 2
#define SPECIE2_MAX_ENERGY_RECOLECTED_PER_CYCLE 20
#define SPECIE2_ENERGY_TO_REPLICATE 11





int main(int argc, char **argv) {
  struct RGBLedMatrixOptions options;
  struct RGBLedMatrix *matrix;
  struct LedCanvas *offscreen_canvas;
  int width, height;
  int x, y, i;

  memset(&options, 0, sizeof(options));
  options.rows = 32;
  options.chain_length = 1;

  /* This supports all the led commandline options. Try --led-help */
  matrix = led_matrix_create_from_options(&options, &argc, &argv);
  if (matrix == NULL)
    return 1;

  /* Let's do an example with double-buffering. We create one extra
   * buffer onto which we draw, which is then swapped on each refresh.
   * This is typically a good aproach for animations and such.
   */
  offscreen_canvas = led_matrix_create_offscreen_canvas(matrix);

  led_canvas_get_size(offscreen_canvas, &width, &height);

  fprintf(stderr, "Size: %dx%d. Hardware gpio mapping: %s\n",
          width, height, options.hardware_mapping);

//void *startLifeSimulatorThread(void *args)
//{
	typedef struct plants
	{
   		int age;
   		int energy;
	} PLANT;

	typedef struct species
	{
   		int age;
   		int energy;
	} SPECIE;

	PLANT plantes[32][32];
	SPECIE specie1[32][32];
	SPECIE specie2[32][32];
	
	int x,y,xp,xm,yp,ym;
	int plants_neighbours,specie1_neighbours,specie2_neighbours;
	int i;
	int available[8];
	memset(available,0,sizeof(available));
	int pos;
	int random_number;
	int rand_pos;
	int loopcount=0;
	int total_energy;

	// Clear the board
	memset(plantes,0,sizeof(plantes));
	memset(specie1,0,sizeof(specie1));
	memset(specie2,0,sizeof(specie2));
	
	srandom(time(NULL));
	while (1) { // bucle principal
		for(x=0;x<32;x++) {
			// Calculate adjacent coordinates with correct wrap at edges
			xp = (x + 1) & 31;
			xm = (x - 1) & 31;

			for(y=0;y<32;y++) {
				yp = (y + 1) & 31;
				ym = (y - 1) & 31;
				//printf("%i\n",loopcount);
				loopcount++;	
				// Count the number of currently live neighbouring cells
				plants_neighbours=0;
				specie1_neighbours=0;
				specie2_neighbours=0;
				// [Plants]
				if (plantes[x][y].age==0 && plantes[xm][y].age>0) { plants_neighbours++; }
				if (plantes[x][y].age==0 && plantes[xp][y].age>0) { plants_neighbours++; }
				if (plantes[x][y].age==0 && plantes[xm][ym].age>0) { plants_neighbours++; }
				if (plantes[x][y].age==0 && plantes[x][ym].age>0) { plants_neighbours++; }
				if (plantes[x][y].age==0 && plantes[xp][ym].age>0) { plants_neighbours++; }
				if (plantes[x][y].age==0 && plantes[xm][yp].age>0) { plants_neighbours++; }
				if (plantes[x][y].age==0 && plantes[x][yp].age>0) { plants_neighbours++; }
				if (plantes[x][y].age==0 && plantes[xp][yp].age>0) { plants_neighbours++; }
				// [Specie1]
				if (specie1[x][y].age==0 && specie1[xm][y].age>0) { specie1_neighbours++; }
				if (specie1[x][y].age==0 && specie1[xp][y].age>0) { specie1_neighbours++; }
				if (specie1[x][y].age==0 && specie1[xm][ym].age>0) { specie1_neighbours++; }
				if (specie1[x][y].age==0 && specie1[x][ym].age>0) { specie1_neighbours++; }
				if (specie1[x][y].age==0 && specie1[xp][ym].age>0) { specie1_neighbours++; }
				if (specie1[x][y].age==0 && specie1[xm][yp].age>0) { specie1_neighbours++; }
				if (specie1[x][y].age==0 && specie1[x][yp].age>0) { specie1_neighbours++; }
				if (specie1[x][y].age==0 && specie1[xp][yp].age>0) { specie1_neighbours++; }
				// [Specie2]
				if (specie2[x][y].age==0 && specie2[xm][y].age>0) { specie2_neighbours++; }
				if (specie2[x][y].age==0 && specie2[xp][y].age>0) { specie2_neighbours++; }
				if (specie2[x][y].age==0 && specie2[xm][ym].age>0) { specie2_neighbours++; }
				if (specie2[x][y].age==0 && specie2[x][ym].age>0) { specie2_neighbours++; }
				if (specie2[x][y].age==0 && specie2[xp][ym].age>0) { specie2_neighbours++; }
				if (specie2[x][y].age==0 && specie2[xm][yp].age>0) { specie2_neighbours++; }
				if (specie2[x][y].age==0 && specie2[x][yp].age>0) { specie2_neighbours++; }
				if (specie2[x][y].age==0 && specie2[xp][yp].age>0) { specie2_neighbours++; }	
				
				// Plants logic
				if (plantes[x][y].age>=PLANTS_LIFE_EXPECTANCY) { plantes[x][y].age=0; plantes[x][y].energy=0;  } // plant dies
				if (plantes[x][y].age>0 && plantes[x][y].age<PLANTS_LIFE_EXPECTANCY && plantes[x][y].energy<=0) { plantes[x][y].age=0; plantes[x][y].energy=0; } // plant dies
				if (plantes[x][y].age>0 && plantes[x][y].age<PLANTS_LIFE_EXPECTANCY ) { plantes[x][y].age++; plantes[x][y].energy=plantes[x][y].energy+PLANTS_ENERGY_BASE_PER_CYCLE; } // plant grows
				if (plantes[x][y].age==0 && plants_neighbours==0) { // no neighbours plant born
					//srand(time(NULL));
					random_number = random() % PLANTS_RANDOM_BORN_CHANCES;
					//printf("%i\n",random_number);
					if (random_number==1) { plantes[x][y].age=1; plantes[x][y].energy=1;}
				} 
				if (plantes[x][y].age==0 && plants_neighbours>0) {  // neighbours plant born
					//srand(time(NULL));
					random_number = random() % PLANTS_RANDOM_NEARBORN_CHANCES;
					if (random_number==1) { plantes[x][y].age=1; plantes[x][y].energy=1; }
				}
				
				// Specie1 logic
				if (specie1[x][y].age>0) { // if there are an individual alive
					// try to eat
					if (plantes[x][y].energy>0) { 
						total_energy=0;
						if (plantes[x][y].energy>SPECIE1_MAX_ENERGY_RECOLECTED_PER_CYCLE) { total_energy=SPECIE1_MAX_ENERGY_RECOLECTED_PER_CYCLE; plantes[x][y].energy=plantes[x][y].energy-SPECIE1_MAX_ENERGY_RECOLECTED_PER_CYCLE; }
					else { total_energy=plantes[x][y].energy; plantes[x][y].energy=0;}
					specie1[x][y].energy=specie1[x][y].energy+total_energy;
					}
					// grow and decrease energy
					specie1[x][y].age++;
					specie1[x][y].energy=specie1[x][y].energy-SPECIE1_ENERGY_NEEDED_PER_CYCLE;
					if (specie1[x][y].energy<0) { specie1[x][y].energy=0; specie1[x][y].age=0;} // die
					// try to replicate
					if (specie1[x][y].energy>SPECIE1_ENERGY_TO_REPLICATE) {
						for (i=0;i<8;i++) { available[i]=0; }
						pos=0;
						//srand(time(NULL));
						random_number = random() % SPECIE1_RANDOM_NEARBORN_CHANCES;
						if (specie1[xm][y].age==0) { available[pos]=1; pos++; }
						if (specie1[xp][y].age==0) { available[pos]=2; pos++; }
						if (specie1[xm][ym].age==0) { available[pos]=3; pos++; }
						if (specie1[x][ym].age==0) { available[pos]=4; pos++; }
						if (specie1[xp][ym].age==0) { available[pos]=5; pos++; }
						if (specie1[xm][yp].age==0) { available[pos]=6; pos++; }
						if (specie1[x][yp].age==0) { available[pos]=7; pos++; }
						if (specie1[xp][yp].age==0) { available[pos]=8; pos++; }
						//srand(time(NULL));
						if (pos>0) {
							rand_pos=random() % pos;
							switch (available[rand_pos]) { // one individual born radomly
								case 1: if (random_number==1) { specie1[xm][y].age=1; specie1[xm][y].energy=SPECIE1_ENERGY_BASE; } break;
								case 2: if (random_number==1) { specie1[xp][y].age=1; specie1[xp][y].energy=SPECIE1_ENERGY_BASE; } break;
								case 3: if (random_number==1) { specie1[xm][ym].age=1; specie1[xm][ym].energy=SPECIE1_ENERGY_BASE; } break;
								case 4: if (random_number==1) { specie1[x][ym].age=1; specie1[x][ym].energy=SPECIE1_ENERGY_BASE; } break;
								case 5: if (random_number==1) { specie1[xp][ym].age=1; specie1[xp][ym].energy=SPECIE1_ENERGY_BASE; } break;
								case 6: if (random_number==1) { specie1[xm][yp].age=1; specie1[xm][yp].energy=SPECIE1_ENERGY_BASE; } break;
								case 7: if (random_number==1) { specie1[x][yp].age=1; specie1[x][yp].energy=SPECIE1_ENERGY_BASE; } break;
								case 8: if (random_number==1) { specie1[xp][yp].age=1; specie1[xp][yp].energy=SPECIE1_ENERGY_BASE; } break;
								default: printf("ERROR\n"); break; // all full
							}
						}
					}
					// die if too old
					if (specie1[x][y].age>SPECIE1_LIFE_EXPECTANCY) { specie1[x][y].energy=0; specie1[x][y].age=0;}
				}
				if (specie1[x][y].age==0) { // if theres no individual, new individual will born? (to avoid extintion)
					if (specie1_neighbours==0) {
						//srand(time(NULL));
						random_number = random() % SPECIE1_RANDOM_BORN_CHANCES;
						if (random_number==1) { specie1[x][y].age=1; specie1[x][y].energy=SPECIE1_ENERGY_BASE; }
					}
				}
				
				// Specie2 logic
				
				if (specie2[x][y].age>0) { // if there are an individual alive
					// try to eat
					if (plantes[x][y].energy>0) { 
						total_energy=0;
						if (plantes[x][y].energy>SPECIE2_MAX_ENERGY_RECOLECTED_PER_CYCLE) { total_energy=SPECIE2_MAX_ENERGY_RECOLECTED_PER_CYCLE; plantes[x][y].energy=plantes[x][y].energy-SPECIE2_MAX_ENERGY_RECOLECTED_PER_CYCLE; }
						else { total_energy=plantes[x][y].energy; plantes[x][y].energy=0;}
						specie2[x][y].energy=specie2[x][y].energy+total_energy;
					}
					// grow and decrease energy
					specie2[x][y].age++;
					specie2[x][y].energy=specie2[x][y].energy-SPECIE2_ENERGY_NEEDED_PER_CYCLE;
					if (specie2[x][y].energy<0) { specie2[x][y].energy=0; specie2[x][y].age=0;} // die
					// try to replicate
					if (specie2[x][y].energy>SPECIE2_ENERGY_TO_REPLICATE) {
						for (i=0;i<8;i++) { available[i]=0; }
						pos=0;
						//srand(time(NULL));
						random_number = random() % SPECIE2_RANDOM_NEARBORN_CHANCES;
						if (specie2[xm][y].age==0) { available[pos]=1; pos++; }
						if (specie2[xp][y].age==0) { available[pos]=2; pos++; }
						if (specie2[xm][ym].age==0) { available[pos]=3; pos++; }
						if (specie2[x][ym].age==0) { available[pos]=4; pos++; }
						if (specie2[xp][ym].age==0) { available[pos]=5; pos++; }
						if (specie2[xm][yp].age==0) { available[pos]=6; pos++; }
						if (specie2[x][yp].age==0) { available[pos]=7; pos++; }
						if (specie2[xp][yp].age==0) { available[pos]=8; pos++; }
						//srand(time(NULL));
						if (pos>0) {
							rand_pos=random() % pos;
							switch (available[rand_pos]) { // one individual born radomly
								case 1: if (random_number==1) { specie2[xm][y].age=1; specie2[xm][y].energy=SPECIE2_ENERGY_BASE; } break;
								case 2: if (random_number==1) { specie2[xp][y].age=1; specie2[xp][y].energy=SPECIE2_ENERGY_BASE; } break;
								case 3: if (random_number==1) { specie2[xm][ym].age=1; specie2[xm][ym].energy=SPECIE2_ENERGY_BASE; }break;
								case 4: if (random_number==1) { specie2[x][ym].age=1; specie2[x][ym].energy=SPECIE2_ENERGY_BASE; } break;
								case 5: if (random_number==1) { specie2[xp][ym].age=1; specie2[xp][ym].energy=SPECIE2_ENERGY_BASE; } break;
								case 6: if (random_number==1) { specie2[xm][yp].age=1; specie2[xm][yp].energy=SPECIE2_ENERGY_BASE; } break;
								case 7: if (random_number==1) { specie2[x][yp].age=1; specie2[x][yp].energy=SPECIE2_ENERGY_BASE; } break;
								case 8: if (random_number==1) { specie2[xp][yp].age=1; specie2[xp][yp].energy=SPECIE2_ENERGY_BASE; } break;
								// default: break; // all full
							}
						}
					}
				// die if too old
				if (specie2[x][y].age>SPECIE2_LIFE_EXPECTANCY) { specie2[x][y].energy=0; specie2[x][y].age=0;}
				}
				if (specie2[x][y].age==0) { // if theres no individual, new individual will born? (to avoid extintion)
					if (specie2_neighbours==0) {
						//srand(time(NULL));
						random_number = random() % SPECIE2_RANDOM_BORN_CHANCES;
						if (random_number==1) { specie2[x][y].age=1; specie2[x][y].energy=SPECIE2_ENERGY_BASE; }
					}
				}
				
				// draw
				if (specie1[x][y].age>0 && specie2[x][y].age>0) { led_canvas_set_pixel(offscreen_canvas,x,y,100,100,0); } // yellow if species comp
				if (specie1[x][y].age>0 && specie2[x][y].age==0) { led_canvas_set_pixel(offscreen_canvas,x,y,100,0,0); } // red only specie1
				if (specie1[x][y].age==0 && specie2[x][y].age>0) { led_canvas_set_pixel(offscreen_canvas,x,y,0,0,100); } // blue only specie2
				if (specie1[x][y].age==0 && specie2[x][y].age==0 && plantes[x][y].age>0) { led_canvas_set_pixel(offscreen_canvas,x,y,0,100,0); } // green only plants
				if (specie1[x][y].age==0 && specie2[x][y].age==0 && plantes[x][y].age==0) { led_canvas_set_pixel(offscreen_canvas,x,y,0,0,0); } // black nothing
			}
		}
				
		//swapBuffers();
		offscreen_canvas = led_matrix_swap_on_vsync(matrix, offscreen_canvas);
		//memset(PixelsB,0,sizeof(Pixels1));
		// Sleep to set game rate
		usleep(1000 *50);
	}
printf("Surt del bucle\n");
//}


/*
 * Make sure to always call led_matrix_delete() in the end to reset the
 * display. Installing signal handlers for defined exit is a good idea.
 */
 led_matrix_delete(matrix);

 return 0;
}


