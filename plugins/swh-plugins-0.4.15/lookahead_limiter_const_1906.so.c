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

#line 10 "lookahead_limiter_const_1906.xml"

#include "ladspa-util.h"
#include "util/db.h"

/* Minimum buffer size in seconds */
#define BUFFER_TIME 0.15f

#define LOOKAHEADLIMITERCONST_LIMIT    0
#define LOOKAHEADLIMITERCONST_DELAY_S  1
#define LOOKAHEADLIMITERCONST_ATTENUATION 2
#define LOOKAHEADLIMITERCONST_IN_1     3
#define LOOKAHEADLIMITERCONST_IN_2     4
#define LOOKAHEADLIMITERCONST_OUT_1    5
#define LOOKAHEADLIMITERCONST_OUT_2    6
#define LOOKAHEADLIMITERCONST_LATENCY  7

static LADSPA_Descriptor *lookaheadLimiterConstDescriptor = NULL;

typedef struct {
	LADSPA_Data *limit;
	LADSPA_Data *delay_s;
	LADSPA_Data *attenuation;
	LADSPA_Data *in_1;
	LADSPA_Data *in_2;
	LADSPA_Data *out_1;
	LADSPA_Data *out_2;
	LADSPA_Data *latency;
	float *      amp_buffer;
	float        atten;
	LADSPA_Data *buffer;
	unsigned int buffer_len;
	unsigned int buffer_mask;
	unsigned int buffer_pos;
	unsigned int fs;
	float        last_delay;
	float        peak;
	unsigned int peak_dist;
	LADSPA_Data run_adding_gain;
} LookaheadLimiterConst;

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
		return lookaheadLimiterConstDescriptor;
	default:
		return NULL;
	}
}

static void activateLookaheadLimiterConst(LADSPA_Handle instance) {
	LookaheadLimiterConst *plugin_data = (LookaheadLimiterConst *)instance;
	float *amp_buffer = plugin_data->amp_buffer;
	float atten = plugin_data->atten;
	LADSPA_Data *buffer = plugin_data->buffer;
	unsigned int buffer_len = plugin_data->buffer_len;
	unsigned int buffer_mask = plugin_data->buffer_mask;
	unsigned int buffer_pos = plugin_data->buffer_pos;
	unsigned int fs = plugin_data->fs;
	float last_delay = plugin_data->last_delay;
	float peak = plugin_data->peak;
	unsigned int peak_dist = plugin_data->peak_dist;
#line 47 "lookahead_limiter_const_1906.xml"
	int i;

	memset(buffer, 0, buffer_len * 2 * sizeof(float));
	for (i=0; i<buffer_len; i++) amp_buffer[i] = 1.0f;

	buffer_pos = 0;
	peak = 0.0f;
	peak_dist = 1;
	atten = 0.0f;
	last_delay = 0.001 * fs;
	plugin_data->amp_buffer = amp_buffer;
	plugin_data->atten = atten;
	plugin_data->buffer = buffer;
	plugin_data->buffer_len = buffer_len;
	plugin_data->buffer_mask = buffer_mask;
	plugin_data->buffer_pos = buffer_pos;
	plugin_data->fs = fs;
	plugin_data->last_delay = last_delay;
	plugin_data->peak = peak;
	plugin_data->peak_dist = peak_dist;

}

static void cleanupLookaheadLimiterConst(LADSPA_Handle instance) {
#line 138 "lookahead_limiter_const_1906.xml"
	LookaheadLimiterConst *plugin_data = (LookaheadLimiterConst *)instance;
	free(plugin_data->buffer);
	free(plugin_data->amp_buffer);
	free(instance);
}

static void connectPortLookaheadLimiterConst(
 LADSPA_Handle instance,
 unsigned long port,
 LADSPA_Data *data) {
	LookaheadLimiterConst *plugin;

	plugin = (LookaheadLimiterConst *)instance;
	switch (port) {
	case LOOKAHEADLIMITERCONST_LIMIT:
		plugin->limit = data;
		break;
	case LOOKAHEADLIMITERCONST_DELAY_S:
		plugin->delay_s = data;
		break;
	case LOOKAHEADLIMITERCONST_ATTENUATION:
		plugin->attenuation = data;
		break;
	case LOOKAHEADLIMITERCONST_IN_1:
		plugin->in_1 = data;
		break;
	case LOOKAHEADLIMITERCONST_IN_2:
		plugin->in_2 = data;
		break;
	case LOOKAHEADLIMITERCONST_OUT_1:
		plugin->out_1 = data;
		break;
	case LOOKAHEADLIMITERCONST_OUT_2:
		plugin->out_2 = data;
		break;
	case LOOKAHEADLIMITERCONST_LATENCY:
		plugin->latency = data;
		break;
	}
}

static LADSPA_Handle instantiateLookaheadLimiterConst(
 const LADSPA_Descriptor *descriptor,
 unsigned long s_rate) {
	LookaheadLimiterConst *plugin_data = (LookaheadLimiterConst *)malloc(sizeof(LookaheadLimiterConst));
	float *amp_buffer = NULL;
	float atten;
	LADSPA_Data *buffer = NULL;
	unsigned int buffer_len;
	unsigned int buffer_mask;
	unsigned int buffer_pos;
	unsigned int fs;
	float last_delay;
	float peak;
	unsigned int peak_dist;

#line 26 "lookahead_limiter_const_1906.xml"
	buffer_len = 4096;
	buffer_pos = 0;
	fs = s_rate;

	db_init();

	/* Find size for power-of-two interleaved delay buffer */
	while(buffer_len < s_rate * BUFFER_TIME) {
	  buffer_len *= 2;
	}
	buffer_mask = buffer_len * 2 - 1;
	buffer = calloc(buffer_len * 2, sizeof(LADSPA_Data));
	amp_buffer = calloc(buffer_len, sizeof(float));

	peak = 0.0f;
	peak_dist = 1;
	atten = 0.0f;
	last_delay = 0.001 * fs;

	plugin_data->amp_buffer = amp_buffer;
	plugin_data->atten = atten;
	plugin_data->buffer = buffer;
	plugin_data->buffer_len = buffer_len;
	plugin_data->buffer_mask = buffer_mask;
	plugin_data->buffer_pos = buffer_pos;
	plugin_data->fs = fs;
	plugin_data->last_delay = last_delay;
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

static void runLookaheadLimiterConst(LADSPA_Handle instance, unsigned long sample_count) {
	LookaheadLimiterConst *plugin_data = (LookaheadLimiterConst *)instance;

	/* Limit (dB) (float value) */
	const LADSPA_Data limit = *(plugin_data->limit);

	/* Lookahead time (s) (float value) */
	const LADSPA_Data delay_s = *(plugin_data->delay_s);

	/* Input 1 (array of floats of length sample_count) */
	const LADSPA_Data * const in_1 = plugin_data->in_1;

	/* Input 2 (array of floats of length sample_count) */
	const LADSPA_Data * const in_2 = plugin_data->in_2;

	/* Output 1 (array of floats of length sample_count) */
	LADSPA_Data * const out_1 = plugin_data->out_1;

	/* Output 2 (array of floats of length sample_count) */
	LADSPA_Data * const out_2 = plugin_data->out_2;
	float * amp_buffer = plugin_data->amp_buffer;
	float atten = plugin_data->atten;
	LADSPA_Data * buffer = plugin_data->buffer;
	unsigned int buffer_len = plugin_data->buffer_len;
	unsigned int buffer_mask = plugin_data->buffer_mask;
	unsigned int buffer_pos = plugin_data->buffer_pos;
	unsigned int fs = plugin_data->fs;
	float last_delay = plugin_data->last_delay;
	float peak = plugin_data->peak;
	unsigned int peak_dist = plugin_data->peak_dist;

#line 60 "lookahead_limiter_const_1906.xml"
	unsigned long pos;
	const float max = DB_CO(limit);
	float sig, gain;
	float delay = last_delay;
	const float delay_delta = (delay_s * fs - last_delay) / (sample_count - 1);
	const unsigned int full_delay = f_round(BUFFER_TIME * fs);
	float a, b;

	for (pos = 0; pos < sample_count; pos++) {
	  delay += delay_delta;
	  buffer[(buffer_pos * 2) & buffer_mask] = in_1[pos];
	  buffer[(buffer_pos * 2 + 1) & buffer_mask] = in_2[pos];

	  a = fabs(buffer[((buffer_pos + f_round(delay)) * 2) & buffer_mask]);
	  b = fabs(buffer[((buffer_pos + f_round(delay)) * 2 + 1) & buffer_mask]);

	  sig = a > b ? a : b;

	  if (sig > max) {
	    sig = lin2db(sig) - limit;
	    if (sig / delay > peak / (float)peak_dist) {
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

	  /* Cacluate the apropriate gain reduction and write it back into the
	   * buffer */
	  gain = 1.0f / db2lin(atten);
	  amp_buffer[(buffer_pos - f_round(delay)) & (buffer_len - 1)] = gain;

	  buffer_write(out_1[pos], buffer[(buffer_pos * 2 - 2) & buffer_mask] *
	          gain);
	  buffer_write(out_2[pos], buffer[(buffer_pos * 2 - 1) & buffer_mask] *
	          gain);

	  /* Ensure that the signal really can't be over the limit,
	   * changes in the lookahead time can cause us to miss peaks */

	  if (out_1[pos] < -max) {
	    buffer_write(out_1[pos], -max);
//printf("c\n");
	  } else if (out_1[pos] > max) {
//printf("c\n");
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
	plugin_data->last_delay = delay;

	*(plugin_data->attenuation) = atten;
	*(plugin_data->latency) = buffer_len;
}
#undef buffer_write
#undef RUN_ADDING
#undef RUN_REPLACING

#define buffer_write(b, v) (b += (v) * run_adding_gain)
#define RUN_ADDING    1
#define RUN_REPLACING 0

static void setRunAddingGainLookaheadLimiterConst(LADSPA_Handle instance, LADSPA_Data gain) {
	((LookaheadLimiterConst *)instance)->run_adding_gain = gain;
}

static void runAddingLookaheadLimiterConst(LADSPA_Handle instance, unsigned long sample_count) {
	LookaheadLimiterConst *plugin_data = (LookaheadLimiterConst *)instance;
	LADSPA_Data run_adding_gain = plugin_data->run_adding_gain;

	/* Limit (dB) (float value) */
	const LADSPA_Data limit = *(plugin_data->limit);

	/* Lookahead time (s) (float value) */
	const LADSPA_Data delay_s = *(plugin_data->delay_s);

	/* Input 1 (array of floats of length sample_count) */
	const LADSPA_Data * const in_1 = plugin_data->in_1;

	/* Input 2 (array of floats of length sample_count) */
	const LADSPA_Data * const in_2 = plugin_data->in_2;

	/* Output 1 (array of floats of length sample_count) */
	LADSPA_Data * const out_1 = plugin_data->out_1;

	/* Output 2 (array of floats of length sample_count) */
	LADSPA_Data * const out_2 = plugin_data->out_2;
	float * amp_buffer = plugin_data->amp_buffer;
	float atten = plugin_data->atten;
	LADSPA_Data * buffer = plugin_data->buffer;
	unsigned int buffer_len = plugin_data->buffer_len;
	unsigned int buffer_mask = plugin_data->buffer_mask;
	unsigned int buffer_pos = plugin_data->buffer_pos;
	unsigned int fs = plugin_data->fs;
	float last_delay = plugin_data->last_delay;
	float peak = plugin_data->peak;
	unsigned int peak_dist = plugin_data->peak_dist;

#line 60 "lookahead_limiter_const_1906.xml"
	unsigned long pos;
	const float max = DB_CO(limit);
	float sig, gain;
	float delay = last_delay;
	const float delay_delta = (delay_s * fs - last_delay) / (sample_count - 1);
	const unsigned int full_delay = f_round(BUFFER_TIME * fs);
	float a, b;

	for (pos = 0; pos < sample_count; pos++) {
	  delay += delay_delta;
	  buffer[(buffer_pos * 2) & buffer_mask] = in_1[pos];
	  buffer[(buffer_pos * 2 + 1) & buffer_mask] = in_2[pos];

	  a = fabs(buffer[((buffer_pos + f_round(delay)) * 2) & buffer_mask]);
	  b = fabs(buffer[((buffer_pos + f_round(delay)) * 2 + 1) & buffer_mask]);

	  sig = a > b ? a : b;

	  if (sig > max) {
	    sig = lin2db(sig) - limit;
	    if (sig / delay > peak / (float)peak_dist) {
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

	  /* Cacluate the apropriate gain reduction and write it back into the
	   * buffer */
	  gain = 1.0f / db2lin(atten);
	  amp_buffer[(buffer_pos - f_round(delay)) & (buffer_len - 1)] = gain;

	  buffer_write(out_1[pos], buffer[(buffer_pos * 2 - 2) & buffer_mask] *
	          gain);
	  buffer_write(out_2[pos], buffer[(buffer_pos * 2 - 1) & buffer_mask] *
	          gain);

	  /* Ensure that the signal really can't be over the limit,
	   * changes in the lookahead time can cause us to miss peaks */

	  if (out_1[pos] < -max) {
	    buffer_write(out_1[pos], -max);
//printf("c\n");
	  } else if (out_1[pos] > max) {
//printf("c\n");
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
	plugin_data->last_delay = delay;

	*(plugin_data->attenuation) = atten;
	*(plugin_data->latency) = buffer_len;
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


	lookaheadLimiterConstDescriptor =
	 (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));

	if (lookaheadLimiterConstDescriptor) {
		lookaheadLimiterConstDescriptor->UniqueID = 1906;
		lookaheadLimiterConstDescriptor->Label = "lookaheadLimiterConst";
		lookaheadLimiterConstDescriptor->Properties =
		 LADSPA_PROPERTY_HARD_RT_CAPABLE;
		lookaheadLimiterConstDescriptor->Name =
		 D_("Lookahead limiter (fixed latency)");
		lookaheadLimiterConstDescriptor->Maker =
		 "Steve Harris <steve@plugin.org.uk>";
		lookaheadLimiterConstDescriptor->Copyright =
		 "GPL";
		lookaheadLimiterConstDescriptor->PortCount = 8;

		port_descriptors = (LADSPA_PortDescriptor *)calloc(8,
		 sizeof(LADSPA_PortDescriptor));
		lookaheadLimiterConstDescriptor->PortDescriptors =
		 (const LADSPA_PortDescriptor *)port_descriptors;

		port_range_hints = (LADSPA_PortRangeHint *)calloc(8,
		 sizeof(LADSPA_PortRangeHint));
		lookaheadLimiterConstDescriptor->PortRangeHints =
		 (const LADSPA_PortRangeHint *)port_range_hints;

		port_names = (char **)calloc(8, sizeof(char*));
		lookaheadLimiterConstDescriptor->PortNames =
		 (const char **)port_names;

		/* Parameters for Limit (dB) */
		port_descriptors[LOOKAHEADLIMITERCONST_LIMIT] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[LOOKAHEADLIMITERCONST_LIMIT] =
		 D_("Limit (dB)");
		port_range_hints[LOOKAHEADLIMITERCONST_LIMIT].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_0;
		port_range_hints[LOOKAHEADLIMITERCONST_LIMIT].LowerBound = -20;
		port_range_hints[LOOKAHEADLIMITERCONST_LIMIT].UpperBound = 0;

		/* Parameters for Lookahead time (s) */
		port_descriptors[LOOKAHEADLIMITERCONST_DELAY_S] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[LOOKAHEADLIMITERCONST_DELAY_S] =
		 D_("Lookahead time (s)");
		port_range_hints[LOOKAHEADLIMITERCONST_DELAY_S].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE;
		port_range_hints[LOOKAHEADLIMITERCONST_DELAY_S].LowerBound = 0.001;
		port_range_hints[LOOKAHEADLIMITERCONST_DELAY_S].UpperBound = 0.15;

		/* Parameters for Attenuation (dB) */
		port_descriptors[LOOKAHEADLIMITERCONST_ATTENUATION] =
		 LADSPA_PORT_OUTPUT | LADSPA_PORT_CONTROL;
		port_names[LOOKAHEADLIMITERCONST_ATTENUATION] =
		 D_("Attenuation (dB)");
		port_range_hints[LOOKAHEADLIMITERCONST_ATTENUATION].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
		port_range_hints[LOOKAHEADLIMITERCONST_ATTENUATION].LowerBound = 0;
		port_range_hints[LOOKAHEADLIMITERCONST_ATTENUATION].UpperBound = 12;

		/* Parameters for Input 1 */
		port_descriptors[LOOKAHEADLIMITERCONST_IN_1] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		port_names[LOOKAHEADLIMITERCONST_IN_1] =
		 D_("Input 1");
		port_range_hints[LOOKAHEADLIMITERCONST_IN_1].HintDescriptor = 0;

		/* Parameters for Input 2 */
		port_descriptors[LOOKAHEADLIMITERCONST_IN_2] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		port_names[LOOKAHEADLIMITERCONST_IN_2] =
		 D_("Input 2");
		port_range_hints[LOOKAHEADLIMITERCONST_IN_2].HintDescriptor = 0;

		/* Parameters for Output 1 */
		port_descriptors[LOOKAHEADLIMITERCONST_OUT_1] =
		 LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		port_names[LOOKAHEADLIMITERCONST_OUT_1] =
		 D_("Output 1");
		port_range_hints[LOOKAHEADLIMITERCONST_OUT_1].HintDescriptor = 0;

		/* Parameters for Output 2 */
		port_descriptors[LOOKAHEADLIMITERCONST_OUT_2] =
		 LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		port_names[LOOKAHEADLIMITERCONST_OUT_2] =
		 D_("Output 2");
		port_range_hints[LOOKAHEADLIMITERCONST_OUT_2].HintDescriptor = 0;

		/* Parameters for latency */
		port_descriptors[LOOKAHEADLIMITERCONST_LATENCY] =
		 LADSPA_PORT_OUTPUT | LADSPA_PORT_CONTROL;
		port_names[LOOKAHEADLIMITERCONST_LATENCY] =
		 D_("latency");
		port_range_hints[LOOKAHEADLIMITERCONST_LATENCY].HintDescriptor = 0;

		lookaheadLimiterConstDescriptor->activate = activateLookaheadLimiterConst;
		lookaheadLimiterConstDescriptor->cleanup = cleanupLookaheadLimiterConst;
		lookaheadLimiterConstDescriptor->connect_port = connectPortLookaheadLimiterConst;
		lookaheadLimiterConstDescriptor->deactivate = NULL;
		lookaheadLimiterConstDescriptor->instantiate = instantiateLookaheadLimiterConst;
		lookaheadLimiterConstDescriptor->run = runLookaheadLimiterConst;
		lookaheadLimiterConstDescriptor->run_adding = runAddingLookaheadLimiterConst;
		lookaheadLimiterConstDescriptor->set_run_adding_gain = setRunAddingGainLookaheadLimiterConst;
	}
}

void _fini() {
	if (lookaheadLimiterConstDescriptor) {
		free((LADSPA_PortDescriptor *)lookaheadLimiterConstDescriptor->PortDescriptors);
		free((char **)lookaheadLimiterConstDescriptor->PortNames);
		free((LADSPA_PortRangeHint *)lookaheadLimiterConstDescriptor->PortRangeHints);
		free(lookaheadLimiterConstDescriptor);
	}

}
