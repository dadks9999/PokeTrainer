#include "pebble.h"
#include "math.h"
#include "string.h"
     
#define MAX_STRING_LEN 100

char buffer[] = "00:00";
static Window *window;
TextLayer *text_layer;
static TextLayer *batt_layer, *accel_layer;
static BitmapLayer *image_layer;
GBitmap *image;
int EXP = 0;
uint32_t ID = RESOURCE_ID_Charmander;

void tick_handler(struct tm *tick_time,TimeUnits Units_changed){
     strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
     text_layer_set_text(text_layer, buffer);
}

void free	(void *ptr);

void up_click_handler(ClickRecognizerRef recognizer, void *context){
     free(image);
     if((ID = RESOURCE_ID_Charmeleon)){
          ID = RESOURCE_ID_Charizard;
          image = gbitmap_create_with_resource(RESOURCE_ID_Charizard);
          bitmap_layer_set_bitmap(image_layer, image);
     }
     else{}
}

void select_click_handler(ClickRecognizerRef recognizer, void *context){
     free(image);
     if((ID = RESOURCE_ID_Charmander)){
          ID = RESOURCE_ID_Charmeleon;
          image = gbitmap_create_with_resource(RESOURCE_ID_Charmeleon);
          bitmap_layer_set_bitmap(image_layer, image);
     }
     else{}
}

void down_click_handler(ClickRecognizerRef recognizer, void *context){
     free(image);
     /*if((image = gbitmap_create_with_resource(RESOURCE_ID_Charizard))){
          //ID = RESOURCE_ID_Charmeleon;
          image = gbitmap_create_with_resource(RESOURCE_ID_Charmeleon);
          bitmap_layer_set_bitmap(image_layer, image);
     }*/
     if((ID = RESOURCE_ID_Charmeleon)){
          ID = RESOURCE_ID_Charmander;
          image = gbitmap_create_with_resource(RESOURCE_ID_Charmander);
          bitmap_layer_set_bitmap(image_layer, image);
     }
     else{}
}

void config_provider(Window *window){
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction){
  switch(axis){
  	case ACCEL_AXIS_X:
    	if(direction > 0){
      	text_layer_set_text(accel_layer, "Accel tap: X (+)");
    	}
    	else{
      	text_layer_set_text(accel_layer, "Accel tap: X (-)");
    	}
    	break;
  	case ACCEL_AXIS_Y:
    	if(direction > 0){
      	text_layer_set_text(accel_layer, "Accel tap: Y (+)");
    	}
    	else{
      	text_layer_set_text(accel_layer, "Accel tap: Y (-)");
    	}
    	break;
  case ACCEL_AXIS_Z:
    	if(direction > 0){
     	text_layer_set_text(accel_layer, "Accel tap: Z (+)");
    	}
    	else{
      	text_layer_set_text(accel_layer, "Accel tap: Z (-)");
    	}
   	break;
  }
}

static void accel_raw_handler(AccelData *data, uint32_t num_samples){
  static char buffer[] = "XYZ: 9999 / 9999 / 9999";
  snprintf(buffer, sizeof("XYZ: 9999 / 9999 / 9999"), "XYZ: %d / %d / %d", data[0].x, data[0].y, data[0].z);
  text_layer_set_text(accel_layer, buffer);
}

void window_load(Window *window){     
     //setup text layer for time
     text_layer = text_layer_create(GRect(0, 120, 132, 168));
     text_layer_set_background_color(text_layer, GColorClear);
     text_layer_set_text_color(text_layer, GColorBlack);
     text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
     text_layer_set_font(text_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_POKEMON_GB_24)));
     layer_add_child(window_get_root_layer(window), (Layer*) text_layer);
     
     //Setup Battery Layer
     batt_layer = text_layer_create(GRect(5, 5, 144, 30));
     text_layer_set_font(batt_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_POKEMON_GB_8)));
     layer_add_child(window_get_root_layer(window), text_layer_get_layer(batt_layer));
 
     //Get info, copy to long-lived buffer and display
     BatteryChargeState state = battery_state_service_peek();
     static char buffer[] = "HP: 100/100";
     snprintf(buffer, sizeof("HP: 100/100"), "HP: %d/100", state.charge_percent);
     text_layer_set_text(batt_layer, buffer);
     
     //Subscribe to AccelerometerService (uncomment one to choose)
     //accel_tap_service_subscribe(accel_tap_handler);
     accel_data_service_subscribe(1, accel_raw_handler);
     
     //Setup Accel Layer
     accel_layer = text_layer_create(GRect(5, 15, 144, 30));
     text_layer_set_font(accel_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_POKEMON_GB_8)));
     text_layer_set_text(accel_layer, "Accel tap: N/A");
     layer_add_child(window_get_root_layer(window), text_layer_get_layer(accel_layer));

     //Get a time structure so that the face doesn't start blank
     struct tm *t;
     time_t temp;
     temp = time(NULL);
     t = localtime(&temp);   
     //Manually call the tick handler when the window is loading
     tick_handler(t, MINUTE_UNIT);
}

void window_unload(Window *window){
     text_layer_destroy(text_layer);
     gbitmap_destroy(image);
     bitmap_layer_destroy(image_layer);
     window_destroy(window);
}

void init(){
     window = window_create();
     window_set_window_handlers(window, (WindowHandlers){.load = window_load, .unload = window_unload,});
     window_stack_push(window, true);
     tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler) tick_handler);
     
     window_set_click_config_provider(window, (ClickConfigProvider) config_provider);
     
     Layer *window_layer = window_get_root_layer(window);
     GRect bounds = layer_get_frame(window_layer);
     
     // This needs to be deinited on app exit which is when the event loop ends
     image = gbitmap_create_with_resource(RESOURCE_ID_Charmander);

     // The bitmap layer holds the image for display
     image_layer = bitmap_layer_create(bounds);
     bitmap_layer_set_bitmap(image_layer, image);
     bitmap_layer_set_alignment(image_layer, GAlignCenter);
     layer_add_child(window_layer, bitmap_layer_get_layer(image_layer));
}

void deinit(){
     tick_timer_service_unsubscribe();
}

int main(void) {
     init();
     app_event_loop();
     deinit();
}