#include <stdlib.h>
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>
#include <extension/data_manager/data_manager.h>

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
int current_display_tab_number = 0;
enum LOAD_STATUS current_load_status = DONE;
WebKitFaviconDatabase *favicon_database;
bool startup = true;
GtkCssProvider *provider = NULL;

struct hldata_manager *data_manager;

GtkWidget *vbox = NULL;
GtkProgressBar *load_progress_bar = NULL;

GtkWidget *go_button = NULL;
GtkWidget *back_button = NULL;
GtkWidget *forward_button = NULL;
GtkWidget *refresh_button = NULL;
GtkWidget *cancel_button = NULL;
GtkWidget *add_button = NULL;

GtkWidget *hbox_tab = NULL;

void wkview_new_tab(GtkWidget *widget, gpointer data);
void wkview_go(GtkWidget *widget, gpointer data);
void wkview_back(GtkWidget *widget, gpointer data);
void wkview_forward(GtkWidget *widget, gpointer data);
void wkview_refresh(GtkWidget *widget, gpointer data);
void wkview_cancel(GtkWidget *widget, gpointer data);
void wkview_tab_delete(GtkWidget *widget, gpointer data);
void wkview_tab_go(GtkWidget *widget, GdkEventButton *event, gpointer data);

void wkview_tab_add(struct hl_session_state_list *session_state);
void wkview_tab_init(struct WKTab *op_tab,struct hl_session_state_list *session_state);
void wkview_tab_init_ui(struct WKTab *op_tab);
WebKitWebView *wkview_tab_init_view(struct WKTab *op_tab, struct hl_session_state_list *session_state);
void wkview_tab_update_display(struct WKTab *op_tab, WebKitWebView *web_view);
struct WKTab *wkview_tab_list_remove_index(struct WKTabList *tab_list, int index);
void wkview_tab_set_display(struct WKTab *tab);
void wkview_set_settings(WebKitSettings *current_settings);
void wkview_save_context(WebKitWebView *web_view);
const gchar *smart_url_completion(const gchar *webview_url);
void cairo_surface_resize(cairo_surface_t *surface, int sizex, int sizey);
void gtk_widget_set_css_classes(GtkWidget *widget, const gchar *class_name);

GtkWidget *hl_button_new_from_icon_name(const gchar *icon_name, GtkIconSize size, GCallback click);

void wkview_load_changed(WebKitWebView *web_view, WebKitLoadEvent load_event, gpointer user_data);
void wkview_set_favicon(GObject* source_object, GAsyncResult* res, gpointer data);





