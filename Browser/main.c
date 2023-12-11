#include <main.h>

// #region wkview action function
void wkview_go(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  const gchar *new_url = smart_url_completion(gtk_entry_get_text(GTK_ENTRY(entry)));
  gtk_entry_set_text(GTK_ENTRY(entry), new_url);
  webkit_web_view_load_uri(webView, new_url);
}

void wkview_new_tab(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  struct WKTab *newTab;
  newTab = malloc(sizeof(struct WKTab));
  currentTab->next = newTab;
  currentTab = newTab;
  tab_number = tabs->tab_count;
  wkview_add_tab();
  wkview_save_context(webView);
  webkit_web_view_load_uri(webView, DEFAULT_URI);
}

void wkview_back(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  if (webkit_web_view_can_go_back(webView))
  {
    webkit_web_view_go_back(webView);
  }
}

void wkview_forward(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  if (webkit_web_view_can_go_forward(webView))
  {
    webkit_web_view_go_forward(webView);
  }
}

void wkview_refresh(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  webkit_web_view_reload(webView);
}

void wkview_cancel(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  webkit_web_view_stop_loading(webView);
}

void wkview_tab_delete(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  struct WKTab *current_tab_data = (struct WKTab *)data;
  int current_tab_index = current_tab_data->tab_number;
  currentTab = wkview_tab_list_remove_index(tabs, current_tab_index);
  wkview_set_tab_display(currentTab);
  gtk_container_remove(GTK_CONTAINER(hbox_tab), current_tab_data->tab_widget);
  gtk_container_remove(GTK_CONTAINER(vbox), GTK_WIDGET(current_tab_data->tab_view));
}

void wkview_tab_go(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  struct WKTab *current_tab_data = (struct WKTab *)data;
  currentTab = current_tab_data;
  wkview_set_tab_display(current_tab_data);
}

// #endregion

// #region wkview helper function
void wkview_add_tab()
{
  tab_number = ++tabs->tab_count;
  GtkWidget *new_tab = gtk_box_new(FALSE, 5);
  GtkWidget *event_box = gtk_event_box_new();
  GtkWidget *tab_title = gtk_label_new("New Tab");
  GtkWidget *tab_icon = gtk_image_new();
  GtkWidget *tab_delete_button = gtk_button_new_from_icon_name("edit-delete", GTK_ICON_SIZE_BUTTON);
  gtk_widget_set_css_classes(new_tab, "button");
  gtk_widget_set_css_classes(tab_delete_button, "button-layered");
  currentTab->tab_number = tab_number;
  g_signal_connect(G_OBJECT(tab_delete_button), "clicked", G_CALLBACK(wkview_tab_delete), (gpointer)currentTab);
  gtk_box_pack_start(GTK_BOX(new_tab), tab_icon, FALSE, TRUE, 5);
  gtk_box_pack_start(GTK_BOX(new_tab), tab_title, FALSE, TRUE, 5);
  gtk_box_pack_end(GTK_BOX(new_tab), tab_delete_button, FALSE, TRUE, 10);
  gtk_container_add(GTK_CONTAINER(event_box), new_tab);
  gtk_box_pack_start(GTK_BOX(hbox_tab), event_box, FALSE, TRUE, 5);
  g_signal_connect(G_OBJECT(event_box), "button-press-event", G_CALLBACK(wkview_tab_go), (gpointer)currentTab);
  gtk_widget_show_all(hbox_tab);
  currentTab->tab_view = init_wkview();
  currentTab->tab_widget = event_box;
  currentTab->tab_title = tab_title;
  currentTab->tab_icon = tab_icon;
  wkview_set_tab_display(currentTab);
}

WebKitWebView *init_wkview()
{

  WebKitWebView *new_wkview = WEBKIT_WEB_VIEW(webkit_web_view_new());
  g_signal_connect(new_wkview, "load_changed", G_CALLBACK(wkview_load_changed), NULL);
  webkit_web_view_load_uri(new_wkview, DEFAULT_URI);
  WebKitSettings *current_settings = webkit_web_view_get_settings(new_wkview);
  if (startup)
  {
    webkit_web_context_set_favicon_database_directory(webkit_web_view_get_context(new_wkview), NULL);
    favicon_database = webkit_web_context_get_favicon_database(webkit_web_view_get_context(new_wkview));
  }

  // #region WebKit Settings
  webkit_settings_set_user_agent(current_settings, "Mozilla/5.0 (Linux aarch64) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.0 Luopan/605.1.15");
  /*
  webkit_settings_set_enable_write_console_messages_to_stdout(current_settings, TRUE);
  webkit_settings_set_enable_developer_extras(current_settings, TRUE);
  webkit_settings_set_enable_javascript(current_settings, TRUE);
  WebKitFeatureList *settings_list = webkit_settings_get_all_features();
  int feature_count = webkit_feature_list_get_length(settings_list);
  for (int i = 0; i < feature_count; i++)
  {
    WebKitFeature *setting_item = webkit_feature_list_get(settings_list, i);
    if (webkit_feature_get_default_value(setting_item) == true)
    {
      printf("%s is True\n", webkit_feature_get_name(setting_item));
    }
    else
    {
      printf("%s is False\n", webkit_feature_get_name(setting_item));
    }
  }*/
  // #endregion

  gtk_box_pack_end(GTK_BOX(vbox), GTK_WIDGET(new_wkview), TRUE, TRUE, 0);
  return new_wkview;
}

void wkview_save_context(WebKitWebView *web_view)
{
  gtk_label_set_label(GTK_LABEL(currentTab->tab_title), webkit_web_view_get_title(web_view));
  currentTab->tab_url = webkit_web_view_get_uri(web_view);
  webkit_favicon_database_get_favicon(favicon_database, currentTab->tab_url, NULL, wkview_set_favicon, (gpointer)currentTab->tab_icon);
  gtk_entry_set_text(GTK_ENTRY(entry), currentTab->tab_url);
  // gtk_widget_hide(load_progress_bar);
}

const gchar *smart_url_completion(const gchar *webview_url)
{
  gchar **textsplit = g_strsplit_set(webview_url, ":./", -1);
  if (g_str_has_prefix(webview_url, "http"))
    return webview_url;
  else if (g_strv_length(textsplit) > 1)
    return g_strjoin(NULL, "https://", webview_url, NULL);
  int i = 0;
  while (1)
  {
    gchar *currentText = textsplit[i++];
    if (currentText == NULL)
      break;
  }
  return NULL;
}

struct WKTab *wkview_tab_list_remove_index(struct WKTabList *tab_list, int index)
{

  struct WKTab *current_op_tab = tab_list->root;
  struct WKTab *return_tab = NULL;

  // g_print("Attempting to remove tab from index: %d\n", index);

  if (index == 1)
  { // If removing the first item
    tab_list->root = tab_list->root->next;
    return_tab = tab_list->root;
  }
  else
  {
    if (index > 2) // Move current op tab to the one previous to the removed one unless we are removing the second one
      for (int i = 1; i < index - 1; i++)
        current_op_tab = current_op_tab->next;
    // g_print("The operation tab has index %d\n", current_op_tab->tab_number);
    // g_print("[Before removal] Current length of Tab: %d\n", tab_list->tab_count);
    //  Remove the reference of the target tab
    if (index - 1 == --tab_list->tab_count)
    { // If removing the last item, the next for the one before is none
      current_op_tab->next = NULL;
      return_tab = tab_list->root; // Return the page with first item
    }
    else
    {
      current_op_tab->next = current_op_tab->next->next;
      return_tab = current_op_tab->next;
    }
    // g_print("[After removal] Current length of Tab: %d\n", tab_list->tab_count);
  }

  // g_print("The return tab index is %d", return_tab->tab_number);

  current_op_tab = tab_list->root; // Reiterate with indexing
  for (int i = 1; i <= tab_list->tab_count; i++)
  {
    current_op_tab->tab_number = i;
    current_op_tab = current_op_tab->next;
  }

  // g_print("Removing Tab at Index %d successful\n", index);

  return return_tab;
}

void wkview_set_tab_display(struct WKTab *tab)
{
  if (!startup)
  {
    gtk_widget_hide(GTK_WIDGET(webView));
  }
  webView = WEBKIT_WEB_VIEW(tab->tab_view);
  gtk_widget_show(GTK_WIDGET(webView));
  wkview_save_context(tab->tab_view);
}

void cairo_surface_resize(cairo_surface_t *surface, int sizex, int sizey)
{
  int surface_width = cairo_image_surface_get_width(surface);
  int surface_height = cairo_image_surface_get_height(surface);
  double sx = (double)surface_width / (double)sizex;
  double sy = (double)surface_height / (double)sizey;
  double sf = sx < sy ? sx : sy;
  cairo_surface_set_device_scale(surface, sf, sf);
}

void gtk_widget_set_css_classes(GtkWidget *widget, const gchar *class_name)
{
  GtkStyleContext *context;
  context = gtk_widget_get_style_context(widget);
  gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
  gtk_style_context_add_class(context, class_name);
}
// #endregion

// #region wkview callback function
void wkview_load_changed(WebKitWebView *web_view, WebKitLoadEvent load_event, gpointer user_data)
{
  switch (load_event)
  {
  case WEBKIT_LOAD_FINISHED:
    gtk_progress_bar_set_fraction(load_progress_bar, 1.0);
    current_load_status = DONE;
    gtk_widget_show(refresh_button);
    gtk_widget_hide(cancel_button);
    wkview_save_context(webView);
    break;
  case WEBKIT_LOAD_STARTED:
    // gtk_widget_show(load_progress_bar);
    gtk_progress_bar_set_fraction(load_progress_bar, 0.3);
    current_load_status = IN_PROGRESS;
    gtk_widget_hide(refresh_button);
    gtk_widget_show(cancel_button);
    break;
  case WEBKIT_LOAD_COMMITTED:
    gtk_progress_bar_set_fraction(load_progress_bar, 0.6);
    break;
  default:
    break;
  }
}

void wkview_set_favicon(GObject *source_object, GAsyncResult *res, gpointer data)
{
  cairo_surface_t *favicon = webkit_favicon_database_get_favicon_finish(favicon_database, res, NULL);
  if (favicon != NULL)
  {
    cairo_surface_resize(favicon, 24, 24);
    gtk_image_set_from_surface(GTK_IMAGE(currentTab->tab_icon), favicon);
  }
}
// #endregion

// #region Changing Background Color
gboolean supports_alpha = FALSE;
static void screen_changed(GtkWidget *widget, GdkScreen *old_screen, gpointer userdata)
{
    /* To check if the display supports alpha channels, get the visual */
    GdkScreen *screen = gtk_widget_get_screen(widget);
    GdkVisual *visual = gdk_screen_get_rgba_visual(screen);

    if (!visual)
    {
        g_print("Your screen does not support alpha channels!\n");
        visual = gdk_screen_get_system_visual(screen);
        supports_alpha = FALSE;
    }
    else
    {
        g_print("Your screen supports alpha channels!\n");
        supports_alpha = TRUE;
    }

    gtk_widget_set_visual(widget, visual);
}

static gboolean draw(GtkWidget *widget, cairo_t *cr, gpointer userdata)
{
    cairo_save (cr);

    if (supports_alpha)
    {
        cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0); /* transparent */
    }
    else
    {
        cairo_set_source_rgb (cr, 1.0, 1.0, 1.0); /* opaque white */
    }

    /* draw the background */
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint (cr);

    cairo_restore (cr);

    return FALSE;
}
// #endregion

GtkWidget *hl_button_new_from_icon_name(const gchar *icon_name, GtkIconSize size, GCallback click) {
  GtkWidget *button_widget = gtk_button_new_from_icon_name(icon_name, size);
  g_signal_connect(G_OBJECT(button_widget), "clicked", click, NULL);
  gtk_widget_set_css_classes(button_widget, "button");
  return button_widget;
}

// ================================================================================================
int main(int argc, char *argv[])
{
  GtkWidget *win = NULL;

  currentTab = malloc(sizeof(struct WKTab));
  tabs = malloc(sizeof(struct WKTabList));
  tabs->root = currentTab;
  tabs->tab_count = 0;

  provider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(provider, "style.css", NULL);

  g_log_set_handler("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc)gtk_false, NULL);
  gtk_init(&argc, &argv);
  g_log_set_handler("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

  g_print("Welcome to Luopan Debug Panel, Version 0.0.1:121020232132\n");

  win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  
  gtk_window_set_title(GTK_WINDOW(win), "Luopan");
  gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);
  gtk_widget_set_app_paintable(win, TRUE);
  g_signal_connect(win, "destroy", gtk_main_quit, NULL);
  g_signal_connect(G_OBJECT(win), "draw", G_CALLBACK(draw), NULL);
  g_signal_connect(G_OBJECT(win), "screen-changed", G_CALLBACK(screen_changed), NULL);

  
  

  vbox = gtk_box_new(TRUE, 10);

  GtkWidget *hbox_address = NULL;

  back_button = hl_button_new_from_icon_name("go-previous", GTK_ICON_SIZE_LARGE_TOOLBAR, G_CALLBACK(wkview_back));
  forward_button = hl_button_new_from_icon_name("go-next", GTK_ICON_SIZE_LARGE_TOOLBAR, G_CALLBACK(wkview_forward));
  go_button = hl_button_new_from_icon_name("go-jump", GTK_ICON_SIZE_LARGE_TOOLBAR, G_CALLBACK(wkview_go));
  refresh_button = hl_button_new_from_icon_name("view-refresh", GTK_ICON_SIZE_LARGE_TOOLBAR, G_CALLBACK(wkview_refresh));
  cancel_button = hl_button_new_from_icon_name("dialog-cancel", GTK_ICON_SIZE_LARGE_TOOLBAR, G_CALLBACK(wkview_cancel));

  GtkWidget *vbox_address_input = gtk_box_new(TRUE, 0);
  entry = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(entry), DEFAULT_URI);
  g_signal_connect(G_OBJECT(entry), "activate", G_CALLBACK(wkview_go), (gpointer)win);
  load_progress_bar = GTK_PROGRESS_BAR(gtk_progress_bar_new());
  gtk_progress_bar_set_fraction(load_progress_bar, 0);
  // gtk_widget_hide(load_progress_bar);

  gtk_box_pack_start(GTK_BOX(vbox_address_input), entry, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox_address_input), GTK_WIDGET(load_progress_bar), FALSE, TRUE, 10);

  hbox_address = gtk_box_new(FALSE, 10);
  gtk_box_pack_start(GTK_BOX(hbox_address), back_button, FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox_address), forward_button, FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox_address), vbox_address_input, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox_address), cancel_button, FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox_address), refresh_button, FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(hbox_address), go_button, FALSE, TRUE, 0);

  gtk_widget_hide(cancel_button);

  GtkWidget *hbox_tab_bar = gtk_box_new(FALSE, 10);
  hbox_tab = gtk_box_new(FALSE, 10);
  wkview_add_tab();

  add_button = hl_button_new_from_icon_name("list-add", GTK_ICON_SIZE_LARGE_TOOLBAR, G_CALLBACK(wkview_new_tab));
  gtk_box_pack_start(GTK_BOX(hbox_tab_bar), hbox_tab, TRUE, TRUE, 0);
  gtk_box_pack_end(GTK_BOX(hbox_tab_bar), add_button, FALSE, TRUE, 0);

  gtk_box_pack_start(GTK_BOX(vbox), hbox_address, FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), hbox_tab_bar, FALSE, TRUE, 0);

  // The VBox will contain the URL bar + button on the first line,
  // and the Webview (the WebKit display) on the second line.

  // URL bar & button

  // WebKit display

  gtk_container_add(GTK_CONTAINER(win), vbox);

  GdkDisplay *current_display = gdk_display_get_default();
  GdkMonitor *current_monitor = gdk_display_get_primary_monitor(current_display);
  GdkRectangle *monitor_geometry = malloc(sizeof(GdkRectangle));
  gdk_monitor_get_geometry(current_monitor, monitor_geometry);
  gtk_window_set_default_size(GTK_WINDOW(win), monitor_geometry->width, monitor_geometry->height);
  startup = false;
  screen_changed(win, NULL, NULL);
  gtk_widget_show_all(win);
  gtk_widget_set_css_classes(win, "window-body");
  gtk_main();
  return 0;
}