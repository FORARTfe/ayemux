#include "mux_gui.h"

void _ex_changed_value(Etk_Object *object, double value, void *data);
void _ex_changed_value_control(Etk_Object *object, double value, void *data);
void _ex_Binding(Etk_Object *obj, void *data);
void _ex_Binding_Control(Etk_Object *obj, void *data);
void _ex_Remove_Binding_Control(Etk_Object *obj, void *data);


static Etk_Widget *midi_win = NULL;
static Etk_Widget *midi_win_control = NULL;
Etk_Widget *spinner, *spinner1, *spinnerControl;
Etk_Widget *combobox;
int spinner_data, spinner1_data, spinnerControl_data;
int        thr_id;  
pthread_t  p_thread;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
void *mydata;

void show_midi_window_control(void *data){
	Etk_Widget *vbox, *vbox1, *hbox;
	Etk_Widget *frame;
	Etk_Widget *button;
	Etk_Widget *label;
	Etk_Combobox_Item *item;
	control_port_t * control_port;
	control_port = (control_port_t *)data;
	mydata = data;
	midi_t * midibinding;
	printf("tenative midi connection to ** %s \n", control_port->control_port_name);

	if (midi_win_control){
		etk_widget_show(ETK_WIDGET(midi_win_control));
		return;
	}

	midi_win_control = etk_window_new();

	etk_window_title_set(ETK_WINDOW(midi_win_control), "Set this controls Midi Properties");
	etk_window_resize(ETK_WINDOW(midi_win_control), 200, 0);
	etk_signal_connect("delete-event", ETK_OBJECT(midi_win_control), ETK_CALLBACK(etk_window_hide_on_delete), NULL);

	vbox = etk_vbox_new(ETK_FALSE, 0);
	etk_container_add(ETK_CONTAINER(midi_win_control), vbox);

	frame = etk_frame_new("Set Midi Binding for Control");
	etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 5);

	hbox = etk_hbox_new(ETK_FALSE, 0);
	etk_container_add(ETK_CONTAINER(frame), hbox);
	label = etk_label_new("select control change ch#");
	etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 3);	
	
	spinnerControl = etk_spinner_new(1.0, 16.0, 1, 1.0, 5.0);
	etk_box_append(ETK_BOX(hbox), spinnerControl, ETK_BOX_START, ETK_BOX_NONE, 7);
	etk_signal_connect("value-changed", ETK_OBJECT(spinnerControl), ETK_CALLBACK(_ex_changed_value_control), 3);

	button = etk_button_new_with_label("Add Binding");
	etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("clicked", ETK_OBJECT(button), _ex_Binding_Control, mydata);
	
	button = etk_button_new_with_label("Remove Binding");
	etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("clicked", ETK_OBJECT(button), _ex_Remove_Binding_Control, mydata);
	
	etk_widget_show_all(midi_win_control);   
}

void show_midi_window(void *data){
	Etk_Widget *vbox, *vbox1, *hbox;
	Etk_Widget *frame;
	Etk_Widget *button;
	Etk_Widget *label;
	Etk_Combobox_Item *item;
	midi_t * midibinding;
	
	if (midi_win){
		etk_widget_show(ETK_WIDGET(midi_win));
		return;
	}
	
	midibinding = (midi_t *)evas_list_nth(live_plugin->midi_data, (int)etk_tree_row_data_get(etk_tree_selected_row_get(rack_tree))-1);
	if(midibinding->channel != 0){
		spinner_data = midibinding->channel;
	}else{
		spinner_data = 1;
	}
	spinner1_data = midibinding->program;
		
	midi_win = etk_window_new();
	etk_window_title_set(ETK_WINDOW(midi_win), "Set Midi Properties");
	etk_window_resize(ETK_WINDOW(midi_win), 200, 0);
	etk_signal_connect("delete-event", ETK_OBJECT(midi_win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
	vbox = etk_vbox_new(ETK_FALSE, 0);
	etk_container_add(ETK_CONTAINER(midi_win), vbox);

	frame = etk_frame_new("Set Midi Binding for Rack State");
	etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 5);
	
	hbox = etk_hbox_new(ETK_FALSE, 0);
	etk_container_add(ETK_CONTAINER(frame), hbox);
	label = etk_label_new("ch#");
	etk_box_append(ETK_BOX(hbox), label, ETK_BOX_START, ETK_BOX_NONE, 3);	
	
	spinner = etk_spinner_new(1.0, 16.0, midibinding->channel, 1.0, 5.0);
	etk_box_append(ETK_BOX(hbox), spinner, ETK_BOX_START, ETK_BOX_NONE, 7);
	etk_signal_connect("value-changed", ETK_OBJECT(spinner), ETK_CALLBACK(_ex_changed_value), 1);

	combobox = etk_combobox_new_default();
	etk_box_append(ETK_BOX(hbox), combobox, ETK_BOX_START, ETK_BOX_NONE, 2);
	item = etk_combobox_item_append(ETK_COMBOBOX(combobox), "Program Change");
	etk_combobox_item_data_set(item, 1);
	if(midibinding->type==1){
		etk_combobox_active_item_set(combobox, item);
	}	
	item = etk_combobox_item_append(ETK_COMBOBOX(combobox), "Control Change");
	etk_combobox_item_data_set(item, 2);
	if(midibinding->type==2){
		etk_combobox_active_item_set(combobox, item);
	}

	spinner1 = etk_spinner_new(0.0, 127.0, midibinding->program, 1.0, 5.0);
	etk_box_append(ETK_BOX(hbox), spinner1, ETK_BOX_START, ETK_BOX_NONE, 7);
	etk_signal_connect("value-changed", ETK_OBJECT(spinner1), ETK_CALLBACK(_ex_changed_value), 2);

	/*
	button = etk_button_new_with_label("Learn");
	etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);	
	*/

	button = etk_button_new_with_label("Add Binding");
	etk_box_append(ETK_BOX(vbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("clicked", ETK_OBJECT(button), _ex_Binding, data);
	
	
	
	etk_widget_show_all(midi_win);   
}

void _ex_changed_value(Etk_Object *object, double value, void *data){
	printf("** %d \n", value);
	if(data==1){
		spinner_data = value;
	}
	if(data == 2){
		spinner1_data = value;
	}
	if(data == 3){
		
		spinnerControl_data = value;
	}
}

void _ex_changed_value_control(Etk_Object *object, double value, void *data){
	printf("** %d \n", value);
	spinnerControl_data = value;
}

void _ex_Remove_Binding_Control(Etk_Object *obj, void *data){
	control_port_t * control_port;
	control_port = (control_port_t *)mydata;
	control_port->midi_pedal_link = NULL;
	control_port->midi_pedal_ui = FALSE;
	printf("remove midi from ** %s \n", control_port->control_port_name);
	etk_widget_hide(ETK_WIDGET(midi_win_control));
	adjust_rack_ui_controls();
}

void _ex_Binding_Control(Etk_Object *obj, void *data){
	control_port_t * control_port;
	control_port = (control_port_t *)mydata;
	printf("add midi to ** %s channel %i\n", control_port->control_port_name, spinnerControl_data);
	control_port->midi_pedal_link = 1;
	control_port->midi_pedal_ui = TRUE;
	etk_widget_hide(ETK_WIDGET(midi_win_control));
	adjust_rack_ui_controls();
}

void _ex_Binding(Etk_Object *obj, void *data){
	Etk_Tree_Row *irow;	
	Etk_Combobox_Item *active_item;
	midi_t * midibinding;
	char midi_label[255];
	irow = etk_tree_selected_row_get(rack_tree);
	active_item = etk_combobox_active_item_get(combobox);
	midibinding = (midi_t *)evas_list_nth(live_plugin->midi_data, (int)etk_tree_row_data_get(irow)-1);

	midibinding->type = (int)etk_combobox_item_data_get(active_item);
	midibinding->channel = spinner_data;
	midibinding->program = spinner1_data;
	midibinding->trow = irow;
	if(midibinding->type==1){
		sprintf(midi_label, "ch# %i PC %i", midibinding->channel, midibinding->program);
	}
	if(midibinding->type==2){
		sprintf(midi_label, "ch# %i CC %i", midibinding->channel, midibinding->program);
	}
	etk_tree_row_fields_set(irow, ETK_FALSE, col2, midi_label, NULL);
	etk_widget_hide(ETK_WIDGET(midi_win));
}

/* built when a state is created */
void midi_create(void){
	midi_t * midibinding;
	
	midibinding = malloc(sizeof(midi_t));
	midibinding->type = 0;
	midibinding->channel = 0;
	midibinding->program = 0;
	midibinding->trow = NULL;

	// here is the crash
	live_plugin->midi_data = evas_list_append(live_plugin->midi_data, midibinding);
	if (evas_list_alloc_error()){
	     fprintf(stderr, "ERROR: Memory is low. List allocation failed.\n");
	     exit(-1);
	}
	
}

void midi_delete(void *data){
	midi_t * midibinding;
	midibinding = (live_plugin_state_t *)evas_list_nth(live_plugin->midi_data, (int)data-1);
	live_plugin->midi_data = evas_list_remove(live_plugin->midi_data, midibinding);
}


void midi_change_rack(live_plugin_t * lp, int state, Etk_Tree_Row *trow){
	rack_hide();
	if(lp->id = live_plugin->id){
		set_state(state);
	}else{
		Unpatch_Jack();
		live_plugin = lp;
		Repatch_Jack();
		set_state(state);
	}
	adjust_rack_ui_controls();
	rack_show();
	etk_tree_row_select(trow);
}

void midi_set(snd_seq_event_t *ev, int type){
	/* loop through all patches */
	live_plugin_t * lp;
	midi_t * mb;
	int c;
	Evas_List *list_a, *list_b, *list_c;
	for (list_a = patches->patch_data; list_a; list_a = evas_list_next(list_a)){
		lp = (live_plugin_t *)list_a->data;
		c=0;
		for (list_b = lp->midi_data; list_b; list_b = evas_list_next(list_b)){
			mb = (midi_t *)list_b->data;
			c++;
			/* test to match */
			if((mb->type == type)&(mb->channel==(ev->data.control.channel+1))&(mb->program==(ev->data.control.value+1))){
				midi_change_rack(lp, c, mb->trow);
				return;
			}
		}
	}
}
void pedal_set(snd_seq_event_t *ev){
	live_plugin_jack_t *live_plugin_jack;
	control_port_t *control_port;	
	Evas_List *l2, *l3;
	for (l2 = live_plugin->plugin_data; l2; l2 = evas_list_next(l2)){
		live_plugin_jack = (live_plugin_jack_t *)l2->data;
		for (l3 = live_plugin_jack->control_ports; l3; l3 = evas_list_next(l3)){
			control_port = (control_port_t *)l3->data;
			printf("hit midi %i %i \n", control_port->midi_pedal_link, ev->data.control.channel);
			if(control_port->midi_pedal_link == ev->data.control.channel+1){
				printf("hit midi %i \n", control_port->midi_pedal_link);
				 control_port->current_real_value = control_port->min_value + (((control_port->max_value - control_port->min_value)/128.0f)*ev->data.control.value);
				adjust_rack_ui_controls();
			}
		}
	}
}
/* midi setup */
snd_seq_t *open_seq(void) {

	snd_seq_t *seq_handle;
	int portid;

	if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_INPUT, 0) < 0) {
		fprintf(stderr, "Error opening ALSA sequencer.\n");
		exit(1);
	}
	snd_seq_set_client_name(seq_handle, "ayeMUX");
	if ((portid = snd_seq_create_simple_port(seq_handle, "IN PORT 0",
	        SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
	        SND_SEQ_PORT_TYPE_APPLICATION)) < 0) {
		fprintf(stderr, "Error creating sequencer port.\n");
		exit(1);
	}
	return(seq_handle);
}

/* handles all the midi calls */
void midi_action(snd_seq_t *seq_handle) {
	snd_seq_event_t *ev;
	do {
	snd_seq_event_input(seq_handle, &ev);
		printf( "%i \n", ev->data.control.channel, ev->data.control.value);
		switch (ev->type) {
			
			case SND_SEQ_EVENT_CONTROLLER: 
				printf("Control event on Channel %2d: %5d       \n",
			        ev->data.control.channel, ev->data.control.value);
				midi_set(ev, 2);
				pedal_set(ev);
			break;
			case SND_SEQ_EVENT_PITCHBEND:
				printf("Pitchbender event on Channel %2d: %5d   \n", 
			        ev->data.control.channel, ev->data.control.value);
			break;
			case SND_SEQ_EVENT_NOTEON:
				printf("Note On event on Channel %2d: %5d       \n",
			        ev->data.control.channel, ev->data.note.note);
			break;        
			case SND_SEQ_EVENT_NOTEOFF: 
				printf("Note Off event on Channel %2d: %5d      \n",         
			        ev->data.control.channel, ev->data.note.note);           
			break;        
			case SND_SEQ_EVENT_PGMCHANGE: 
				printf("PGM event on Channel %2d: %5d      \n",         
			        ev->data.control.channel, ev->data.control.value);   
				midi_set(ev, 1);
			break; 
		}
	   	snd_seq_free_event(ev);
	  } while (snd_seq_event_input_pending(seq_handle, 0) > 0);
}

/* midi thread */

void midi_thread(void *data){
	snd_seq_t *seq_handle;
	int npfd;
	struct pollfd *pfd;

	seq_handle = open_seq();
	npfd = snd_seq_poll_descriptors_count(seq_handle, POLLIN);
	pfd = (struct pollfd *)alloca(npfd * sizeof(struct pollfd));
	snd_seq_poll_descriptors(seq_handle, pfd, npfd, POLLIN);
	while (1) {
		if (poll(pfd, npfd, 100000) > 0) {
			midi_action(seq_handle);
		}  
	}
}

void midi_start(void){
	thr_id = pthread_create(&p_thread, NULL, (void*(*)(void*))midi_thread, (void*)0);
}
