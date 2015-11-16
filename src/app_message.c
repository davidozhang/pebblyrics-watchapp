#include <pebble.h>

Window *window;	
	
enum {
	ARTIST_KEY = 0,	
	TRACK_KEY = 1
};

void send_message(DictionaryIterator *iterator){
    Tuple* artist = dict_find(iterator, ARTIST_KEY);
    Tuple* track = dict_find(iterator, TRACK_KEY);
	app_message_outbox_begin(&iterator);
	dict_write_cstring(iterator, ARTIST_KEY, artist->value->cstring);
    dict_write_cstring(iterator, TRACK_KEY, track->value->cstring);
	
	dict_write_end(iterator);
  	app_message_outbox_send();
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Message received!");
  send_message(iterator);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

void init(void) {
	window = window_create();
	window_stack_push(window, true);
	
	// Register AppMessage handlers
	app_message_register_inbox_received(inbox_received_callback); 
	app_message_register_inbox_dropped(inbox_dropped_callback); 
	app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
		
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

void deinit(void) {
	app_message_deregister_callbacks();
	window_destroy(window);
}

int main( void ) {
	init();
	app_event_loop();
	deinit();
}