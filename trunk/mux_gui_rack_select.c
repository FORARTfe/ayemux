#include "mux_gui.h"

void _mouse_select_plugin(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data);
static int _tree_compare_cb(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data);

static Etk_Widget *win = NULL;
void _create_select_rack_window(void *data)
{
   
   Etk_Widget *vbox;
   Etk_Widget *tree;
   Etk_Tree_Col *col1, *col2, *col3, *col4, *col5;
   Etk_Tree_Row *row;
   Etk_Widget *statusbar;
   Etk_Color c_warn;
   Evas_List *list;
   plugin_desc_t * desc;
   plugin_mgr_t * plugin_mgr;
   char row_name[128];
   const char *stock_key;
   int i;
  
   plugin_mgr = (plugin_desc_t *) data;

   if (win)
   {
      etk_widget_show(ETK_WIDGET(win));
      return;
   }

   /* Create the window and the vbox where the widgets will be packed into */
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Select plugin to add to rack");
   etk_window_resize(ETK_WINDOW(win), 440, 500);
   etk_signal_connect("delete-event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(win), vbox);
   
   /* Create the tree widget */
   tree = etk_tree_new();
   etk_tree_mode_set(ETK_TREE(tree), ETK_TREE_MODE_TREE);
   /*etk_tree_multiple_select_set(ETK_TREE(tree), ETK_TRUE);*/
   etk_widget_padding_set(tree, 5, 5, 5, 5);	
   etk_box_append(ETK_BOX(vbox), tree, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   /* We first create the columns of the tree, and then we "build" the tree with etk_tree_build() */
   col1 = etk_tree_col_new(ETK_TREE(tree), "Plugin", 130, 0.0);
   etk_tree_col_model_add(col1, etk_tree_model_image_new());
   etk_tree_col_model_add(col1, etk_tree_model_text_new());
   
        
   etk_tree_build(ETK_TREE(tree));
   
   etk_tree_freeze(ETK_TREE(tree));
   etk_theme_color_get(NULL, ETK_COLOR_WARNING_FG, &c_warn.r, &c_warn.g, &c_warn.b, &c_warn.a);
  
  printf("%i \n", (int*)mux_plugin_mgr->plugin_count);
  for (list = plugin_mgr->all_plugins; list; list = evas_list_next(list)){
        desc = (plugin_desc_t *) list->data;
        sprintf(row_name, "%s", desc->name);
	if(desc->rt == TRUE){
		stock_key = etk_stock_key_get(ETK_STOCK_PLACES_USER_HOME, ETK_STOCK_SMALL);
        	row = etk_tree_row_append(ETK_TREE(tree), NULL, col1, etk_theme_icon_path_get(), stock_key, row_name, NULL);
        	etk_tree_row_data_set(row, list->data); 
	}	
   }
   	
   etk_tree_thaw(ETK_TREE(tree));
   etk_tree_col_sort_set(col1, _tree_compare_cb, NULL);
   etk_tree_col_sort(col1, TRUE);
   /* Finally we create the statusbar used to display the events on the tree */
   statusbar = etk_statusbar_new();
   etk_box_append(ETK_BOX(vbox), statusbar, ETK_BOX_START, ETK_BOX_FILL, 0);
   
   
   etk_signal_connect("row-clicked", ETK_OBJECT(tree),
      ETK_CALLBACK(_mouse_select_plugin), statusbar);
   
   etk_widget_show_all(win);
}

static int _tree_compare_cb(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data)
{
   const char *str1, *str2;
   
   etk_tree_row_fields_get(row1, col, NULL, NULL, &str1, NULL);
   etk_tree_row_fields_get(row2, col, NULL, NULL, &str2, NULL);
   return strcmp(str1, str2);
}


void _mouse_select_plugin(Etk_Object *object, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data)
{
   Etk_Tree *tree;
   Etk_Statusbar *statusbar;
   char *row_name;
   char message[1024];
   void *mydata;
   if (!(tree = ETK_TREE(object)) || !(statusbar = ETK_STATUSBAR(data)) || !row || !event)
      return;
   
    etk_tree_row_fields_get(row, etk_tree_nth_col_get(tree, 0), NULL, NULL, &row_name, NULL);
   mydata = etk_tree_row_data_get(row);
   apply_plugin(mydata, FALSE);
   etk_widget_hide(ETK_WIDGET(win));
   
}

