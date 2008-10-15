#include "mux_gui.h"

plugin_is_valid (const LADSPA_Descriptor * descriptor)
{
  unsigned long i;
  unsigned long icount = 0;
  unsigned long ocount = 0;
  
  for (i = 0; i < descriptor->PortCount; i++)
    {
      if (!LADSPA_IS_PORT_AUDIO (descriptor->PortDescriptors[i]))
        continue;
      
      if (LADSPA_IS_PORT_INPUT (descriptor->PortDescriptors[i]))
        icount++;
      else
        ocount++;
    }
  
  if (icount == 0 || ocount == 0)
    return FALSE;
  
  return TRUE;
}




static void
plugin_desc_init (plugin_desc_t * pd)
{
  pd->object_file      = NULL;
  pd->id               = 0;
  pd->name             = NULL;
  pd->properties       = 0;
  pd->channels         = 0;
  pd->port_count       = 0;
  pd->port_descriptors = NULL;
  pd->port_range_hints = NULL;
  pd->audio_input_port_indicies = NULL;
  pd->audio_output_port_indicies = NULL;
  pd->audio_aux_port_indicies = NULL;
  pd->control_port_count = 0;
  pd->control_port_indicies = NULL;
  pd->aux_channels = 0;
  pd->aux_are_input = TRUE;
 
}

plugin_desc_t *
plugin_desc_new (void)
{
  plugin_desc_t * pd;
  pd = malloc (sizeof (plugin_desc_t));
  plugin_desc_init (pd);
  return pd;
}


plugin_desc_t *
plugin_desc_new_with_descriptor (const char * object_file,
                                 unsigned long index,
                                 const LADSPA_Descriptor * descriptor)
{
  int i;
  plugin_desc_t * pd;
  pd = plugin_desc_new ();
  pd->descriptor = descriptor;
  pd->object_file = calloc(strlen(object_file) + 1, sizeof(char));
  sprintf(pd->object_file, "%s", object_file);  
  pd->index = index;
  pd->id = descriptor->UniqueID;
  pd->name = calloc(strlen(descriptor->Name) + 1, sizeof(char));
  sprintf(pd->name, "%s", descriptor->Name);  

  pd->properties = descriptor->Properties;

  pd->port_range_hints = malloc (sizeof (LADSPA_PortRangeHint) * descriptor->PortCount);
  memcpy (pd->port_range_hints, descriptor->PortRangeHints, sizeof (LADSPA_PortRangeHint) * descriptor->PortCount);

  pd->port_descriptors = malloc (sizeof (LADSPA_PortDescriptor) * descriptor->PortCount);
  memcpy (pd->port_descriptors, descriptor->PortDescriptors, sizeof (LADSPA_PortDescriptor) * descriptor->PortCount);

  pd->port_count = descriptor->PortCount;

  pd->port_names = malloc (sizeof (char *) * descriptor->PortCount);
  for (i = 0; i < descriptor->PortCount; i++)
  	pd->port_names[i] = strdup (descriptor->PortNames[i]);
  
  pd->rt = LADSPA_IS_HARD_RT_CAPABLE(descriptor->Properties) ? TRUE : FALSE;

  return pd;

}

static void
plugin_mgr_get_object_file_plugins (plugin_mgr_t * plugin_mgr, const char * filename)
{
  const char * dlerr;
  void * dl_handle;
  LADSPA_Descriptor_Function get_descriptor;
  const LADSPA_Descriptor * descriptor;
  unsigned long plugin_index;
  plugin_desc_t * desc, * other_desc = NULL;
  Evas_List * list;
  Evas_Bool exists;
  int err;
  
  /* open the object file */
  dl_handle = dlopen (filename, RTLD_NOW|RTLD_GLOBAL);
  if (!dl_handle)
    {
      /*fprintf (stderr, "%s: error opening shared object file '%s': %s\n",
               __FUNCTION__, filename, dlerror());*/
      return;
    }
  
  
  /* get the get_descriptor function */
  dlerror (); /* clear the error report */
  
  get_descriptor = (LADSPA_Descriptor_Function)
    dlsym (dl_handle, "ladspa_descriptor");
  
  dlerr = dlerror();
  if (dlerr) {
    /*fprintf (stderr, "%s: error finding ladspa_descriptor symbol in object file '%s': %s\n",
             __FUNCTION__, filename, dlerr);*/
    dlclose (dl_handle);
    return;
  }
  
  plugin_index = 0;
  while ( (descriptor = get_descriptor (plugin_index)) )
    {
      if (!plugin_is_valid (descriptor))
        {
          plugin_index++;
          continue;
        }
      
      /* check it doesn't already exist */
      exists = FALSE;
      for (list = plugin_mgr->all_plugins; list; list = evas_list_next(list))
        { 
	other_desc = (plugin_desc_t *) list->data;
          
          if (other_desc->id == descriptor->UniqueID)
            {
              exists = TRUE;
              break;
            }
	
        }
     
    if (exists)
        {
          /*printf ("Plugin %ld exists in both '%s' and '%s'; using version in '%s'\n",
                  descriptor->UniqueID, other_desc->object_file, filename, other_desc->object_file);*/
          plugin_index++;
          continue;
        }
     
      /*printf("%i\n", descriptor->UniqueID);*/
     
      desc = plugin_desc_new_with_descriptor (filename, plugin_index, descriptor);
      plugin_mgr->all_plugins = evas_list_append (plugin_mgr->all_plugins, desc);
      
      plugin_index++;
      plugin_mgr->plugin_count++;
      /* print in the splash screen */
      /*ui_display_splash_text (ui, "Loaded plugin '%s'", desc->name);*/
    }
  
  err = dlclose (dl_handle);
  if (err)
    {
      /*fprintf (stderr, "%s: error closing object file '%s': %s\n",
               __FUNCTION__, filename, dlerror ());*/
    }
  
}

static void
plugin_mgr_get_dir_plugins (plugin_mgr_t * plugin_mgr, const char * dir)
{
  DIR * dir_stream;
  struct dirent * dir_entry;
  char * file_name;
  int err;
  size_t dirlen;
  
  dir_stream = opendir (dir);
  if (!dir_stream)
    {
/*      fprintf (stderr, "%s: error opening directory '%s': %s\n",
               __FUNCTION__, dir, strerror (errno)); */
      return;
    }
  
  dirlen = strlen (dir);
  
  while ( (dir_entry = readdir (dir_stream)) )
    {
      if (strcmp (dir_entry->d_name, ".") == 0 ||
          strcmp (dir_entry->d_name, "..") == 0)
        continue;
  
     file_name = malloc (dirlen + 1 + strlen (dir_entry->d_name) + 1);
   
      strcpy (file_name, dir);
      if (file_name[dirlen - 1] == '/')
        strcpy (file_name + dirlen, dir_entry->d_name);
      else
        {
          file_name[dirlen] = '/';
          strcpy (file_name + dirlen + 1, dir_entry->d_name);
        }
      /*printf ("%s\n", file_name);*/
      plugin_mgr_get_object_file_plugins (plugin_mgr, file_name);
      /**/
      E_FREE (file_name);
    }

  err = closedir (dir_stream);
  if (err)
    fprintf (stderr, "%s: error closing directory '%s': %s\n",
             __FUNCTION__, dir, strerror (errno));
}


/* needs heaps more work */

void plugin_mgr_get_path_plugins (plugin_mgr_t * plugin_mgr)
{
  char ladspa_path[255];
  char *dir;
  
  /*snprintf(ladspa_path, sizeof(ladspa_path), "%s", getenv ("LADSPA_PATH"));*/
  /*ecore_strlcpy (ladspa_path, getenv ("LADSPA_PATH"), sizeof(ladspa_path));*/
  /*ladspa_path = g_strdup (getenv ("LADSPA_PATH"));*/
  /*if (!ladspa_path)*/
   // ecore_strlcpy (ladspa_path, "/usr/local/lib/ladspa:/usr/lib/ladspa", sizeof(ladspa_path));
    snprintf(ladspa_path, sizeof(ladspa_path), "%s", "plugins");//"/usr/local/lib/ladspa:/usr/lib/ladspa");
    /*ladspa_path = g_strdup ("/usr/local/lib/ladspa:/usr/lib/ladspa");*/
  
  dir = strtok (ladspa_path, ":");
  do
    plugin_mgr_get_dir_plugins (plugin_mgr, dir);
  while ((dir = strtok (NULL, ":")));

  /*E_FREE(ladspa_path);*/
}


