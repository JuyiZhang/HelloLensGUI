#include<extension/desktop_data_manager/desktop_data_manager.h>

char *default_layout_dir = "/.heyos/data/desktop/layout.data";
char *default_widget_data_dir = "/.heyos/data/desktop/widget/";

char *default_cache_path = "/.heyos/cache/desktop/data_manager/session_state";
char *default_datamanager_path = "/.heyos/data/desktop/data_manager/luopan.data";
char *default_log_path = "/.heyos/data/desktop/data_manager/luopan.log";

struct stat st = {0};

int desktop_data_manager_get_all_elements(struct desktop_widget_element_list *root_element) {

    default_layout_dir = get_absolute_directory(default_layout_dir);
    default_widget_data_dir = get_absolute_directory(default_widget_data_dir);

    FILE *fp = fopen(default_layout_dir, "r");
    if (fp == NULL) {
        g_print("File Does not Exist!\n");
        return 0;
    }
    struct desktop_widget_element_list *iterator = root_element;
    int count = 0;
    while(1) {
        iterator->widget_element = malloc(sizeof(struct desktop_widget_element));
        struct desktop_widget_element *op_element = iterator->widget_element;
        char widget_def[20];
        char *widget_data_location, *widget_data_location_full;
        widget_data_location = malloc(sizeof(char)*2048);
        widget_data_location_full = malloc(sizeof(char)*2048);
        fgets(widget_def, 20, fp);
        sscanf(widget_def, "%d %d %d %d %c", &op_element->x, &op_element->y, &op_element->width, &op_element->height, &op_element->type);
        fgets(widget_data_location, 2048, fp);
        widget_data_location[strcspn(widget_data_location, "\r\n")] = 0;
        op_element->data = malloc(sizeof(struct widget_data));
        strcpy(widget_data_location_full,widget_data_location);
        widget_data_location_full = g_strconcat("",default_widget_data_dir,widget_data_location_full, NULL);
        if(stat(widget_data_location_full, &st) == -1) {
            widget_get_definition(op_element->data, widget_data_location);
        } else {
            widget_get_definition(op_element->data, widget_data_location_full);
        }
        free(widget_data_location);
        free(widget_data_location_full);
        if (feof(fp)) {
            g_print("Count: %d\n", count);
            break;
        }
        count++;
        iterator->next = malloc(sizeof(struct desktop_widget_element));
        iterator = iterator->next;
    }
    return count;
}

void widget_get_definition(struct widget_data *data,char *location) {
    FILE *fp = fopen(location, "r");
    if (fp == NULL) {
        return;
    }
    setlocale(LC_ALL,"");
    char *current_locale = setlocale(LC_CTYPE, NULL); 
    while (TRUE)
    {
        char instruction[32];
        fgets(instruction, 32, fp);
        instruction[strcspn(instruction, "\r\n")] = 0;
        if(!strcmp("language", instruction)) {
            int localize_count;
            fscanf(fp, "%d\n", &localize_count);
            data->name = malloc(sizeof(wchar_t)*64);
            for(int i=0;i<localize_count;i++) {
                char locale_string[32];
                fgets(locale_string, 32, fp);
                locale_string[strcspn(locale_string, "\r\n")] = 0;
                char locale_corresponding_name[64];
                fgets(locale_corresponding_name, 64, fp);
                locale_corresponding_name[strcspn(locale_corresponding_name, "\r\n")] = 0;
                if (strstr(current_locale, locale_string) || i==0) {
                    strcpy(data->name, locale_corresponding_name);
                }
            }
        }
        if(!strcmp("cmd", instruction)) {
            data->exec_cmd = malloc(sizeof(char)*2048);
            fscanf(fp,"%s\n", data->exec_cmd);
        }
        if(!strcmp("loc", instruction)) {
            data->location = malloc(sizeof(char)*2048);
            fscanf(fp,"%s\n", data->location);
            break;
        }
        if(feof(fp)) {
            break;
        }
        /* code */
    }

}

char *get_absolute_directory(char *directory) {
    char *home_dir = getenv("HOME");
    return g_strconcat("", home_dir, directory, NULL);
}
