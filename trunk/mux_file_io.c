#include "mux_gui.h"
void _ex_save(Etk_Object *obj, void *data);
void _ex_open(Etk_Object *obj, void *data);
void _ex_set_entry(Etk_Object *obj, void *data);
void _ex_confirm_save(Etk_Object *obj, int response_id, void *data);
void file_load(char *filename);

Etk_Widget *entry, *label, *button;
static Etk_Widget *dialog = NULL;
Etk_Widget *filechooser;
char buff[1024];

void filechooser_window_create(void *data){
	
	if (dialog){
		etk_window_delete_request(ETK_WIDGET(dialog));
	}

	dialog = etk_dialog_new();
	etk_window_title_set(ETK_WINDOW(dialog), "Select an Aye MUX Session to load!");
	etk_signal_connect("delete-event", ETK_OBJECT(dialog), ETK_CALLBACK(etk_window_hide_on_delete), NULL);

	filechooser = etk_filechooser_widget_new();
	etk_dialog_pack_in_main_area(ETK_DIALOG(dialog), filechooser, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	button = etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DOCUMENT_OPEN,  ETK_RESPONSE_OK);
	etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_ex_open), filechooser);
	
	etk_dialog_action_area_alignment_set(ETK_DIALOG(dialog), 1.0);

	etk_widget_show_all(dialog);
}

void _ex_open(Etk_Object *obj, void *data){
	sprintf(buff, "%s/%s", etk_filechooser_widget_current_folder_get(filechooser), etk_filechooser_widget_selected_file_get(filechooser)); 
	if (ecore_file_exists(buff) && !ecore_file_is_dir(buff)){
		etk_window_delete_request(ETK_WIDGET(dialog));
		file_load(buff);
	}
}


void filesaver_window_create(void *data){
	if (dialog){
		etk_window_delete_request(ETK_WIDGET(dialog));
	}

	dialog = etk_dialog_new();
	etk_window_title_set(ETK_WINDOW(dialog), "Save an Aye MUX Session as");
	etk_signal_connect("delete-event", ETK_OBJECT(dialog), ETK_CALLBACK(etk_window_hide_on_delete), NULL);

	filechooser = etk_filechooser_widget_new();
	etk_dialog_pack_in_main_area(ETK_DIALOG(dialog), filechooser, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	label = etk_label_new("Aye MUX Session filename : ");	
	etk_dialog_pack_widget_in_action_area(ETK_DIALOG(dialog), label, ETK_BOX_START, ETK_BOX_NONE, 2);
	entry = etk_entry_new();
	etk_signal_connect("row-clicked", ETK_OBJECT(ETK_FILECHOOSER_WIDGET(filechooser)->files_tree),  ETK_CALLBACK(_ex_set_entry), NULL);

	etk_dialog_pack_widget_in_action_area(ETK_DIALOG(dialog), entry, ETK_BOX_START, ETK_BOX_NONE, 2);
	etk_widget_size_request_set(entry, 300, 30);
	button = etk_dialog_button_add_from_stock(ETK_DIALOG(dialog), ETK_STOCK_DOCUMENT_SAVE,  ETK_RESPONSE_OK);
	etk_signal_connect("clicked", ETK_OBJECT(button), ETK_CALLBACK(_ex_save), filechooser);
	etk_dialog_action_area_alignment_set(ETK_DIALOG(dialog), 1.0);
	etk_dialog_action_area_homogeneous_set(ETK_DIALOG(dialog), FALSE);
	etk_widget_show_all(dialog);
}
 
void _ex_set_entry(Etk_Object *obj, void *data){
	char file_path[1024];
	sprintf(file_path, "%s/%s", etk_filechooser_widget_current_folder_get(filechooser), etk_filechooser_widget_selected_file_get(filechooser));
	if (ecore_file_exists(file_path) && !ecore_file_is_dir(file_path)){
		etk_entry_text_set(ETK_ENTRY(entry), etk_filechooser_widget_selected_file_get (filechooser));

	}else{
		etk_entry_text_set(ETK_ENTRY(entry), "");
	}
}

void _ex_save(Etk_Object *obj, void *data){
	Etk_Widget *msg;
	if(strcmp(etk_entry_text_get(ETK_ENTRY(entry)), "") != 0){
		sprintf(buff, "%s/%s", etk_filechooser_widget_current_folder_get(filechooser), etk_entry_text_get(ETK_ENTRY(entry))); 
		if (ecore_file_exists(buff) && !ecore_file_is_dir(buff)){
			msg = etk_message_dialog_new(ETK_MESSAGE_DIALOG_WARNING, ETK_MESSAGE_DIALOG_OK_CANCEL, "file exists! are you sure you want to over write it?");	 
			etk_signal_connect("response", ETK_OBJECT(msg), ETK_CALLBACK(_ex_confirm_save), msg);		
			etk_widget_show_all(msg);
		}else{
			etk_window_delete_request(ETK_WIDGET(dialog));
			file_save(buff);
		}
	}else{
		return;
	}
}

void _ex_confirm_save(Etk_Object *obj, int response_id, void *data){
	switch(response_id){
		case ETK_RESPONSE_OK:
			etk_object_destroy(ETK_OBJECT(obj));
			etk_window_delete_request(ETK_WIDGET(dialog));
			file_save(buff);
			break;
		default:
			etk_object_destroy(ETK_OBJECT(obj));
			break;
	}	
	
}
/* construct an xml file of rack
need to convert data into strings then into XML
 */
void file_save(char *filename){
	jack_patch_t *sjack_patch;
	live_plugin_t *slive_plugin;
	control_port_state_t *scontrol_port_state;
	live_plugin_state_t *slive_plugin_state;
	live_plugin_jack_t *slive_plugin_jack;
	control_port_t *scontrol_port;	
	midi_t *smidi;
	Evas_List *l1, *l2, *l3, *l4;
	xmlDocPtr doc = NULL;       
	xmlNodePtr root_node = NULL, node = NULL, node1 = NULL, node2 = NULL, node3 = NULL;
    	char buff[256];

	patches->file_name = strdup("filename");

	doc = xmlNewDoc(BAD_CAST "1.0");
    	root_node = xmlNewNode(NULL, BAD_CAST "ayeMux");
    	xmlDocSetRootElement(doc, root_node);
	for (l1 = patches->patch_data; l1; l1 = evas_list_next(l1)){
		slive_plugin = (live_plugin_t *)l1->data;
		node = xmlNewChild(root_node, NULL, BAD_CAST "patch", NULL);
		xmlNewProp(node, BAD_CAST "patch_name", slive_plugin->patch_name);
		sprintf(buff, "%i", slive_plugin->state_count);
		xmlNewProp(node, BAD_CAST "state_count", buff);
    		for (l2 = slive_plugin->plugin_data; l2; l2 = evas_list_next(l2)){
			slive_plugin_jack = (live_plugin_jack_t *)l2->data;
			node1 = xmlNewChild(node, NULL, BAD_CAST "live_plugin_jack", NULL);
			xmlNewProp(node1, BAD_CAST "ladspa_plugin-object_file", BAD_CAST slive_plugin_jack->ladspa_plugin->object_file);
			xmlNewProp(node1, BAD_CAST "plugin_name", BAD_CAST slive_plugin_jack->plugin_name);
			sprintf(buff, "%i", slive_plugin_jack->plugin_ref_ID);
			xmlNewProp(node1, BAD_CAST "plugin_ref_ID", BAD_CAST buff);
					
			for (l3 = slive_plugin_jack->control_ports; l3; l3 = evas_list_next(l3)){
				scontrol_port = (control_port_t *)l3->data;
				node2 = xmlNewChild(node1, NULL, BAD_CAST "control_port", NULL);
				xmlNewProp(node2, BAD_CAST "control_port_name", BAD_CAST scontrol_port->control_port_name);
				sprintf(buff, "%i", scontrol_port->midi_pedal_link);
				xmlNewProp(node2, BAD_CAST "midi_pedal_link", BAD_CAST buff);
				for (l4 = scontrol_port->state_data; l4; l4 = evas_list_next(l4)){
					node3 = xmlNewChild(node2, NULL, BAD_CAST "control_port_state", NULL);					
					scontrol_port_state = (control_port_state_t *)l4->data;
					sprintf(buff, "%f", scontrol_port_state->current_real_value);
					xmlNewProp(node3, BAD_CAST "control_port_state-current_real_value", BAD_CAST buff);
				}
				
			}
			node2 = xmlNewChild(node1, NULL, BAD_CAST "state", NULL);
			for (l3 = slive_plugin_jack->state_data; l3; l3 = evas_list_next(l3)){
				node3 = xmlNewChild(node2, NULL, BAD_CAST "live_plugin_state", NULL);				
				slive_plugin_state = (live_plugin_state_t *)l3->data;
				sprintf(buff, "%f", slive_plugin_state->wet_dry_value);
				xmlNewProp(node3, BAD_CAST "wet_dry_value", BAD_CAST buff);
				sprintf(buff, "%i", slive_plugin_state->bypass);				
				xmlNewProp(node3, BAD_CAST "bypass", BAD_CAST buff);
			}	
			
		}
		node1 = xmlNewChild(node, NULL, BAD_CAST "midi", NULL);
		for (l3 = slive_plugin->midi_data; l3; l3 = evas_list_next(l3)){
			smidi = (midi_t *)l3->data;
			node3 = xmlNewChild(node1, NULL, BAD_CAST "midi_state", NULL);
			sprintf(buff, "%i", smidi->type);
			xmlNewProp(node3, BAD_CAST "type", BAD_CAST buff);
			sprintf(buff, "%i", smidi->channel);
			xmlNewProp(node3, BAD_CAST "channel", BAD_CAST buff);
			sprintf(buff, "%i", smidi->program);
			xmlNewProp(node3, BAD_CAST "program", BAD_CAST buff);
		}
		node1 = xmlNewChild(node, NULL, BAD_CAST "jack", NULL);
		for (l3 = slive_plugin->Jack_Patch; l3; l3 = evas_list_next(l3)){
			node3 = xmlNewChild(node1, NULL, BAD_CAST "jack_patch", NULL);
			sjack_patch = (jack_patch_t *)l3->data;
			xmlNewProp(node3, BAD_CAST "port", BAD_CAST sjack_patch->port);
			xmlNewProp(node3, BAD_CAST "connected_to", BAD_CAST sjack_patch->connected_to);				
		}
	}
	xmlSaveFormatFileEnc(filename, doc, "UTF-8", 1);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	xmlMemoryDump();
}
xmlNode *parent_node = NULL;
int rack_count = 0;
int plugin_count = 0;
int plugin_state_count = 0;
int control_count = 0;
int state_count = 0;
int midi_count = 0;
int jack_count = 0;
Etk_Tree_Row *row_parent;
Etk_Tree_Row *nrow;
static void process_node(xmlNode * a_node){
	xmlNode *cur_node = NULL;
	xmlChar *char_data;
	live_plugin_t *live_plugin_1;
	plugin_desc_t *plugin_desc;
	live_plugin_state_t * live_plugin_state;
	live_plugin_jack_t * live_plugin_jack;
	control_port_t * control_port;	
	control_port_state_t * control_port_state;
	midi_t * midi;
	jack_patch_t *jack_patch;
	Evas_List * list;
	
	char midi_label[255];
	long li;
	int  i;
	
	char state_title[255];
	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			/* this where a the loading happens */
			
			/* this how we test */
			// use some brute force
			if(strcmp(cur_node->name, "patch") == 0){
				/* ports need to be Purged */
				//Repatch_Jack();
				//printf("creating Patch %s\n", xmlGetProp(cur_node, "patch_name"));
				//printf("Unpatching Jack\n");	
				Unpatch_Jack(); 
				//printf("Jack Unpatched\n");				
				
				rack_hide();
				rack_id_count++;
				live_plugin_1 = malloc(sizeof(live_plugin_t));
				live_plugin_1->plugin_data = NULL;
				live_plugin_1->plugin_count = 0;
				live_plugin_1->rack_base=0;
				live_plugin_1->state_count = 0;
				live_plugin_1->Jack_Patch = NULL;
				live_plugin_1->midi_data = NULL;
				live_plugin_1->patch_name = strdup(xmlGetProp(cur_node, "patch_name"));
				live_plugin_1->id = rack_id_count;
				live_plugin = live_plugin_1;
				
				patches->patch_data = evas_list_append(patches->patch_data, live_plugin);
				row = etk_tree_row_append(ETK_TREE(rack_tree), NULL, col1, etk_theme_icon_path_get(), etk_stock_key_get(ETK_STOCK_PLACES_USER_HOME, ETK_STOCK_SMALL), xmlGetProp(cur_node, "patch_name"), NULL);
				etk_tree_row_data_set(row, live_plugin);
				row_parent = row;
				for(i=0;i<strtol(xmlGetProp(cur_node,"state_count"),NULL,10);i++){
					add_state(live_plugin);
					sprintf(state_title, "state %i", live_plugin->state_count);
					nrow = etk_tree_row_append(ETK_TREE(rack_tree), row_parent, col1, etk_theme_icon_path_get(), etk_stock_key_get(ETK_STOCK_AUDIO_X_GENERIC, ETK_STOCK_SMALL), state_title, NULL);
					etk_tree_row_data_set(nrow, live_plugin->state_count);
					etk_tree_row_unfold(row_parent);
				}
			}
			//rack plugins
			if(strcmp(cur_node->name, "live_plugin_jack") == 0){
				printf("Creating Plugin : %s\n", xmlGetProp(cur_node, "plugin_ref_ID"));	
				char_data = xmlGetProp(cur_node, "plugin_ref_ID");
				li = strtol(char_data,NULL,10);
				for (list = mux_plugin_mgr->all_plugins; list; list = evas_list_next(list)){
					plugin_desc = (plugin_desc_t *)list->data;
					if(li == plugin_desc->id){
						printf("%i %i\n", plugin_desc->id, li);
						apply_plugin(plugin_desc, TRUE);				
						printf("Plugin Created: %s\n", xmlGetProp(cur_node, "plugin_ref_ID"));
						plugin_count++;
						control_count=0;
						plugin_state_count=0;
						midi_count=0;
						jack_count=0;
						break;
					}
				}
				
			}
			if(plugin_count == live_plugin->plugin_count){
				if(strcmp(cur_node->name, "control_port") == 0){
					control_count++;
					state_count=0;
				}
				if(strcmp(cur_node->name, "midi_pedal_link") == 0){
					live_plugin_jack = (live_plugin_jack_t *)evas_list_nth(live_plugin->plugin_data, (plugin_count-1));
					control_port = (control_port_t *)evas_list_nth(live_plugin_jack->control_ports, (control_count-1));
					control_port->midi_pedal_link = strtol(xmlGetProp(cur_node,"midi_pedal_link"),NULL,10);
				}
				if(strcmp(cur_node->name, "control_port_state") == 0){
					live_plugin_jack = (live_plugin_jack_t *)evas_list_nth(live_plugin->plugin_data, (plugin_count-1));
					control_port = (control_port_t *)evas_list_nth(live_plugin_jack->control_ports, (control_count-1));
					control_port_state = (control_port_state_t *)evas_list_nth(control_port->state_data, state_count);
					control_port_state->current_real_value = strtod(xmlGetProp(cur_node, "control_port_state-current_real_value"), NULL);
					state_count++;
				}
				if(strcmp(cur_node->name, "live_plugin_state") == 0){
					plugin_state_count++;
					if(plugin_state_count <= live_plugin->state_count){
						live_plugin_jack = (live_plugin_jack_t *)evas_list_nth(live_plugin->plugin_data, (plugin_count-1));
						live_plugin_state = (live_plugin_state_t *)evas_list_nth(live_plugin_jack->state_data, (plugin_state_count-1));
						live_plugin_state->wet_dry_value = strtod(xmlGetProp(cur_node, "wet_dry_value"), NULL);
						live_plugin_state->bypass = strtol(xmlGetProp(cur_node,"bypass"),NULL,10);
					}
				}
				
			}
			//midi
			if(strcmp(cur_node->name, "midi_state") == 0){
				printf("%s \n", cur_node->name);
				midi = (midi_t *)evas_list_nth(live_plugin->midi_data, midi_count);
				midi->type = strtol(xmlGetProp(cur_node,"type"), NULL, 10);
				midi->channel = strtol(xmlGetProp(cur_node,"channel"), NULL, 10);
				midi->program = strtol(xmlGetProp(cur_node,"program"), NULL, 10);
				// update interface
				if(midi_count==0){
					nrow = etk_tree_row_first_child_get (row_parent);
					
				}else{
					nrow = etk_tree_row_next_get(nrow);
				}
				
				if(midi->type==1){
					sprintf(midi_label, "ch# %i PC %i", midi->channel, midi->program);
				}
				if(midi->type==2){
					sprintf(midi_label, "ch# %i CC %i", midi->channel, midi->program);
				}
				etk_tree_row_fields_set(nrow, ETK_FALSE, col2, midi_label, NULL);
				etk_tree_row_select(nrow);
				midi_count++;
			}
			
			//jack connections
			if(strcmp(cur_node->name, "jack_patch") == 0){
				//jack_patch = malloc(sizeof(jack_patch_t));
				//jack_patch->port = strdup(xmlGetProp(cur_node,"port"));
				//jack_patch->connected_to = strdup(xmlGetProp(cur_node,"connected_to"));
				//live_plugin->Jack_Patch = evas_list_append(live_plugin->Jack_Patch, jack_patch);
				//printf("live_plugin->Jack_Patch count %i \n", evas_list_count(live_plugin->Jack_Patch));
				if(jack_port_flags(jack_port_by_name (jack_client, strdup(xmlGetProp(cur_node,"port")))) ==  (JackPortIsPhysical|JackPortIsOutput)){
					printf("JackPortIsPhysical : %s \n",  strdup(xmlGetProp(cur_node,"port")));
				}
				if(jack_port_flags(jack_port_by_name (jack_client, strdup(xmlGetProp(cur_node,"port")))) ==  JackPortIsOutput){
					printf("ouput : %s \n",  strdup(xmlGetProp(cur_node,"port")));
				}
				//printf("connect  %s > %s\n", strdup(xmlGetProp(cur_node,"port")), strdup(xmlGetProp(cur_node,"connected_to")));
				if(jack_connect(jack_client, strdup(xmlGetProp(cur_node,"port")), strdup(xmlGetProp(cur_node,"connected_to")))){
					printf("^^^^^ connection error ^^^^^ \n");
				}
			}
		}
		parent_node = cur_node;
		process_node(cur_node->children);
	}
}



void file_load(char *filename){
	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;	

	printf("file to load : %s \n", filename);
	
	/* make sur all jack coonectios are gone */
	doc = xmlReadFile(filename, NULL, 0);

	if (doc == NULL) {
	/* throw the not valid aymux file error*/
	fprintf(stderr, "Failed to parse %s\n", filename);
	return;
	}
	
	purge_current_session();
	root_element = xmlDocGetRootElement(doc);
	
	patches = malloc(sizeof(patches_t));
	patches->patch_data = NULL;
	patches->file_name = strdup(filename);
	
	
	process_node(root_element);
	
		
	//Repatch_Jack();
	set_state(etk_tree_row_data_get(etk_tree_selected_row_get(ETK_TREE(rack_tree))));
	adjust_rack_ui_controls();
	rack_show();	

	xmlFreeDoc(doc);
	xmlCleanupParser();
}

void purge_current_session(void){
	etk_tree_clear(rack_tree);
	/* clear the ui */
	rack_hide();
	rack_id_count = 0;
	Unpatch_Jack();
	/* TODO this should free memory of all racks rather that just leaving it */
	E_FREE(patches);
	patches = NULL;
}
