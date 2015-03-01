#include <pebble.h>

static Window *s_main_window; //the main screen of the watchface s_ stands for static
static TextLayer *s_time_layer; //the text we will use to display the time
static TextLayer *s_date_layer; //the text to display the date
static GFont *s_time_font;  // the font for the clock
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
//function prototypes
static void tickHandler(struct tm *tick_time, TimeUnits unit_change);
static void update_time();
  
static void tickHandler(struct tm *tick_time, TimeUnits unit_change){
  update_time();
}
static void update_time(){
static char buf[] = "00:00"; //the buffer we use to display the time, static so it lasts through more than one call
static char datebuf[] = "00/00/00"; //the buffer to display the date
  
  //get the tm struct
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp); //pass it time_t to get the local time
  
  //update the buffer with the time
  if(clock_is_24h_style() == true){
    strftime(buf, sizeof("00:00"), "%H:%M",tick_time);
  }else{
    strftime(buf,sizeof("00:00"), "%I:%M",tick_time);
  }
  
  //update the date
  strftime(datebuf, sizeof("00/00/00"), "%D", tick_time);
  
  //set the text to the date
  text_layer_set_text(s_time_layer,buf);
  text_layer_set_text(s_date_layer,datebuf);
}

//load the data for the window
static void loadWindow(Window *window){
  //load the background bitmap
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_NOTEBOOK); //load the bitmap image
  s_background_layer = bitmap_layer_create(GRect(0,0,144,168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);//add the bitmap image to the bitmap layer
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer)); //add the layer to the watchface
  
  //load the time text layer
  s_time_layer = text_layer_create(GRect(20, 40, 115, 38)); //initializes a new text layer
  text_layer_set_background_color(s_time_layer, GColorWhite);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  
  //load the date text layer
  s_date_layer = text_layer_create(GRect(21, 85, 115, 38)); //initializes a new text layer
  text_layer_set_background_color(s_date_layer, GColorWhite);
  text_layer_set_text_color(s_date_layer, GColorBlack);
 
 
  //load the custom font
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_KINDERGARTEN_30));
  
  //set it as the text font
  //for the time
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  //and for the date
  text_layer_set_font(s_date_layer, s_time_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer)); //add the time layer to the watchface
  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer)); //add the date layer to the watchface

}

//free the data from the Window
static void unloadWindow(Window *window){
  
  //unload the bitmap layer and the bitmap image
  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);
  
  //unload the custom font
  fonts_unload_custom_font(s_time_font);
  text_layer_destroy(s_date_layer); //free the memory from the date layer
  text_layer_destroy(s_time_layer); //free the memory from the time layer
}

// called when the app starts to allocate memory
static void init(){
  //create a new instance of window and attach it to the main pointer
  s_main_window = window_create();
  
  //set the event handlers for event in the window
  window_set_window_handlers(s_main_window, (WindowHandlers){
    .load = loadWindow, 
    .unload = unloadWindow
  });
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tickHandler);
  
  //show the window on the Pebble, animation = true
  window_stack_push(s_main_window, true);
  update_time();
  
}

//deallocates memory
static void deinit(){
  
  //get rid of the memory used for the window
  window_destroy(s_main_window);
}


int main(){
  init();
  app_event_loop(); //called to allow pebble to listen to system events
  deinit();
}