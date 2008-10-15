#include "mux_gui.h"

static void _main_quit_cb(void *data);

static Etk_Widget *_etk_menu_item_new(const char *label, Etk_Menu_Shell *menu_shell);
static Etk_Widget *_etk_menu_stock_item_new(const char *label, Etk_Stock_Id stock_id, Etk_Menu_Shell *menu_shell);

void _ex_New(Etk_Object *obj, void *data);
void _ex_Open(Etk_Object *obj, void *data);
void _ex_Save(Etk_Object *obj, void *data);
void _ex_Save_As(Etk_Object *obj, void *data);
void _ex_Duplicate_Rack(Etk_Object *obj, void *data);
void _ex_Jack_Setup(Etk_Object *obj, void *data);
void _ex_Midi_Setup(Etk_Object *obj, void *data);
void _ex_Add_Effect(Etk_Object *obj, void *data);
void _ex_Add_State(Etk_Object *obj, void *data);
void _ex_Save_State(Etk_Object *obj, void *data);
void _ex_New_Rack(Etk_Object *obj, void *data);
void _ex_Rename(Etk_Object *obj, void *data);
void _ex_RenameName(Etk_Object *obj, void *data);
void _ex_Name(Etk_Object *obj, void *data);
void _ex_Delete(Etk_Object *obj, void *data);
void _ex_Select_Rack(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data);
void _ex_Save(Etk_Object *obj, void *data);
void _ex_Midi(Etk_Object *obj, void *data);
void _ex_Patch_Bay(Etk_Object *obj, void *data);
static int ui_animation_timer(void *data);
void process_state(void);


/* hold on to the tree data */



void mux_main_show()
{
	
	Etk_Widget *win;
	Etk_Widget *vbox,*vbox2, *hbox, *hbox2, *hbox3;
	Etk_Widget *button;
	Etk_Widget *menu_bar;
	Etk_Widget *menu;
	Etk_Widget *menu_item;
	Etk_Widget *menu_item1;

	Evas *evas;
	Evas_Object *object;
	live_plugin_t *live_plugin_1;
	etk_theme_widget_set_from_path("ui/detour-etk.edj");
	/* intialise the plugin manager patches and default rack*/
	rack_id_count = 1;
	live_plugin_1 = malloc(sizeof(live_plugin_t));
	live_plugin_1->plugin_data = NULL;
	live_plugin_1->plugin_count = 0;
	live_plugin_1->rack_base=0;
	live_plugin_1->state_count=0;
	live_plugin_1->Jack_Patch = NULL;
	live_plugin_1->id = rack_id_count;
	live_plugin = live_plugin_1;	
	
	patches = malloc(sizeof(patches_t));
	patches->patch_data = NULL;
	patches->file_name = NULL;
	patches->patch_data = evas_list_append(patches->patch_data, live_plugin);

	
	mux_plugin_mgr = malloc (sizeof(plugin_mgr_t));
	mux_plugin_mgr->all_plugins = NULL;
	mux_plugin_mgr->plugins = NULL;
	mux_plugin_mgr->plugin_count = 0;
	plugin_mgr_get_path_plugins (mux_plugin_mgr);

	/* main window */
	win = etk_window_new();
	etk_window_title_set(ETK_WINDOW(win), "AYE MUX");
	etk_window_resize(ETK_WINDOW(win), 700, 500);
	etk_signal_connect_swapped("destroyed", ETK_OBJECT(win), ETK_CALLBACK(_main_quit_cb), NULL);


	vbox = etk_vbox_new(ETK_FALSE, 0);
	etk_container_add(ETK_CONTAINER(win), vbox);

	

	/* Window body */
	hbox = etk_hbox_new(ETK_FALSE, 0);
	etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

	canvas = etk_canvas_new();
	etk_box_append(ETK_BOX(hbox), canvas, ETK_BOX_START, ETK_BOX_FILL, 0);
	etk_widget_size_request_set(canvas, 900, 400); 
	object = evas_object_image_add(etk_widget_toplevel_evas_get(canvas));
	evas_object_image_file_set(object, "ui/mux_ui.edj", "images/0");
	evas_object_resize(object, 900, 900);
	evas_object_image_fill_set(object, 0, 0, 900, 100);
	evas_object_image_smooth_scale_set(object, 0);
	evas_object_show(object);

	object = evas_object_image_add(etk_widget_toplevel_evas_get(canvas));
	evas_object_image_file_set(object, "ui/mux_ui.edj", "images/1");
	evas_object_resize(object, 900, 900);
	evas_object_image_fill_set(object, 0, 0, 900, 100);
	evas_object_image_smooth_scale_set(object, 0);

	/* side menu */
	vbox2 = etk_vbox_new(ETK_FALSE, 0);
	etk_box_append(ETK_BOX(hbox), vbox2, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	/* top menu */
	/* Menubar */
   	menu_bar = etk_menu_bar_new();
   	etk_box_append(ETK_BOX(vbox2), menu_bar, ETK_BOX_START, ETK_BOX_NONE, 0);
	/* menu items */
	menu_item = _etk_menu_item_new("File", ETK_MENU_SHELL(menu_bar));
	menu = etk_menu_new();
	etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
	menu_item1 = _etk_menu_stock_item_new("New", ETK_STOCK_DOCUMENT_OPEN, ETK_MENU_SHELL(menu));
	etk_signal_connect("activated", ETK_OBJECT(menu_item1), ETK_CALLBACK(_ex_New), NULL);
	menu_item1 = _etk_menu_stock_item_new("Open", ETK_STOCK_DOCUMENT_OPEN, ETK_MENU_SHELL(menu));
	etk_signal_connect("activated", ETK_OBJECT(menu_item1), ETK_CALLBACK(_ex_Open), NULL);
	menu_item1 = _etk_menu_stock_item_new("Save", ETK_STOCK_DOCUMENT_SAVE, ETK_MENU_SHELL(menu));
	etk_signal_connect("activated", ETK_OBJECT(menu_item1), ETK_CALLBACK(_ex_Save), NULL);	
	menu_item1 = _etk_menu_stock_item_new("Save As", ETK_STOCK_DOCUMENT_SAVE_AS, ETK_MENU_SHELL(menu));
	etk_signal_connect("activated", ETK_OBJECT(menu_item1), ETK_CALLBACK(_ex_Save_As), NULL);

	/*menu_item = _etk_menu_item_new("Prefs", ETK_MENU_SHELL(menu_bar));
	menu = etk_menu_new();
	etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
	_etk_menu_item_new("Jack Setup", ETK_MENU_SHELL(menu));
	_etk_menu_item_new("Midi Setup", ETK_MENU_SHELL(menu));
	*/
	menu_item = _etk_menu_item_new("Help", ETK_MENU_SHELL(menu_bar));
	menu = etk_menu_new();
	etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
	/*_etk_menu_item_new("Contents", ETK_MENU_SHELL(menu));*/
	_etk_menu_item_new("About", ETK_MENU_SHELL(menu));
	

	/* buttons */
	hbox2 = etk_hbox_new(ETK_FALSE, 0);
	button = etk_button_new_with_label("Create New Rack");
	etk_box_append(ETK_BOX(vbox2), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("clicked", ETK_OBJECT(button), _ex_New_Rack, NULL);	

	/*
	button = etk_button_new_with_label("Store as New Rack");
	etk_box_append(ETK_BOX(vbox2), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	*/
	etk_box_append(ETK_BOX(vbox2), hbox2, ETK_BOX_START, ETK_BOX_NONE, 0);	   
	/* some controls */
	
	button = etk_button_new_with_label("Add Effect");
	etk_box_append(ETK_BOX(vbox2), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("clicked", ETK_OBJECT(button), _ex_Add_Effect, (void *)mux_plugin_mgr);	

	
	/*button = etk_button_new_with_label("Patch Bay");
	etk_box_append(ETK_BOX(vbox2), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("clicked", ETK_OBJECT(button), _ex_Patch_Bay, object);
	*/
	button = etk_button_new_with_label("Set Midi");
	etk_box_append(ETK_BOX(vbox2), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	etk_signal_connect("clicked", ETK_OBJECT(button), _ex_Midi, NULL);
	
	button = etk_button_new_with_label("New Rack State");
	etk_box_append(ETK_BOX(hbox2), button, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	etk_signal_connect("clicked", ETK_OBJECT(button), _ex_Add_State, NULL);	

	button = etk_button_new_with_label("Save State");
	etk_box_append(ETK_BOX(hbox2), button, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	etk_signal_connect("clicked", ETK_OBJECT(button), _ex_Save_State, NULL);
	/* tree with racks */

	rack_tree = etk_tree_new();
	etk_tree_mode_set(ETK_TREE(rack_tree), ETK_TREE_MODE_TREE);
	etk_widget_padding_set(rack_tree, 5, 5, 5, 5);	
	etk_widget_size_request_set(rack_tree, 180, 400);
	col1 = etk_tree_col_new(ETK_TREE(rack_tree), "Racks", 130, 0.0);
	col2 = etk_tree_col_new(ETK_TREE(rack_tree), "Midi", 50, 0.0);
	etk_tree_col_model_add(col1, etk_tree_model_image_new());
	etk_tree_col_model_add(col1, etk_tree_model_text_new());
	etk_tree_col_model_add(col2, etk_tree_model_text_new());
	const char *stock_key;

	/* make untitled rack */
	etk_tree_build(ETK_TREE(rack_tree));
	etk_tree_freeze(ETK_TREE(rack_tree));
	stock_key = etk_stock_key_get(ETK_STOCK_PLACES_USER_HOME, ETK_STOCK_SMALL);
	
	row = etk_tree_row_append(ETK_TREE(rack_tree), NULL, col1, etk_theme_icon_path_get(), stock_key, "untitled rack", NULL);

	live_plugin->patch_name = strdup("Untitled Rack");
	etk_tree_row_data_set(row, live_plugin);
	etk_tree_thaw(ETK_TREE(rack_tree));
	etk_tree_row_select(row);
	etk_signal_connect("row-clicked", ETK_OBJECT(rack_tree),  ETK_CALLBACK(_ex_Select_Rack), NULL);
	process_state();
	etk_box_append(ETK_BOX(vbox2), rack_tree, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	
	button = etk_button_new_with_label("Delete Selected");
	hbox3 = etk_hbox_new(ETK_FALSE, 0);
	etk_box_append(ETK_BOX(vbox2), hbox3, ETK_BOX_START, ETK_BOX_NONE, 0);
	button = etk_button_new_with_label("Rename Rack");
	etk_box_append(ETK_BOX(hbox3), button, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	etk_signal_connect("clicked", ETK_OBJECT(button), _ex_Rename, NULL);	

	button = etk_button_new_with_label("Delete");
	etk_box_append(ETK_BOX(hbox3), button, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	etk_signal_connect("clicked", ETK_OBJECT(button), _ex_Delete, NULL);	


	etk_widget_show_all(win);
	evas_object_show(object);

	ecore_timer_add(0.07, ui_animation_timer, NULL);
}


static Etk_Widget *textwin = NULL;
Etk_Widget *rack_title;
void name_rename_window(void *data, int *type){
	Etk_Widget *vbox, *hbox;
	Etk_Widget *frame;
	Etk_Widget *button;	
	Etk_Tree_Row * row;
	live_plugin_t * live_plugin_1;
	if (textwin){
		etk_window_delete_request(ETK_WIDGET(textwin));
	}
	
	textwin = etk_window_new();
	etk_window_title_set(ETK_WINDOW(textwin), "Enter a New Name");
	etk_window_resize(ETK_WINDOW(textwin), 277, 0);
	etk_signal_connect("delete-event", ETK_OBJECT(textwin), ETK_CALLBACK(etk_window_hide_on_delete), NULL);

	vbox = etk_vbox_new(ETK_FALSE, 0);
	etk_container_add(ETK_CONTAINER(textwin), vbox);

	frame = etk_frame_new("Enter a new name for the rack");
	etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	
	hbox = etk_hbox_new(ETK_FALSE, 0);
	etk_container_add(ETK_CONTAINER(frame), hbox);
	rack_title = etk_entry_new();	
	
	etk_box_append(ETK_BOX(hbox), rack_title, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	button = etk_button_new_with_label("Ok");
	etk_box_append(ETK_BOX(hbox), button, ETK_BOX_START, ETK_BOX_NONE, 0);
	if(type == 1){
		row = (Etk_Tree_Row *)data;
		live_plugin_1 = (live_plugin_t *)etk_tree_row_data_get(row);
		etk_entry_text_set(ETK_ENTRY(rack_title), live_plugin_1->patch_name);
		etk_signal_connect("clicked", ETK_OBJECT(button), _ex_RenameName, data);
	}else{
		etk_signal_connect("clicked", ETK_OBJECT(button), _ex_Name, NULL);
	}
	etk_widget_show_all(textwin);
}

void _ex_Name(Etk_Object *obj, void *data){
	live_plugin_t * live_plugin_1;
	const char *stock_key;
	Etk_Tree_Row * row;
	
	rack_hide();
	Unpatch_Jack();

	rack_id_count++;
	live_plugin_1 = malloc(sizeof(live_plugin_t));
	live_plugin_1->plugin_data = NULL;
	live_plugin_1->plugin_count = 0;
	live_plugin_1->rack_base=0;
	live_plugin_1->state_count=0;
	live_plugin_1->Jack_Patch = NULL;
	live_plugin_1->midi_data = NULL;
	live_plugin = live_plugin_1;
	live_plugin_1->id = rack_id_count;
	patches->patch_data = evas_list_append(patches->patch_data, live_plugin);
	
	etk_tree_freeze(ETK_TREE(rack_tree));
	stock_key = etk_stock_key_get(ETK_STOCK_PLACES_USER_HOME, ETK_STOCK_SMALL);
	if(!obj){
		row = etk_tree_row_append(ETK_TREE(rack_tree), NULL, col1, etk_theme_icon_path_get(), stock_key, data, NULL);
		live_plugin->patch_name = strdup(data);
	}else{
		row = etk_tree_row_append(ETK_TREE(rack_tree), NULL, col1, etk_theme_icon_path_get(), stock_key, etk_entry_text_get(ETK_ENTRY(rack_title)), NULL);
		live_plugin->patch_name = strdup(etk_entry_text_get(ETK_ENTRY(rack_title)));
		etk_widget_hide(ETK_WIDGET(textwin));
		
	}
	
	etk_tree_row_select(row);
	etk_tree_row_data_set(row, live_plugin);
	etk_tree_thaw(ETK_TREE(rack_tree));
	process_state();
	
	
}

void _ex_Select_Rack(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data){
	live_plugin_t * live_plugin_1;
	rack_hide();
	if(etk_tree_row_parent_get(row)){
		live_plugin_1 = (live_plugin_t *)etk_tree_row_data_get(etk_tree_row_parent_get(row));
		if(live_plugin->id != live_plugin_1->id){
			Unpatch_Jack();		
			live_plugin = live_plugin_1;
			Repatch_Jack();
		}
		set_state(etk_tree_row_data_get(row));
	}else{
		Unpatch_Jack();
		live_plugin = (live_plugin_t *)etk_tree_row_data_get(row);
		Repatch_Jack();
		set_state(etk_tree_row_data_get(etk_tree_row_first_child_get(row)));
		etk_tree_row_select(etk_tree_row_first_child_get(row));
	}
	adjust_rack_ui_controls();
	rack_show();
}

void _ex_RenameName(Etk_Object *obj, void *data){
	live_plugin_t * live_plugin_1;
	const char *stock_key;
	Etk_Tree_Row * row;
	row = (Etk_Tree_Row *)data;
	live_plugin_1 = (live_plugin_t *)etk_tree_row_data_get(row);
	live_plugin_1->patch_name = strdup(etk_entry_text_get(ETK_ENTRY(rack_title)));
	etk_tree_freeze(ETK_TREE(rack_tree));
	etk_tree_row_fields_set(row, ETK_FALSE, col1, etk_theme_icon_path_get(), etk_stock_key_get(ETK_STOCK_PLACES_USER_HOME, ETK_STOCK_SMALL), live_plugin_1->patch_name, NULL);
	etk_tree_thaw(ETK_TREE(rack_tree));
	etk_widget_hide(ETK_WIDGET(textwin));
}
void _ex_Rename(Etk_Object *obj, void *data){
	Etk_Tree_Row * row;
	live_plugin_t * live_plugin_1;
	row =  etk_tree_row_parent_get(etk_tree_selected_row_get(rack_tree));
	live_plugin_1 = (live_plugin_t *)etk_tree_row_data_get(row);	
	if (textwin){
		etk_entry_text_set(ETK_ENTRY(rack_title), live_plugin_1->patch_name);
	}
	name_rename_window(row, 1);
}

void _ex_Add_Effect(Etk_Object *obj, void *data){
	_create_select_rack_window(data);
}

void _ex_New_Rack(Etk_Object *obj, void *data){
	if (textwin){
		etk_entry_text_set(ETK_ENTRY(rack_title), "");
	}
	name_rename_window(NULL, 0);	
}

void _ex_Midi(Etk_Object *obj, void *data){
	show_midi_window(rack_tree);
}

void _ex_Save_State(Etk_Object *obj, void *data){
	save_state(etk_tree_row_data_get(etk_tree_selected_row_get(rack_tree)));
}

void _ex_Delete(Etk_Object *obj, void *data){
	int a;
	Etk_Tree_Row *last_row;
	Etk_Tree_Row *c_row = NULL;
	char state_title[255];
	if(live_plugin->state_count > 1){
		/* if there is more than one state delete selected state */
		last_row = etk_tree_row_last_child_get(etk_tree_row_parent_get(etk_tree_selected_row_get(rack_tree)));
		for (c_row = last_row; c_row != etk_tree_selected_row_get(rack_tree); c_row = etk_tree_row_prev_get(c_row)){
			a = (int)etk_tree_row_data_get(c_row)-1;
			etk_tree_row_data_set(c_row, a);
			sprintf(state_title, "state %i", a);
			etk_tree_row_fields_set(c_row, ETK_FALSE, col1, etk_theme_icon_path_get(), etk_stock_key_get(ETK_STOCK_AUDIO_X_GENERIC, ETK_STOCK_SMALL), state_title, NULL);
			
		}
		if(etk_tree_row_parent_get(etk_tree_selected_row_get(rack_tree))){
			c_row = etk_tree_row_parent_get(etk_tree_selected_row_get(rack_tree));
		}else{
			c_row = etk_tree_row_prev_get(etk_tree_selected_row_get(rack_tree));		
		}
		a = (int)etk_tree_row_data_get(etk_tree_selected_row_get(rack_tree))-1;
		midi_delete(a);
		remove_state(a);
		etk_tree_row_delete(etk_tree_selected_row_get(rack_tree));
		etk_tree_row_select(c_row);
		_ex_Select_Rack(NULL, c_row, NULL, NULL);
	}else{
				
		live_plugin_jack_t * live_plugin_jack;
		Evas_List *list;
		for (list = live_plugin->plugin_data; list; list = evas_list_next(list)){
			live_plugin_jack = (live_plugin_jack_t *)list->data;
			rack_delete(live_plugin_jack);
		}
		if(etk_tree_row_prev_get(etk_tree_row_parent_get(etk_tree_selected_row_get(rack_tree)))){
			c_row = etk_tree_row_prev_get(etk_tree_row_parent_get(etk_tree_selected_row_get(rack_tree)));
		}else {
			c_row = etk_tree_row_next_get(etk_tree_row_parent_get(etk_tree_selected_row_get(rack_tree)));
		}
		patches->patch_data = evas_list_remove(patches->patch_data, live_plugin);
		etk_tree_row_delete(etk_tree_row_parent_get(etk_tree_selected_row_get(rack_tree)));
		etk_tree_row_delete(etk_tree_selected_row_get(rack_tree));
		Unpatch_Jack();	
		if(c_row){
			etk_tree_row_select(c_row);
			_ex_Select_Rack(NULL, c_row, NULL, NULL);
		}
	}
	if(evas_list_count(patches->patch_data) < 1){
		sprintf(state_title, "Untitled Rack");
		_ex_Name(NULL, (void *)state_title);
	}
}

void process_state(void){
	char state_title[255];
	Etk_Tree_Row * row, *row_parent;
	add_state(live_plugin);
	if(etk_tree_row_parent_get(etk_tree_selected_row_get(rack_tree))){
		row_parent = etk_tree_row_parent_get(etk_tree_selected_row_get(rack_tree));
	}else{
		row_parent = etk_tree_selected_row_get(rack_tree);
	}
	sprintf(state_title, "state %i", live_plugin->state_count);
	row = etk_tree_row_append(ETK_TREE(rack_tree), row_parent, col1, etk_theme_icon_path_get(), etk_stock_key_get(ETK_STOCK_AUDIO_X_GENERIC, ETK_STOCK_SMALL), state_title, NULL);
	etk_tree_row_data_set(row, live_plugin->state_count);
	etk_tree_row_select(row);
	etk_tree_row_unfold(row_parent);
}

void _ex_Add_State(Etk_Object *obj, void *data){
	process_state();	
}

void _ex_Save(Etk_Object *obj, void *data){
	store_jack_connections();
	if(patches->file_name){
		file_save(patches->file_name);
	}else{
		filesaver_window_create(NULL);
	}
}

/* menu builders and cb's */

static Etk_Widget *_etk_menu_stock_item_new(const char *label, Etk_Stock_Id stock_id, Etk_Menu_Shell *menu_shell)
{
	Etk_Widget *menu_item;
	Etk_Widget *image;

	if (!menu_shell)
	return NULL;

	menu_item = etk_menu_item_image_new_with_label(label);
	image = etk_image_new_from_stock(stock_id, ETK_STOCK_SMALL);
	etk_menu_item_image_set(ETK_MENU_ITEM_IMAGE(menu_item), ETK_IMAGE(image));
	etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));

	return menu_item;
}

static Etk_Widget *_etk_menu_item_new(const char *label, Etk_Menu_Shell *menu_shell)
{
   Etk_Widget *menu_item;
   
   if (!menu_shell)
      return NULL;
   
   menu_item = etk_menu_item_new_with_label(label);
   etk_menu_shell_append(menu_shell, ETK_MENU_ITEM(menu_item));
   
   return menu_item;
}

/* MENU CB's */

void _ex_New(Etk_Object *obj, void *data){
	live_plugin_t *live_plugin_1;	
	char rack_title[256];
			
	purge_current_session();
	/* intialise the plugin manager patches and default rack*/
	rack_id_count = 1;
	live_plugin_1 = malloc(sizeof(live_plugin_t));
	live_plugin_1->plugin_data = NULL;
	live_plugin_1->plugin_count = 0;
	live_plugin_1->rack_base=0;
	live_plugin_1->state_count=0;
	live_plugin_1->Jack_Patch = NULL;
	live_plugin_1->id = rack_id_count;
	live_plugin = live_plugin_1;	
	
	patches = malloc(sizeof(patches_t));
	patches->patch_data = NULL;
	patches->file_name = NULL;
	patches->patch_data = evas_list_append(patches->patch_data, live_plugin);
	sprintf(rack_title, "Untitled Rack");
	_ex_Name(NULL, (void *)rack_title);
	
}

void _ex_Open(Etk_Object *obj, void *data){
	filechooser_window_create(NULL);	
}


void _ex_Save_As(Etk_Object *obj, void *data){
	store_jack_connections();
	filesaver_window_create(NULL);
}

void _ex_Patch_Bay(Etk_Object *obj, void *data){
		
	Evas_Object *object;
	object = (Evas_Object *)data;	
	if(evas_object_visible_get(object) == 1){
		//hide
		evas_object_hide(object);
		rack_hide();
		patch_bay_show();
		etk_button_label_set (obj, "Effects Rack");
	}else{
		//show
		evas_object_show(object);
		rack_show();
		patch_bay_hide();
		etk_button_label_set (obj, "Patch Bay");
	}
}









/* animation timer */
static int ui_animation_timer(void *data) {
	live_plugin_animate(NULL);
	return 1;
}

/* called when the window is closed - gets window and user data pointer */
static void _main_quit_cb(void *data)
{
   etk_main_quit();
}



