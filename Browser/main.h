#include <stdlib.h>
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

struct WKTab
{
    GtkWidget *tab_widget;
    WebKitWebView *tab_view;
    const gchar *tab_url;
    GtkWidget *tab_title;
    GtkWidget *tab_icon;
    struct WKTab *next;
    int tab_number;
};

struct WKTabList
{
    int tab_count;
    struct WKTab *root;
};

enum LOAD_STATUS
{
    DONE,
    IN_PROGRESS
};

WebKitWebView *webView = NULL;
GtkWidget *entry = NULL;
char *DEFAULT_URI = "http://acid3.acidtests.org/";
struct WKTabList *tabs;
struct WKTab *currentTab;
int tab_number = 0;
enum LOAD_STATUS current_load_status = DONE;
WebKitFaviconDatabase *favicon_database;
bool startup = true;
GtkCssProvider *provider = NULL;

GtkWidget *vbox = NULL;
GtkProgressBar *load_progress_bar = NULL;

GtkWidget *go_button = NULL;
GtkWidget *back_button = NULL;
GtkWidget *forward_button = NULL;
GtkWidget *refresh_button = NULL;
GtkWidget *cancel_button = NULL;
GtkWidget *add_button = NULL;

GtkWidget *hbox_tab = NULL;

void wkview_new_tab(GtkWidget *widget, GdkEventButton *event, gpointer data);
void wkview_go(GtkWidget *widget, GdkEventButton *event, gpointer data);
void wkview_back(GtkWidget *widget, GdkEventButton *event, gpointer data);
void wkview_forward(GtkWidget *widget, GdkEventButton *event, gpointer data);
void wkview_refresh(GtkWidget *widget, GdkEventButton *event, gpointer data);
void wkview_cancel(GtkWidget *widget, GdkEventButton *event, gpointer data);
void wkview_tab_delete(GtkWidget *widget, GdkEventButton *event, gpointer data);
void wkview_tab_go(GtkWidget *widget, GdkEventButton *event, gpointer data);

void wkview_add_tab();
WebKitWebView *init_wkview();
void wkview_save_context(WebKitWebView *web_view);
const gchar *smart_url_completion(const gchar *webview_url);
struct WKTab *wkview_tab_list_remove_index(struct WKTabList *tab_list, int index);
void wkview_set_tab_display(struct WKTab *tab);
void cairo_surface_resize(cairo_surface_t *surface, int sizex, int sizey);
void gtk_widget_set_css_classes(GtkWidget *widget, const gchar *class_name);

void wkview_load_changed(WebKitWebView *web_view, WebKitLoadEvent load_event, gpointer user_data);
void wkview_set_favicon(GObject* source_object, GAsyncResult* res, gpointer data);





