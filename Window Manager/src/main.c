#define _POSIX_C_SOURCE 200112L
#include<stdio.h>
#include<stdlib.h>
#include<assert.h>

#include<wayland-server-core.h>

#include<wlr/backend.h>
#include<wlr/render/wlr_renderer.h>
#include<wlr/render/allocator.h>
#include<wlr/types/wlr_compositor.h>
#include<wlr/types/wlr_data_device.h>

#include<wlr/types/wlr_input_device.h>
#include<wlr/types/wlr_cursor.h>
#include<wlr/types/wlr_keyboard.h>

#include<wlr/types/wlr_output.h>
#include<wlr/types/wlr_output_layout.h>

#include<wlr/types/wlr_scene.h>
#include<wlr/types/wlr_seat.h>

#include<wlr/types/wlr_xdg_shell.h>

struct hlwm_server
{
    struct wl_display *wl_display;
    struct wlr_backend *backend;
    struct wlr_renderer *renderer;
    struct wlr_allocator *allocator;

    struct wlr_xdg_shell *xdg_shell;
    struct wl_listener new_xdg_surface;

    struct wlr_scene *scene;
    struct wl_list views;

    struct wl_list outputs;
    struct wlr_output_layout *output_layout;
    struct wl_listener new_output;
};

struct hlwm_output {
    struct wl_list link;
    struct hlwm_server *server;
    struct wlr_output *wlr_output;
    struct wl_listener frame;
};

struct hlwm_view
{
    struct wl_list link;
	struct hlwm_server *server;
	struct wlr_xdg_surface *xdg_surface;
	struct wlr_scene_node *scene_node;
	struct wl_listener map;
	struct wl_listener unmap;
	struct wl_listener destroy;
	struct wl_listener request_move;
	struct wl_listener request_resize;
	int x, y;
};


static void output_frame(struct wl_listener *listener, void *data) {
    struct hlwm_output *output = wl_container_of(listener, output, frame);
    struct wlr_scene *scene = output->server->scene;
    struct wlr_scene_output *scene_output = wlr_scene_get_scene_output(scene, output->wlr_output);

    wlr_scene_output_commit(scene_output);

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    wlr_scene_output_send_frame_done(scene_output, &now);
}

static void server_new_output(struct wl_listener *listener, void *data) {
    struct hlwm_server *server = wl_container_of(listener, server, new_output);
    struct wlr_output *wlr_output = data;

    wlr_output_init_render(wlr_output, server->allocator, server->renderer);

    if (!wl_list_empty(&wlr_output->modes)) {
        struct wlr_output_mode *mode = wlr_output_preferred_mode(wlr_output);
        wlr_output_set_mode(wlr_output, mode);
        wlr_output_enable(wlr_output, true);
        if (!wlr_output_commit(wlr_output)) {
            return;
        }
    }

    struct hlwm_output *output = calloc(1, sizeof(struct hlwm_output));
    output->wlr_output = wlr_output;
    output->server = server;
    output->frame.notify = output_frame;
    wl_signal_add(&wlr_output->events.frame, &output->frame);
    wl_list_insert(&server->outputs, &output->link);

    wlr_output_layout_add_auto(server->output_layout, wlr_output);

}

static void server_new_xdg_surface(struct wl_listener *listener, void *data){
    struct hlwm_server *server = wl_container_of(listener, server, new_xdg_surface);
    struct wlr_xdg_surface *xdg_surface = data;

    if(xdg_surface->role == WLR_XDG_SURFACE_ROLE_POPUP) {
        struct wlr_xdg_surface *parent = wlr_xdg_surface_from_wlr_surface(xdg_surface->popup->parent);
        struct wlr_scene_node *parent_node = parent->data;
        xdg_surface->data = wlr_scene_xdg_surface_create(parent_node, xdg_surface);
        return;
    }
    assert(xdg_surface->role == WLR_XDG_SURFACE_ROLE_TOPLEVEL);

    struct hlwm_view *view = calloc(1, sizeof(struct hlwm_view));
    view->server = server;
    view->xdg_surface = xdg_surface;
    view->scene_node = wlr_scene_xdg_surface_create(&view->server->scene->node, view->xdg_surface);
    view->scene_node->data = view;
    xdg_surface->data = view->scene_node;
    
    
    
}

int main() {
    struct hlwm_server server;
    server.wl_display = wl_display_create();

    server.backend = wlr_backend_autocreate(server.wl_display);

    server.renderer = wlr_renderer_autocreate(server.backend);
    wlr_renderer_init_wl_display(server.renderer, server.wl_display);

    server.allocator = wlr_allocator_autocreate(server.backend, server.renderer);

    wlr_compositor_create(server.wl_display, server.renderer);
    wlr_data_device_manager_create(server.wl_display);

    server.output_layout = wlr_output_layout_create();

    wl_list_init(&server.outputs);
    server.new_output.notify = server_new_output;
    wl_signal_add(&server.backend->events.new_output, &server.new_output);

    server.scene = wlr_scene_create();
    wlr_scene_attach_output_layout(server.scene, server.output_layout);

    wl_list_init(&server.views);
    server.xdg_shell = wlr_xdg_shell_create(server.wl_display);
    server.new_xdg_surface.notify = server_new_xdg_surface;


    //wl_signal_add(&server.xdg_shell->events.)
    return 0;
}