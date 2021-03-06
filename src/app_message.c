#include <pebble.h>

Layer* layer;
GRect bounds;
static Window* window;  
static TextLayer* text_layer;
static ScrollLayer* scroll_layer;

enum {
    ARTIST_KEY = 0, 
    TRACK_KEY = 1,
    LYRICS_KEY = 2
};

void send_message(DictionaryIterator* iterator){
    Tuple* artist = dict_find(iterator, ARTIST_KEY);
    Tuple* track = dict_find(iterator, TRACK_KEY);
    app_message_outbox_begin(&iterator);
    dict_write_cstring(iterator, ARTIST_KEY, artist->value->cstring);
    dict_write_cstring(iterator, TRACK_KEY, track->value->cstring);

    dict_write_end(iterator);
    app_message_outbox_send();
}

static void inbox_received_callback(DictionaryIterator* iterator, void* context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Message received!");
    Tuple* lyrics = dict_find(iterator, LYRICS_KEY);
    if (dict_find(iterator, LYRICS_KEY)) {
        scroll_layer = scroll_layer_create(bounds);
        scroll_layer_set_click_config_onto_window(scroll_layer, window);

        #ifdef PBL_ROUND
            scroll_layer_set_paging(scroll_layer, true);
        #endif
    
        text_layer_set_text(text_layer, lyrics->value->cstring);
        scroll_layer_add_child(scroll_layer, text_layer_get_layer(text_layer));
        layer_add_child(layer, scroll_layer_get_layer(scroll_layer));

        #ifdef PBL_ROUND
          text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
          uint8_t inset = 4;
          text_layer_enable_screen_text_flow_and_paging(text_layer, inset);
        #endif
        
        GRect bounds = layer_get_frame(layer);
        GSize max_size = text_layer_get_content_size(text_layer);
        text_layer_set_size(text_layer, max_size);
        scroll_layer_set_content_size(scroll_layer, GSize(bounds.size.w, max_size.h + 4));
    } else {    // redirect the iterator to JS app instead
      send_message(iterator);
    }
}

static void inbox_dropped_callback(AppMessageResult reason, void* context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator* iterator, AppMessageResult reason, void* context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator* iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

void init(void) {
    window = window_create();
    window_stack_push(window, true);

    layer = window_get_root_layer(window);
    bounds = layer_get_frame(layer);
    GRect max_text_bounds = GRect(0, 0, bounds.size.w, 2000);
    
    text_layer = text_layer_create(max_text_bounds);
    text_layer_set_text(text_layer, "Pebblyrics\n\nWaiting for phone..");
    text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
    text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
    layer_add_child(layer, text_layer_get_layer(text_layer));

    app_message_register_inbox_received(inbox_received_callback); 
    app_message_register_inbox_dropped(inbox_dropped_callback); 
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);

    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

void deinit(void) {
    app_message_deregister_callbacks();
    text_layer_destroy(text_layer);
    scroll_layer_destroy(scroll_layer);
    window_destroy(window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}