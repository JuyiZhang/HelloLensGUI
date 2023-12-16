#ifndef WEBVIEW_MANAGER
#define WEBVIEW_MANAGER

#include<webkit2/webkit2.h>
#include<gtk/gtk.h>

struct WKTab
{
    GtkWidget *tab_widget;
    WebKitWebView *tab_view;
    char *tab_url;
    GtkWidget *tab_title;
    GtkWidget *tab_icon;
    struct WKTab *next;
    int tab_number;
    char *tab_id;
};

struct WKTabList
{
    int tab_count;
    struct WKTab *root;
};

#endif