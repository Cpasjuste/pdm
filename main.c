#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/io.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>

#include <libconfig.h>

#include <GLES2D/GLES2D.h>

#include "config.h"
#include "main.h"

#define PIDFILE "/var/run/pdm.pid"
#define BUF_LEN 32

const int pnd_button_1 = 1, pnd_button_2 = 2, pnd_button_L = 4, pnd_button_R = 5, \
	pnd_button_SELECT = 8, pnd_button_START = 9;

static int alpha, alpha_up, selected_item, scroll_count;

GLES2D_TTFont *font;
GLES2D_Texture *background, *highlight, *icon_bg[15];

int write_pid()
{
	int fd = open (PIDFILE, O_WRONLY | O_CREAT, 0640);

	if (fd < 0)
	{
		perror ("open");
		return 0;
	}
	else
	{
		char buf[BUF_LEN];
		snprintf (buf, sizeof(buf), "%d\n", getpid());
		int ret = write (fd, buf, strlen(buf));

		if (ret < 0)
		{
			perror ("write");
			return 0;
		}

		if (close (fd))
		{
			perror ("close");
			return 0;
		}

	}
	return 1;
}

int gui_init()
{
	putenv ("SDL_MOUSEDRV=TSLIB");
	GLES2D_InitVideoMode( 800, 480, 1, 1, 0 );	
}

void gui_load()
{
	int i;

	if ( ( background = GLES2D_LoadTexture( "/pdm/data/gui/backg.png", RGBA8888  ) ) == NULL )
		printf("GLES2D_LoadTexture: Unable to load ""/pdm/data/gui/backg.png""\n" );

	if ( ( highlight = GLES2D_LoadTexture( "/pdm/data/gui/highlight.png", RGBA8888  ) ) == NULL )
		printf("GLES2D_LoadTexture: Unable to load ""/pdm/data/gui/highlight.png""\n" );
	
	for(i = 0; i < general->items_count; i++ )
	{
		icon_bg[i] = GLES2D_LoadTexture( item->icon_path[i], RGBA8888 );
		item->icon_w[i] = icon_bg[i]->w;
		item->icon_h[i] = icon_bg[i]->h;
	}

	font = GLES2D_LoadTTFont( "/pdm/data/gui/font.ttf", 16 );

	scroll_count = 600;
}

void gui_clean()
{
	int i;
	for(i = 0; i < general->items_count; i++ )
		GLES2D_FreeTexture( icon_bg[i] );

	GLES2D_FreeTexture( background );
	GLES2D_FreeTexture( highlight );
	GLES2D_FreeTTFont( font ); 
	GLES2D_Quit();
}

void gui_draw()
{
	int i;

	GLES2D_DrawTextureSimple( background, 0, 0 );

	for(i = 0; i < general->items_count; i++ )
	{
		if(i == selected_item)
		{
			if( alpha < 170 && alpha_up == 1)
			{
				alpha += 2;
				if(alpha == 168) alpha_up = 0;
					else alpha_up = 1;
			}
			else if( alpha > 100 && alpha_up != 1)
			{
				alpha -= 2;
				if(alpha == 102) alpha_up = 1;
					else alpha_up = 0;
			}
			GLES2D_SetTextureAlpha( highlight, alpha );
			GLES2D_DrawTextureCentered( highlight, item->icon_x[i], item->icon_y[i] );
		}
		GLES2D_DrawTextureCentered( icon_bg[i], item->icon_x[i], item->icon_y[i] );	
		GLES2D_DrawTextScroll( font, item->name[selected_item], scroll_count, general->scrolling_text_y, 175, 600 );	
	}

	scroll_count--;
	if ( scroll_count < ( 175 - GLES2D_GetTextWidth( font, item->name[selected_item] ) ) )
		scroll_count = 600;

	GLES2D_SwapBuffers();
}

void gui_quit_exec(int n)
{
	gui_clean();

	free(item);
	free(general);

	execl(item->exec_path[n], item->exec_path[n], NULL);
	
	exit(0);
}

void gui_app_exec(int n)
{
	pid_t childpid;
	int status;

	gui_clean();

	if ((childpid = fork()) == -1)
	{
		perror("Error in the fork");
	}
	else if (childpid == 0)
	{
   		if (execl(item->exec_path[n], item->exec_path[n], NULL) < 0)
		{
			perror("Exec failed");
		}
	}
	else if (childpid != wait(&status))
	{ 
		perror("A signal occurred before the child exited");
	}

	gui_init();
	gui_load();
}


int main(char *argc, char *argv[])
{
	int gui_done = 0;
	selected_item = 0;
	scroll_count = 800;
	alpha_up = 1;
	alpha = 0;

	write_pid();
	
	cfg_read("/pdm/data/pdm.conf");

	if(general->item_to_boot >= 0)
	{
		execl(item->exec_path[general->item_to_boot], item->exec_path[general->item_to_boot], NULL);
		free(item);
		free(general);
		exit(0);
	}

	gui_init();
	gui_load();

	while(!gui_done)
	{
		gui_draw();
		
		GLES2D_HandleEvents( 90 );

		if ( GLES2D_PadPressed ( MENU ) )
		{
//			gui_done = 1;
		}

		if ( GLES2D_PadPressed ( PAD_LEFT ) )
		{
			scroll_count = 600;

			if ( selected_item > 0 )
				selected_item--;
		}

		if ( GLES2D_PadPressed ( PAD_RIGHT ) )
		{
			scroll_count = 600;

			if ( selected_item < general->items_count-1 )
				selected_item++;
		}

		if ( GLES2D_PadPressed ( B ) )
		{
			if(item->standalone[selected_item]) gui_quit_exec(selected_item);
				else gui_app_exec(selected_item);
		}
		

/*
			while( SDL_PollEvent( &event ) )
			{
				int x = 0, y = 0;
				if( event.type == SDL_MOUSEBUTTONDOWN )
				{
					if( event.button.button == SDL_BUTTON_LEFT )
					{
						x = event.button.x;
						y = event.button.y;

						int i;
						for(i = 0; i < general->items_count; i++ )
						{
							if(( x > (item->icon_x[i] - (item->icon_w[i] / 2 ))) \
							&& ( x < (item->icon_x[i] + (item->icon_w[i] / 2 ))) \
							&& ( y > (item->icon_y[i] - (item->icon_h[i] / 2 ))) \
							&& ( y < (item->icon_y[i] + (item->icon_h[i] / 2 ))))
							{
							 	printf("Item : %i\nName : %s\nPath : %s\n\n", i, item->name[i], item->exec_path[i]);

								if(selected_item == i)
									if(item->standalone[i]) gui_quit_exec(i);
										else gui_app_exec(i);

								alpha = 0;
								alpha_up = 1;
								selected_item = i;
								scroll_count = 800;
							}
						}
	 				}
				}

				if( event.type == SDL_QUIT )
				{
					gui_quit();
				}
			 }
*/
	}	

	gui_clean();
	free(item);
	free(general);

	return 0;
}

