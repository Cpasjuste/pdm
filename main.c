#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <libconfig.h>
#include <GLES2D/GLES2D.h>

#include "main.h"
#include "config.h"
#include "utils.h"

int gui_done = 0;

int gui_init()
{
	putenv ("SDL_MOUSEDRV=TSLIB");
	GLES2D_InitVideoMode( 800, 480, 1, 1, 0 );	
}

void gui_load()
{
	int i;

	if ( ( background = GLES2D_LoadTexture( pdm_dir_and( "data/gui/backg.png" ), RGBA8888  ) ) == NULL )
		printf("GLES2D_LoadTexture: Unable to load ""%s""\n", pdm_dir_and( "data/gui/backg.png" ) );

	if ( ( highlight = GLES2D_LoadTexture( pdm_dir_and( "data/gui/highlight.png" ), RGBA8888  ) ) == NULL )
		printf("GLES2D_LoadTexture: Unable to load ""%s""\n", pdm_dir_and( "data/gui/highlight.png" ) );
	
	for(i = 0; i < items_count; i++ )
	{
		printf("loading %s\n", item->icon_path[i] );
		icon_bg[i] = GLES2D_LoadTexture( item->icon_path[i], RGBA8888 );
	}

	font = GLES2D_LoadTTFont( pdm_dir_and( "data/gui/font.ttf" ), 16 );

	scroll_count = 600;

}

void gui_clean()
{
	int i;

	for(i = 0; i < items_count; i++ )
	{
		printf("freeing %s\n", item->icon_path[i] );
		GLES2D_FreeTexture( icon_bg[i] );
	}

	GLES2D_FreeTexture( background );
	GLES2D_FreeTexture( highlight );
	GLES2D_FreeTTFont( font ); 
	GLES2D_Quit();
}

void gui_quit_exec(int n)
{
	gui_clean();

	execl(item->exec_path[n], item->exec_path[n], NULL);

	free(item);
	free(general);
	
	exit(0);
}

void gui_app_exec(int n)
{
	pid_t childpid;
	int status;

	int lenght = strlen ( item->exec_path[n] );

	while ( item->exec_path[n][lenght] != '/' )
		lenght--;

	gui_clean();

	if ((childpid = fork()) == -1)
	{
		perror("Error in the fork");
	}
	else if (childpid == 0)
	{
   		if ( execl( pdm_dir_and("exec.sh"), pdm_dir_and("exec.sh"), item->exec_path[n], str_sub( item->exec_path[n], 0, lenght ), NULL) < 0 )
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

void gui_events()
{
	GLES2D_HandleEvents( 90 );

	if ( GLES2D_PadPressed ( PAD_LEFT ) )
	{
		scroll_count = 600;

		if ( selected_item > 0 )
			selected_item--;
	}

	if ( GLES2D_PadPressed ( PAD_RIGHT ) )
	{
		scroll_count = 600;

		if ( selected_item < items_count - 1 )
			selected_item++;
	}

	if ( GLES2D_PadPressed ( B ) )
	{
		if(item->standalone[selected_item]) gui_quit_exec(selected_item);
			else gui_app_exec(selected_item);
	}
}

void gui_draw()
{
	int i;

	GLES2D_DrawTextureSimple( background, 0, 0 );

	for(i = 0; i < items_count; i++ )
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

int main( int argc, char *argv[] )
{
	selected_item = 0;
	scroll_count = 800;
	alpha_up = 1;
	alpha = 0;

	if ( argc > 1 )
	{
		strcpy( pdm_dir, argv[1] );
		if ( pdm_dir[strlen(pdm_dir)-1] != '/' )
			strcat( pdm_dir, "/" );
	}
	else
	{
		fprintf(stderr, "Usage: %s pdm_exec_dir\n", argv[0] );
		exit(0);
	}

	write_pid();
	
	printf("reading configuration file: %s\n", pdm_dir_and("data/pdm.conf") );
	cfg_read( pdm_dir_and("data/pdm.conf") );

	if( general->autoboot_item >= 0 )
	{
		execl( item->exec_path[general->autoboot_item], item->exec_path[general->autoboot_item], NULL);
		free( item );
		free( general );
		exit(0);
	}

	gui_init();
	gui_load();

	while( !gui_done )
	{
		gui_draw();

		gui_events();
	}	

	gui_clean();
	free(item);
	free(general);

	return 0;
}

