#ifndef __CONFIG_H__
#define __CONFIG_H__

struct config_t cfg;

typedef struct
{
	int autoboot_item;
	int scrolling_text_y;

} GENERAL_CONFIG;

typedef struct
{
	char name[15][256];
	char exec_path[15][256];
	char icon_path[15][256];
	int icon_x[15];
	int icon_y[15];
	int standalone[15];

} PER_ITEM_CONFIG;

GENERAL_CONFIG *general;
PER_ITEM_CONFIG *item;

int cfg_read(char *file);

int items_count;

#endif

