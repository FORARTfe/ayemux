#include "mux_gui.h"
void create_controls(live_plugin_jack_t * live_plugin_jack);
LADSPA_Data plugin_desc_get_default_control_value (plugin_desc_t * pd, unsigned long port_index, jack_nframes_t sample_rate);
/* more work needs to be done here to get all the plugin details so we can pass them on to the ui */
control_port_t * get_control_port_data(plugin_desc_t * desc, int i){
	control_port_t * control_port;
	
	
	control_port = malloc(sizeof(control_port_t));
	control_port->control_port_index = 0;
	control_port->control_port_type = 0;
	control_port->control_port_logarithmic = FALSE;
	control_port->max_value = 0;
	control_port->min_value = 0;
	control_port->control_port_name = NULL;	
	control_port->control_back = NULL;
	control_port->control_title = NULL;	
	control_port->clipper = NULL;
	control_port->control_obj_1 = NULL;
	control_port->control_obj_2 = NULL;
	control_port->control_obj_3 = NULL;
	control_port->control_obj_4 = NULL;
	control_port->control_obj_5 = NULL;	
	control_port->state_data = NULL;
	control_port->control_port_index = i;
	control_port->control_port_type = 0;
	control_port->control_port_name = strdup(desc->port_names[i]);
	
        
	if (LADSPA_IS_HINT_TOGGLED(desc->port_range_hints[i].HintDescriptor)) {
		control_port->control_port_type = 1;
		//printf("toggle\n");
	}else  if (LADSPA_IS_HINT_INTEGER(desc->port_range_hints[i].HintDescriptor)) {
		control_port->control_port_type = 2;
		//printf("integer\n");
	}else{
		control_port->control_port_type = 3;
		//printf("float\n");
	}	
	//printf("control type : %i\n", control_port->control_port_type);
	if (LADSPA_IS_HINT_LOGARITHMIC(desc->port_range_hints[i].HintDescriptor)) {
		control_port->control_port_logarithmic = TRUE;
		//printf("is logarithmic\n");
	}else{
		control_port->control_port_logarithmic = FALSE;
	}	
	return control_port;
	
}


void apply_plugin(void * data, Evas_Bool * load_from_file){
	live_plugin_jack_t *live_plugin_jack, *other_live_plugin_jack;	
	jack_port_t *jack_port;
	plugin_desc_t * desc;
	control_port_t * control_port;
	audio_port_t * audio_port;
	char *portname;
	LADSPA_Data DummyControl;
	Evas_List * list;
	int i;
	unsigned long icount = 0;
	unsigned long ocount = 0;
	unsigned long pcount = 0;
	Evas_Bool exists;

	desc = (plugin_desc_t *) data;
	
	live_plugin_jack = malloc(sizeof(live_plugin_jack_t));
	live_plugin_jack->Jack_input_ports = NULL;
   	live_plugin_jack->Jack_output_ports = NULL;
	live_plugin_jack->plugin_name  = NULL;
	live_plugin_jack->plugin_ref_ID = 0;
	live_plugin_jack->ladspa_plugin = desc;
        live_plugin_jack->ladspa_active = FALSE;
	live_plugin_jack->control_ports = NULL;
	live_plugin_jack->state_data = NULL;
	live_plugin_jack->control_port_count = 0;
	live_plugin_jack->bypass = 0;
	live_plugin_jack->wet_dry_value = 0;
	
	/* load ad initailise the plugin could move to a new function but am lazy */
	LADSPA_Descriptor_Function get_descriptor;
	const LADSPA_Descriptor * descriptor;
	unsigned long plugin_index;
	void * dl_handle;
	exists = FALSE;
	dl_handle = dlopen (live_plugin_jack->ladspa_plugin->object_file, RTLD_NOW|RTLD_GLOBAL);
	printf("%s \n", live_plugin_jack->ladspa_plugin->object_file);
	if (!dl_handle){
	      fprintf (stderr, "%s: error opening shared object file '%s': %s\n",
	               __FUNCTION__, live_plugin_jack->ladspa_plugin->object_file, dlerror());
	      return;
	}

	get_descriptor = (LADSPA_Descriptor_Function)dlsym (dl_handle, "ladspa_descriptor");
	plugin_index = 0;
	live_plugin_jack->ladspa_plugin->descriptor = get_descriptor (live_plugin_jack->ladspa_plugin->index);
	live_plugin_jack->ladspa_handle = malloc(sizeof(LADSPA_Handle));
	live_plugin_jack->ladspa_handle = live_plugin_jack->ladspa_plugin->descriptor->instantiate(live_plugin_jack->ladspa_plugin->descriptor, jack_get_sample_rate (jack_client));
	
	/* search for the same id if so we append the name */
	exists = FALSE;
	for (list = live_plugin->plugin_data; list; list = evas_list_next(list)){
		other_live_plugin_jack = (live_plugin_jack_t *) list->data;
		if(other_live_plugin_jack->plugin_ref_ID == desc->id){
			pcount++;
			exists = TRUE;
		}
	}
	if(exists){
		live_plugin_jack->plugin_name = malloc (strlen (desc->name) + 6);
		sprintf(live_plugin_jack->plugin_name, "%s : %i", desc->name, pcount);
	}else{
		live_plugin_jack->plugin_name = strdup(desc->name);
	}
	live_plugin_jack->plugin_ref_ID = desc->id;
	
	for (i = 0; i < desc->port_count; i++){
	     /*printf("port name : %s\n", desc->port_names[i]);*/
	      	  live_plugin_jack->ladspa_plugin->descriptor->connect_port(live_plugin_jack->ladspa_handle, i, &DummyControl);			
		  if (LADSPA_IS_PORT_CONTROL(desc->port_descriptors[i])){
			
			//printf("-------------\ncontrol port %i : %s \n", i, desc->port_names[i]);
			control_port = get_control_port_data(desc, i);
			control_port->parent = live_plugin_jack;
			if (LADSPA_IS_HINT_SAMPLE_RATE(desc->port_descriptors[i])) {
			    control_port->max_value = desc->port_range_hints[i].UpperBound * (LADSPA_Data) jack_get_sample_rate (jack_client);
			    control_port->min_value = desc->port_range_hints[i].LowerBound * (LADSPA_Data) jack_get_sample_rate (jack_client);
 			} else {
 			    control_port->max_value = desc->port_range_hints[i].UpperBound;
 			    control_port->min_value = desc->port_range_hints[i].LowerBound;
			    
  			}
			
			control_port->current_real_value = plugin_desc_get_default_control_value(desc, i, jack_get_sample_rate (jack_client)); 
	
					
			/* this is for ranges */
			if(control_port->control_port_type == 3){
				control_port->current_ui_value = (270.0f/100.0f) * ((control_port->current_real_value - control_port->min_value) / ((control_port->max_value - control_port->min_value)/100.0f));
			}else{	
				control_port->current_ui_value = control_port->current_real_value;
			}
			//printf("port type is : %i\n", control_port->control_port_type);
			live_plugin_jack->control_ports = evas_list_append(live_plugin_jack->control_ports, control_port);
			live_plugin_jack->control_port_count++;
		
		  }else{
			if (LADSPA_IS_PORT_AUDIO (desc->port_descriptors[i]))
		        {
			          if (LADSPA_IS_PORT_INPUT (desc->port_descriptors[i])){
					icount++;
					portname = malloc (strlen (live_plugin_jack->plugin_name) + 5 + strlen (desc->port_names[i]) + 1);
		 			//printf("audio input %i : %s \n", icount, desc->port_names[i]);
					sprintf(portname, "%s : %s", live_plugin_jack->plugin_name, desc->port_names[i]);
					audio_port = malloc(sizeof(audio_port_t));
					audio_port->name = strdup(portname);
					audio_port->port = NULL;
					audio_port->port = jack_port_register (jack_client, portname, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
					audio_port->index = i;
					live_plugin_jack->Jack_input_ports = evas_list_append (live_plugin_jack->Jack_input_ports, audio_port);
			           	free(portname);
			        }else if (LADSPA_IS_PORT_OUTPUT (desc->port_descriptors[i])){
					ocount++;
					//printf("audio ouput %i : %s \n", ocount, desc->port_names[i]);
				        portname = malloc (strlen (live_plugin_jack->plugin_name) + 5 + strlen (desc->port_names[i]) + 1);
		 			sprintf(portname, "%s : %s", live_plugin_jack->plugin_name, desc->port_names[i]);
					audio_port = malloc(sizeof(audio_port_t));
					audio_port->name = strdup(portname);
					audio_port->port = NULL;
					audio_port->port = jack_port_register (jack_client, portname, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
					audio_port->index = i;
					live_plugin_jack->Jack_output_ports = evas_list_append (live_plugin_jack->Jack_output_ports, audio_port);
					free(portname);
				}
			}
		}
	}
        live_plugin_jack->parent = live_plugin;
	live_plugin->plugin_data = evas_list_append(live_plugin->plugin_data, live_plugin_jack);
	live_plugin->plugin_count++;
	create_new_effect_front(live_plugin_jack);
	connect_plugins_port_buffers();	
	new_plugin_state(live_plugin_jack);
	rack_show();
}

LADSPA_Data
plugin_desc_get_default_control_value (plugin_desc_t * pd, unsigned long port_index, jack_nframes_t sample_rate)
{
  LADSPA_Data upper, lower;
  LADSPA_PortRangeHintDescriptor hint_descriptor;
  
  hint_descriptor = pd->port_range_hints[port_index].HintDescriptor;
  
  
  if (LADSPA_IS_HINT_SAMPLE_RATE(hint_descriptor)) {
    upper = pd->port_range_hints[port_index].UpperBound * (LADSPA_Data) sample_rate;
    lower = pd->port_range_hints[port_index].LowerBound * (LADSPA_Data) sample_rate;
  } else {
    upper = pd->port_range_hints[port_index].UpperBound;
    lower = pd->port_range_hints[port_index].LowerBound;
  }
  
  if (LADSPA_IS_HINT_LOGARITHMIC(hint_descriptor))
    {
      if (lower < FLT_EPSILON)
        lower = FLT_EPSILON;
    }
    

  if (LADSPA_IS_HINT_HAS_DEFAULT(hint_descriptor)) {
      
           if (LADSPA_IS_HINT_DEFAULT_MINIMUM(hint_descriptor)) {
    
      return lower;
       
    } else if (LADSPA_IS_HINT_DEFAULT_LOW(hint_descriptor)) {
        
      if (LADSPA_IS_HINT_LOGARITHMIC(hint_descriptor)) {
        return exp(log(lower) * 0.75 + log(upper) * 0.25);
      } else {
        return lower * 0.75 + upper * 0.25;
      }

    } else if (LADSPA_IS_HINT_DEFAULT_MIDDLE(hint_descriptor)) {
        
      if (LADSPA_IS_HINT_LOGARITHMIC(hint_descriptor)) {
        return exp(log(lower) * 0.5 + log(upper) * 0.5);
      } else {
        return lower * 0.5 + upper * 0.5;
      }

    } else if (LADSPA_IS_HINT_DEFAULT_HIGH(hint_descriptor)) {
      
      if (LADSPA_IS_HINT_LOGARITHMIC(hint_descriptor)) {
        return exp(log(lower) * 0.25 + log(upper) * 0.75);
      } else {
        return lower * 0.25 + upper * 0.75;
      }
      
    } else if (LADSPA_IS_HINT_DEFAULT_MAXIMUM(hint_descriptor)) {
      
      return upper;
    
    } else if (LADSPA_IS_HINT_DEFAULT_0(hint_descriptor)) {
      
      return 0.0;
      
    } else if (LADSPA_IS_HINT_DEFAULT_1(hint_descriptor)) {
      
      if (LADSPA_IS_HINT_SAMPLE_RATE(hint_descriptor)) {
        return (LADSPA_Data) sample_rate;
      } else {
        return 1.0;
      }
      
    } else if (LADSPA_IS_HINT_DEFAULT_100(hint_descriptor)) {
      
      if (LADSPA_IS_HINT_SAMPLE_RATE(hint_descriptor)) {
        return 100.0 * (LADSPA_Data) sample_rate;
      } else {
        return 100.0;
      }
      
    } else if (LADSPA_IS_HINT_DEFAULT_440(hint_descriptor)) {
      
      if (LADSPA_IS_HINT_SAMPLE_RATE(hint_descriptor)) {
        return 440.0 * (LADSPA_Data) sample_rate;
      } else {
        return 440.0;
      }
      
    }  
      
  } else { 
        
           if (LADSPA_IS_HINT_BOUNDED_BELOW(hint_descriptor)) {
      return lower;
    } else if (LADSPA_IS_HINT_BOUNDED_ABOVE(hint_descriptor)) {
      return upper;
    }
  }

  return 0.0;
}

