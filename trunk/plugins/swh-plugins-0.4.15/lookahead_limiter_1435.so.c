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

#line 10 "lookahead_limiter_1435.xml"

#include "ladspa-util.h"
#include "util/db.h"

/* Minimum buffer size in seconds */
#define BUFFER_TIME 2

#define LOOKAHEADLIMITER_LIMIT         0
#define LOOKAHEADLIMITER_DELAY_S       1
#define LOOKAHEADLIMITER_ATTENUATION   2
#define LOOKAHEADLIMITER_IN_1          3
#define LOOKAHEADLIMITER_IN_2          4
#define LOOKAHEADLIMITER_OUT_1         5
#define LOOKAHEADLIMITER_OUT_2         6
#define LOOKAHEADLIMITER_LATENCY       7

static LADSPA_Descriptor *lookaheadLimiterDescriptor = NULL;

typedef struct {
	LADSPA_Data *limit;
	LADSPA_Data *delay_s;
	LADSPA_Data *attenuation;
	LADSPA_Data *in_1;
	LADSPA_Data *in_2;
	LADSPA_Data *out_1;
	LADSPA_Data *out_2;
	LADSPA_Data *latency;
	float        atten;
	LADSPA_Data *buffer;
	unsigned int buffer_len;
	unsigned int buffer_pos;
	unsigned int fs;
	float        peak;
	unsigned int peak_dist;
	LADSPA_Data run_adding_gain;
} LookaheadLimiter;

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
		return lookaheadLimiterDescriptor;
	default:
		return NULL;
	}
}

static void activateLookaheadLimiter(LADSPA_Handle instance) {
	LookaheadLimiter *plugin_data = (LookaheadLimiter *)instance;
	float atten = plugin_data->atten;
	LADSPA_Data *buffer = plugin_data->buffer;
	unsigned int buffer_len = plugin_data->buffer_len;
	unsigned int buffer_pos = plugin_data->buffer_pos;
	unsigned int fs = plugin_data->fs;
	float peak = plugin_data->peak;
	unsigned int peak_dist = plugin_data->peak_dist;
#line 41 "lookahead_limiter_1435.xml"
	memset(buffer, 0, buffer_len * sizeof(float));

	peak = 0.0f;
	peak_dist = 1;
	atten = 0.0f;
	plugin_data->atten = atten;
	plugin_data->buffer = buffer;
	plugin_data->buffer_len = buffer_len;
	plugin_data->buffer_pos = buffer_pos;
	plugin_data->fs = fs;
	plugin_data->peak = peak;
	plugin_data->peak_dist = peak_dist;

}

static void cleanupLookaheadLimiter(LADSPA_Handle instance) {
#line 114 "lookahead_limiter_1435.xml"
	LookaheadLimiter *plugin_data = (LookaheadLimiter *)instance;
	free(plugin_data->buffer);
	free(instance);
}

static void connectPortLookaheadLimiter(
 LADSPA_Handle instance,
 unsigned long port,
 LADSPA_Data *data) {
	LookaheadLimiter *plugin;

	plugin = (LookaheadLimiter *)instance;
	switch (port) {
	case LOOKAHEADLIMITER_LIMIT:
		plugin->limit = data;
		break;
	case LOOKAHEADLIMITER_DELAY_S:
		plugin->delay_s = data;
		break;
	case LOOKAHEADLIMITER_ATTENUATION:
		plugin->attenuation = data;
		break;
	case LOOKAHEADLIMITER_IN_1:
		plugin->in_1 = data;
		break;
	case LOOKAHEADLIMITER_IN_2:
		plugin->in_2 = data;
		break;
	case LOOKAHEADLIMITER_OUT_1:
		plugin->out_1 = data;
		break;
	case LOOKAHEADLIMITER_OUT_2:
		plugin->out_2 = data;
		break;
	case LOOKAHEADLIMITER_LATENCY:
		plugin->latency = data;
		break;
	}
}

static LADSPA_Handle instantiateLookaheadLimiter(
 const LADSPA_Descriptor *descriptor,
 unsigned long s_rate) {
	LookaheadLimiter *plugin_data = (LookaheadLimiter *)malloc(sizeof(LookaheadLimiter));
	float atten;
	LADSPA_Data *buffer = NULL;
	unsigned int buffer_len;
	unsigned int buffer_pos;
	unsigned int fs;
	float peak;
	unsigned int peak_dist;

#line 23 "lookahead_limiter_1435.xml"
	buffer_len = 16384;
	buffer_pos = 0;
	fs = s_rate;

	db_init();

	/* Find size for power-of-two interleaved delay buffer */
	while(buffer_len < s_rate * BUFFER_TIME * 2) {
	  buffer_len *= 2;
	}
	buffer = calloc(buffer_len, sizeof(float));

	peak = 0.0f;
	peak_dist = 1;
	atten = 0.0f;

	plugin_data->atten = atten;
	plugin_data->buffer = buffer;
	plugin_data->buffer_len = buffer_len;
	plugin_data->buffer_pos = buffer_pos;
	plugin_data->fs = fs;
	plugin_data->peak = peak;
	plugin_data->peak_dist = peak_dist;

	return (LADSPA_Handle)plugin_data;
}

#undef buffer_write
#undef RUN_ADDING
#undef RUN_REPLACING

#define buffer_write(b, v) (b = v)
#define RUN_ADDING    0
#define RUN_REPLACING 1

static void runLookaheadLimiter(LADSPA_Handle instance, unsigned long sample_count) {
	LookaheadLimiter *plugin_data = (LookaheadLimiter *)instance;

	/* Limit (dB) (float value) */
	const LADSPA_Data limit = *(plugin_data->limit);

	/* Lookahead delay (float value) */
	const LADSPA_Data delay_s = *(plugin_data->delay_s);

	/* Input 1 (array of floats of length sample_count) */
	const LADSPA_Data * const in_1 = plugin_data->in_1;

	/* Input 2 (array of floats of length sample_count) */
	const LADSPA_Data * const in_2 = plugin_data->in_2;

	/* Output 1 (array of floats of length sample_count) */
	LADSPA_Data * const out_1 = plugin_data->out_1;

	/* Output 2 (array of floats of length sample_count) */
	LADSPA_Data * const out_2 = plugin_data->out_2;
	float atten = plugin_data->atten;
	LADSPA_Data * buffer = plugin_data->buffer;
	unsigned int buffer_len = plugin_data->buffer_len;
	unsigned int buffer_pos = plugin_data->buffer_pos;
	unsigned int fs = plugin_data->fs;
	float peak = plugin_data->peak;
	unsigned int peak_dist = plugin_data->peak_dist;

#line 49 "lookahead_limiter_1435.xml"
	unsigned long pos;
	const float max = DB_CO(limit);
	float sig, gain;
	const unsigned int delay = delay_s * fs;

	for (pos = 0; pos < sample_count; pos++) {
	  buffer[(buffer_pos * 2) & (buffer_len - 1)] = in_1[pos];
	  buffer[(buffer_pos * 2 + 1) & (buffer_len - 1)] = in_2[pos];

	  sig = fabs(in_1[pos]) > fabs(in_2[pos]) ? fabs(in_1[pos]) :
	          fabs(in_2[pos]);

	  if (sig > max) {
	    sig = lin2db(sig) - limit;
	    if (sig / (float)delay > peak / (float)peak_dist) {
	      peak_dist = delay;
	      peak = sig;
	    }
	  }
	  if (sig > 0.0f && sig / (float)delay > peak / (float)peak_dist) {
	    peak_dist = delay;
	    peak = sig;
	  }

	  /* Incremenatlly approach the correct attenuation for the next peak */
	  atten -= (atten - peak) / (float)(peak_dist + 1);

	  if (peak_dist-- == 0) {
	          peak_dist = delay;
	          peak = 0.0f;
	  }

	  gain = 1.0f / db2lin(atten);
	  buffer_write(out_1[pos], buffer[(buffer_pos * 2 - delay * 2) &
	                                  (buffer_len - 1)] * gain);
	  buffer_write(out_2[pos], buffer[(buffer_pos * 2 - delay * 2 + 1) &
	                                  (buffer_len - 1)] * gain);

	  /* Ensure that the signal really can't be over the limit, potentially
	   * changes in the lookahead time could cause us to miss peaks */

	  if (out_1[pos] < -max) {
	    buffer_write(out_1[pos], -max);
	  } else if (out_1[pos] > max) {
	    buffer_write(out_1[pos], max);
	  }
	  if (out_2[pos] < -max) {
	    buffer_write(out_2[pos], -max);
	  } else if (out_2[pos] > max) {
	    buffer_write(out_2[pos], max);
	  }

	  buffer_pos++;
	}

	plugin_data->buffer_pos = buffer_pos;
	plugin_data->peak = peak;
	plugin_data->peak_dist = peak_dist;
	plugin_data->atten = atten;

	*(plugin_data->attenuation) = atten;
	*(plugin_data->latency) = delay;
}
#undef buffer_write
#undef RUN_ADDING
#undef RUN_REPLACING

#define buffer_write(b, v) (b += (v) * run_adding_gain)
#define RUN_ADDING    1
#define RUN_REPLACING 0

static void setRunAddingGainLookaheadLimiter(LADSPA_Handle instance, LADSPA_Data gain) {
	((LookaheadLimiter *)instance)->run_adding_gain = gain;
}

static void runAddingLookaheadLimiter(LADSPA_Handle instance, unsigned long sample_count) {
	LookaheadLimiter *plugin_data = (LookaheadLimiter *)instance;
	LADSPA_Data run_adding_gain = plugin_data->run_adding_gain;

	/* Limit (dB) (float value) */
	const LADSPA_Data limit = *(plugin_data->limit);

	/* Lookahead delay (float value) */
	const LADSPA_Data delay_s = *(plugin_data->delay_s);

	/* Input 1 (array of floats of length sample_count) */
	const LADSPA_Data * const in_1 = plugin_data->in_1;

	/* Input 2 (array of floats of length sample_count) */
	const LADSPA_Data * const in_2 = plugin_data->in_2;

	/* Output 1 (array of floats of length sample_count) */
	LADSPA_Data * const out_1 = plugin_data->out_1;

	/* Output 2 (array of floats of length sample_count) */
	LADSPA_Data * const out_2 = plugin_data->out_2;
	float atten = plugin_data->atten;
	LADSPA_Data * buffer = plugin_data->buffer;
	unsigned int buffer_len = plugin_data->buffer_len;
	unsigned int buffer_pos = plugin_data->buffer_pos;
	unsigned int fs = plugin_data->fs;
	float peak = plugin_data->peak;
	unsigned int peak_dist = plugin_data->peak_dist;

#line 49 "lookahead_limiter_1435.xml"
	unsigned long pos;
	const float max = DB_CO(limit);
	float sig, gain;
	const unsigned int delay = delay_s * fs;

	for (pos = 0; pos < sample_count; pos++) {
	  buffer[(buffer_pos * 2) & (buffer_len - 1)] = in_1[pos];
	  buffer[(buffer_pos * 2 + 1) & (buffer_len - 1)] = in_2[pos];

	  sig = fabs(in_1[pos]) > fabs(in_2[pos]) ? fabs(in_1[pos]) :
	          fabs(in_2[pos]);

	  if (sig > max) {
	    sig = lin2db(sig) - limit;
	    if (sig / (float)delay > peak / (float)peak_dist) {
	      peak_dist = delay;
	      peak = sig;
	    }
	  }
	  if (sig > 0.0f && sig / (float)delay > peak / (float)peak_dist) {
	    peak_dist = delay;
	    peak = sig;
	  }

	  /* Incremenatlly approach the correct attenuation for the next peak */
	  atten -= (atten - peak) / (float)(peak_dist + 1);

	  if (peak_dist-- == 0) {
	          peak_dist = delay;
	          peak = 0.0f;
	  }

	  gain = 1.0f / db2lin(atten);
	  buffer_write(out_1[pos], buffer[(buffer_pos * 2 - delay * 2) &
	                                  (buffer_len - 1)] * gain);
	  buffer_write(out_2[pos], buffer[(buffer_pos * 2 - delay * 2 + 1) &
	                                  (buffer_len - 1)] * gain);

	  /* Ensure that the signal really can't be over the limit, potentially
	   * changes in the lookahead time could cause us to miss peaks */

	  if (out_1[pos] < -max) {
	    buffer_write(out_1[pos], -max);
	  } else if (out_1[pos] > max) {
	    buffer_write(out_1[pos], max);
	  }
	  if (out_2[pos] < -max) {
	    buffer_write(out_2[pos], -max);
	  } else if (out_2[pos] > max) {
	    buffer_write(out_2[pos], max);
	  }

	  buffer_pos++;
	}

	plugin_data->buffer_pos = buffer_pos;
	plugin_data->peak = peak;
	plugin_data->peak_dist = peak_dist;
	plugin_data->atten = atten;

	*(plugin_data->attenuation) = atten;
	*(plugin_data->latency) = delay;
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


	lookaheadLimiterDescriptor =
	 (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));

	if (lookaheadLimiterDescriptor) {
		lookaheadLimiterDescriptor->UniqueID = 1435;
		lookaheadLimiterDescriptor->Label = "lookaheadLimiter";
		lookaheadLimiterDescriptor->Properties =
		 LADSPA_PROPERTY_HARD_RT_CAPABLE;
		lookaheadLimiterDescriptor->Name =
		 D_("Lookahead limiter");
		lookaheadLimiterDescriptor->Maker =
		 "Steve Harris <steve@plugin.org.uk>";
		lookaheadLimiterDescriptor->Copyright =
		 "GPL";
		lookaheadLimiterDescriptor->PortCount = 8;

		port_descriptors = (LADSPA_PortDescriptor *)calloc(8,
		 sizeof(LADSPA_PortDescriptor));
		lookaheadLimiterDescriptor->PortDescriptors =
		 (const LADSPA_PortDescriptor *)port_descriptors;

		port_range_hints = (LADSPA_PortRangeHint *)calloc(8,
		 sizeof(LADSPA_PortRangeHint));
		lookaheadLimiterDescriptor->PortRangeHints =
		 (const LADSPA_PortRangeHint *)port_range_hints;

		port_names = (char **)calloc(8, sizeof(char*));
		lookaheadLimiterDescriptor->PortNames =
		 (const char **)port_names;

		/* Parameters for Limit (dB) */
		port_descriptors[LOOKAHEADLIMITER_LIMIT] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[LOOKAHEADLIMITER_LIMIT] =
		 D_("Limit (dB)");
		port_range_hints[LOOKAHEADLIMITER_LIMIT].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_0;
		port_range_hints[LOOKAHEADLIMITER_LIMIT].LowerBound = -20;
		port_range_hints[LOOKAHEADLIMITER_LIMIT].UpperBound = 0;

		/* Parameters for Lookahead delay */
		port_descriptors[LOOKAHEADLIMITER_DELAY_S] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[LOOKAHEADLIMITER_DELAY_S] =
		 D_("Lookahead delay");
		port_range_hints[LOOKAHEADLIMITER_DELAY_S].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE;
		port_range_hints[LOOKAHEADLIMITER_DELAY_S].LowerBound = 0.001;
		port_range_hints[LOOKAHEADLIMITER_DELAY_S].UpperBound = 2.0;

		/* Parameters for Attenuation (dB) */
		port_descriptors[LOOKAHEADLIMITER_ATTENUATION] =
		 LADSPA_PORT_OUTPUT | LADSPA_PORT_CONTROL;
		port_names[LOOKAHEADLIMITER_ATTENUATION] =
		 D_("Attenuation (dB)");
		port_range_hints[LOOKAHEADLIMITER_ATTENUATION].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
		port_range_hints[LOOKAHEADLIMITER_ATTENUATION].LowerBound = 0;
		port_range_hints[LOOKAHEADLIMITER_ATTENUATION].UpperBound = 12;

		/* Parameters for Input 1 */
		port_descriptors[LOOKAHEADLIMITER_IN_1] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		port_names[LOOKAHEADLIMITER_IN_1] =
		 D_("Input 1");
		port_range_hints[LOOKAHEADLIMITER_IN_1].HintDescriptor = 0;

		/* Parameters for Input 2 */
		port_descriptors[LOOKAHEADLIMITER_IN_2] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		port_names[LOOKAHEADLIMITER_IN_2] =
		 D_("Input 2");
		port_range_hints[LOOKAHEADLIMITER_IN_2].HintDescriptor = 0;

		/* Parameters for Output 1 */
		port_descriptors[LOOKAHEADLIMITER_OUT_1] =
		 LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		port_names[LOOKAHEADLIMITER_OUT_1] =
		 D_("Output 1");
		port_range_hints[LOOKAHEADLIMITER_OUT_1].HintDescriptor = 0;

		/* Parameters for Output 2 */
		port_descriptors[LOOKAHEADLIMITER_OUT_2] =
		 LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		port_names[LOOKAHEADLIMITER_OUT_2] =
		 D_("Output 2");
		port_range_hints[LOOKAHEADLIMITER_OUT_2].HintDescriptor = 0;

		/* Parameters for latency */
		port_descriptors[LOOKAHEADLIMITER_LATENCY] =
		 LADSPA_PORT_OUTPUT | LADSPA_PORT_CONTROL;
		port_names[LOOKAHEADLIMITER_LATENCY] =
		 D_("latency");
		port_range_hints[LOOKAHEADLIMITER_LATENCY].HintDescriptor = 0;

		lookaheadLimiterDescriptor->activate = activateLookaheadLimiter;
		lookaheadLimiterDescriptor->cleanup = cleanupLookaheadLimiter;
		lookaheadLimiterDescriptor->connect_port = connectPortLookaheadLimiter;
		lookaheadLimiterDescriptor->deactivate = NULL;
		lookaheadLimiterDescriptor->instantiate = instantiateLookaheadLimiter;
		lookaheadLimiterDescriptor->run = runLookaheadLimiter;
		lookaheadLimiterDescriptor->run_adding = runAddingLookaheadLimiter;
		lookaheadLimiterDescriptor->set_run_adding_gain = setRunAddingGainLookaheadLimiter;
	}
}

void _fini() {
	if (lookaheadLimiterDescriptor) {
		free((LADSPA_PortDescriptor *)lookaheadLimiterDescriptor->PortDescriptors);
		free((char **)lookaheadLimiterDescriptor->PortNames);
		free((LADSPA_PortRangeHint *)lookaheadLimiterDescriptor->PortRangeHints);
		free(lookaheadLimiterDescriptor);
	}

}
