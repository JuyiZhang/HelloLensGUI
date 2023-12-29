#include<gtk/gtk.h>
#include<stdio.h>
#include<time.h>
#include<extension/desktop_data_manager/desktop_data_manager.h>
#include<extension/widget_element_manager/widget_element_manager.h>

GtkCssProvider *provider = NULL;

static void activate(GtkApplication *app, gpointer user_data);
void gtk_widget_remove_css_classes(GtkWidget *widget, const gchar *class_name);
void gtk_widget_add_css_classes(GtkWidget *widget, const gchar *class_name);
void scroll_layout_generate(GtkWidget *parent);
void exec_action(GtkWidget *widget, GdkEventButton *event, gpointer data);
void hl_desktop_widget_add_with_size(GtkWidget *parent_grid, GtkWidget *child_widget, int x, int y, int width, int height);
void hl_desktop_icon_size_cfg(GtkWidget *widget, struct widget_data *data, int width, int height);
void hl_widget_add_bg(GtkWidget *widget, int width, int height);