#include <main.h>

// #region wkview action function
void wkview_go(GtkWidget *widget, gpointer data)
{
  const gchar *new_url = smart_url_completion(gtk_entry_get_text(GTK_ENTRY(entry)));
  gtk_entry_set_text(GTK_ENTRY(entry), new_url);
  webkit_web_view_load_uri(webView, new_url);
}

void wkview_new_tab(GtkWidget *widget, gpointer data)
{
  wkview_tab_add(NULL);
}

void wkview_back(GtkWidget *widget, gpointer data)
{
  if (webkit_web_view_can_go_back(webView))
  {
    webkit_web_view_go_back(webView);
  }
}

void wkview_forward(GtkWidget *widget, gpointer data)
{
  if (webkit_web_view_can_go_forward(webView))
  {
    webkit_web_view_go_forward(webView);
  }
}

void wkview_refresh(GtkWidget *widget, gpointer data)
{
  webkit_web_view_reload(webView);
}

void wkview_cancel(GtkWidget *widget, gpointer data)
{
  webkit_web_view_stop_loading(webView);
}

void wkview_tab_delete(GtkWidget *widget, gpointer data)
{
  struct WKTab *current_tab_data = (struct WKTab *)data;
  g_print("Removing tab with URL: %s\n", current_tab_data->tab_id);
  int current_tab_index = current_tab_data->tab_number;
  currentTab = wkview_tab_list_remove_index(tabs, current_tab_index);
  wkview_tab_set_display(currentTab);
  g_print("Removing tab\n");
  gtk_container_remove(GTK_CONTAINER(hbox_tab), current_tab_data->tab_widget);
  g_print("Removing view\n");
  gtk_container_remove(GTK_CONTAINER(vbox), GTK_WIDGET(current_tab_data->tab_view));
}

void wkview_tab_go(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  struct WKTab *current_tab_data = (struct WKTab *)data;
  g_print("Going to tab with URL: %s\n", current_tab_data->tab_url);
  currentTab = current_tab_data;
  wkview_tab_set_display(current_tab_data);
}

// #endregion

// #region wkview helper function
void wkview_tab_add(struct hl_session_state_list *session_state)
{
  struct WKTab *new_tab = malloc(sizeof(struct WKTab));
  if (tabs->tab_count == 0) {
    tabs->root = new_tab;
  } else {
    currentTab->next = new_tab;
  }
  currentTab = new_tab;
  tab_number = tabs->tab_count;
  wkview_tab_init(new_tab, session_state);
}

void wkview_tab_init(struct WKTab *op_tab,struct hl_session_state_list *session_state) {
  if (session_state == NULL) { // Init tab without previous session state
    gint64 current_time;
    current_time = g_get_real_time();
    op_tab->tab_id = malloc(sizeof(char)*64);
    sprintf(op_tab->tab_id,"%ld", current_time);
  } else {
    op_tab->tab_id = session_state->state_id;
  }
  tab_number = ++tabs->tab_count;
  op_tab->tab_number = tab_number;
  op_tab->tab_view = wkview_tab_init_view(op_tab, session_state);
  wkview_tab_init_ui(op_tab);
}

void wkview_tab_init_ui(struct WKTab *op_tab){
  GtkWidget *new_tab = gtk_box_new(FALSE, 5);
  GtkWidget *event_box = gtk_event_box_new();
  GtkWidget *tab_title = gtk_label_new("New Tab");
  GtkWidget *tab_icon = gtk_image_new();
  GtkWidget *tab_delete_button = gtk_button_new_from_icon_name("edit-delete", GTK_ICON_SIZE_BUTTON);
  gtk_widget_set_css_classes(new_tab, "button");
  gtk_widget_set_css_classes(tab_delete_button, "button-layered");
  gtk_box_pack_start(GTK_BOX(new_tab), tab_icon, FALSE, TRUE, 5);
  gtk_box_pack_start(GTK_BOX(new_tab), tab_title, FALSE, TRUE, 5);
  gtk_box_pack_end(GTK_BOX(new_tab), tab_delete_button, FALSE, TRUE, 10);
  gtk_container_add(GTK_CONTAINER(event_box), new_tab);
  gtk_box_pack_start(GTK_BOX(hbox_tab), event_box, FALSE, TRUE, 5);
  g_signal_connect(G_OBJECT(event_box), "button-press-event", G_CALLBACK(wkview_tab_go), (gpointer)op_tab);
  g_signal_connect(G_OBJECT(tab_delete_button), "clicked", G_CALLBACK(wkview_tab_delete), (gpointer)op_tab);
  gtk_widget_show_all(hbox_tab);
  op_tab->tab_widget = event_box;
  op_tab->tab_title = tab_title;
  op_tab->tab_icon = tab_icon;
  wkview_tab_set_display(op_tab);
}

WebKitWebView *wkview_tab_init_view(struct WKTab *op_tab, struct hl_session_state_list *session_state) {
  
  WebKitWebView *new_wkview = WEBKIT_WEB_VIEW(webkit_web_view_new());
  g_signal_connect(new_wkview, "load_changed", G_CALLBACK(wkview_load_changed), (gpointer)op_tab);
  if (session_state == NULL) {
    webkit_web_view_load_uri(new_wkview, DEFAULT_URI);
  } else {
    webkit_web_view_load_uri(new_wkview, session_state->state_url);
    webkit_web_view_restore_session_state(new_wkview, webkit_web_view_session_state_new(session_state->state_data));
  }
  WebKitSettings *current_settings = webkit_web_view_get_settings(new_wkview);
  if (startup)
  {
    webkit_web_context_set_favicon_database_directory(webkit_web_view_get_context(new_wkview), NULL);
    favicon_database = webkit_web_context_get_favicon_database(webkit_web_view_get_context(new_wkview));
  }

  wkview_set_settings(current_settings);

  gtk_box_pack_end(GTK_BOX(vbox), GTK_WIDGET(new_wkview), TRUE, TRUE, 0);
  return new_wkview;
}

void wkview_tab_set_display(struct WKTab *tab)
{
  if (startup) {
    startup = false;
  } else {
    g_print("Hiding previous view\n");
    gtk_widget_hide(GTK_WIDGET(webView));
  }
  webView = WEBKIT_WEB_VIEW(tab->tab_view);
  gtk_widget_show(GTK_WIDGET(webView));
  current_display_tab_number = tab->tab_number;
}

void wkview_tab_update_display(struct WKTab *op_tab, WebKitWebView *web_view) {
  gtk_label_set_label(GTK_LABEL(op_tab->tab_title), webkit_web_view_get_title(web_view));
  op_tab->tab_url = (gchar*) webkit_web_view_get_uri(web_view);
  webkit_favicon_database_get_favicon(favicon_database, op_tab->tab_url, NULL, wkview_set_favicon, (gpointer)op_tab->tab_icon);
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

void wkview_set_settings(WebKitSettings *current_settings) {
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
}

void wkview_save_context(WebKitWebView *web_view)
{
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
  struct WKTab *op_tab = (struct WKTab *)user_data;
  switch (load_event)
  {
  case WEBKIT_LOAD_FINISHED:
    gtk_progress_bar_set_fraction(load_progress_bar, 1.0);
    current_load_status = DONE;
    gtk_widget_show(refresh_button);
    gtk_widget_hide(cancel_button);
    wkview_tab_update_display(op_tab, web_view);
    gtk_entry_set_text(GTK_ENTRY(entry), currentTab->tab_url);
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
  GtkImage *tab_icon = GTK_IMAGE(data);
  cairo_surface_t *favicon = webkit_favicon_database_get_favicon_finish(favicon_database, res, NULL);
  if (favicon != NULL)
  {
    cairo_surface_resize(favicon, 24, 24);
    gtk_image_set_from_surface(tab_icon, favicon);
  }
}
// #endregion


GtkWidget *hl_button_new_from_icon_name(const gchar *icon_name, GtkIconSize size, GCallback click) {
  GtkWidget *button_widget = gtk_button_new_from_icon_name(icon_name, size);
  g_signal_connect(G_OBJECT(button_widget), "clicked", click, NULL);
  gtk_widget_set_css_classes(button_widget, "button");
  return button_widget;
}

void app_quit(){
  g_print("Quitting App\n");
  hl_data_manager_update_session_state_all(tabs, data_manager);
  g_print("Saving Session State\n");
  hl_data_manager_save(data_manager);
  g_print("Freeing Unused Space\n");
  hl_data_manager_destroy(data_manager);
  g_print("All Done, see you!\n");
  gtk_main_quit();
}

void app_tab_init(){
  
  tabs = malloc(sizeof(struct WKTabList));
  tabs->tab_count = 0;

  data_manager = hl_data_manager_init();
  //struct hl_session_state_list **prev_session_list = hl_data_manager_load(data_manager);
  hl_data_manager_load(data_manager);
  

  struct hl_session_state_list *op_state = data_manager->root_session_state;
  if (data_manager->state_count!=0){
    g_print("Previous State Found, Count: %d\n",data_manager->state_count);
    
    for(int i=0;i<data_manager->state_count;i++){
      wkview_tab_add(op_state);
      op_state = op_state->next;
    }
    
  } else {
    wkview_tab_add(NULL);
  }

  
}

// ================================================================================================
int main(int argc, char *argv[])
{
  GtkWidget *win = NULL;

  g_log_set_handler("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc)gtk_false, NULL);
  gtk_init(&argc, &argv);
  g_log_set_handler("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

  provider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(provider, "style.css", NULL);

  g_print("Welcome to Luopan Debug Panel, Version 0.0.1:12162046\n");

  win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  
  gtk_window_set_title(GTK_WINDOW(win), "Luopan");
  gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);
  gtk_widget_set_app_paintable(win, TRUE);
  g_signal_connect(win, "destroy", app_quit, NULL);
  //g_signal_connect(G_OBJECT(win), "draw", G_CALLBACK(draw), NULL);
  //g_signal_connect(G_OBJECT(win), "screen-changed", G_CALLBACK(screen_changed), NULL);

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
  gtk_widget_set_css_classes(GTK_WIDGET(load_progress_bar), "wkview-load-progress-bar");
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
  
  add_button = hl_button_new_from_icon_name("list-add", GTK_ICON_SIZE_LARGE_TOOLBAR, G_CALLBACK(wkview_new_tab));
  gtk_box_pack_start(GTK_BOX(hbox_tab_bar), hbox_tab, TRUE, TRUE, 0);
  gtk_box_pack_end(GTK_BOX(hbox_tab_bar), add_button, FALSE, TRUE, 0);

  gtk_box_pack_start(GTK_BOX(vbox), hbox_address, FALSE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), hbox_tab_bar, FALSE, TRUE, 0);

  gtk_container_add(GTK_CONTAINER(win), vbox);

  

  GdkDisplay *current_display = gdk_display_get_default();
  GdkMonitor *current_monitor = gdk_display_get_primary_monitor(current_display);
  GdkRectangle *monitor_geometry = malloc(sizeof(GdkRectangle));
  gdk_monitor_get_geometry(current_monitor, monitor_geometry);
  gtk_window_set_default_size(GTK_WINDOW(win), 1280, 720);
  gtk_widget_show_all(win);
  app_tab_init();
  gtk_widget_set_css_classes(win, "window-body");
  gtk_main();
  return 0;
}
