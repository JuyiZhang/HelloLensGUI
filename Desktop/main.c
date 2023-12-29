#include<main.h>

int component_division = 4;
int component_margin = 0;
int edge_margin = 10;
int largest_component_size = 2;
int component_pixel_size = 0;
int screen_scale_factor = 0;
int window_size_height;

char *default_icon_location = "/.heyos/data/desktop/icon/";

struct stat st_data = {0};
struct timespec action_start_time;

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0
#endif

gchar* _T(char* str)

{
return g_locale_to_utf8(str, -1, 0, 0, 0);
}

void exec_action(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    struct timespec action_end_time;
    if (clock_gettime(CLOCK_REALTIME, &action_end_time) != -1) {
        long time_int = (action_start_time.tv_nsec - action_end_time.tv_nsec)/1000000;
        if (time_int <= 1500) { //Pressing interval smaller than 1500
            g_print("Attempt to init short press");
            execl("/bin/sh","/bin/sh", "-c", (char *)data, (void *)NULL);
        } else {
            g_print("Attempt to init long press");
        }
    }

    //execl("/bin/sh", "/bin/sh", "-c", (char*)data, (void *)NULL);
}

void begin_action(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    clock_gettime(CLOCK_REALTIME, &action_start_time);
    g_print("Time start to be %ld", action_start_time.tv_nsec);
}

void scroll_layout_generate(GtkWidget *parent)
{
    GtkWidget *home_layout = gtk_grid_new(); // Creating the vertical view with infinite stack
    gtk_grid_set_row_spacing(GTK_GRID(home_layout), component_margin);
    gtk_grid_set_column_spacing(GTK_GRID(home_layout), component_margin);
    struct desktop_widget_element_list *iterator = malloc(sizeof(struct desktop_widget_element_list));
    int count = desktop_data_manager_get_all_elements(iterator);
    for(int i=0; i<count; i++)
    {
        struct desktop_widget_element *widget_to_create = iterator->widget_element;
        GtkWidget *widget_parent = gtk_overlay_new();
        //g_print("Creating widget with url: %s\n", widget_to_create->data->location);
        hl_widget_add_bg(widget_parent, widget_to_create->width, widget_to_create->height);
        switch (widget_to_create->type)
        {
        case 'i': // Icon
            hl_desktop_icon_size_cfg(widget_parent, widget_to_create->data, widget_to_create->width, widget_to_create->height);
            break;
        default:
            break;
        }
        
        gtk_widget_set_size_request(widget_parent, widget_to_create->width*component_pixel_size, widget_to_create->height*component_pixel_size);
        gtk_grid_attach(GTK_GRID(home_layout), widget_parent, widget_to_create->x, widget_to_create->y, widget_to_create->width, widget_to_create->height);
        //hl_desktop_widget_add_with_size(home_layout, widget_parent, widget_to_create->x, widget_to_create->y, widget_to_create->width, widget_to_create->height);
        if (iterator->next == NULL)
            break;
        iterator = iterator->next;
    }
    gtk_container_add(GTK_CONTAINER(parent), home_layout);
}

void hl_widget_add_bg(GtkWidget *widget, int width, int height) {
    char bg_name[32];
    sprintf(bg_name,"widget-%d-%d",width,height);
    gtk_widget_add_css_classes(widget, bg_name);
    /*sprintf(bg_name,"res/backplate/Glass_%d_%d.png",width,height);
    //g_print("Adding background with name: %s\n", bg_name);
    GtkWidget *bg_image = gtk_image_new_from_file(bg_name);
    gtk_widget_set_halign(bg_image,GTK_ALIGN_FILL);
    gtk_widget_set_valign(bg_image,GTK_ALIGN_FILL);
    gtk_overlay_add_overlay(GTK_OVERLAY(widget), bg_image);*/
}

void hl_desktop_icon_size_cfg(GtkWidget *widget, struct widget_data *data, int width, int height){
    
    int icon_height = height * screen_scale_factor;

    char size_str[4];
    sprintf(size_str,"@%dx", icon_height);

    char *logo_str_full = "";
    logo_str_full = g_strconcat(logo_str_full, default_icon_location, size_str, "/", data->location, ".png", NULL);
    //g_print("The logo location is %s\n", logo_str_full);
    
    if(stat(logo_str_full, &st_data) == -1) {
        g_print("Failed to find file, fall back to absolute directory");
        logo_str_full = data->location;
    }
    GtkWidget *background_widget = gtk_image_new();
    GtkWidget *icon_widget = gtk_image_new_from_file(logo_str_full);
    GtkWidget *title = gtk_label_new(data->name);
    GtkWidget *icon_event_box = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(icon_event_box), icon_widget);
    g_signal_connect(icon_event_box, "button-release-event", G_CALLBACK(exec_action), (gpointer)data->exec_cmd);
    g_signal_connect(icon_event_box, "button-press-event", G_CALLBACK(begin_action), NULL);
    
    gtk_label_set_max_width_chars(GTK_LABEL(title), 32);
    gtk_widget_add_css_classes(title, "widget-label");

    gtk_widget_set_halign(icon_event_box, GTK_ALIGN_FILL);
    gtk_widget_set_valign(icon_event_box, GTK_ALIGN_FILL);
    gtk_widget_set_halign(title, GTK_ALIGN_END);
    gtk_widget_set_valign(title, GTK_ALIGN_END);
    gtk_widget_set_margin_end(title, 30);
    gtk_widget_set_margin_bottom(title, 30);

    hl_widget_add_bg(icon_event_box, width, height);

    gtk_overlay_add_overlay(GTK_OVERLAY(widget), icon_event_box);
    gtk_overlay_add_overlay(GTK_OVERLAY(widget), title);
    
}

void gtk_widget_add_css_classes(GtkWidget *widget, const gchar *class_name) {
  GtkStyleContext *context;
  context = gtk_widget_get_style_context(widget);
  gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
  gtk_style_context_add_class(context, class_name);
}

void gtk_widget_remove_css_classes(GtkWidget *widget, const gchar *class_name) {
  GtkStyleContext *context;
  context = gtk_widget_get_style_context(widget);
  gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
  gtk_style_context_remove_class(context, class_name);
}

static void activate(GtkApplication *app, gpointer user_data) {

    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "style.css", NULL);

    GtkWidget *window;
    FILE *fp;
    GtkWidget *button;
    GtkWidget *button_box;
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Window");

    GdkDisplay *current_display = gdk_display_get_default();
    GdkMonitor *current_monitor = gdk_display_get_primary_monitor(current_display);
    GdkRectangle *monitor_geometry = malloc(sizeof(GdkRectangle));
    gdk_monitor_get_geometry(current_monitor, monitor_geometry);
    gtk_window_set_default_size(GTK_WINDOW(window), 1920, 1080);//monitor_geometry->width, monitor_geometry->height);

    gint *width = malloc(sizeof(gint));

    gtk_window_get_size(GTK_WINDOW(window), width, &window_size_height);
    g_print("Height of window is %d\n", window_size_height);

    component_pixel_size = (window_size_height - (component_division - 1) * component_margin - edge_margin * 2) / component_division;
    screen_scale_factor = component_pixel_size/256;

    gtk_widget_add_css_classes(window, "body");

    //GtkScrolledWindow *scroll_parent = gtk_scrolled_window_new(gtkadjustment)

    //gtk_widget_set_margin_start(edge_margin);
    //gtk_widget_set_margin_top(edge_margin);
    //gtk_widget_set_margin_bottom(edge_margin);

    scroll_layout_generate(window);

    gtk_widget_show_all (window);
}

int main (int argc, char **argv) {
    GtkApplication *app;
    int status;
    default_icon_location = get_absolute_directory(default_icon_location);
    app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);
    return status;
}