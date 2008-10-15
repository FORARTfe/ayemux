#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include "config.h"
#endif

#ifdef ENABLE_NLS
#include <libintl.h>
#endif

#define         _ISOC9X_SOURCE  1
#define         _ISOC99_SOURCE  1
#define         __USE_ISOC99    1
#define         __USE_ISOC9X    1

#include <math.h>

#include "ladspa.h"

#ifdef WIN32
#define _WINDOWS_DLL_EXPORT_ __declspec(dllexport)
int bIsFirstTime = 1; 
void _init(); // forward declaration
#else
#define _WINDOWS_DLL_EXPORT_ 
#endif

#line 9 "debug_1184.xml"

#include "stdio.h"

#define DEBUG_ALLVALS                  0
#define DEBUG_RESET                    1
#define DEBUG_INPUT                    2
#define DEBUG_OUTPUT                   3

static LADSPA_Descriptor *debugDescriptor = NULL;

typedef struct {
	LADSPA_Data *allvals;
	LADSPA_Data *reset;
	LADSPA_Data *input;
	LADSPA_Data *output;
	LADSPA_Data run_adding_gain;
} Debug;

_WINDOWS_DLL_EXPORT_
const LADSPA_Descriptor *ladspa_descriptor(unsigned long index) {

#ifdef WIN32
	if (bIsFirstTime) {
		_init();
		bIsFirstTime = 0;
	}
#endif
	switch (index) {
	case 0:
		return debugDescriptor;
	default:
		return NULL;
	}
}

static void cleanupDebug(LADSPA_Handle instance) {
	free(instance);
}

static void connectPortDebug(
 LADSPA_Handle instance,
 unsigned long port,
 LADSPA_Data *data) {
	Debug *plugin;

	plugin = (Debug *)instance;
	switch (port) {
	case DEBUG_ALLVALS:
		plugin->allvals = data;
		break;
	case DEBUG_RESET:
		plugin->reset = data;
		break;
	case DEBUG_INPUT:
		plugin->input = data;
		break;
	case DEBUG_OUTPUT:
		plugin->output = data;
		break;
	}
}

static LADSPA_Handle instantiateDebug(
 const LADSPA_Descriptor *descriptor,
 unsigned long s_rate) {
	Debug *plugin_data = (Debug *)malloc(sizeof(Debug));

#line 19 "debug_1184.xml"
	printf("sample rate %ld\n", s_rate);


	return (LADSPA_Handle)plugin_data;
}

#undef buffer_write
#undef RUN_ADDING
#undef RUN_REPLACING

#define buffer_write(b, v) (b = v)
#define RUN_ADDING    0
#define RUN_REPLACING 1

static void runDebug(LADSPA_Handle instance, unsigned long sample_count) {
	Debug *plugin_data = (Debug *)instance;

	/* Diplay all values? (float value) */
	const LADSPA_Data allvals = *(plugin_data->allvals);

	/* Reset counters? (float value) */
	const LADSPA_Data reset = *(plugin_data->reset);

	/* Input (array of floats of length sample_count) */
	const LADSPA_Data * const input = plugin_data->input;

	/* Output (array of floats of length sample_count) */
	LADSPA_Data * const output = plugin_data->output;

#line 23 "debug_1184.xml"
	static LADSPA_Data max, min, maxl, minl;
	unsigned long pos;

	if (reset) {
	        max = 0;
	        min = 0;
	        maxl = 0;
	        minl = 1;
	}

	for (pos = 0; pos < sample_count; pos++) {
	        if (allvals) {
	                printf("%f\n", input[pos]);
	        }
	        max = fabs(input[pos]) > max?fabs(input[pos]):max;
	        min = fabs(input[pos]) < min?fabs(input[pos]):min;
	        maxl = input[pos] > maxl?input[pos]:maxl;
	        minl = input[pos] < minl?input[pos]:minl;
	        buffer_write(output[pos], input[pos]);
	}
	printf("amplitude (%f, %f)\t", min, max);
	printf("level (%f, %f)\n", minl, maxl);
}
#undef buffer_write
#undef RUN_ADDING
#undef RUN_REPLACING

#define buffer_write(b, v) (b += (v) * run_adding_gain)
#define RUN_ADDING    1
#define RUN_REPLACING 0

static void setRunAddingGainDebug(LADSPA_Handle instance, LADSPA_Data gain) {
	((Debug *)instance)->run_adding_gain = gain;
}

static void runAddingDebug(LADSPA_Handle instance, unsigned long sample_count) {
	Debug *plugin_data = (Debug *)instance;
	LADSPA_Data run_adding_gain = plugin_data->run_adding_gain;

	/* Diplay all values? (float value) */
	const LADSPA_Data allvals = *(plugin_data->allvals);

	/* Reset counters? (float value) */
	const LADSPA_Data reset = *(plugin_data->reset);

	/* Input (array of floats of length sample_count) */
	const LADSPA_Data * const input = plugin_data->input;

	/* Output (array of floats of length sample_count) */
	LADSPA_Data * const output = plugin_data->output;

#line 23 "debug_1184.xml"
	static LADSPA_Data max, min, maxl, minl;
	unsigned long pos;

	if (reset) {
	        max = 0;
	        min = 0;
	        maxl = 0;
	        minl = 1;
	}

	for (pos = 0; pos < sample_count; pos++) {
	        if (allvals) {
	                printf("%f\n", input[pos]);
	        }
	        max = fabs(input[pos]) > max?fabs(input[pos]):max;
	        min = fabs(input[pos]) < min?fabs(input[pos]):min;
	        maxl = input[pos] > maxl?input[pos]:maxl;
	        minl = input[pos] < minl?input[pos]:minl;
	        buffer_write(output[pos], input[pos]);
	}
	printf("amplitude (%f, %f)\t", min, max);
	printf("level (%f, %f)\n", minl, maxl);
}

void _init() {
	char **port_names;
	LADSPA_PortDescriptor *port_descriptors;
	LADSPA_PortRangeHint *port_range_hints;

#ifdef ENABLE_NLS
#define D_(s) dgettext(PACKAGE, s)
	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
#else
#define D_(s) (s)
#endif


	debugDescriptor =
	 (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));

	if (debugDescriptor) {
		debugDescriptor->UniqueID = 1184;
		debugDescriptor->Label = "debug";
		debugDescriptor->Properties =
		 LADSPA_PROPERTY_HARD_RT_CAPABLE;
		debugDescriptor->Name =
		 D_("Debug Plugin");
		debugDescriptor->Maker =
		 "Steve Harris <steve@plugin.org.uk>";
		debugDescriptor->Copyright =
		 "GPL";
		debugDescriptor->PortCount = 4;

		port_descriptors = (LADSPA_PortDescriptor *)calloc(4,
		 sizeof(LADSPA_PortDescriptor));
		debugDescriptor->PortDescriptors =
		 (const LADSPA_PortDescriptor *)port_descriptors;

		port_range_hints = (LADSPA_PortRangeHint *)calloc(4,
		 sizeof(LADSPA_PortRangeHint));
		debugDescriptor->PortRangeHints =
		 (const LADSPA_PortRangeHint *)port_range_hints;

		port_names = (char **)calloc(4, sizeof(char*));
		debugDescriptor->PortNames =
		 (const char **)port_names;

		/* Parameters for Diplay all values? */
		port_descriptors[DEBUG_ALLVALS] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[DEBUG_ALLVALS] =
		 D_("Diplay all values?");
		port_range_hints[DEBUG_ALLVALS].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
		port_range_hints[DEBUG_ALLVALS].LowerBound = 0;
		port_range_hints[DEBUG_ALLVALS].UpperBound = 1;

		/* Parameters for Reset counters? */
		port_descriptors[DEBUG_RESET] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[DEBUG_RESET] =
		 D_("Reset counters?");
		port_range_hints[DEBUG_RESET].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
		port_range_hints[DEBUG_RESET].LowerBound = 0;
		port_range_hints[DEBUG_RESET].UpperBound = 1;

		/* Parameters for Input */
		port_descriptors[DEBUG_INPUT] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		port_names[DEBUG_INPUT] =
		 D_("Input");
		port_range_hints[DEBUG_INPUT].HintDescriptor = 0;

		/* Parameters for Output */
		port_descriptors[DEBUG_OUTPUT] =
		 LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		port_names[DEBUG_OUTPUT] =
		 D_("Output");
		port_range_hints[DEBUG_OUTPUT].HintDescriptor = 0;

		debugDescriptor->activate = NULL;
		debugDescriptor->cleanup = cleanupDebug;
		debugDescriptor->connect_port = connectPortDebug;
		debugDescriptor->deactivate = NULL;
		debugDescriptor->instantiate = instantiateDebug;
		debugDescriptor->run = runDebug;
		debugDescriptor->run_adding = runAddingDebug;
		debugDescriptor->set_run_adding_gain = setRunAddingGainDebug;
	}
}

void _fini() {
	if (debugDescriptor) {
		free((LADSPA_PortDescriptor *)debugDescriptor->PortDescriptors);
		free((char **)debugDescriptor->PortNames);
		free((LADSPA_PortRangeHint *)debugDescriptor->PortRangeHints);
		free(debugDescriptor);
	}

}
