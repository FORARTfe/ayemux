CC=gcc
CFLAGS=`/usr/local/bin/etk-config --libs --cflags` `pkg-config --cflags --libs jack` `xml2-config --cflags --libs` -lasound -lpthread -I.
hellomake: mux.o mux_gui.o mux_gui_rack_select.o mux_plugin_manager.o mux_expose_plugin.o mux_jackit.o mux_live_plugin_ui.o mux_state.o mux_midi.o mux_file_io.o mux_patch_bay_ui.o
	$(CC) -o ayemux mux.o mux_gui.o mux_gui_rack_select.o mux_plugin_manager.o mux_expose_plugin.o mux_jackit.o mux_live_plugin_ui.o mux_state.o  mux_midi.o mux_file_io.o mux_patch_bay_ui.o $(CFLAGS)
