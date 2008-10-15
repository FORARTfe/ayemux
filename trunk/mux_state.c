#include "mux_gui.h"

void add_state(void *data){
	printf("add state \n");
	control_port_state_t * control_port_state;
	live_plugin_state_t * live_plugin_state;
	live_plugin_jack_t * live_plugin_jack;
	control_port_t * control_port;	
	Evas_List *list, *listc;
	live_plugin->state_count++;
	midi_create();
	for (list = live_plugin->plugin_data; list; list = evas_list_next(list)){
		live_plugin_jack = (live_plugin_jack_t *)list->data;
		live_plugin_state = malloc(sizeof(live_plugin_state_t));
		live_plugin_state->wet_dry_value = live_plugin_jack->wet_dry_value;
		live_plugin_state->bypass = live_plugin_jack->bypass;
		for (listc = live_plugin_jack->control_ports; listc; listc = evas_list_next(listc)){
			control_port = (control_port_t *)listc->data;
			control_port_state = malloc(sizeof(control_port_state_t));
			control_port_state->current_real_value = control_port->current_real_value;
			control_port->state_data = evas_list_append(control_port->state_data, control_port_state);
		}
		live_plugin_jack->state_data = evas_list_append(live_plugin_jack->state_data, live_plugin_state);
	}
	
}

void new_plugin_state(live_plugin_jack_t * live_plugin_jack){
	printf("made new state \n");
	control_port_state_t * control_port_state;
	live_plugin_state_t * live_plugin_state;
	control_port_t * control_port;
	Evas_List *listc;
	int i;
	printf("statecount = %i\n", (int)live_plugin->state_count);
	for(i=0;i<live_plugin->state_count;i++){
		live_plugin_state = malloc(sizeof(live_plugin_state_t));
		live_plugin_state->wet_dry_value = live_plugin_jack->wet_dry_value;
		live_plugin_state->bypass = (float)live_plugin_jack->bypass;
		live_plugin_jack->state_data = evas_list_append(live_plugin_jack->state_data, live_plugin_state);
	}
	
	for (listc = live_plugin_jack->control_ports; listc; listc = evas_list_next(listc)){
		control_port = (control_port_t *)listc->data;
		for(i=0;i<live_plugin->state_count;i++){
			control_port_state = malloc(sizeof(control_port_state_t));
			control_port_state->current_real_value = control_port->current_real_value;
			control_port->state_data = evas_list_append(control_port->state_data, control_port_state);
		}
	}
	
}

void remove_state(void *data){
	control_port_state_t * control_port_state;
	live_plugin_state_t * live_plugin_state;
	live_plugin_jack_t * live_plugin_jack;
	control_port_t * control_port;	
	Evas_List *list, *listc;
	for (list = live_plugin->plugin_data; list; list = evas_list_next(list)){
		live_plugin_jack = (live_plugin_jack_t *)list->data;
		live_plugin_state = (live_plugin_state_t *)evas_list_nth(live_plugin_jack->state_data, (int)data);
		live_plugin_jack->state_data = evas_list_remove(live_plugin_jack->state_data, live_plugin_state);
		for (listc = live_plugin_jack->control_ports; listc; listc = evas_list_next(listc)){
			control_port = (control_port_t *)listc->data;
			control_port_state = (control_port_state_t *)evas_list_nth(control_port->state_data, (int)data);
			control_port->state_data = evas_list_remove(control_port->state_data, control_port_state);
		}
	}
	live_plugin->state_count--;
}

void set_state(void *data){
	control_port_state_t * control_port_state;
	live_plugin_state_t * live_plugin_state;
	live_plugin_jack_t * live_plugin_jack;
	control_port_t * control_port;	
	Evas_List *list, *listc;
	for (list = live_plugin->plugin_data; list; list = evas_list_next(list)){
		live_plugin_jack = (live_plugin_jack_t *)list->data;
		live_plugin_state = (live_plugin_state_t *)evas_list_nth(live_plugin_jack->state_data, (int)data-1);
		live_plugin_jack->wet_dry_value = live_plugin_state->wet_dry_value;
		live_plugin_jack->bypass = live_plugin_state->bypass;
		for (listc = live_plugin_jack->control_ports; listc; listc = evas_list_next(listc)){
			control_port = (control_port_t *)listc->data;
			control_port_state = (control_port_state_t *)evas_list_nth(control_port->state_data, (int)data-1);
			control_port->current_real_value = control_port_state->current_real_value;
		}
	}
}

void save_state(void *data){
	control_port_state_t * control_port_state;
	live_plugin_state_t * live_plugin_state;
	live_plugin_jack_t * live_plugin_jack;
	control_port_t * control_port;	
	Evas_List *list, *listc;
	for (list = live_plugin->plugin_data; list; list = evas_list_next(list)){
		live_plugin_jack = (live_plugin_jack_t *)list->data;
		
		live_plugin_state = (live_plugin_state_t *)evas_list_nth(live_plugin_jack->state_data, (int)data-1);
		live_plugin_state->wet_dry_value = live_plugin_jack->wet_dry_value;
				
		live_plugin_state->bypass = live_plugin_jack->bypass;
		
		for (listc = live_plugin_jack->control_ports; listc; listc = evas_list_next(listc)){
			control_port = (control_port_t *)listc->data;
			control_port_state = (control_port_state_t *)evas_list_nth(control_port->state_data, (int)data-1);
			control_port_state->current_real_value = control_port->current_real_value;
		}
	}
}
