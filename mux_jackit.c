#include "mux_gui.h"

void port_registration_callback(jack_port_id_t port, int register, void *arg);
/* ladspa section could be moved into mux_live_plugin_connect.c in future*/

void activate_plugins(void){
	live_plugin_jack_t * live_plugin_jack;
	Evas_List *list, *listc;
	for (list = live_plugin->plugin_data; list; list = evas_list_next(list)){
		if(live_plugin_jack->ladspa_active != TRUE){
			live_plugin_jack = (live_plugin_jack_t *)list->data;
			live_plugin_jack->ladspa_plugin->descriptor->activate(live_plugin_jack->ladspa_handle);
			live_plugin_jack->ladspa_active = TRUE;
		}
	}
}

void deactivate_plugins(void){
	live_plugin_jack_t * live_plugin_jack;
	Evas_List *list, *listc;
	for (list = live_plugin->plugin_data; list; list = evas_list_next(list)){
		live_plugin_jack = (live_plugin_jack_t *)list->data;
		live_plugin_jack->ladspa_plugin->descriptor->deactivate(live_plugin_jack->ladspa_handle);
	}
}

void connect_plugins_port_buffers(void){
	live_plugin_jack_t * live_plugin_jack;
	control_port_t * control_port;
	audio_port_t * audio_port;
	Evas_List *list, *lista;
	if(live_plugin->plugin_data){ 
		for (list = live_plugin->plugin_data; list; list = evas_list_next(list)){
			live_plugin_jack = (live_plugin_jack_t *)list->data;
			for (lista = live_plugin_jack->control_ports; lista; lista = evas_list_next(lista)){
				control_port  = (control_port_t *)lista->data;
				live_plugin_jack->ladspa_plugin->descriptor->connect_port(live_plugin_jack->ladspa_handle, control_port->control_port_index, &control_port->current_real_value);
			}	
		}
	}
}

/* end ladspa connect stuff */

int process (jack_nframes_t nframes, void *arg){
	live_plugin_jack_t * live_plugin_jack;
	control_port_t * control_port;
	audio_port_t * audio_port, * audio_port_out;
	int c;
	Evas_List *list, *lista;
	if(live_plugin){ 
		for (list = live_plugin->plugin_data; list; list = evas_list_next(list)){
			live_plugin_jack = (live_plugin_jack_t *)list->data;
			if(live_plugin_jack->bypass==1){
				for (lista = live_plugin_jack->Jack_input_ports; lista; lista = evas_list_next(lista)){
					audio_port  = (audio_port_t *)lista->data;
					audio_port->data = jack_port_get_buffer (audio_port->port, nframes);
					live_plugin_jack->ladspa_plugin->descriptor->connect_port(live_plugin_jack->ladspa_handle, audio_port->index, audio_port->data);
				}
				for (lista = live_plugin_jack->Jack_output_ports; lista; lista = evas_list_next(lista)){
					audio_port  = (audio_port_t *)lista->data;
					audio_port->data = jack_port_get_buffer (audio_port->port, nframes);	
					live_plugin_jack->ladspa_plugin->descriptor->connect_port(live_plugin_jack->ladspa_handle, audio_port->index, audio_port->data);
				}
				live_plugin_jack->ladspa_plugin->descriptor->run(live_plugin_jack->ladspa_handle, nframes);
			}else{
				c=0;
				for (lista = live_plugin_jack->Jack_input_ports; lista; lista = evas_list_next(lista)){
					audio_port  = (audio_port_t *)lista->data;
					audio_port->data = jack_port_get_buffer (audio_port->port, nframes);
					audio_port_out = NULL;
					audio_port_out = (audio_port_t *)evas_list_nth(live_plugin_jack->Jack_output_ports, c);
					if(audio_port_out){
						audio_port_out->data = jack_port_get_buffer (audio_port_out->port, nframes);
						memcpy (audio_port_out->data, audio_port->data, sizeof (jack_default_audio_sample_t) * nframes);
					}
					c++;
				}		
			}
		}
	}
	return 0;      
}

void jack_shutdown (void *arg)
{
	exit (1);
}

void jack_up(const char *client_name){
	const char **ports;
	const char *server_name = NULL;
	jack_options_t options = JackNullOption;
	jack_status_t status;
	
	jack_client = jack_client_open (client_name, options, &status, server_name);
	if (jack_client == NULL) {
		fprintf (stderr, "jack_client_open() failed, "
			 "status = 0x%2.0x\n", status);
		if (status & JackServerFailed) {
			fprintf (stderr, "Unable to connect to JACK server\n");
		}
		exit (1);
	}
	if (status & JackServerStarted) {
		fprintf (stderr, "JACK server started\n");
	}
	if (status & JackNameNotUnique) {
		client_name = jack_get_client_name(jack_client);
		fprintf (stderr, "unique name `%s' assigned\n", client_name);
	}
	jack_set_process_callback (jack_client, process, 0);
	jack_on_shutdown (jack_client, jack_shutdown, 0);
	jack_set_port_registration_callback (jack_client, port_registration_callback, NULL);

	printf ("engine sample rate: %" PRIu32 "\n", jack_get_sample_rate (jack_client));
	
	if (jack_activate (jack_client)) {
		fprintf (stderr, "cannot activate client");
		exit (1);
	}
}

void port_registration_callback(jack_port_id_t port, int reg, void *arg){
	printf("registered port %s \n", jack_port_name(jack_port_by_id (jack_client, port)));
	//printf("port registered %i \n", port);
}

void disconnect_jack_ports(live_plugin_jack_t *live_plugin_jack){
	audio_port_t *audio_port;
	Evas_List *list;
	const char **jack_ports = NULL;
	const char **jack_port_connections = NULL;
	unsigned long port_index, connection_index;

	

	jack_ports = jack_get_ports (jack_client, NULL, NULL, 0);
	for( port_index = 0; jack_ports[port_index] != NULL; port_index++ ){
	        jack_port_connections = jack_port_get_all_connections (jack_client, jack_port_by_name (jack_client, jack_ports[port_index]));
		if(jack_port_connections){	
			for( connection_index = 0; jack_port_connections[connection_index] != NULL; connection_index++ ){
				if(jack_disconnect (jack_client, jack_ports[port_index], jack_port_connections[connection_index])){
					printf("disconnection error %s => %s \n", jack_ports[port_index], jack_port_connections[connection_index]);
				}
			}
		}
		
	}
	// only unregister mux jack ports only
	for (list = live_plugin_jack->Jack_input_ports; list; list = evas_list_next(list)){
		audio_port = (audio_port_t *)list->data;
		jack_port_unregister (jack_client, audio_port->port);
		
	}
	for (list = live_plugin_jack->Jack_output_ports; list; list = evas_list_next(list)){
		audio_port = (audio_port_t *)list->data;
		jack_port_unregister (jack_client, audio_port->port);
	}
	
	/*jack_ports = jack_get_ports (jack_client, NULL, NULL, 0);
	for( port_index = 0; jack_ports[port_index] != NULL; port_index++ ){
	        if(jack_port_unregister (jack_client, jack_port_by_name (jack_client, jack_ports[port_index]))){
			printf("unregister error %s  \n", jack_ports[port_index]);
		}
	}*/
}

void register_ports(live_plugin_jack_t *live_plugin_jack){
	audio_port_t *audio_port;
	Evas_List *list;
	for (list = live_plugin_jack->Jack_input_ports; list; list = evas_list_next(list)){
		audio_port = (audio_port_t *)list->data;
		//jack_port_unregister (jack_client, audio_port->port);
		audio_port->port = jack_port_register (jack_client, audio_port->name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
		
	}
	for (list = live_plugin_jack->Jack_output_ports; list; list = evas_list_next(list)){
		audio_port = (audio_port_t *)list->data;
		//jack_port_unregister (jack_client, audio_port->port);
		audio_port->port = jack_port_register (jack_client, audio_port->name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	}
}

void store_jack_connections(void){
	jack_patch_t * jack_patch;
	const char **jack_ports = NULL;
	const char **jack_port_connections = NULL;
	char *port_name;
	unsigned long port_index, connection_index;
	if(live_plugin->Jack_Patch){
		live_plugin->Jack_Patch = evas_list_free(live_plugin->Jack_Patch);
	}	
	
	jack_ports = jack_get_ports (jack_client, 0, 0,  JackPortIsOutput);
	if (jack_ports)
		{			
		for( port_index = 0; jack_ports[port_index] != NULL; port_index++ ){
			jack_port_connections = jack_port_get_all_connections (jack_client, jack_port_by_name (jack_client, jack_ports[port_index]));
			if(jack_port_connections){	
				printf("%s \n", jack_ports[port_index]);
				for( connection_index = 0; jack_port_connections[connection_index] != NULL; connection_index++ ){
					jack_patch = malloc(sizeof(jack_patch_t));
					jack_patch->port = strdup(jack_ports[port_index]);
					jack_patch->connected_to = strdup(jack_port_connections[connection_index]);
					live_plugin->Jack_Patch = evas_list_append(live_plugin->Jack_Patch, jack_patch);
				}
			}
		}
	}
	/*jack_ports = jack_get_ports (jack_client, NULL, NULL, 0);
	for( port_index = 0; jack_ports[port_index] != NULL; port_index++ ){
	        jack_port_connections = jack_port_get_all_connections (jack_client, jack_port_by_name (jack_client, jack_ports[port_index]));
		if(jack_port_connections){	
			printf("%s \n", jack_ports[port_index]);
			for( connection_index = 0; jack_port_connections[connection_index] != NULL; connection_index++ ){
				jack_patch = malloc(sizeof(jack_patch_t));
				jack_patch->port = strdup(jack_ports[port_index]);
				jack_patch->connected_to = strdup(jack_port_connections[connection_index]);
				live_plugin->Jack_Patch = evas_list_append(live_plugin->Jack_Patch, jack_patch);
			}
		}
	}*/
	
}




void restore_jack_connections(void){
	Evas_List *list;
	jack_patch_t * jack_patch;
	printf("%i \n", evas_list_count(live_plugin->Jack_Patch));
	for (list = live_plugin->Jack_Patch; list; list = evas_list_next(list)){
		jack_patch = ( jack_patch_t*)list->data;
		
		if(jack_connect(jack_client, jack_patch->port, jack_patch->connected_to)){
			printf("connectioning %s --> %s\n", jack_patch->port, jack_patch->connected_to);
			printf("^^^^^ connection error ^^^^^ \n");
		}
	}
}


void Unpatch_Jack(void){
	live_plugin_jack_t * live_plugin_jack;
	Evas_List *list;
	store_jack_connections();
	for (list = live_plugin->plugin_data; list; list = evas_list_next(list)){
		live_plugin_jack = (live_plugin_jack_t *)list->data;
		disconnect_jack_ports(live_plugin_jack);
	}
	
	//deactivate_plugins();
}

void Repatch_Jack(void){
	live_plugin_jack_t * live_plugin_jack;
	Evas_List *list;
	for (list = live_plugin->plugin_data; list; list = evas_list_next(list)){
		live_plugin_jack = (live_plugin_jack_t *)list->data;
		register_ports(live_plugin_jack);
	}
	printf("ACTIVATE LADSPA plugins \n");
	activate_plugins();
	printf("CONNECT LADSPA PORT BUFFERS \n");
	connect_plugins_port_buffers();
	printf("RESTORE JACK CONNECTIONS \n");
	restore_jack_connections();
	
	
}
