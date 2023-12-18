#ifndef DESKTOP_DATA_MANAGER
#define DESKTOP_DATA_MANAGER

#include<stdio.h>
#include<extension/widget_element_manager/widget_element_manager.h>


int desktop_data_manager_get_all_elements(struct desktop_widget_element_list *root_element);
void widget_get_definition(struct widget_data *data,char *location);
char *get_absolute_directory(char *directory);

#endif