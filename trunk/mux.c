#include "mux_gui.h"

int main(int argc, char **argv){
   etk_init(&argc, &argv);
   midi_start();
   jack_up("ayeMux"); 
   mux_main_show();
   etk_main();
   etk_shutdown();
   jack_client_close (jack_client);
   return 0;
}





