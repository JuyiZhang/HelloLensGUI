#ifndef WKVIEW_DATA_MANAGER
#define WKVIEW_DATA_MANAGER

#include<stdio.h>
#include<sys/stat.h>
#include<extension/webview_manager/webview_manager.h>
#include<gtk/gtk.h>



struct hldata_manager {
    FILE *data_manager_fp;
    FILE *session_state_fp;
    int state_count;
    struct hl_session_state_list *root_session_state;
};

struct hl_session_state_list {
    char *state_file_name;
    GBytes *state_data;
    char *state_url;
    char *state_id;
    char *state_url_name;
    struct hl_session_state_list *next;
};



struct hldata_manager *hl_data_manager_init();

void hl_data_manager_destroy(struct hldata_manager *data_manager);
void hl_data_manager_save(struct hldata_manager *data_manager);
void hl_data_manager_update_session_state_all(struct WKTabList *list, struct hldata_manager *data_manager);
char **hl_data_manager_load_urls(struct hldata_manager *data_manager);
void hl_data_manager_load(struct hldata_manager *data_manager);


void hl_data_manager_update_tab_session_state(struct WKTab *tab, struct hldata_manager *data_manager);
void hl_session_state_list_set(struct hl_session_state_list *state, char *state_file_name, char *state_url, char *state_id, char *state_url_name);
void hl_session_state_list_cpy(struct hl_session_state_list *state_dst, struct hl_session_state_list *state_src);
char *get_absolute_directory(char *directory);
char *get_cache_url_from_id(char *id);

#endif