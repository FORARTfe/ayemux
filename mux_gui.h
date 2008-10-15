#include <Imlib2.h>
#include <Etk.h>
#include <Eet.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_File.h>
#include <Ecore_X_Cursor.h>
#include <Evas.h>
#include <Edje.h>
#include <Ecore_Evas.h>
#include <Epsilon.h>
#include <ladspa.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <dlfcn.h>
#include <math.h>
#include <float.h>
#include <jack/jack.h>
#include <alsa/asoundlib.h>
#include <pthread.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>

typedef struct _plugin_mgr plugin_mgr_t;
struct _plugin_mgr
{
  Evas_List * all_plugins;
  Evas_List * plugins;
  long plugin_count;
};

typedef struct _plugin_desc plugin_desc_t;

struct _plugin_desc
{
	LADSPA_Descriptor *      descriptor;
	char *                   object_file;
	unsigned long            index;
	unsigned long            id;
	char *                   name;
	LADSPA_Properties        properties;
	Evas_Bool                 rt;

	unsigned long            channels;

	Evas_Bool                 aux_are_input;
	unsigned long            aux_channels;

	unsigned long            port_count;
	LADSPA_PortDescriptor *  port_descriptors;
	LADSPA_PortRangeHint *   port_range_hints;
	char **                  port_names;

	unsigned long *          audio_input_port_indicies;
	unsigned long *          audio_output_port_indicies;

	unsigned long *          audio_aux_port_indicies;

	unsigned long            control_port_count;
	unsigned long *          control_port_indicies;
};

typedef struct _patches patches_t;
struct _patches{
	Evas_List * patch_data;
	char * file_name;
};

typedef struct _live_plugin live_plugin_t;
struct _live_plugin{
	Evas_List * plugin_data;
	unsigned long plugin_count;
	unsigned long rack_base;
	unsigned long rack_top;
	char * patch_name;
	Evas_List * midi_data;
	int state_count;
	int id;
	Evas_List * Jack_Patch;
};

typedef struct _jack_patch jack_patch_t;
struct _jack_patch{
	char *port;
	char *connected_to;
};



typedef struct _midi midi_t;
struct _midi{
	int type;
	int channel;
	int program;
	Etk_Tree_Row *trow;
};

typedef struct _live_plugin_state live_plugin_state_t;
struct _live_plugin_state{
	float wet_dry_value;
	int bypass;
};

typedef struct _control_port_state control_port_state_t;
struct _control_port_state {
	LADSPA_Data current_real_value;
};

typedef struct _audio_port audio_port_t;
struct _audio_port {
	unsigned long index;
	char * name;
	jack_port_t * port;
	LADSPA_Data * data;
};

typedef struct _live_plugin_jack live_plugin_jack_t;
struct _live_plugin_jack{
	Evas_Bool ladspa_active;
	LADSPA_Descriptor_Function get_descriptor;
	plugin_desc_t *ladspa_plugin;
	LADSPA_Handle *ladspa_handle;	
	live_plugin_t *parent;
	Evas_List * Jack_input_ports;
	Evas_List * Jack_output_ports;
	char * plugin_name;
	unsigned long plugin_ref_ID; 
	Evas_Object *rack_back;
	Evas_Object *rack_top;
	Evas_Object *rack_bottom;
	Evas_Object *rack_title;
	Evas_Object *rack_data_text_1;
	Evas_Object *rack_data_text_2;
	Evas_Object *rack_data_text_3;
	Evas_Object *rack_data_clipper;
	Evas_Object *on_switch;
	Evas_Object *off_switch;
	Evas_Object *delete;
	Evas_Object *delete_over;
	Evas_Object *bypass_title;
	Evas_Object *wet_dry_text;
	Evas_Object *wet_dry_hotspot;
	Evas_Object *wet_dry_dial;
	Evas_Object *wet_dry_line;	
	Evas_List *control_ports;
	int control_port_count;
	int rack_data_text_1_animate_switch;
	float wet_dry_value;
	int bypass;
	Evas_List * state_data;
};

typedef struct _control_port control_port_t;
struct _control_port{
	unsigned long control_port_index;
	int control_port_type;
	Evas_Bool control_port_logarithmic;
	LADSPA_Data max_value;
	LADSPA_Data min_value;
	LADSPA_Data current_real_value;
	float current_ui_value;
	char *control_port_name;	
	Evas_Object *control_back;
	Evas_Object *control_back_midi;
	Evas_Object *control_title;
	Evas_Object *clipper;
	Evas_Object *control_obj_1;
	Evas_Object *control_obj_2;
	Evas_Object *control_obj_3;
	Evas_Object *control_obj_4;
	Evas_Object *control_obj_5;
	live_plugin_jack_t *parent;
	Evas_List * state_data;
	Evas_Object *expression_highlight;
	Evas_Bool midi_pedal_ui;
	int midi_pedal_link;
};

#define PI	(3.141592654)

plugin_mgr_t * mux_plugin_mgr;
jack_client_t *jack_client;
patches_t *patches;

/* is now the current live rack */
live_plugin_t *live_plugin;

/* gui globals*/
Etk_Widget *canvas;
Etk_Widget *rack_tree;
Etk_Tree_Row *row;
Etk_Tree_Col *col1, *col2;
int rack_id_count;

void rack_hide(void);
void rack_show(void);
void patch_bay_show(void);
void patch_bay_hide(void);
void create_new_effect_front(live_plugin_jack_t *live_plugin_jack);
void plugin_mgr_get_path_plugins (plugin_mgr_t * plugin_mgr);
void _create_select_rack_window(void * data);
void mux_main_show();
void apply_plugin(void * data, Evas_Bool * load_from_file);
void jack_up(const char *client_name);
void live_plugin_animate(void *data);
void add_state(void *data);
void create_new_effect_front(live_plugin_jack_t *live_plugin_jack);
void disconnect_jack_ports(live_plugin_jack_t *live_plugin_jack);
void Repatch_Jack(void);
void Unpatch_Jack(void);
void set_state(void *data);
void save_state(void *data);
void adjust_rack_ui_controls(void);
void new_plugin_state(live_plugin_jack_t * live_plugin_jack);
void store_jack_connections(void);
void show_midi_window(void *data);
void show_midi_window_control(void *data);
void midi_create(void);
void midi_delete(void *data);
void midi_start(void);
void activate_plugins(void);
void connect_plugins_port_buffers(void);
void filechooser_window_create(void *data);
void filesaver_window_create(void *data);
void file_save(char *filename);
void purge_current_session(void);

#define E_FREE(ptr) do { if(ptr) { free(ptr); ptr = NULL; } } while(0);





