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

#line 10 "surround_encoder_1401.xml"

#include "util/biquad.h"

#define SURROUNDENCODER_L              0
#define SURROUNDENCODER_R              1
#define SURROUNDENCODER_C              2
#define SURROUNDENCODER_S              3
#define SURROUNDENCODER_LT             4
#define SURROUNDENCODER_RT             5

static LADSPA_Descriptor *surroundEncoderDescriptor = NULL;

typedef struct {
	LADSPA_Data *l;
	LADSPA_Data *r;
	LADSPA_Data *c;
	LADSPA_Data *s;
	LADSPA_Data *lt;
	LADSPA_Data *rt;
	LADSPA_Data *buffer;
	unsigned int buffer_pos;
	unsigned int buffer_size;
	biquad *     hc;
	biquad *     lc;
	LADSPA_Data run_adding_gain;
} SurroundEncoder;

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
		return surroundEncoderDescriptor;
	default:
		return NULL;
	}
}

static void activateSurroundEncoder(LADSPA_Handle instance) {
	SurroundEncoder *plugin_data = (SurroundEncoder *)instance;
	LADSPA_Data *buffer = plugin_data->buffer;
	unsigned int buffer_pos = plugin_data->buffer_pos;
	unsigned int buffer_size = plugin_data->buffer_size;
	biquad *hc = plugin_data->hc;
	biquad *lc = plugin_data->lc;
#line 44 "surround_encoder_1401.xml"
	memset(buffer, 0, buffer_size * sizeof(LADSPA_Data));
	biquad_init(lc);
	biquad_init(hc);
	plugin_data->buffer = buffer;
	plugin_data->buffer_pos = buffer_pos;
	plugin_data->buffer_size = buffer_size;
	plugin_data->hc = hc;
	plugin_data->lc = lc;

}

static void cleanupSurroundEncoder(LADSPA_Handle instance) {
#line 50 "surround_encoder_1401.xml"
	SurroundEncoder *plugin_data = (SurroundEncoder *)instance;
	free(plugin_data->lc);
	free(plugin_data->hc);
	free(plugin_data->buffer);
	free(instance);
}

static void connectPortSurroundEncoder(
 LADSPA_Handle instance,
 unsigned long port,
 LADSPA_Data *data) {
	SurroundEncoder *plugin;

	plugin = (SurroundEncoder *)instance;
	switch (port) {
	case SURROUNDENCODER_L:
		plugin->l = data;
		break;
	case SURROUNDENCODER_R:
		plugin->r = data;
		break;
	case SURROUNDENCODER_C:
		plugin->c = data;
		break;
	case SURROUNDENCODER_S:
		plugin->s = data;
		break;
	case SURROUNDENCODER_LT:
		plugin->lt = data;
		break;
	case SURROUNDENCODER_RT:
		plugin->rt = data;
		break;
	}
}

static LADSPA_Handle instantiateSurroundEncoder(
 const LADSPA_Descriptor *descriptor,
 unsigned long s_rate) {
	SurroundEncoder *plugin_data = (SurroundEncoder *)malloc(sizeof(SurroundEncoder));
	LADSPA_Data *buffer = NULL;
	unsigned int buffer_pos;
	unsigned int buffer_size;
	biquad *hc = NULL;
	biquad *lc = NULL;

#line 32 "surround_encoder_1401.xml"
	buffer_size = (int)(0.0072f * s_rate);
	buffer_pos = 0;
	buffer = calloc(buffer_size, sizeof(LADSPA_Data));
	lc = malloc(sizeof(biquad));
	hc = malloc(sizeof(biquad));
	biquad_init(lc);
	biquad_init(hc);
	ls_set_params(lc, 100.0f, -70.0f, 1.0f, s_rate);
	hs_set_params(hc, 7000.0f, -70.0f, 1.0f, s_rate);

	plugin_data->buffer = buffer;
	plugin_data->buffer_pos = buffer_pos;
	plugin_data->buffer_size = buffer_size;
	plugin_data->hc = hc;
	plugin_data->lc = lc;

	return (LADSPA_Handle)plugin_data;
}

#undef buffer_write
#undef RUN_ADDING
#undef RUN_REPLACING

#define buffer_write(b, v) (b = v)
#define RUN_ADDING    0
#define RUN_REPLACING 1

static void runSurroundEncoder(LADSPA_Handle instance, unsigned long sample_count) {
	SurroundEncoder *plugin_data = (SurroundEncoder *)instance;

	/* L (array of floats of length sample_count) */
	const LADSPA_Data * const l = plugin_data->l;

	/* R (array of floats of length sample_count) */
	const LADSPA_Data * const r = plugin_data->r;

	/* C (array of floats of length sample_count) */
	const LADSPA_Data * const c = plugin_data->c;

	/* S (array of floats of length sample_count) */
	const LADSPA_Data * const s = plugin_data->s;

	/* Lt (array of floats of length sample_count) */
	LADSPA_Data * const lt = plugin_data->lt;

	/* Rt (array of floats of length sample_count) */
	LADSPA_Data * const rt = plugin_data->rt;
	LADSPA_Data * buffer = plugin_data->buffer;
	unsigned int buffer_pos = plugin_data->buffer_pos;
	unsigned int buffer_size = plugin_data->buffer_size;
	biquad * hc = plugin_data->hc;
	biquad * lc = plugin_data->lc;

#line 56 "surround_encoder_1401.xml"
	unsigned long pos;
	LADSPA_Data s_delayed;

	for (pos = 0; pos < sample_count; pos++) {
	  s_delayed = buffer[buffer_pos];
	  buffer[buffer_pos++] = s[pos];
	  buffer_pos %= buffer_size;

	  s_delayed = biquad_run(lc, s_delayed);
	  s_delayed = biquad_run(hc, s_delayed);

	  buffer_write(lt[pos], l[pos] + c[pos] * 0.707946f +
	               s_delayed * 0.707946f);
	  buffer_write(rt[pos], r[pos] + c[pos] * 0.707946f -
	               s_delayed * 0.707946f);
	}

	plugin_data->buffer_pos = buffer_pos;
}
#undef buffer_write
#undef RUN_ADDING
#undef RUN_REPLACING

#define buffer_write(b, v) (b += (v) * run_adding_gain)
#define RUN_ADDING    1
#define RUN_REPLACING 0

static void setRunAddingGainSurroundEncoder(LADSPA_Handle instance, LADSPA_Data gain) {
	((SurroundEncoder *)instance)->run_adding_gain = gain;
}

static void runAddingSurroundEncoder(LADSPA_Handle instance, unsigned long sample_count) {
	SurroundEncoder *plugin_data = (SurroundEncoder *)instance;
	LADSPA_Data run_adding_gain = plugin_data->run_adding_gain;

	/* L (array of floats of length sample_count) */
	const LADSPA_Data * const l = plugin_data->l;

	/* R (array of floats of length sample_count) */
	const LADSPA_Data * const r = plugin_data->r;

	/* C (array of floats of length sample_count) */
	const LADSPA_Data * const c = plugin_data->c;

	/* S (array of floats of length sample_count) */
	const LADSPA_Data * const s = plugin_data->s;

	/* Lt (array of floats of length sample_count) */
	LADSPA_Data * const lt = plugin_data->lt;

	/* Rt (array of floats of length sample_count) */
	LADSPA_Data * const rt = plugin_data->rt;
	LADSPA_Data * buffer = plugin_data->buffer;
	unsigned int buffer_pos = plugin_data->buffer_pos;
	unsigned int buffer_size = plugin_data->buffer_size;
	biquad * hc = plugin_data->hc;
	biquad * lc = plugin_data->lc;

#line 56 "surround_encoder_1401.xml"
	unsigned long pos;
	LADSPA_Data s_delayed;

	for (pos = 0; pos < sample_count; pos++) {
	  s_delayed = buffer[buffer_pos];
	  buffer[buffer_pos++] = s[pos];
	  buffer_pos %= buffer_size;

	  s_delayed = biquad_run(lc, s_delayed);
	  s_delayed = biquad_run(hc, s_delayed);

	  buffer_write(lt[pos], l[pos] + c[pos] * 0.707946f +
	               s_delayed * 0.707946f);
	  buffer_write(rt[pos], r[pos] + c[pos] * 0.707946f -
	               s_delayed * 0.707946f);
	}

	plugin_data->buffer_pos = buffer_pos;
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


	surroundEncoderDescriptor =
	 (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));

	if (surroundEncoderDescriptor) {
		surroundEncoderDescriptor->UniqueID = 1401;
		surroundEncoderDescriptor->Label = "surroundEncoder";
		surroundEncoderDescriptor->Properties =
		 LADSPA_PROPERTY_HARD_RT_CAPABLE;
		surroundEncoderDescriptor->Name =
		 D_("Surround matrix encoder");
		surroundEncoderDescriptor->Maker =
		 "Steve Harris <steve@plugin.org.uk>";
		surroundEncoderDescriptor->Copyright =
		 "GPL";
		surroundEncoderDescriptor->PortCount = 6;

		port_descriptors = (LADSPA_PortDescriptor *)calloc(6,
		 sizeof(LADSPA_PortDescriptor));
		surroundEncoderDescriptor->PortDescriptors =
		 (const LADSPA_PortDescriptor *)port_descriptors;

		port_range_hints = (LADSPA_PortRangeHint *)calloc(6,
		 sizeof(LADSPA_PortRangeHint));
		surroundEncoderDescriptor->PortRangeHints =
		 (const LADSPA_PortRangeHint *)port_range_hints;

		port_names = (char **)calloc(6, sizeof(char*));
		surroundEncoderDescriptor->PortNames =
		 (const char **)port_names;

		/* Parameters for L */
		port_descriptors[SURROUNDENCODER_L] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		port_names[SURROUNDENCODER_L] =
		 D_("L");
		port_range_hints[SURROUNDENCODER_L].HintDescriptor = 0;

		/* Parameters for R */
		port_descriptors[SURROUNDENCODER_R] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		port_names[SURROUNDENCODER_R] =
		 D_("R");
		port_range_hints[SURROUNDENCODER_R].HintDescriptor = 0;

		/* Parameters for C */
		port_descriptors[SURROUNDENCODER_C] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		port_names[SURROUNDENCODER_C] =
		 D_("C");
		port_range_hints[SURROUNDENCODER_C].HintDescriptor = 0;

		/* Parameters for S */
		port_descriptors[SURROUNDENCODER_S] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		port_names[SURROUNDENCODER_S] =
		 D_("S");
		port_range_hints[SURROUNDENCODER_S].HintDescriptor = 0;

		/* Parameters for Lt */
		port_descriptors[SURROUNDENCODER_LT] =
		 LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		port_names[SURROUNDENCODER_LT] =
		 D_("Lt");
		port_range_hints[SURROUNDENCODER_LT].HintDescriptor = 0;

		/* Parameters for Rt */
		port_descriptors[SURROUNDENCODER_RT] =
		 LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		port_names[SURROUNDENCODER_RT] =
		 D_("Rt");
		port_range_hints[SURROUNDENCODER_RT].HintDescriptor = 0;

		surroundEncoderDescriptor->activate = activateSurroundEncoder;
		surroundEncoderDescriptor->cleanup = cleanupSurroundEncoder;
		surroundEncoderDescriptor->connect_port = connectPortSurroundEncoder;
		surroundEncoderDescriptor->deactivate = NULL;
		surroundEncoderDescriptor->instantiate = instantiateSurroundEncoder;
		surroundEncoderDescriptor->run = runSurroundEncoder;
		surroundEncoderDescriptor->run_adding = runAddingSurroundEncoder;
		surroundEncoderDescriptor->set_run_adding_gain = setRunAddingGainSurroundEncoder;
	}
}

void _fini() {
	if (surroundEncoderDescriptor) {
		free((LADSPA_PortDescriptor *)surroundEncoderDescriptor->PortDescriptors);
		free((char **)surroundEncoderDescriptor->PortNames);
		free((LADSPA_PortRangeHint *)surroundEncoderDescriptor->PortRangeHints);
		free(surroundEncoderDescriptor);
	}

}
