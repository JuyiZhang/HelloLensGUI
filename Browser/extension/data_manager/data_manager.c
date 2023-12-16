#include <extension/data_manager/data_manager.h>

char *default_dir = "/.heyos/data/luopan/data_manager";
char *default_cache_path = "/.heyos/cache/luopan/data_manager/session_state";
char *default_datamanager_path = "/.heyos/data/luopan/data_manager/luopan.data";
char *default_log_path = "/.heyos/data/luopan/data_manager/luopan.log";

struct stat st = {0};

struct hldata_manager *hl_data_manager_init() {
    default_dir = get_absolute_directory(default_dir);
    default_cache_path = get_absolute_directory(default_cache_path);
    default_datamanager_path = get_absolute_directory(default_datamanager_path);
    default_log_path = get_absolute_directory(default_log_path);
    struct hldata_manager *new_data_manager = NULL;
    new_data_manager = malloc(sizeof(struct hldata_manager));
    if (stat(default_dir,&st) == -1) { //File not found
        int result = mkdir(default_dir, 0700);
        printf("File Not Found, creating return a value of %d", result);
    }
    if (stat(default_cache_path,&st) == -1) { //File not found
        mkdir(default_cache_path, 0700);
    }
    new_data_manager->state_count = 0;
    return new_data_manager;
}

void hl_data_manager_destroy(struct hldata_manager *data_manager) {
    struct hl_session_state_list *op_state_list = data_manager->root_session_state;
    for(int i=0;i<data_manager->state_count;i++){
        struct hl_session_state_list *next_state_list = op_state_list->next;
        free(op_state_list);
        op_state_list = next_state_list;
    }
    free(data_manager);
}

void hl_data_manager_save(struct hldata_manager *data_manager) {
    struct hl_session_state_list *op_session_state = data_manager->root_session_state;
    data_manager->data_manager_fp = fopen(default_datamanager_path, "w");
    
    fprintf(data_manager->data_manager_fp,"%d\n",data_manager->state_count);
    for(int i=0;i<data_manager->state_count;i++) {
        fprintf(data_manager->data_manager_fp,"%s\n%s\n",op_session_state->state_id, op_session_state->state_url);
        op_session_state = op_session_state->next;
    }
}

char **hl_data_manager_load_urls(struct hldata_manager *data_manager) {
    if(stat(default_datamanager_path, &st) == -1) {
        data_manager->state_count = 0;
        return NULL;
    }
    data_manager->data_manager_fp = fopen(default_datamanager_path, "r");
    fscanf(data_manager->data_manager_fp,"%d",&data_manager->state_count);
    if (data_manager->state_count == 0) {
        return NULL;
    }
    char **url_list = calloc(data_manager->state_count,sizeof(char *));
    for(int i=0;i<data_manager->state_count;i++) {
        url_list[i] = malloc(sizeof(char)*256);
        fscanf(data_manager->data_manager_fp,"%s",url_list[i]);
        fscanf(data_manager->data_manager_fp,"%s",url_list[i]);
    }
    for(int i=0;i<data_manager->state_count;i++){
        g_print("The url is: %s\n", url_list[i]);
    }
    fclose(data_manager->data_manager_fp);
    return url_list;
}

void hl_data_manager_load(struct hldata_manager *data_manager) {
    if(stat(default_datamanager_path, &st) == -1) {
        data_manager->state_count = 0;
        return;
    }
    data_manager->data_manager_fp = fopen(default_datamanager_path, "r");
    fscanf(data_manager->data_manager_fp,"%d",&data_manager->state_count);
    if (data_manager->state_count == 0) {
        return;
    }
    struct hl_session_state_list *state_list = malloc(sizeof(struct hl_session_state_list));
    data_manager->root_session_state = state_list;
    for(int i=0;i<data_manager->state_count;i++){
        state_list->state_id = malloc(sizeof(char)*64);
        state_list->state_url = malloc(sizeof(char)*2048);
        fscanf(data_manager->data_manager_fp,"%s",state_list->state_id);
        fscanf(data_manager->data_manager_fp,"%s",state_list->state_url);
        state_list->state_file_name = get_cache_url_from_id(state_list->state_id);
        data_manager->session_state_fp = fopen(state_list->state_file_name, "r");
        stat(state_list->state_file_name, &st);
        gsize file_size = st.st_size;
        gconstpointer *data;
        data = malloc(st.st_size);
        fread(data, 1, file_size, data_manager->session_state_fp);
        state_list->state_data = g_bytes_new(data, st.st_size);
        fclose(data_manager->session_state_fp);
        if (i != data_manager->state_count-1) {
            state_list->next = malloc(sizeof(struct hl_session_state_list));
            state_list = state_list->next;
        }
    }
    fclose(data_manager->data_manager_fp);
    return;
}

void hl_data_manager_update_session_state_all(struct WKTabList *list, struct hldata_manager *data_manager){ //Adding the state for all tabs
    struct WKTab *op_tab = list->root;
    g_print("Updating total of %d tabs\n", list->tab_count);
    for(int i=0;i<list->tab_count;i++){
        g_print("Updating tab No. %d\n", i);
        hl_data_manager_update_tab_session_state(op_tab, data_manager);
        g_print("Updating tab No. %d Finished\n\n", i);
        op_tab = op_tab->next;
    }
}

void hl_data_manager_update_tab_session_state(struct WKTab *tab, struct hldata_manager *data_manager){ //everytime we call this function the tab state is saved

    //We get the data of the state here
    WebKitWebViewSessionState *op_state = webkit_web_view_get_session_state(tab->tab_view);
    GBytes *serialized_state = webkit_web_view_session_state_serialize(op_state);

    //Then we update the data of the state to a local file
    gchar *file_name = get_cache_url_from_id(tab->tab_id);
    g_print("The file name is %s\n", file_name);
    data_manager->session_state_fp = fopen(file_name,"w");
    gsize size_tab_state_data = g_bytes_get_size(serialized_state);
    fwrite(g_bytes_get_data(serialized_state, &size_tab_state_data), 1, size_tab_state_data, data_manager->session_state_fp);
    fclose(data_manager->session_state_fp);

    //we check if the tab already exists, if not it will be appended to the list
    struct hl_session_state_list *new_state_list = malloc(sizeof(struct hl_session_state_list));
    hl_session_state_list_set(new_state_list, file_name, tab->tab_url, tab->tab_id, "");
    if (data_manager->state_count == 0) { //If we need to init
        g_print("First tab, Initiating...\n");
        data_manager->root_session_state = new_state_list;
        data_manager->state_count++;
        return;
    }
    struct hl_session_state_list *op_state_list = data_manager->root_session_state;
    for (int i=0;i<data_manager->state_count;i++){
        g_print("The file name for looked state is %s\n", op_state_list->state_file_name);
        if (!strcmp(op_state_list->state_id, tab->tab_id)) {//If the name already exist, we dont need to do anything
            hl_session_state_list_cpy(op_state_list, new_state_list);
            g_print("Tab name found, update tab only\n");
            return;
        }
        if (i == data_manager->state_count-1)
            break;
        op_state_list = op_state_list->next; //We move to the next element
    } //at the end if the function does not return it means we did not find the tab, and we need to add it
    g_print("Updating with new tab...\n");
    data_manager->state_count++;
    g_print("Count of State: %d with state id: %s\n", data_manager->state_count, new_state_list->state_id);
    op_state_list->next = new_state_list;
}

char *get_absolute_directory(char *directory) {
    char *home_dir = getenv("HOME");
    return g_strconcat("", home_dir, directory, NULL);
}

char *get_cache_url_from_id(char *id) {
    return g_strconcat("",default_cache_path,"/",id,".cache",NULL);
}

void hl_session_state_list_set(struct hl_session_state_list *state, char *state_file_name, char *state_url, char *state_id, char *state_url_name) {
    state->state_id = state_id;
    state->state_file_name = state_file_name;
    state->state_url = state_url;
    state->state_url_name = state_url_name;
}

void hl_session_state_list_cpy(struct hl_session_state_list *state_dst, struct hl_session_state_list *state_src) {
    state_dst->state_id = state_src->state_id;
    state_dst->state_file_name = state_src->state_file_name;
    state_dst->state_url = state_src->state_url;
    state_dst->state_url_name = state_src->state_url_name;
}