#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libconfig.h>

#include "config.h"

int cfg_read(char *file)
{
	int i;

	config_init(&cfg);

	printf("loading [%s]..\n", file);
	if (!config_read_file(&cfg, file)) printf("config_read_file() failed\n");
	else
	{
		item = (PER_ITEM_CONFIG *) malloc( sizeof(PER_ITEM_CONFIG));
		general = (GENERAL_CONFIG *) malloc( sizeof(GENERAL_CONFIG));

		config_setting_t *search = NULL;
		search = config_lookup(&cfg, "general");	

		if (!search) 
		{
				printf("config_lookup() failed\n");
		}
		else
		{
			config_setting_t *tmp = config_setting_get_member (search, "items_count");
			if (tmp)
			{
				general->items_count=config_setting_get_int(tmp);
				printf("items_count = %i\n", general->items_count);
			}

			tmp = config_setting_get_member(search, "autoboot_item");
			if(tmp)
			{ 
				strcpy(general->autoboot_item, config_setting_get_string(tmp));

				for(i=0; i<general->items_count; i++)
				{
					char tmpItem[256];
					sprintf(tmpItem, "item%i", i); 
					if(strcmp(general->autoboot_item, tmpItem) == 0) general->item_to_boot = i;		
				}
				printf("autoboot_item = %s (%i)\n", general->autoboot_item, general->item_to_boot);

			} else general->item_to_boot = -1;

			tmp = config_setting_get_member (search, "scrolling_text_y");
			if (tmp)
			{
				general->scrolling_text_y=config_setting_get_int(tmp);
				printf("scrolling_text_y = %i\n", general->scrolling_text_y);
			}
		}	

		for(i=0; i<general->items_count; i++)
		{
			char tmpItem[256];
			sprintf(tmpItem, "item%i", i); 
			search = config_lookup(&cfg, tmpItem);

			printf("looking for item : %s\n", tmpItem);

			if (!search) 
			{
				printf("config_lookup failed\n");
			}
			else 
			{
				config_setting_t *tmp = config_setting_get_member(search, "name");
				if(tmp) { 
					strcpy(item->name[i], config_setting_get_string(tmp));
					printf("name->%i = %s\n", i, item->name[i]);
				}

				tmp = config_setting_get_member(search, "exec_path");
				if(tmp) {
					strcpy(item->exec_path[i], config_setting_get_string(tmp));
					printf("exec_path->%i = %s\n", i, item->exec_path[i]);
				}

				tmp = config_setting_get_member(search, "icon_path");
				if(tmp) {
					strcpy(item->icon_path[i], config_setting_get_string(tmp));
					printf("icon_path->%i = %s\n", i, item->icon_path[i]);
				}

				tmp = config_setting_get_member (search, "icon_x");
			    	if (tmp) {
					item->icon_x[i]=config_setting_get_int(tmp);
					printf("icon_x->%i = %i\n", i, item->icon_x[i]);
				}

				tmp = config_setting_get_member (search, "icon_y");
			    	if (tmp) {
					item->icon_y[i]=config_setting_get_int(tmp);
					printf("icon_y->%i = %i\n", i, item->icon_y[i]);
				}

				tmp = config_setting_get_member (search, "standalone");
			    	if (tmp) {
					item->standalone[i]=config_setting_get_int(tmp);
					printf("standalone->%i = %i\n", i, item->standalone[i]);
				}
			}
		}
	}
	config_destroy(&cfg);

	return 0;
}
