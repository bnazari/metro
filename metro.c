#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "http.h"
#include "itoa.h"

/* POST variables */
#define PAGE 1
#define BUTTON 2
#define KEY_LATITUDE 3
#define KEY_LONGITUDE 4
#define URL "http://[Link to webserver]/metro.php"

 
//  #define URL "http://requestb.in/patwe6pa"
/* If compiling this for iOS, set ANDROID to be false. */
#define ANDROID false

#if ANDROID
#define MY_UUID { 0x91, 0x41, 0xB6, 0x28, 0xBC, 0x89, 0x49, 0x8E, 0xB1, 0x47, 0x10, 0x34, 0xBF, 0xBE, 0x12, 0x98 }
#else
#define MY_UUID HTTP_UUID
#endif

#define HTTP_COOKIE 4887

PBL_APP_INFO(MY_UUID, "LA Metro", "Bernie Nazari", 1, 0,  RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_STANDARD_APP);

void get_sched();
void handle_init(AppContextRef ctx);
void http_success(int32_t request_id, int http_status, DictionaryIterator* received, void* context);
void http_failure(int32_t request_id, int http_status, void* context);
void window_appear(Window* me);
void httpebble_error(int error_code);

Window window;
TextLayer layer_text0;
TextLayer layer_text1;
TextLayer layer_text2;
TextLayer layer_text3;
TextLayer layer_text4;
static int current_page = 0;
static int our_latitude, our_longitude;
static bool located = false;



void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  DictionaryIterator* dict;
  HTTPResult  result = http_out_get(URL, HTTP_COOKIE, &dict);
  if (result != HTTP_OK) {
    httpebble_error(result);
    return;
  }
  dict_write_cstring(dict, PAGE, "0");
  dict_write_cstring(dict, BUTTON, "1");

  dict_write_int32(dict, KEY_LATITUDE, our_latitude);
	dict_write_int32(dict, KEY_LONGITUDE, our_longitude);
  result = http_out_send();
  if (result != HTTP_OK) {
    httpebble_error(result);
    return;
  }
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  DictionaryIterator* dict;
  HTTPResult  result = http_out_get(URL, HTTP_COOKIE, &dict);
  if (result != HTTP_OK) {
    httpebble_error(result);
    return;
  }
  dict_write_cstring(dict, PAGE, "0");
  dict_write_cstring(dict, BUTTON, "2");

	dict_write_int32(dict, KEY_LATITUDE, our_latitude);
	dict_write_int32(dict, KEY_LONGITUDE, our_longitude);
  result = http_out_send();
  if (result != HTTP_OK) {
    httpebble_error(result);
    return;
  }
}

void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  current_page=current_page + 1;
  DictionaryIterator* dict;
  HTTPResult  result = http_out_get(URL, HTTP_COOKIE, &dict);
  if (result != HTTP_OK) {
    httpebble_error(result);
    return;
  }
  dict_write_int32(dict, PAGE, current_page);
  dict_write_cstring(dict, BUTTON, "3");
	dict_write_int32(dict, KEY_LATITUDE, our_latitude);
	dict_write_int32(dict, KEY_LONGITUDE, our_longitude);
  result = http_out_send();
  if (result != HTTP_OK) {
    httpebble_error(result);
    return;
  }
}


void click_config_provider(ClickConfig **config, Window *window) {
  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;
  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
}



void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .messaging_info = {
      .buffer_sizes = {
        .inbound = 256,
        .outbound = 256,
      }
    }
  };

  app_event_loop(params, &handlers);
}

void http_success(int32_t request_id, int http_status, DictionaryIterator* received, void* context) {
//   if (request_id != HTTP_LOCATION) {
//     return;
//   }
  Tuple* tuple0 = dict_find(received, 0);
  Tuple* tuple1 = dict_find(received, 1);
  Tuple* tuple2 = dict_find(received, 2);
  Tuple* tuple3 = dict_find(received, 3);

  text_layer_set_text(&layer_text0, tuple0->value->cstring);
  text_layer_set_text(&layer_text1, tuple1->value->cstring);
  text_layer_set_text(&layer_text2, tuple2->value->cstring);
  text_layer_set_text(&layer_text3, tuple3->value->cstring);

}

void http_failure(int32_t request_id, int http_status, void* context) {
  httpebble_error(http_status >= 1000 ? http_status - 1000 : http_status);
}

void window_appear(Window* me) {
text_layer_set_text(&layer_text4, "Data by NextBus");
}

void location(float latitude, float longitude, float altitude, float accuracy, void* context) {
	// Fix the floats
	our_latitude = latitude * 10000;
	our_longitude = longitude * 10000;
	located = true;
	get_sched();

	}

void handle_init(AppContextRef ctx) {
  http_set_app_id(76782702);

  http_register_callbacks((HTTPCallbacks) {
    .success = http_success,
    .failure = http_failure,
    .location = location
    
  }, NULL);

  window_init(&window, "LA Metro");
  window_stack_push(&window, true);
  window_set_window_handlers(&window, (WindowHandlers){
    .appear  = window_appear
  });
  text_layer_init(&layer_text0, GRect(0, 0, 144, 30));
  text_layer_set_text_color(&layer_text0, GColorClear);
  text_layer_set_background_color(&layer_text0, GColorBlack);
  text_layer_set_font(&layer_text0, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(&layer_text0, GTextAlignmentCenter);
  layer_add_child(&window.layer, &layer_text0.layer);

  text_layer_init(&layer_text1, GRect(0, 30, 144, 30));
  text_layer_set_text_color(&layer_text1, GColorClear);
  text_layer_set_background_color(&layer_text1, GColorBlack);
  text_layer_set_font(&layer_text1, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(&layer_text1, GTextAlignmentCenter);
  layer_add_child(&window.layer, &layer_text1.layer);

  text_layer_init(&layer_text2, GRect(0, 60, 144, 22));
  text_layer_set_text_color(&layer_text2, GColorBlack);
  text_layer_set_background_color(&layer_text2, GColorClear);
  text_layer_set_font(&layer_text2, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(&layer_text2, GTextAlignmentCenter);
  layer_add_child(&window.layer, &layer_text2.layer);

  text_layer_init(&layer_text3, GRect(0, 82, 144, 32));
  text_layer_set_text_color(&layer_text3, GColorBlack);
  text_layer_set_background_color(&layer_text3, GColorClear);
  text_layer_set_font(&layer_text3, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(&layer_text3, GTextAlignmentCenter);
  layer_add_child(&window.layer, &layer_text3.layer);
  
  text_layer_init(&layer_text4, GRect(0, 125, 144, 32));
  text_layer_set_text_color(&layer_text4, GColorBlack);
  text_layer_set_background_color(&layer_text4, GColorClear);
  text_layer_set_font(&layer_text4, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(&layer_text4, GTextAlignmentCenter);
  layer_add_child(&window.layer, &layer_text4.layer);

  graphics_draw_line(ctx, GPoint(0, 55), GPoint(144, 59));

 located = false;
 get_sched();

   window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);
}


void get_sched(){
	if(!located) {
		http_location_request();
		return;
	}

  DictionaryIterator* dict;
  HTTPResult  result = http_out_get(URL, HTTP_COOKIE, &dict);
  if (result != HTTP_OK) {
    httpebble_error(result);
    return;
  }
  dict_write_cstring(dict, PAGE, "0");
  dict_write_cstring(dict, BUTTON, "0");

	dict_write_int32(dict, KEY_LATITUDE, our_latitude);
	dict_write_int32(dict, KEY_LONGITUDE, our_longitude);
  result = http_out_send();
  if (result != HTTP_OK) {
    httpebble_error(result);
    
    return;
  }
}


void httpebble_error(int error_code) {

  static char error_message[] = "UNKNOWN_HTTP_ERRROR_CODE_GENERATED";

  switch (error_code) {
    case HTTP_SEND_TIMEOUT:
      strcpy(error_message, "HTTP_SEND_TIMEOUT");
    break;
    case HTTP_SEND_REJECTED:
      strcpy(error_message, "HTTP_SEND_REJECTED");
    break;
    case HTTP_NOT_CONNECTED:
      strcpy(error_message, "HTTP_NOT_CONNECTED");
    break;
    case HTTP_BRIDGE_NOT_RUNNING:
      strcpy(error_message, "HTTP_BRIDGE_NOT_RUNNING");
    break;
    case HTTP_INVALID_ARGS:
      strcpy(error_message, "HTTP_INVALID_ARGS");
    break;
    case HTTP_BUSY:
      strcpy(error_message, "HTTP_BUSY");
    break;
    case HTTP_BUFFER_OVERFLOW:
      strcpy(error_message, "HTTP_BUFFER_OVERFLOW");
    break;
    case HTTP_ALREADY_RELEASED:
      strcpy(error_message, "HTTP_ALREADY_RELEASED");
    break;
    case HTTP_CALLBACK_ALREADY_REGISTERED:
      strcpy(error_message, "HTTP_CALLBACK_ALREADY_REGISTERED");
    break;
    case HTTP_CALLBACK_NOT_REGISTERED:
      strcpy(error_message, "HTTP_CALLBACK_NOT_REGISTERED");
    break;
    case HTTP_NOT_ENOUGH_STORAGE:
      strcpy(error_message, "HTTP_NOT_ENOUGH_STORAGE");
    break;
    case HTTP_INVALID_DICT_ARGS:
      strcpy(error_message, "HTTP_INVALID_DICT_ARGS");
    break;
    case HTTP_INTERNAL_INCONSISTENCY:
      strcpy(error_message, "HTTP_INTERNAL_INCONSISTENCY");
    break;
    case HTTP_INVALID_BRIDGE_RESPONSE:
      strcpy(error_message, "HTTP_INVALID_BRIDGE_RESPONSE");
    break;
    default: {
      strcpy(error_message, "HTTP_ERROR_UNKNOWN");
    }
  }

  text_layer_set_text(&layer_text1, error_message);
}
