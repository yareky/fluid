#include <allegro.h>
#include <stdio.h>
#include <math.h>
#define FRAND (((rand()%2000)-1000)/1000.0f)
#include <algorithm>
#include "CFluidOpt.h"

//#include "fluid.h"
//#define N 200
//#define size ((N+2)*(N+2))


BITMAP *bufor;
 clock_t s,k;
 float czas=0,czas2=0;
 float fps_time, fps;
 int fps_counter;

//float u[size], v[size], u_prev[size], v_prev[size], dens[size], dens_prev[size];


void draw_dens(CFluidOpt *f, BITMAP *d)
{
    for(int i=0; i< f->GetN(); i++)
    for(int j=0; j< f->GetN(); j++)
    {
        int c = (int)( f->dens[f->IX(i,j)]*100);

        if(c<0) c=0;
        else if(c>255) c= 255;


        ((long *)d->line[j])[i] = makecol32(c,c,c);
    }
}

void draw_mouse()
{
    line(bufor, mouse_x-5, mouse_y, mouse_x+5, mouse_y, makecol(255,0,0));
    line(bufor, mouse_x, mouse_y-5, mouse_x, mouse_y+5, makecol(255,0,0));
}


int main(void)
{
        allegro_init();

		install_mouse();
		install_keyboard();


		set_color_depth(32);

		if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0)<0)
		{
		 allegro_message("Error: %s\n",allegro_error);
		 return(1);
		}


		bufor = create_bitmap(SCREEN_W, SCREEN_H);
		BITMAP *smoke = create_bitmap(128, 128);

        CFluidOpt fluid(256);


		s=clock();
		while( !key[KEY_ESC])
		{
		//-------------
		k=clock();
		czas=(float)(k-s)/CLOCKS_PER_SEC;
		s=clock();
		czas2+=czas;
		fps_time+=czas;
		//-------------
		fps_counter++;

		clear_to_color(bufor, makecol(128,255,128));


            //vel_step ( N, u, v, u_prev, v_prev, 0, czas );
            //dens_step ( N, dens, dens_prev, u, v, 0, czas );
            fluid.VelStep(0, czas );
            fluid.DensStep(0, czas );
            draw_dens(&fluid, bufor);

            if(mouse_b == 1)
            {
                fluid.AddGas(mouse_x, mouse_y, 10);
            }

            if(mouse_b == 2)
            {
                fluid.AddBlow(mouse_x, mouse_y);
            }

            if(fps_time>=1)
            {
                fps = fps_counter/fps_time;
                fps_counter=0;
                fps_time = 0;
            }

            //stretch_blit(smoke, bufor, 0, 0, smoke->w, smoke->h, 0, 0, bufor->w, bufor->h);
            textprintf_ex(bufor,font, 10, 10, 0xffffff, 0, "fps: %.1f", fps);
            draw_mouse();
            blit(bufor, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
		}

		destroy_bitmap(bufor);
        return 0;
}
END_OF_MAIN()
