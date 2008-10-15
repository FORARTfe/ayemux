#include "mux_gui.h"
/* globals */
int last_drag_x, last_drag_y;

int drag_last_x, drag_last_y;
/* callbacks */
void control_mouse_over(void *data, Evas *e, Evas_Object *obj, void *event_info);
void control_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info);
void control_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info);
void control_toggle(void *data, Evas *e, Evas_Object *obj, void *event_info);
void control_toggle_up_1(void *data, Evas *e, Evas_Object *obj, void *event_info);
void control_toggle_up_2(void *data, Evas *e, Evas_Object *obj, void *event_info);
void control_toggle_down_1(void *data, Evas *e, Evas_Object *obj, void *event_info);
void control_toggle_down_2(void *data, Evas *e, Evas_Object *obj, void *event_info);
void control_right_click(void *data, Evas *e, Evas_Object *obj, void *event_info);
void delete_over(void *data, Evas *e, Evas_Object *obj, void *event_info);
void delete_out(void *data, Evas *e, Evas_Object *obj, void *event_info);
void delete_up(void *data, Evas *e, Evas_Object *obj, void *event_info);
void bypass_switch(void *data, Evas *e, Evas_Object *obj, void *event_info);
void wetdry_mouse_over(void *data, Evas *e, Evas_Object *obj, void *event_info);
void wetdry_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info);
void wetdry_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info);
void delete_rack_confirm(Etk_Object *object, int response_id, void *data);
void rack_scroll(void *data, Evas *e, Evas_Object *obj, void *event_info); 
void rack_scroll_in(void *data, Evas *e, Evas_Object *obj, void *event_info); 
void rack_scroll_out(void *data, Evas *e, Evas_Object *obj, void *event_info); 

void rack_move(live_plugin_jack_t * live_plugin_jack, Evas_Coord x, Evas_Coord y);
void rack_delete(live_plugin_jack_t * live_plugin_jack);
void draw_dial_line(control_port_t *control_port);
void wet_dry_draw_dial_line(live_plugin_jack_t * live_plugin_jack);

void create_new_effect_front(live_plugin_jack_t *live_plugin_jack){
	Evas *evas;
	Evas_Coord ox, oy;
	Evas_Coord cox, coy;
	Evas_Coord cx, cy, cw, ch;
	live_plugin->rack_top = 0; // could move this to mux_expose_plugin.c

	evas = etk_widget_toplevel_evas_get(canvas);
	

	/*rack back*/
	live_plugin_jack->rack_back = evas_object_image_add(evas);
	////etk_canvas_object_add(canvas, live_plugin_jack->rack_back);
	evas_object_image_file_set(live_plugin_jack->rack_back, "ui/mux_ui.edj", "images/2");
	evas_object_resize(live_plugin_jack->rack_back, 900, 100);
	evas_object_image_fill_set(live_plugin_jack->rack_back, 0, 0, 900, 100);
	evas_object_image_smooth_scale_set(live_plugin_jack->rack_back, 0);
	evas_object_move(live_plugin_jack->rack_back, 0, live_plugin->rack_base);
	evas_object_show(live_plugin_jack->rack_back); 
	evas_object_event_callback_add(live_plugin_jack->rack_back, EVAS_CALLBACK_MOUSE_IN, rack_scroll_in, NULL);
	evas_object_event_callback_add(live_plugin_jack->rack_back, EVAS_CALLBACK_MOUSE_MOVE, rack_scroll, NULL);
	evas_object_event_callback_add(live_plugin_jack->rack_back, EVAS_CALLBACK_MOUSE_OUT, rack_scroll_out, NULL);

	/*rack top */
	live_plugin_jack->rack_top = evas_object_image_add(evas);
	//etk_canvas_object_add(canvas, live_plugin_jack->rack_top);
	evas_object_image_file_set(live_plugin_jack->rack_top, "ui/mux_ui.edj", "images/3");
	evas_object_resize(live_plugin_jack->rack_top, 900, 100);
	evas_object_image_fill_set(live_plugin_jack->rack_top, 0, 0, 900, 100);
	evas_object_image_smooth_scale_set(live_plugin_jack->rack_top, 0);
	evas_object_move(live_plugin_jack->rack_top, 0, live_plugin->rack_base);
	evas_object_show(live_plugin_jack->rack_top);
	evas_object_event_callback_add(live_plugin_jack->rack_top, EVAS_CALLBACK_MOUSE_IN, rack_scroll_in, NULL);
	evas_object_event_callback_add(live_plugin_jack->rack_top, EVAS_CALLBACK_MOUSE_MOVE, rack_scroll, NULL);
	evas_object_event_callback_add(live_plugin_jack->rack_top, EVAS_CALLBACK_MOUSE_OUT, rack_scroll_out, NULL);

	/*rack bottom */
	live_plugin_jack->rack_bottom = evas_object_image_add(evas);
	//etk_canvas_object_add(canvas, live_plugin_jack->rack_bottom);
	evas_object_image_file_set(live_plugin_jack->rack_bottom, "ui/mux_ui.edj", "images/4");
	evas_object_resize(live_plugin_jack->rack_bottom, 900, 100);
	evas_object_image_fill_set(live_plugin_jack->rack_bottom, 0, 0, 900, 100);
	evas_object_image_smooth_scale_set(live_plugin_jack->rack_bottom, 0);
	evas_object_show(live_plugin_jack->rack_bottom);
	evas_object_event_callback_add(live_plugin_jack->rack_bottom, EVAS_CALLBACK_MOUSE_IN, rack_scroll_in, NULL);
	evas_object_event_callback_add(live_plugin_jack->rack_bottom, EVAS_CALLBACK_MOUSE_MOVE, rack_scroll, NULL);
	evas_object_event_callback_add(live_plugin_jack->rack_bottom, EVAS_CALLBACK_MOUSE_OUT, rack_scroll_out, NULL);

	/*rack title*/
	live_plugin_jack->rack_title = evas_object_text_add(evas);
	//etk_canvas_object_add(canvas, live_plugin_jack->rack_title);
	evas_font_path_append(etk_widget_toplevel_evas_get(canvas), "./ui/font/");
	evas_object_text_font_set(live_plugin_jack->rack_title, "BitDust Two", 12);
	evas_object_text_text_set(live_plugin_jack->rack_title, live_plugin_jack->plugin_name);
	evas_object_color_set(live_plugin_jack->rack_title, 254, 196, 36, 255);
	evas_object_text_shadow_color_set(live_plugin_jack->rack_title, 0, 0, 0, 255);
	evas_object_move( live_plugin_jack->rack_title, 84, live_plugin->rack_base+9);
	evas_object_show(live_plugin_jack->rack_title); 
	evas_object_event_callback_add(live_plugin_jack->rack_title, EVAS_CALLBACK_MOUSE_IN, rack_scroll_in, NULL);
	evas_object_event_callback_add(live_plugin_jack->rack_title, EVAS_CALLBACK_MOUSE_MOVE, rack_scroll, NULL);
	evas_object_event_callback_add(live_plugin_jack->rack_title, EVAS_CALLBACK_MOUSE_OUT, rack_scroll_out, NULL);

	/*rack data_text_1*/
	live_plugin_jack->rack_data_text_1 = evas_object_text_add(evas);
	//etk_canvas_object_add(canvas, live_plugin_jack->rack_data_text_1);
	evas_font_path_append(etk_widget_toplevel_evas_get(canvas), "./ui/font/");
	evas_object_text_font_set(live_plugin_jack->rack_data_text_1, "ROTORcap", 10);
	evas_object_text_text_set(live_plugin_jack->rack_data_text_1, live_plugin_jack->plugin_name);
	evas_object_color_set(live_plugin_jack->rack_data_text_1, 0, 0, 0, 255);
	evas_object_move( live_plugin_jack->rack_data_text_1, 673, live_plugin->rack_base+29);
	evas_object_show(live_plugin_jack->rack_data_text_1); 
	live_plugin_jack->rack_data_text_1_animate_switch = 0;

	/*rack data_text_2*/
	live_plugin_jack->rack_data_text_2 = evas_object_text_add(evas);
	//etk_canvas_object_add(canvas, live_plugin_jack->rack_data_text_2);
	evas_font_path_append(etk_widget_toplevel_evas_get(canvas), "./ui/font/");
	evas_object_text_font_set(live_plugin_jack->rack_data_text_2, "ROTORcap", 10);
	evas_object_text_text_set(live_plugin_jack->rack_data_text_2, "data 1");
	evas_object_color_set(live_plugin_jack->rack_data_text_2, 0, 0, 0, 255);
	evas_object_move( live_plugin_jack->rack_data_text_2, 673, live_plugin->rack_base+47);
	evas_object_show(live_plugin_jack->rack_data_text_2); 

	/*rack data_text_3*/
	live_plugin_jack->rack_data_text_3 = evas_object_text_add(evas);
	//etk_canvas_object_add(canvas, live_plugin_jack->rack_data_text_3);
	evas_font_path_append(etk_widget_toplevel_evas_get(canvas), "./ui/font/");
	evas_object_text_font_set(live_plugin_jack->rack_data_text_3, "ROTORcap", 10);
	evas_object_text_text_set(live_plugin_jack->rack_data_text_3, "");
	evas_object_color_set(live_plugin_jack->rack_data_text_3, 0, 0, 0, 255);
	evas_object_move( live_plugin_jack->rack_data_text_3, 673, live_plugin->rack_base+64);
	evas_object_show(live_plugin_jack->rack_data_text_3); 

	/*data text clipper*/
	live_plugin_jack->rack_data_clipper = evas_object_rectangle_add(evas);
	//etk_canvas_object_add(canvas, live_plugin_jack->rack_data_clipper);
	evas_object_color_set(live_plugin_jack->rack_data_clipper, 255, 255, 255, 255);
	evas_object_move( live_plugin_jack->rack_data_clipper, 673, live_plugin->rack_base+27);
	evas_object_resize(live_plugin_jack->rack_data_clipper, 128, 47);
	evas_object_clip_set(live_plugin_jack->rack_data_text_1, live_plugin_jack->rack_data_clipper);
	evas_object_clip_set(live_plugin_jack->rack_data_text_2, live_plugin_jack->rack_data_clipper);
	evas_object_clip_set(live_plugin_jack->rack_data_text_3, live_plugin_jack->rack_data_clipper);
	evas_object_show(live_plugin_jack->rack_data_clipper);	
	
	/* on off switch */
	live_plugin_jack->off_switch = evas_object_image_add(evas);
	//etk_canvas_object_add(canvas, live_plugin_jack->off_switch);
	evas_object_image_file_set(live_plugin_jack->off_switch, "ui/mux_ui.edj", "images/5");
	evas_object_image_size_get(live_plugin_jack->off_switch, &ox, &oy);
	evas_object_resize(live_plugin_jack->off_switch, ox, oy);
	evas_object_image_fill_set(live_plugin_jack->off_switch, 0, 0, ox, oy);
	evas_object_move( live_plugin_jack->off_switch, 32, live_plugin->rack_base + 27);
	evas_object_show(live_plugin_jack->off_switch);
	evas_object_geometry_get(live_plugin_jack->off_switch, &cx, &cy, &cw, &ch);	
	evas_object_event_callback_add(live_plugin_jack->off_switch, EVAS_CALLBACK_MOUSE_DOWN, bypass_switch, live_plugin_jack);

	live_plugin_jack->on_switch = evas_object_image_add(evas);
	//etk_canvas_object_add(canvas, live_plugin_jack->on_switch);
	evas_object_image_file_set(live_plugin_jack->on_switch, "ui/mux_ui.edj", "images/6");
	evas_object_image_size_get(live_plugin_jack->on_switch, &ox, &oy);
	evas_object_resize(live_plugin_jack->on_switch, ox, oy);
	evas_object_image_fill_set(live_plugin_jack->on_switch, 0, 0, ox, oy);
	evas_object_move( live_plugin_jack->on_switch, 32, live_plugin->rack_base + 27);
	evas_object_event_callback_add(live_plugin_jack->on_switch, EVAS_CALLBACK_MOUSE_DOWN, bypass_switch, live_plugin_jack);
	if(live_plugin_jack->bypass == 1){
		evas_object_hide(live_plugin_jack->on_switch);
	}

	live_plugin_jack->bypass_title = evas_object_text_add(evas);
	evas_object_text_font_set(live_plugin_jack->bypass_title, "ROTORcap", 8);
	evas_object_text_text_set(live_plugin_jack->bypass_title, "ON");
	evas_object_color_set(live_plugin_jack->bypass_title, 255, 255, 255, 255);
	evas_object_text_shadow_color_set(live_plugin_jack->bypass_title, 0, 0, 0, 255);
	evas_object_move(live_plugin_jack->bypass_title, cx+17, cy+3);
	evas_object_show(live_plugin_jack->bypass_title);	

	/* wet/dry range */
	
	
	live_plugin_jack->wet_dry_dial = evas_object_image_add(evas);
	//etk_canvas_object_add(canvas, live_plugin_jack->wet_dry_dial);
	evas_object_image_file_set(live_plugin_jack->wet_dry_dial, "ui/mux_ui.edj", "images/7");
	evas_object_image_size_get(live_plugin_jack->wet_dry_dial, &ox, &oy);
	evas_object_resize(live_plugin_jack->wet_dry_dial, ox, oy);
	evas_object_image_fill_set(live_plugin_jack->wet_dry_dial, 0, 0, ox, oy);
	evas_object_move( live_plugin_jack->wet_dry_dial, 818, live_plugin->rack_base + 27);
	evas_object_show(live_plugin_jack->wet_dry_dial);
	evas_object_geometry_get(live_plugin_jack->wet_dry_dial, &cx, &cy, &cw, &ch);
	evas_object_event_callback_add(live_plugin_jack->wet_dry_dial, EVAS_CALLBACK_MOUSE_IN, wetdry_mouse_over, live_plugin_jack);	
	evas_object_event_callback_add(live_plugin_jack->wet_dry_dial, EVAS_CALLBACK_MOUSE_OUT, wetdry_mouse_out, live_plugin_jack);	
	

	live_plugin_jack->wet_dry_line = evas_object_line_add(evas);
	wet_dry_draw_dial_line(live_plugin_jack);

	live_plugin_jack->wet_dry_text = evas_object_text_add(evas);
	evas_object_text_font_set(live_plugin_jack->wet_dry_text, "ROTORcap", 8);
	evas_object_text_text_set(live_plugin_jack->wet_dry_text, "WET:DRY");
	evas_object_color_set(live_plugin_jack->wet_dry_text, 255, 255, 255, 255);
	evas_object_text_shadow_color_set(live_plugin_jack->wet_dry_text, 0, 0, 0, 255);
	evas_object_move(live_plugin_jack->wet_dry_text, cx+3, cy+3);
	evas_object_show(live_plugin_jack->wet_dry_text);	
	evas_object_event_callback_add(live_plugin_jack->wet_dry_text, EVAS_CALLBACK_MOUSE_IN, wetdry_mouse_over, live_plugin_jack);	
	evas_object_event_callback_add(live_plugin_jack->wet_dry_text, EVAS_CALLBACK_MOUSE_OUT, wetdry_mouse_out, live_plugin_jack);	
	

	live_plugin_jack->wet_dry_hotspot = evas_object_image_add(evas);
	evas_object_image_file_set(live_plugin_jack->wet_dry_hotspot, "ui/mux_ui.edj", "images/8");
	evas_object_image_size_get(live_plugin_jack->wet_dry_hotspot, &cox, &coy);
	evas_object_resize(live_plugin_jack->wet_dry_hotspot, cox, coy);
	evas_object_image_fill_set(live_plugin_jack->wet_dry_hotspot, 0, 0, cox, coy); 
	evas_object_move(live_plugin_jack->wet_dry_hotspot, cx+10, cy+25);
	evas_object_show(live_plugin_jack->wet_dry_hotspot);
	evas_object_event_callback_add(live_plugin_jack->wet_dry_hotspot, EVAS_CALLBACK_MOUSE_IN, wetdry_mouse_over, live_plugin_jack);	
	evas_object_event_callback_add(live_plugin_jack->wet_dry_hotspot, EVAS_CALLBACK_MOUSE_OUT, wetdry_mouse_out, live_plugin_jack);	
	evas_object_event_callback_add(live_plugin_jack->wet_dry_hotspot, EVAS_CALLBACK_MOUSE_MOVE, wetdry_mouse_move, live_plugin_jack);
	


	/* Delete Rack Button */
	live_plugin_jack->delete = evas_object_image_add(evas);
	//etk_canvas_object_add(canvas, live_plugin_jack->delete);
	evas_object_image_file_set(live_plugin_jack->delete, "ui/mux_ui.edj", "images/9");
	evas_object_image_size_get(live_plugin_jack->delete, &ox, &oy);
	evas_object_resize(live_plugin_jack->delete, ox, oy);
	evas_object_image_fill_set(live_plugin_jack->delete, 0, 0, ox, oy);
	evas_object_move( live_plugin_jack->delete, 47, live_plugin->rack_base + 8);
	evas_object_show(live_plugin_jack->delete);
	evas_object_event_callback_add(live_plugin_jack->delete, EVAS_CALLBACK_MOUSE_IN, delete_over, live_plugin_jack);
	

	live_plugin_jack->delete_over = evas_object_image_add(evas);
	//etk_canvas_object_add(canvas, live_plugin_jack->delete_over);
	evas_object_image_file_set(live_plugin_jack->delete_over, "ui/mux_ui.edj", "images/10");
	evas_object_image_size_get(live_plugin_jack->delete_over, &ox, &oy);
	evas_object_resize(live_plugin_jack->delete_over, ox, oy);
	evas_object_image_fill_set(live_plugin_jack->delete_over, 0, 0, ox, oy);
	evas_object_move( live_plugin_jack->delete_over, 47, live_plugin->rack_base + 8);
	evas_object_event_callback_add(live_plugin_jack->delete_over, EVAS_CALLBACK_MOUSE_OUT, delete_out, live_plugin_jack);
	evas_object_event_callback_add(live_plugin_jack->delete_over, EVAS_CALLBACK_MOUSE_UP, delete_up, live_plugin_jack);
	
	
	/*create the controls */
	create_controls(live_plugin_jack);
	//etk_widget_show_all(canvas);
        printf("controls built \n");
}



void create_evas_controls(unsigned long *lastx, unsigned long *lasty, control_port_t * control_port, live_plugin_jack_t * live_plugin_jack, char * file_to_load){
	Evas *evas;
	evas = etk_widget_toplevel_evas_get(canvas);
	Evas_Coord ox, oy;
	Evas_Coord cox, coy;
	Evas_Coord cx, cy, cw, ch;
	Evas_Coord clipper_height;
 	char control_value[255];
	control_port->midi_pedal_ui = FALSE;
	control_port->control_back_midi = evas_object_image_add(evas);
	//etk_canvas_object_add(canvas, control_port->control_back_midi);
	if(control_port->control_port_type == 3){
		evas_object_image_file_set(control_port->control_back_midi, "ui/mux_ui.edj", "images/11");
	}else{
		evas_object_image_file_set(control_port->control_back, file_to_load, NULL);
	}
	evas_object_image_size_get(control_port->control_back_midi, &ox, &oy);
	evas_object_resize(control_port->control_back_midi, ox, oy);
	evas_object_image_fill_set(control_port->control_back_midi, 0, 0, ox, oy);
	evas_object_move( control_port->control_back_midi, *lastx, live_plugin->rack_base + (*lasty));
	evas_object_show(control_port->control_back_midi);
	evas_object_event_callback_add(control_port->control_back_midi, EVAS_CALLBACK_MOUSE_IN, control_mouse_over, control_port);	
	evas_object_event_callback_add(control_port->control_back_midi, EVAS_CALLBACK_MOUSE_OUT, control_mouse_out, control_port);	



	control_port->control_back = evas_object_image_add(evas);
	//etk_canvas_object_add(canvas, control_port->control_back);
	evas_object_image_file_set(control_port->control_back, file_to_load, NULL);
	evas_object_image_size_get(control_port->control_back, &ox, &oy);
	evas_object_resize(control_port->control_back, ox, oy);
	evas_object_image_fill_set(control_port->control_back, 0, 0, ox, oy);
	evas_object_move( control_port->control_back, *lastx, live_plugin->rack_base + (*lasty));
	evas_object_show(control_port->control_back);
	evas_object_geometry_get(control_port->control_back, &cx, &cy, &cw, &ch);
	printf("%i %i %i %i \n", cx, cy, cw, ch);
	/* temp move to object controls */	
	evas_object_event_callback_add(control_port->control_back, EVAS_CALLBACK_MOUSE_IN, control_mouse_over, control_port);	
	evas_object_event_callback_add(control_port->control_back, EVAS_CALLBACK_MOUSE_OUT, control_mouse_out, control_port);	
	/*evas_object_event_callback_add(control_port->control_back, EVAS_CALLBACK_MOUSE_MOVE, control_mouse_move, control_port);*/
	control_port->control_title = evas_object_text_add(evas);
	evas_object_text_font_set(control_port->control_title, "ROTORcap", 8);
	evas_object_text_text_set(control_port->control_title, control_port->control_port_name);
	evas_object_color_set(control_port->control_title, 255, 255, 255, 255);
	evas_object_text_shadow_color_set(control_port->control_title, 255, 255, 255, 255);
	evas_object_move(control_port->control_title, cx+3, cy+3);
	evas_object_show(control_port->control_title);
	
	control_port->clipper = evas_object_rectangle_add(evas);
	evas_object_color_set(control_port->clipper, 255, 255, 255, 255);
	evas_object_move(control_port->clipper, cx, cy);
	evas_object_resize(control_port->clipper, cw, 12);
	evas_object_clip_set(control_port->control_title, control_port->clipper);
	evas_object_show(control_port->clipper);
	

	if(control_port->control_port_type == 3){
		/*load image*/
		control_port->control_obj_1 = NULL;
		draw_dial_line(control_port);
		
		control_port->control_obj_2 = evas_object_image_add(evas);
		evas_object_image_file_set(control_port->control_obj_2, "ui/mux_ui.edj", "images/8");
		evas_object_image_size_get(control_port->control_obj_2, &cox, &coy);
		evas_object_resize(control_port->control_obj_2, cox, coy);
		evas_object_image_fill_set(control_port->control_obj_2, 0, 0, cox, coy); 
		evas_object_move(control_port->control_obj_2, cx+10, cy+25);
		evas_object_show(control_port->control_obj_2);
		
		evas_object_event_callback_add(control_port->control_obj_2, EVAS_CALLBACK_MOUSE_IN, control_mouse_over, control_port);	
		evas_object_event_callback_add(control_port->control_obj_2, EVAS_CALLBACK_MOUSE_OUT, control_mouse_out, control_port);	
		evas_object_event_callback_add(control_port->control_obj_2, EVAS_CALLBACK_MOUSE_MOVE, control_mouse_move, control_port);
		evas_object_event_callback_add(control_port->control_obj_2, EVAS_CALLBACK_MOUSE_UP, control_right_click, control_port);
		
	}
	if(control_port->control_port_type == 2){
		
		control_port->control_obj_1 = evas_object_image_add(evas);
		evas_object_image_file_set(control_port->control_obj_1, "ui/mux_ui.edj", "images/12");
		evas_object_image_size_get(control_port->control_obj_1, &cox, &coy);
		evas_object_resize(control_port->control_obj_1, cox, coy);
		evas_object_image_fill_set(control_port->control_obj_1, 0, 0, cox, coy); 
		evas_object_move(control_port->control_obj_1, cx+56, cy+21);
		evas_object_show(control_port->control_obj_1);
		evas_object_event_callback_add(control_port->control_obj_1, EVAS_CALLBACK_MOUSE_DOWN, control_toggle_down_1, control_port);
		evas_object_event_callback_add(control_port->control_obj_1, EVAS_CALLBACK_MOUSE_UP, control_toggle_up_1, control_port);
		evas_object_event_callback_add(control_port->control_obj_1, EVAS_CALLBACK_MOUSE_IN, control_mouse_over, control_port);	
		evas_object_event_callback_add(control_port->control_obj_1, EVAS_CALLBACK_MOUSE_OUT, control_mouse_out, control_port);	
		
		control_port->control_obj_2 = evas_object_image_add(evas);
		evas_object_image_file_set(control_port->control_obj_2, "ui/mux_ui.edj", "images/13");
		evas_object_image_size_get(control_port->control_obj_2, &cox, &coy);
		evas_object_resize(control_port->control_obj_2, cox, coy);
		evas_object_image_fill_set(control_port->control_obj_2, 0, 0, cox, coy); 
		evas_object_move(control_port->control_obj_2, cx+56, cy+21);
		evas_object_event_callback_add(control_port->control_obj_2, EVAS_CALLBACK_MOUSE_IN, control_mouse_over, control_port);	
		evas_object_event_callback_add(control_port->control_obj_2, EVAS_CALLBACK_MOUSE_OUT, control_mouse_out, control_port);	
		
		control_port->control_obj_3 = evas_object_image_add(evas);
		evas_object_image_file_set(control_port->control_obj_3, "ui/mux_ui.edj", "images/12");
		evas_object_image_size_get(control_port->control_obj_3, &cox, &coy);
		evas_object_resize(control_port->control_obj_3, cox, coy);
		evas_object_image_fill_set(control_port->control_obj_3, 0, 0, cox, coy); 
		evas_object_move(control_port->control_obj_3, cx+56, cy+40);
		evas_object_show(control_port->control_obj_3);
		evas_object_event_callback_add(control_port->control_obj_3, EVAS_CALLBACK_MOUSE_DOWN, control_toggle_down_2, control_port);
		evas_object_event_callback_add(control_port->control_obj_3, EVAS_CALLBACK_MOUSE_UP, control_toggle_up_2, control_port);
		evas_object_event_callback_add(control_port->control_obj_3, EVAS_CALLBACK_MOUSE_IN, control_mouse_over, control_port);	
		evas_object_event_callback_add(control_port->control_obj_3, EVAS_CALLBACK_MOUSE_OUT, control_mouse_out, control_port);	

		control_port->control_obj_4 = evas_object_image_add(evas);
		evas_object_image_file_set(control_port->control_obj_4, "ui/mux_ui.edj", "images/13");
		evas_object_image_size_get(control_port->control_obj_4, &cox, &coy);
		evas_object_resize(control_port->control_obj_4, cox, coy);
		evas_object_image_fill_set(control_port->control_obj_4, 0, 0, cox, coy); 
		evas_object_move(control_port->control_obj_4, cx+56, cy+40);
		evas_object_event_callback_add(control_port->control_obj_4, EVAS_CALLBACK_MOUSE_IN, control_mouse_over, control_port);	
		evas_object_event_callback_add(control_port->control_obj_4, EVAS_CALLBACK_MOUSE_OUT, control_mouse_out, control_port);	

		control_port->control_obj_5 = evas_object_text_add(evas);
		evas_object_text_font_set(control_port->control_obj_5, "ROTORcap", 15);
		int crvi = control_port->current_real_value;
		sprintf(control_value, "%i", crvi);
		evas_object_text_text_set(control_port->control_obj_5, control_value); /* to do */
		evas_object_color_set(control_port->control_obj_5, 0, 0, 0, 255);
		evas_object_move(control_port->control_obj_5, cx+12, cy+34);
		evas_object_show(control_port->control_obj_5);
		evas_object_event_callback_add(control_port->control_obj_5, EVAS_CALLBACK_MOUSE_IN, control_mouse_over, control_port);	
		evas_object_event_callback_add(control_port->control_obj_5, EVAS_CALLBACK_MOUSE_OUT, control_mouse_out, control_port);	
		
	}
	if(control_port->control_port_type == 1){
		control_port->control_obj_2 = evas_object_image_add(evas);
		//etk_canvas_object_add(canvas, control_port->control_obj_2);
		evas_object_image_file_set(control_port->control_obj_2, "ui/mux_ui.edj", "images/6");
		evas_object_image_size_get(control_port->control_obj_2, &ox, &oy);
		evas_object_resize(control_port->control_obj_2, ox, oy);
		evas_object_image_fill_set(control_port->control_obj_2, 0, 0, ox, oy);
		evas_object_move(control_port->control_obj_2, cx, cy);

		evas_object_event_callback_add(control_port->control_back, EVAS_CALLBACK_MOUSE_DOWN, control_toggle, control_port);
		evas_object_event_callback_add(control_port->control_obj_2, EVAS_CALLBACK_MOUSE_DOWN, control_toggle, control_port);
		evas_object_event_callback_add(control_port->control_obj_2, EVAS_CALLBACK_MOUSE_IN, control_mouse_over, control_port);	
		evas_object_event_callback_add(control_port->control_obj_2, EVAS_CALLBACK_MOUSE_OUT, control_mouse_out, control_port);	
		
		if(control_port->current_real_value == 1.0f){
			evas_object_show(control_port->control_obj_2);
		}
	}
	
	*lastx = *lastx + 10 + ox; /*8 problem*/
	if(((*lasty)==27) && ((*lastx + ox) > 655)){
		*lastx=43;
		*lasty= *lasty + 100;
	}else if((*lastx + ox) > 850){
		*lastx=43;
		*lasty= *lasty + 100;
	}
	 

	
}

void create_controls(live_plugin_jack_t * live_plugin_jack){
	control_port_t * control_port;
	Evas_List *list;
	int last_control_type;
	int control_count;
	unsigned long lastx, lasty;
	lastx = 84;
	lasty = 27;
	
	for (list = live_plugin_jack->control_ports; list; list = evas_list_next(list)){
		control_port = (control_port_t *)list->data;
		control_count++;
		control_port->control_obj_1 = NULL;
		control_port->control_obj_2 = NULL;
		control_port->control_obj_3 = NULL;
		control_port->control_obj_4 = NULL;
		control_port->control_obj_5 = NULL;		
	
		if(control_port->min_value != control_port->max_value){
			if(control_port->control_port_type == 3){
				create_evas_controls(&lastx, &lasty, control_port, live_plugin_jack, "ui/rack_range_control.png");
			}
			if(control_port->control_port_type == 2){
				create_evas_controls(&lastx, &lasty, control_port, live_plugin_jack, "ui/rack_integer_control.png");
			}
			if(control_port->control_port_type == 1){
				create_evas_controls(&lastx, &lasty, control_port, live_plugin_jack, "ui/rack_toggle_control.png");
			}
		}
	}
	evas_object_resize(live_plugin_jack->rack_back, 900, (lasty - 27) + 100);
	if(lastx == 43){
		live_plugin->rack_base = live_plugin->rack_base + (lasty - 27);
	}else{
		live_plugin->rack_base = live_plugin->rack_base + (lasty - 27) + 100;
	}
	evas_object_move( live_plugin_jack->rack_bottom, 0, live_plugin->rack_base-100);
}

void live_plugin_animate(void *data){
	Evas_List *list;
	live_plugin_jack_t * live_plugin_jack;
	Evas_Coord ax, ay, aw, ah;
	Evas_Coord bx, by, bw, bh;
	for (list = live_plugin->plugin_data; list; list = evas_list_next(list)){
		live_plugin_jack = (live_plugin_jack_t *)list->data;
		evas_object_geometry_get(live_plugin_jack->rack_data_text_1, &ax, &ay, &aw, &ah);
		evas_object_geometry_get(live_plugin_jack->rack_data_clipper, &bx, &by, &bw, &bh);
		if(aw > bw){
			if(ax+aw == bx+bw){
				live_plugin_jack->rack_data_text_1_animate_switch=1;
			}
			if(ax == bx){
				live_plugin_jack->rack_data_text_1_animate_switch=0;
			}
			if(live_plugin_jack->rack_data_text_1_animate_switch == 0){
				evas_object_move(live_plugin_jack->rack_data_text_1, ax-1,ay);
			}else{
				evas_object_move(live_plugin_jack->rack_data_text_1, ax+1,ay);
			}
		}
	}
}


void scroll_rack(int scroll_by){
	live_plugin_jack_t * live_plugin_jack;
	Evas_Coord ax, ay, aw, ah;
	Evas_List *list;

	live_plugin->rack_top = live_plugin->rack_top + scroll_by;
	live_plugin->rack_base = live_plugin->rack_base + scroll_by;
	printf("live_plugin->rack_base : %i : live_plugin->rack_top : %i\n", live_plugin->rack_base, live_plugin->rack_top);
	for (list = live_plugin->plugin_data; list; list = evas_list_next(list)){
		live_plugin_jack = (live_plugin_jack_t *)list->data;
		evas_object_geometry_get(live_plugin_jack->rack_back, &ax, &ay, &aw, &ah);
		rack_move(live_plugin_jack, ax, (ay + scroll_by));
	}	
}

void rack_scroll(void *data, Evas *e, Evas_Object *obj, void *event_info){
	
	Evas_Event_Mouse_Down *ev;
	Evas_Coord drag_x, drag_y;
	
	//int scroll_by = 0;
	int ex, ey, ew, eh;
	
	evas_output_viewport_get (e, &ex, &ey, &ew, &eh);
	ev = event_info;	
	evas_pointer_output_xy_get(e, &drag_x, &drag_y);
	if(ev->button == 1){
		if(last_drag_y > drag_y){
			if(live_plugin->rack_base >= eh){
				scroll_rack(-1);
			}
		}else if(last_drag_y < drag_y){
			if((live_plugin->rack_top < -1) & (live_plugin->rack_top != 0)){
				scroll_rack(1);
			}
		}
		
		last_drag_y = drag_y;
		//}
	}
}

void rack_scroll_in(void *data, Evas *e, Evas_Object *obj, void *event_info){
	int ex, ey, ew, eh;
	evas_output_viewport_get (e, &ex, &ey, &ew, &eh);
	if((live_plugin->rack_base > eh) || (live_plugin->rack_top < 0)){
		//etk_toplevel_pointer_push(etk_toplevel_type_get(), ETK_POINTER_MOVE);
	}
}

void rack_scroll_out(void *data, Evas *e, Evas_Object *obj, void *event_info){

}

/* mouse control back */
 
void control_mouse_over(void *data, Evas *e, Evas_Object *obj, void *event_info){
	drag_last_x = 0;
	drag_last_y = 0;
	control_port_t * control_port;
	Evas_Coord ax, ay, aw, ah;
	char control_value[255];
	control_port = (control_port_t *)data;
	evas_object_text_text_set(control_port->parent->rack_data_text_1, control_port->control_port_name);
	evas_object_geometry_get(control_port->parent->rack_data_text_1, &ax, &ay, &aw, &ah);
	evas_object_text_text_set(control_port->parent->rack_data_text_1, control_port->control_port_name);
	evas_object_move(control_port->parent->rack_data_text_1, 673, ay);
	if(control_port->control_port_type == 1){
		if(control_port->current_real_value == 1.0f){
			evas_object_text_text_set(control_port->parent->rack_data_text_2, "on");
		}else{
			evas_object_text_text_set(control_port->parent->rack_data_text_2, "off");
		}
	}
	if(control_port->control_port_type == 2){
		int crvi = control_port->current_real_value;
		sprintf(control_value, "%i", crvi);
		evas_object_text_text_set(control_port->parent->rack_data_text_2, control_value);
	}
	if(control_port->control_port_type == 3){
		sprintf(control_value, "%f", control_port->current_real_value);
		evas_object_text_text_set(control_port->parent->rack_data_text_2, control_value);
	}
	
	
}	

void control_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info){
	control_port_t * control_port;
	Evas_Coord ax, ay, aw, ah;
	control_port = (control_port_t *)data;
	evas_object_geometry_get(control_port->parent->rack_data_text_1, &ax, &ay, &aw, &ah);
	evas_object_text_text_set(control_port->parent->rack_data_text_1, control_port->parent->plugin_name);
	evas_object_move(control_port->parent->rack_data_text_1, 673, ay);
	evas_object_text_text_set(control_port->parent->rack_data_text_2, "");
}


	
void control_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info){
	control_port_t * control_port;
	Evas_Event_Mouse_Down *ev;
	int drag_x, drag_y;
	char control_value[255];
	control_port = (control_port_t *)data;
	ev = event_info;
	
	evas_pointer_output_xy_get(e, &drag_x, &drag_y);
	if(ev->button == 1){
		if(last_drag_y > drag_y){
			if(control_port->current_ui_value < 270){
				control_port->current_ui_value++;
			}
		}else if(last_drag_y < drag_y){
			if(control_port->current_ui_value > 0){
				control_port->current_ui_value--;
			}
		}
		draw_dial_line(control_port);
		control_port->current_real_value = (((control_port->max_value - control_port->min_value)/100) * (control_port->current_ui_value/(2.7))+control_port->min_value);
	}
	
	sprintf(control_value, "%f", control_port->current_real_value);
	evas_object_text_text_set(control_port->parent->rack_data_text_2, control_value);
	last_drag_x = drag_x; 
	last_drag_y = drag_y;
}


void control_right_click(void *data, Evas *e, Evas_Object *obj, void *event_info){
	control_port_t * control_port;
	Evas_Event_Mouse_Down *ev;
	control_port = (control_port_t *)data;
	ev = event_info;
	if(ev->button == 3){
		show_midi_window_control(data);
	}
}


void control_toggle(void *data, Evas *e, Evas_Object *obj, void *event_info){
	control_port_t * control_port;
	char control_value[255];
	control_port = (control_port_t *)data;
	printf("value = %f\n", control_port->current_real_value);
	if(control_port->current_real_value == 0.0f){
		evas_object_show(control_port->control_obj_2);
		control_port->current_real_value = 1.0f;
		evas_object_text_text_set(control_port->parent->rack_data_text_2, "on");
	}else{
		evas_object_hide(control_port->control_obj_2);
		control_port->current_real_value = 0.0f;
		evas_object_text_text_set(control_port->parent->rack_data_text_2, "off");
	}
	
}


void control_toggle_up_1(void *data, Evas *e, Evas_Object *obj, void *event_info){
	control_port_t * control_port;
	char control_value[255];
	control_port = (control_port_t *)data;
	evas_object_hide(control_port->control_obj_2);
	if(control_port->current_real_value < control_port->max_value){
		control_port->current_real_value = control_port->current_real_value + 1.0;
	}
	int crvi = control_port->current_real_value;
	sprintf(control_value, "%i", crvi);
	evas_object_text_text_set(control_port->parent->rack_data_text_2, control_value);
	evas_object_text_text_set(control_port->control_obj_5, control_value);
}
void control_toggle_up_2(void *data, Evas *e, Evas_Object *obj, void *event_info){
	control_port_t * control_port;
	char control_value[255];
	control_port = (control_port_t *)data;
	evas_object_hide(control_port->control_obj_4);
	if(control_port->current_real_value > control_port->min_value){
		control_port->current_real_value = control_port->current_real_value - 1.0;	
	}
	int crvi = control_port->current_real_value;
	sprintf(control_value, "%i", crvi);
	evas_object_text_text_set(control_port->parent->rack_data_text_2, control_value);
	evas_object_text_text_set(control_port->control_obj_5, control_value);
}
void control_toggle_down_1(void *data, Evas *e, Evas_Object *obj, void *event_info){
	control_port_t * control_port;
	char control_value[255];
	control_port = (control_port_t *)data;
	evas_object_show(control_port->control_obj_2);
	int crvi = control_port->current_real_value;
	sprintf(control_value, "%i", crvi);
	evas_object_text_text_set(control_port->parent->rack_data_text_2, control_value);
	evas_object_text_text_set(control_port->control_obj_5, control_value);
}
void control_toggle_down_2(void *data, Evas *e, Evas_Object *obj, void *event_info){
	control_port_t * control_port;
	char control_value[255];
	control_port = (control_port_t *)data;
	evas_object_show(control_port->control_obj_4);
	int crvi = control_port->current_real_value;
	sprintf(control_value, "%i", crvi);
	evas_object_text_text_set(control_port->parent->rack_data_text_2, control_value);
	evas_object_text_text_set(control_port->control_obj_5, control_value);
}



void bypass_switch(void *data, Evas *e, Evas_Object *obj, void *event_info){
	live_plugin_jack_t *live_plugin_jack;
	live_plugin_jack = (live_plugin_jack_t *)data;
	if(live_plugin_jack->bypass == 1){
		evas_object_hide(live_plugin_jack->on_switch);
		live_plugin_jack->bypass = 0;
		
	}else{
		evas_object_show(live_plugin_jack->on_switch);
		live_plugin_jack->bypass = 1;
	}
}


void wetdry_mouse_over(void *data, Evas *e, Evas_Object *obj, void *event_info){
	live_plugin_jack_t *live_plugin_jack;
	char control_value[255];
	float value;
	Evas_Coord ax, ay, aw, ah;
	live_plugin_jack = (live_plugin_jack_t *)data;	
	evas_object_geometry_get(live_plugin_jack->rack_data_text_1, &ax, &ay, &aw, &ah);
	evas_object_move(live_plugin_jack->rack_data_text_1, 673, ay);
	value = ((1.0/100.0) * (live_plugin_jack->wet_dry_value/2.7));
	sprintf(control_value, "%f", value);
	evas_object_text_text_set(live_plugin_jack->rack_data_text_1, "WET ~ DRY");
	evas_object_text_text_set(live_plugin_jack->rack_data_text_2, control_value);

}
void wetdry_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info){
	live_plugin_jack_t *live_plugin_jack;
	Evas_Coord ax, ay, aw, ah;
	live_plugin_jack = (live_plugin_jack_t *)data;
	evas_object_geometry_get(live_plugin_jack->rack_data_text_1, &ax, &ay, &aw, &ah);
	evas_object_text_text_set(live_plugin_jack->rack_data_text_1, live_plugin_jack->plugin_name);
	evas_object_move(live_plugin_jack->rack_data_text_1, 673, ay);
	evas_object_text_text_set(live_plugin_jack->rack_data_text_2, "");
}

void wetdry_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info){
	live_plugin_jack_t *live_plugin_jack;
	Evas_Event_Mouse_Down *ev;
	int drag_x, drag_y;
	char control_value[255];
	float value;	
	live_plugin_jack = (live_plugin_jack_t *)data;
	ev = event_info;
	
	evas_pointer_output_xy_get(e, &drag_x, &drag_y);
	if(ev->button == 1){
		if(last_drag_y > drag_y){
			if(live_plugin_jack->wet_dry_value < 270){
				live_plugin_jack->wet_dry_value++;
			}
		}else if(last_drag_y < drag_y){
			if(live_plugin_jack->wet_dry_value > 0){
				live_plugin_jack->wet_dry_value--;
			}
		}
		wet_dry_draw_dial_line(live_plugin_jack);
		
	}
	value = ((1.0/100.0) * (live_plugin_jack->wet_dry_value/2.7));
	sprintf(control_value, "%f", value);
	evas_object_text_text_set(live_plugin_jack->rack_data_text_2, control_value);
	last_drag_x = drag_x; 
	last_drag_y = drag_y;

}


void draw_dial_line(control_port_t *control_port){
	Evas_Coord cx, cy, cw, ch;
	int x1, y1, x2, y2; 
	int c1 = 10;
	int c2	= 5;
	double angle = (control_port->current_ui_value+130) * PI/180;
	x1 = cos(angle)*c1;
	y1 = sin(angle)*c1;
   	x2 = cos(angle)*c2;
   	y2 = sin(angle)*c2;
	

	if(control_port->control_obj_1){
		evas_object_del(control_port->control_obj_1);
	}
	control_port->control_obj_1 = evas_object_line_add(etk_widget_toplevel_evas_get(canvas));
	evas_object_geometry_get(control_port->control_back, &cx, &cy, &cw, &ch);
	cx = cx + 22;
	cy = cy + 37;
	evas_object_line_xy_set(control_port->control_obj_1, cx+x1, cy+y1, cx+x2, cy+y2);
	evas_object_color_set(control_port->control_obj_1, 255, 255, 255, 255);
	evas_object_show(control_port->control_obj_1);
}

void wet_dry_draw_dial_line(live_plugin_jack_t * live_plugin_jack){
	Evas_Coord cx, cy, cw, ch;
	int x1, y1, x2, y2; 
	int c1 = 10;
	int c2	= 5;
	double angle = (live_plugin_jack->wet_dry_value+130) * PI/180;
	x1 = cos(angle)*c1;
	y1 = sin(angle)*c1;
   	x2 = cos(angle)*c2;
   	y2 = sin(angle)*c2;
	if(live_plugin_jack->wet_dry_line){
		evas_object_del(live_plugin_jack->wet_dry_line);
	}
	live_plugin_jack->wet_dry_line = evas_object_line_add(etk_widget_toplevel_evas_get(canvas));
	evas_object_geometry_get(live_plugin_jack->wet_dry_dial, &cx, &cy, &cw, &ch);
	cx = cx + 22;
	cy = cy + 37;
	evas_object_line_xy_set(live_plugin_jack->wet_dry_line, cx+x1, cy+y1, cx+x2, cy+y2);
	evas_object_color_set(live_plugin_jack->wet_dry_line, 255, 255, 255, 255);
	evas_object_show(live_plugin_jack->wet_dry_line);
}

void delete_over(void *data, Evas *e, Evas_Object *obj, void *event_info){
	live_plugin_jack_t *live_plugin_jack;
	live_plugin_jack = (live_plugin_jack_t *)data;
	evas_object_show(live_plugin_jack->delete_over);
}

void delete_out(void *data, Evas *e, Evas_Object *obj, void *event_info){
	live_plugin_jack_t *live_plugin_jack;
	live_plugin_jack = (live_plugin_jack_t *)data;
	evas_object_hide(live_plugin_jack->delete_over);
}
Etk_Widget * prompt;
void delete_up(void *data, Evas *e, Evas_Object *obj, void *event_info){
	live_plugin_jack_t *live_plugin_jack;
	live_plugin_jack = (live_plugin_jack_t *)data;
	
	prompt = etk_message_dialog_new(ETK_MESSAGE_DIALOG_QUESTION, ETK_MESSAGE_DIALOG_YES_NO, "<b>This will effect all your sub effect patches!</b><br>You will have to repatch your effect connections.<br><br>Do want still want to delete this effect?");	
	etk_signal_connect_swapped("delete-event", ETK_OBJECT(prompt), ETK_CALLBACK(etk_object_destroy), prompt);
        etk_signal_connect("response", ETK_OBJECT(prompt), ETK_CALLBACK(delete_rack_confirm), (void *)live_plugin_jack);
	etk_widget_show_all(prompt);
	
}
void delete_rack_confirm(Etk_Object *object, int response_id, void *data){
	live_plugin_jack_t *live_plugin_jack;
	live_plugin_jack = (live_plugin_jack_t *)data;
	switch(response_id){
		case ETK_RESPONSE_YES:
			rack_delete(live_plugin_jack);	
		break;
		
	}
   	etk_object_destroy(object);	
}

void rack_delete(live_plugin_jack_t * live_plugin_jack){
	live_plugin_jack_t * live_plugin_jack_p;
	live_plugin_t * plugin_parent;
	Evas_Coord cx, cy, cw, ch;
	Evas_Coord px, py, pw, ph;
	control_port_t * control_port;	
	Evas_List *list;
	Evas_List *plist;
	disconnect_jack_ports(live_plugin_jack);
	evas_object_geometry_get(live_plugin_jack->rack_back, &cx, &cy, &cw, &ch);
	plugin_parent = live_plugin_jack->parent;
	/* destroy evas objects free memory */	
	evas_object_del(live_plugin_jack->rack_back);
	evas_object_del(live_plugin_jack->rack_top);
	evas_object_del(live_plugin_jack->rack_bottom);
	evas_object_del(live_plugin_jack->rack_title);
	evas_object_del(live_plugin_jack->rack_data_text_1);
	evas_object_del(live_plugin_jack->rack_data_text_2);
	evas_object_del(live_plugin_jack->rack_data_text_3);
	evas_object_del(live_plugin_jack->rack_data_clipper);
	evas_object_del(live_plugin_jack->on_switch);
	evas_object_del(live_plugin_jack->off_switch);
	evas_object_del(live_plugin_jack->delete);
	evas_object_del(live_plugin_jack->delete_over);
	evas_object_del(live_plugin_jack->bypass_title);
	evas_object_del(live_plugin_jack->wet_dry_text);
	evas_object_del(live_plugin_jack->wet_dry_hotspot);
	evas_object_del(live_plugin_jack->wet_dry_dial);
	evas_object_del(live_plugin_jack->wet_dry_line);
	for (list = live_plugin_jack->control_ports; list; list = evas_list_next(list)){
		control_port = (control_port_t *)list->data;
		evas_object_del(control_port->control_back);
		evas_object_del(control_port->control_title);
		evas_object_del(control_port->clipper);
		evas_object_del(control_port->control_back_midi);
		if(control_port->control_obj_1){
			evas_object_del(control_port->control_obj_1);
		}
		if(control_port->control_obj_2){
			evas_object_del(control_port->control_obj_2);
		}
		if(control_port->control_obj_3){
			evas_object_del(control_port->control_obj_3);
		}
		if(control_port->control_obj_4){
			evas_object_del(control_port->control_obj_4);
		}
		if(control_port->control_obj_5){
			evas_object_del(control_port->control_obj_5);
		}
	}
	list = evas_list_remove (plugin_parent->plugin_data, (void *)live_plugin_jack);
	plugin_parent->plugin_data = list;
	plugin_parent->plugin_count--;
	plugin_parent->rack_base = plugin_parent->rack_base - ch;
	for (plist = plugin_parent->plugin_data; plist; plist = evas_list_next(plist)){
		live_plugin_jack_p = (live_plugin_jack_t *)plist->data;
		evas_object_geometry_get(live_plugin_jack_p->rack_back, &px, &py, &pw, &ph);
		if(py > cy){
			rack_move(live_plugin_jack_p, px-cx, py-ch);
		}
	}
	
}

void move_evas_object(Evas_Object * eo, Evas_Coord x, Evas_Coord y,  Evas_Coord rx, Evas_Coord ry){
	Evas_Coord cx, cy, cw, ch;
	evas_object_geometry_get(eo, &cx, &cy, &cw, &ch);
	evas_object_move(eo, x+(cx-rx), y+(cy-ry));
}

void rack_move(live_plugin_jack_t * live_plugin_jack, Evas_Coord x, Evas_Coord y){
	control_port_t * control_port;	
	Evas_List *list;
	Evas_Coord cx, cy, cw, ch;
	evas_object_geometry_get(live_plugin_jack->rack_back, &cx, &cy, &cw, &ch);
	move_evas_object(live_plugin_jack->rack_back, x, y, cx, cy);
	move_evas_object(live_plugin_jack->rack_top, x, y, cx, cy);
	move_evas_object(live_plugin_jack->rack_bottom, x, y, cx, cy);
	move_evas_object(live_plugin_jack->rack_title, x, y, cx, cy);
	move_evas_object(live_plugin_jack->rack_data_text_1, x, y, cx, cy);
	move_evas_object(live_plugin_jack->rack_data_text_2, x, y, cx, cy);
	move_evas_object(live_plugin_jack->rack_data_text_3, x, y, cx, cy);
	move_evas_object(live_plugin_jack->rack_data_clipper, x, y, cx, cy);
	move_evas_object(live_plugin_jack->on_switch, x, y, cx, cy);
	move_evas_object(live_plugin_jack->off_switch, x, y, cx, cy);
	move_evas_object(live_plugin_jack->delete, x, y, cx, cy);
	move_evas_object(live_plugin_jack->delete_over, x, y, cx, cy);
	move_evas_object(live_plugin_jack->bypass_title, x, y, cx, cy);
	move_evas_object(live_plugin_jack->wet_dry_text, x, y, cx, cy);
	move_evas_object(live_plugin_jack->wet_dry_hotspot, x, y, cx, cy);
	move_evas_object(live_plugin_jack->wet_dry_dial, x, y, cx, cy);
	move_evas_object(live_plugin_jack->wet_dry_line, x, y, cx, cy);
	for (list = live_plugin_jack->control_ports; list; list = evas_list_next(list)){
		control_port = (control_port_t *)list->data;
		move_evas_object(control_port->control_back_midi, x, y, cx, cy);
		move_evas_object(control_port->control_back, x, y, cx, cy);
		move_evas_object(control_port->control_title, x, y, cx, cy);
		move_evas_object(control_port->clipper, x, y, cx, cy);
		if(control_port->control_obj_1){
			move_evas_object(control_port->control_obj_1, x, y, cx, cy);
		}
		move_evas_object(control_port->control_obj_2, x, y, cx, cy);
		if(control_port->control_obj_3){
			move_evas_object(control_port->control_obj_3, x, y, cx, cy);
		}
		if(control_port->control_obj_4, ch){
			move_evas_object(control_port->control_obj_4, x, y, cx, cy);
		}
		if(control_port->control_obj_5){
			move_evas_object(control_port->control_obj_5, x, y, cx, cy);
		}
	}
	
}

void rack_hide(void){
	live_plugin_jack_t * live_plugin_jack;
	control_port_t * control_port;	
	Evas_List *list, *listc;
	for (list = live_plugin->plugin_data; list; list = evas_list_next(list)){
		live_plugin_jack = (live_plugin_jack_t *)list->data;
		evas_object_hide(live_plugin_jack->rack_back);
		evas_object_hide(live_plugin_jack->rack_top);
		evas_object_hide(live_plugin_jack->rack_bottom);
		evas_object_hide(live_plugin_jack->rack_title);
		evas_object_hide(live_plugin_jack->rack_data_text_1);
		evas_object_hide(live_plugin_jack->rack_data_text_2);
		evas_object_hide(live_plugin_jack->rack_data_text_3);
		evas_object_hide(live_plugin_jack->rack_data_clipper);
		evas_object_hide(live_plugin_jack->on_switch);
		evas_object_hide(live_plugin_jack->off_switch);
		evas_object_hide(live_plugin_jack->delete);
		evas_object_hide(live_plugin_jack->delete_over);
		evas_object_hide(live_plugin_jack->bypass_title);
		evas_object_hide(live_plugin_jack->wet_dry_text);
		evas_object_hide(live_plugin_jack->wet_dry_hotspot);
		evas_object_hide(live_plugin_jack->wet_dry_dial);
		evas_object_hide(live_plugin_jack->wet_dry_line);
		for (listc = live_plugin_jack->control_ports; listc; listc = evas_list_next(listc)){
			control_port = (control_port_t *)listc->data;
			evas_object_hide(control_port->control_back_midi);
			evas_object_hide(control_port->control_back);
			evas_object_hide(control_port->control_title);
			evas_object_hide(control_port->clipper);
			evas_object_hide(control_port->control_obj_1);
			evas_object_hide(control_port->control_obj_2);
			evas_object_hide(control_port->control_obj_3);
			evas_object_hide(control_port->control_obj_4);
			evas_object_hide(control_port->control_obj_5);
		}
	}
}

void rack_show(void){
	live_plugin_jack_t * live_plugin_jack;
	control_port_t * control_port;	
	Evas_List *list, *listc;
	for (list = live_plugin->plugin_data; list; list = evas_list_next(list)){
		live_plugin_jack = (live_plugin_jack_t *)list->data;
		evas_object_show(live_plugin_jack->rack_back);
		evas_object_show(live_plugin_jack->rack_top);
		evas_object_show(live_plugin_jack->rack_bottom);
		evas_object_show(live_plugin_jack->rack_title);
		evas_object_show(live_plugin_jack->rack_data_text_1);
		evas_object_show(live_plugin_jack->rack_data_text_2);
		evas_object_show(live_plugin_jack->rack_data_text_3);
		evas_object_show(live_plugin_jack->rack_data_clipper);
		if(live_plugin_jack->bypass==1){
			evas_object_show(live_plugin_jack->on_switch);
		}
		evas_object_show(live_plugin_jack->off_switch);
		evas_object_show(live_plugin_jack->delete);
		evas_object_show(live_plugin_jack->bypass_title);
		evas_object_show(live_plugin_jack->wet_dry_text);
		evas_object_show(live_plugin_jack->wet_dry_hotspot);
		evas_object_show(live_plugin_jack->wet_dry_dial);
		evas_object_show(live_plugin_jack->wet_dry_line);
		for (listc = live_plugin_jack->control_ports; listc; listc = evas_list_next(listc)){
			control_port = (control_port_t *)listc->data;
			evas_object_show(control_port->control_back_midi);
			evas_object_show(control_port->control_back);
			evas_object_show(control_port->control_title);
			evas_object_show(control_port->clipper);
			evas_object_show(control_port->control_obj_1);			
		
			if(control_port->control_port_type == 1){
				if(control_port->current_real_value == 1.0f){
					evas_object_show(control_port->control_obj_2);
				}
			}else if(control_port->control_port_type == 2){
				evas_object_show(control_port->control_obj_5);
			}else{
				evas_object_show(control_port->control_obj_2);
				evas_object_show(control_port->control_obj_3);
				evas_object_show(control_port->control_obj_5);
			}
		}
	}
}

void adjust_rack_ui_controls(void){
	char control_value[255];
	live_plugin_jack_t * live_plugin_jack;
	control_port_t * control_port;	
	Evas_List *list, *listc;
	int crvi;
	for (list = live_plugin->plugin_data; list; list = evas_list_next(list)){
		live_plugin_jack = (live_plugin_jack_t *)list->data;
		for (listc = live_plugin_jack->control_ports; listc; listc = evas_list_next(listc)){
			control_port = (control_port_t *)listc->data;
			wet_dry_draw_dial_line(live_plugin_jack);
			if(control_port->midi_pedal_ui == TRUE){
				evas_object_hide(control_port->control_back);
			}else{
				evas_object_show(control_port->control_back);
			}
			if(control_port->control_port_type == 1){
				if(control_port->current_real_value == 1.0f){
					evas_object_show(control_port->control_obj_2);
				}
			}else if(control_port->control_port_type == 2){
				evas_object_show(control_port->control_obj_1);
				evas_object_show(control_port->control_obj_3);
				evas_object_show(control_port->control_obj_5);
				crvi = control_port->current_real_value;
				sprintf(control_value, "%i", crvi);
				evas_object_text_text_set(control_port->control_obj_5, control_value);
			}else{
				control_port->current_ui_value = (270.0f/100.0f) * ((control_port->current_real_value - control_port->min_value) / ((control_port->max_value - control_port->min_value)/100.0f));
				draw_dial_line(control_port);
				evas_object_show(control_port->control_obj_2);
				evas_object_show(control_port->control_obj_3);
				evas_object_show(control_port->control_obj_5);	
				
			}
		}
	}
}

