#ifndef WIDGET_ELEMENT_MANAGER
#define WIDGET_ELEMENT_MANAGER

#include<gtk/gtk.h>
#include<wchar.h>
#include<locale.h>
#include<sys/stat.h>

struct desktop_widget_element {
    int x;
    int y;
    int width; //1 grid equals subdivision of screen area (default 256)
    int height;
    char type; //i:icon w:widget
    //char *location; // Position of the widget manifest, if location is icon, search relative directory first
    // Location prioritizes search in widget_def directory
    struct widget_data *data;
};

struct widget_data {
    char *name; //Name of the widget, max 64 character
    char *exec_cmd; //Command Execution of widget
    char *location; //Location of the widget definition
};

struct desktop_widget_element_list {
    struct desktop_widget_element *widget_element;
    struct desktop_widget_element_list *next;
};

#endif