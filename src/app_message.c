#include <pebble.h>

static Window* window;	
static TextLayer* text_layer;

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
        text_layer_set_text(text_layer, lyrics->value->cstring);
        text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
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
    
    Layer* layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(layer);
    
    text_layer = text_layer_create(bounds);
	text_layer_set_text(text_layer, "Pebblyrics");
    text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
    layer_add_child(layer, text_layer_get_layer(text_layer));
	// Register AppMessage handlers
	app_message_register_inbox_received(inbox_received_callback); 
	app_message_register_inbox_dropped(inbox_dropped_callback); 
	app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
		
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

void deinit(void) {
	app_message_deregister_callbacks();
    text_layer_destroy(text_layer);
	window_destroy(window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}