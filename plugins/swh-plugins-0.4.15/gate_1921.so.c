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

#line 10 "gate_1921.xml"

#include "ladspa-util.h"
#include "util/biquad.h"

#define ENV_TR 0.0001f

#define CLOSED  1
#define OPENING 2
#define OPEN    3
#define CLOSING 4

#define GATE_LF_FC                     0
#define GATE_HF_FC                     1
#define GATE_LEVEL                     2
#define GATE_THRESHOLD                 3
#define GATE_ATTACK                    4
#define GATE_HOLD                      5
#define GATE_DECAY                     6
#define GATE_RANGE                     7
#define GATE_SELECT                    8
#define GATE_INPUT                     9
#define GATE_OUTPUT                    10
#define STEREO_GATE_LF_FC              0
#define STEREO_GATE_HF_FC              1
#define STEREO_GATE_LEVEL              2
#define STEREO_GATE_THRESHOLD          3
#define STEREO_GATE_ATTACK             4
#define STEREO_GATE_HOLD               5
#define STEREO_GATE_DECAY              6
#define STEREO_GATE_RANGE              7
#define STEREO_GATE_SELECT             8
#define STEREO_GATE_IN1                9
#define STEREO_GATE_IN2                10
#define STEREO_GATE_OUT1               11
#define STEREO_GATE_OUT2               12

static LADSPA_Descriptor *gateDescriptor = NULL;

typedef struct {
	LADSPA_Data *lf_fc;
	LADSPA_Data *hf_fc;
	LADSPA_Data *level;
	LADSPA_Data *threshold;
	LADSPA_Data *attack;
	LADSPA_Data *hold;
	LADSPA_Data *decay;
	LADSPA_Data *range;
	LADSPA_Data *select;
	LADSPA_Data *input;
	LADSPA_Data *output;
	float        env;
	float        fs;
	float        gate;
	biquad *     hf;
	int          hold_count;
	biquad *     lf;
	int          state;
	LADSPA_Data run_adding_gain;
} Gate;

static LADSPA_Descriptor *stereo_gateDescriptor = NULL;

typedef struct {
	LADSPA_Data *lf_fc;
	LADSPA_Data *hf_fc;
	LADSPA_Data *level;
	LADSPA_Data *threshold;
	LADSPA_Data *attack;
	LADSPA_Data *hold;
	LADSPA_Data *decay;
	LADSPA_Data *range;
	LADSPA_Data *select;
	LADSPA_Data *in1;
	LADSPA_Data *in2;
	LADSPA_Data *out1;
	LADSPA_Data *out2;
	float        env;
	float        fs;
	float        gate;
	biquad *     hf;
	int          hold_count;
	biquad *     lf;
	int          state;
	LADSPA_Data run_adding_gain;
} Stereo_gate;

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
		return gateDescriptor;
	case 1:
		return stereo_gateDescriptor;
	default:
		return NULL;
	}
}

static void activateGate(LADSPA_Handle instance) {
	Gate *plugin_data = (Gate *)instance;
	float env = plugin_data->env;
	float fs = plugin_data->fs;
	float gate = plugin_data->gate;
	biquad *hf = plugin_data->hf;
	int hold_count = plugin_data->hold_count;
	biquad *lf = plugin_data->lf;
	int state = plugin_data->state;
#line 41 "gate_1921.xml"
	env = 0.0f;
	gate = 0.0f;
	state = CLOSED;
	biquad_init(lf);
	biquad_init(hf);
	plugin_data->env = env;
	plugin_data->fs = fs;
	plugin_data->gate = gate;
	plugin_data->hf = hf;
	plugin_data->hold_count = hold_count;
	plugin_data->lf = lf;
	plugin_data->state = state;

}

static void cleanupGate(LADSPA_Handle instance) {
	free(instance);
}

static void connectPortGate(
 LADSPA_Handle instance,
 unsigned long port,
 LADSPA_Data *data) {
	Gate *plugin;

	plugin = (Gate *)instance;
	switch (port) {
	case GATE_LF_FC:
		plugin->lf_fc = data;
		break;
	case GATE_HF_FC:
		plugin->hf_fc = data;
		break;
	case GATE_LEVEL:
		plugin->level = data;
		break;
	case GATE_THRESHOLD:
		plugin->threshold = data;
		break;
	case GATE_ATTACK:
		plugin->attack = data;
		break;
	case GATE_HOLD:
		plugin->hold = data;
		break;
	case GATE_DECAY:
		plugin->decay = data;
		break;
	case GATE_RANGE:
		plugin->range = data;
		break;
	case GATE_SELECT:
		plugin->select = data;
		break;
	case GATE_INPUT:
		plugin->input = data;
		break;
	case GATE_OUTPUT:
		plugin->output = data;
		break;
	}
}

static LADSPA_Handle instantiateGate(
 const LADSPA_Descriptor *descriptor,
 unsigned long s_rate) {
	Gate *plugin_data = (Gate *)malloc(sizeof(Gate));
	float env;
	float fs;
	float gate;
	biquad *hf = NULL;
	int hold_count;
	biquad *lf = NULL;
	int state;

#line 28 "gate_1921.xml"
	fs = s_rate;
	env = 0.0f;
	gate = 0.0f;
	state = CLOSED;
	hold_count = 0;

	lf = malloc(sizeof(biquad));
	hf = malloc(sizeof(biquad));
	biquad_init(lf);
	biquad_init(hf);

	plugin_data->env = env;
	plugin_data->fs = fs;
	plugin_data->gate = gate;
	plugin_data->hf = hf;
	plugin_data->hold_count = hold_count;
	plugin_data->lf = lf;
	plugin_data->state = state;

	return (LADSPA_Handle)plugin_data;
}

#undef buffer_write
#undef RUN_ADDING
#undef RUN_REPLACING

#define buffer_write(b, v) (b = v)
#define RUN_ADDING    0
#define RUN_REPLACING 1

static void runGate(LADSPA_Handle instance, unsigned long sample_count) {
	Gate *plugin_data = (Gate *)instance;

	/* LF key filter (Hz) (float value) */
	const LADSPA_Data lf_fc = *(plugin_data->lf_fc);

	/* HF key filter (Hz) (float value) */
	const LADSPA_Data hf_fc = *(plugin_data->hf_fc);

	/* Threshold (dB) (float value) */
	const LADSPA_Data threshold = *(plugin_data->threshold);

	/* Attack (ms) (float value) */
	const LADSPA_Data attack = *(plugin_data->attack);

	/* Hold (ms) (float value) */
	const LADSPA_Data hold = *(plugin_data->hold);

	/* Decay (ms) (float value) */
	const LADSPA_Data decay = *(plugin_data->decay);

	/* Range (dB) (float value) */
	const LADSPA_Data range = *(plugin_data->range);

	/* Output select (-1 = key listen, 0 = gate, 1 = bypass) (float value) */
	const LADSPA_Data select = *(plugin_data->select);

	/* Input (array of floats of length sample_count) */
	const LADSPA_Data * const input = plugin_data->input;

	/* Output (array of floats of length sample_count) */
	LADSPA_Data * const output = plugin_data->output;
	float env = plugin_data->env;
	float fs = plugin_data->fs;
	float gate = plugin_data->gate;
	biquad * hf = plugin_data->hf;
	int hold_count = plugin_data->hold_count;
	biquad * lf = plugin_data->lf;
	int state = plugin_data->state;

#line 49 "gate_1921.xml"
	unsigned long pos;
	float cut = DB_CO(range);
	float t_level = DB_CO(threshold);
	float a_rate = 1000.0f / (attack * fs);
	float d_rate = 1000.0f / (decay * fs);
	float post_filter, apost_filter;
	int op = f_round(select);

	ls_set_params(lf, lf_fc, -40.0f, 0.6f, fs);
	hs_set_params(hf, hf_fc, -50.0f, 0.6f, fs);

	for (pos = 0; pos < sample_count; pos++) {
	  post_filter = biquad_run(lf, input[pos]);
	  post_filter = biquad_run(hf, post_filter);
	  apost_filter = fabs(post_filter);

	  if (apost_filter > env) {
	    env = apost_filter;
	  } else {
	    env = apost_filter * ENV_TR + env * (1.0f - ENV_TR);
	  }

	  if (state == CLOSED) {
	    if (env >= t_level) {
	      state = OPENING;
	    }
	  } else if (state == OPENING) {
	    gate += a_rate;
	    if (gate >= 1.0f) {
	      gate = 1.0f;
	      state = OPEN;
	      hold_count = f_round(hold * fs * 0.001f);
	      plugin_data->hold_count = hold_count;
	    }
	  } else if (state == OPEN) {
	    if (hold_count <= 0) {
	      if (env < t_level) {
	        state = CLOSING;
	      }
	    } else {
	      hold_count--;
	    }
	  } else if (state == CLOSING) {
	    gate -= d_rate;
	    if (env >= t_level) {
	      state = OPENING;
	    } else if (gate <= 0.0f) {
	      gate = 0.0f;
	      state = CLOSED;
	    }
	  }

	  if (op == 0) {
	    buffer_write(output[pos], input[pos] * (cut * (1.0f - gate) + gate));
	  } else if (op == -1) {
	    buffer_write(output[pos], post_filter);
	  } else {
	    buffer_write(output[pos], input[pos]);
	  }
	}
	*(plugin_data->level) = CO_DB(env);

	plugin_data->env = env;
	plugin_data->gate = gate;
	plugin_data->state = state;
	plugin_data->hold_count = hold_count;
}
#undef buffer_write
#undef RUN_ADDING
#undef RUN_REPLACING

#define buffer_write(b, v) (b += (v) * run_adding_gain)
#define RUN_ADDING    1
#define RUN_REPLACING 0

static void setRunAddingGainGate(LADSPA_Handle instance, LADSPA_Data gain) {
	((Gate *)instance)->run_adding_gain = gain;
}

static void runAddingGate(LADSPA_Handle instance, unsigned long sample_count) {
	Gate *plugin_data = (Gate *)instance;
	LADSPA_Data run_adding_gain = plugin_data->run_adding_gain;

	/* LF key filter (Hz) (float value) */
	const LADSPA_Data lf_fc = *(plugin_data->lf_fc);

	/* HF key filter (Hz) (float value) */
	const LADSPA_Data hf_fc = *(plugin_data->hf_fc);

	/* Threshold (dB) (float value) */
	const LADSPA_Data threshold = *(plugin_data->threshold);

	/* Attack (ms) (float value) */
	const LADSPA_Data attack = *(plugin_data->attack);

	/* Hold (ms) (float value) */
	const LADSPA_Data hold = *(plugin_data->hold);

	/* Decay (ms) (float value) */
	const LADSPA_Data decay = *(plugin_data->decay);

	/* Range (dB) (float value) */
	const LADSPA_Data range = *(plugin_data->range);

	/* Output select (-1 = key listen, 0 = gate, 1 = bypass) (float value) */
	const LADSPA_Data select = *(plugin_data->select);

	/* Input (array of floats of length sample_count) */
	const LADSPA_Data * const input = plugin_data->input;

	/* Output (array of floats of length sample_count) */
	LADSPA_Data * const output = plugin_data->output;
	float env = plugin_data->env;
	float fs = plugin_data->fs;
	float gate = plugin_data->gate;
	biquad * hf = plugin_data->hf;
	int hold_count = plugin_data->hold_count;
	biquad * lf = plugin_data->lf;
	int state = plugin_data->state;

#line 49 "gate_1921.xml"
	unsigned long pos;
	float cut = DB_CO(range);
	float t_level = DB_CO(threshold);
	float a_rate = 1000.0f / (attack * fs);
	float d_rate = 1000.0f / (decay * fs);
	float post_filter, apost_filter;
	int op = f_round(select);

	ls_set_params(lf, lf_fc, -40.0f, 0.6f, fs);
	hs_set_params(hf, hf_fc, -50.0f, 0.6f, fs);

	for (pos = 0; pos < sample_count; pos++) {
	  post_filter = biquad_run(lf, input[pos]);
	  post_filter = biquad_run(hf, post_filter);
	  apost_filter = fabs(post_filter);

	  if (apost_filter > env) {
	    env = apost_filter;
	  } else {
	    env = apost_filter * ENV_TR + env * (1.0f - ENV_TR);
	  }

	  if (state == CLOSED) {
	    if (env >= t_level) {
	      state = OPENING;
	    }
	  } else if (state == OPENING) {
	    gate += a_rate;
	    if (gate >= 1.0f) {
	      gate = 1.0f;
	      state = OPEN;
	      hold_count = f_round(hold * fs * 0.001f);
	      plugin_data->hold_count = hold_count;
	    }
	  } else if (state == OPEN) {
	    if (hold_count <= 0) {
	      if (env < t_level) {
	        state = CLOSING;
	      }
	    } else {
	      hold_count--;
	    }
	  } else if (state == CLOSING) {
	    gate -= d_rate;
	    if (env >= t_level) {
	      state = OPENING;
	    } else if (gate <= 0.0f) {
	      gate = 0.0f;
	      state = CLOSED;
	    }
	  }

	  if (op == 0) {
	    buffer_write(output[pos], input[pos] * (cut * (1.0f - gate) + gate));
	  } else if (op == -1) {
	    buffer_write(output[pos], post_filter);
	  } else {
	    buffer_write(output[pos], input[pos]);
	  }
	}
	*(plugin_data->level) = CO_DB(env);

	plugin_data->env = env;
	plugin_data->gate = gate;
	plugin_data->state = state;
	plugin_data->hold_count = hold_count;
}

static void activateStereo_gate(LADSPA_Handle instance) {
	Stereo_gate *plugin_data = (Stereo_gate *)instance;
	float env = plugin_data->env;
	float fs = plugin_data->fs;
	float gate = plugin_data->gate;
	biquad *hf = plugin_data->hf;
	int hold_count = plugin_data->hold_count;
	biquad *lf = plugin_data->lf;
	int state = plugin_data->state;
#line 41 "gate_1921.xml"
	env = 0.0f;
	gate = 0.0f;
	state = CLOSED;
	biquad_init(lf);
	biquad_init(hf);
	plugin_data->env = env;
	plugin_data->fs = fs;
	plugin_data->gate = gate;
	plugin_data->hf = hf;
	plugin_data->hold_count = hold_count;
	plugin_data->lf = lf;
	plugin_data->state = state;

}

static void cleanupStereo_gate(LADSPA_Handle instance) {
	free(instance);
}

static void connectPortStereo_gate(
 LADSPA_Handle instance,
 unsigned long port,
 LADSPA_Data *data) {
	Stereo_gate *plugin;

	plugin = (Stereo_gate *)instance;
	switch (port) {
	case STEREO_GATE_LF_FC:
		plugin->lf_fc = data;
		break;
	case STEREO_GATE_HF_FC:
		plugin->hf_fc = data;
		break;
	case STEREO_GATE_LEVEL:
		plugin->level = data;
		break;
	case STEREO_GATE_THRESHOLD:
		plugin->threshold = data;
		break;
	case STEREO_GATE_ATTACK:
		plugin->attack = data;
		break;
	case STEREO_GATE_HOLD:
		plugin->hold = data;
		break;
	case STEREO_GATE_DECAY:
		plugin->decay = data;
		break;
	case STEREO_GATE_RANGE:
		plugin->range = data;
		break;
	case STEREO_GATE_SELECT:
		plugin->select = data;
		break;
	case STEREO_GATE_IN1:
		plugin->in1 = data;
		break;
	case STEREO_GATE_IN2:
		plugin->in2 = data;
		break;
	case STEREO_GATE_OUT1:
		plugin->out1 = data;
		break;
	case STEREO_GATE_OUT2:
		plugin->out2 = data;
		break;
	}
}

static LADSPA_Handle instantiateStereo_gate(
 const LADSPA_Descriptor *descriptor,
 unsigned long s_rate) {
	Stereo_gate *plugin_data = (Stereo_gate *)malloc(sizeof(Stereo_gate));
	float env;
	float fs;
	float gate;
	biquad *hf = NULL;
	int hold_count;
	biquad *lf = NULL;
	int state;

#line 28 "gate_1921.xml"
	fs = s_rate;
	env = 0.0f;
	gate = 0.0f;
	state = CLOSED;
	hold_count = 0;

	lf = malloc(sizeof(biquad));
	hf = malloc(sizeof(biquad));
	biquad_init(lf);
	biquad_init(hf);

	plugin_data->env = env;
	plugin_data->fs = fs;
	plugin_data->gate = gate;
	plugin_data->hf = hf;
	plugin_data->hold_count = hold_count;
	plugin_data->lf = lf;
	plugin_data->state = state;

	return (LADSPA_Handle)plugin_data;
}

#undef buffer_write
#undef RUN_ADDING
#undef RUN_REPLACING

#define buffer_write(b, v) (b = v)
#define RUN_ADDING    0
#define RUN_REPLACING 1

static void runStereo_gate(LADSPA_Handle instance, unsigned long sample_count) {
	Stereo_gate *plugin_data = (Stereo_gate *)instance;

	/* LF key filter (Hz) (float value) */
	const LADSPA_Data lf_fc = *(plugin_data->lf_fc);

	/* HF key filter (Hz) (float value) */
	const LADSPA_Data hf_fc = *(plugin_data->hf_fc);

	/* Threshold (dB) (float value) */
	const LADSPA_Data threshold = *(plugin_data->threshold);

	/* Attack (ms) (float value) */
	const LADSPA_Data attack = *(plugin_data->attack);

	/* Hold (ms) (float value) */
	const LADSPA_Data hold = *(plugin_data->hold);

	/* Decay (ms) (float value) */
	const LADSPA_Data decay = *(plugin_data->decay);

	/* Range (dB) (float value) */
	const LADSPA_Data range = *(plugin_data->range);

	/* Output select (-1 = key listen, 0 = gate, 1 = bypass) (float value) */
	const LADSPA_Data select = *(plugin_data->select);

	/* Input 1 (array of floats of length sample_count) */
	const LADSPA_Data * const in1 = plugin_data->in1;

	/* Input 2 (array of floats of length sample_count) */
	const LADSPA_Data * const in2 = plugin_data->in2;

	/* Output 1 (array of floats of length sample_count) */
	LADSPA_Data * const out1 = plugin_data->out1;

	/* Output 2 (array of floats of length sample_count) */
	LADSPA_Data * const out2 = plugin_data->out2;
	float env = plugin_data->env;
	float fs = plugin_data->fs;
	float gate = plugin_data->gate;
	biquad * hf = plugin_data->hf;
	int hold_count = plugin_data->hold_count;
	biquad * lf = plugin_data->lf;
	int state = plugin_data->state;

#line 49 "gate_1921.xml"
	unsigned long pos;
	float cut = DB_CO(range);
	float t_level = DB_CO(threshold);
	float a_rate = 1000.0f / (attack * fs);
	float d_rate = 1000.0f / (decay * fs);
	float post_filter, apost_filter;
	float sample;
	int op = f_round(select);

	ls_set_params(lf, lf_fc, -40.0f, 0.6f, fs);
	hs_set_params(hf, hf_fc, -50.0f, 0.6f, fs);

	for (pos = 0; pos < sample_count; pos++) {
	  sample = (in1[pos] + in2[pos]) / 2.0f;
	  post_filter = biquad_run(lf, sample);
	  post_filter = biquad_run(hf, post_filter);
	  apost_filter = fabs(post_filter);

	  if (apost_filter > env) {
	    env = apost_filter;
	  } else {
	    env = apost_filter * ENV_TR + env * (1.0f - ENV_TR);
	  }

	  if (state == CLOSED) {
	    if (env >= t_level) {
	      state = OPENING;
	    }
	  } else if (state == OPENING) {
	    gate += a_rate;
	    if (gate >= 1.0f) {
	      gate = 1.0f;
	      state = OPEN;
	      hold_count = f_round(hold * fs * 0.001f);
	      plugin_data->hold_count = hold_count;
	    }
	  } else if (state == OPEN) {
	    if (hold_count <= 0) {
	      if (env < t_level) {
	        state = CLOSING;
	      }
	    } else {
	      hold_count--;
	    }
	  } else if (state == CLOSING) {
	    gate -= d_rate;
	    if (env >= t_level) {
	      state = OPENING;
	    } else if (gate <= 0.0f) {
	      gate = 0.0f;
	      state = CLOSED;
	    }
	  }

	  if (op == 0) {
	    buffer_write(out1[pos], in1[pos] * (cut * (1.0f - gate) + gate));
	    buffer_write(out2[pos], in2[pos] * (cut * (1.0f - gate) + gate));
	  } else if (op == -1) {
	    buffer_write(out1[pos], post_filter);
	    buffer_write(out2[pos], post_filter);
	  } else {
	    buffer_write(out1[pos], in1[pos]);
	    buffer_write(out2[pos], in2[pos]);
	  }
	}
	*(plugin_data->level) = CO_DB(env);

	plugin_data->env = env;
	plugin_data->gate = gate;
	plugin_data->state = state;
	plugin_data->hold_count = hold_count;
}
#undef buffer_write
#undef RUN_ADDING
#undef RUN_REPLACING

#define buffer_write(b, v) (b += (v) * run_adding_gain)
#define RUN_ADDING    1
#define RUN_REPLACING 0

static void setRunAddingGainStereo_gate(LADSPA_Handle instance, LADSPA_Data gain) {
	((Stereo_gate *)instance)->run_adding_gain = gain;
}

static void runAddingStereo_gate(LADSPA_Handle instance, unsigned long sample_count) {
	Stereo_gate *plugin_data = (Stereo_gate *)instance;
	LADSPA_Data run_adding_gain = plugin_data->run_adding_gain;

	/* LF key filter (Hz) (float value) */
	const LADSPA_Data lf_fc = *(plugin_data->lf_fc);

	/* HF key filter (Hz) (float value) */
	const LADSPA_Data hf_fc = *(plugin_data->hf_fc);

	/* Threshold (dB) (float value) */
	const LADSPA_Data threshold = *(plugin_data->threshold);

	/* Attack (ms) (float value) */
	const LADSPA_Data attack = *(plugin_data->attack);

	/* Hold (ms) (float value) */
	const LADSPA_Data hold = *(plugin_data->hold);

	/* Decay (ms) (float value) */
	const LADSPA_Data decay = *(plugin_data->decay);

	/* Range (dB) (float value) */
	const LADSPA_Data range = *(plugin_data->range);

	/* Output select (-1 = key listen, 0 = gate, 1 = bypass) (float value) */
	const LADSPA_Data select = *(plugin_data->select);

	/* Input 1 (array of floats of length sample_count) */
	const LADSPA_Data * const in1 = plugin_data->in1;

	/* Input 2 (array of floats of length sample_count) */
	const LADSPA_Data * const in2 = plugin_data->in2;

	/* Output 1 (array of floats of length sample_count) */
	LADSPA_Data * const out1 = plugin_data->out1;

	/* Output 2 (array of floats of length sample_count) */
	LADSPA_Data * const out2 = plugin_data->out2;
	float env = plugin_data->env;
	float fs = plugin_data->fs;
	float gate = plugin_data->gate;
	biquad * hf = plugin_data->hf;
	int hold_count = plugin_data->hold_count;
	biquad * lf = plugin_data->lf;
	int state = plugin_data->state;

#line 49 "gate_1921.xml"
	unsigned long pos;
	float cut = DB_CO(range);
	float t_level = DB_CO(threshold);
	float a_rate = 1000.0f / (attack * fs);
	float d_rate = 1000.0f / (decay * fs);
	float post_filter, apost_filter;
	float sample;
	int op = f_round(select);

	ls_set_params(lf, lf_fc, -40.0f, 0.6f, fs);
	hs_set_params(hf, hf_fc, -50.0f, 0.6f, fs);

	for (pos = 0; pos < sample_count; pos++) {
	  sample = (in1[pos] + in2[pos]) / 2.0f;
	  post_filter = biquad_run(lf, sample);
	  post_filter = biquad_run(hf, post_filter);
	  apost_filter = fabs(post_filter);

	  if (apost_filter > env) {
	    env = apost_filter;
	  } else {
	    env = apost_filter * ENV_TR + env * (1.0f - ENV_TR);
	  }

	  if (state == CLOSED) {
	    if (env >= t_level) {
	      state = OPENING;
	    }
	  } else if (state == OPENING) {
	    gate += a_rate;
	    if (gate >= 1.0f) {
	      gate = 1.0f;
	      state = OPEN;
	      hold_count = f_round(hold * fs * 0.001f);
	      plugin_data->hold_count = hold_count;
	    }
	  } else if (state == OPEN) {
	    if (hold_count <= 0) {
	      if (env < t_level) {
	        state = CLOSING;
	      }
	    } else {
	      hold_count--;
	    }
	  } else if (state == CLOSING) {
	    gate -= d_rate;
	    if (env >= t_level) {
	      state = OPENING;
	    } else if (gate <= 0.0f) {
	      gate = 0.0f;
	      state = CLOSED;
	    }
	  }

	  if (op == 0) {
	    buffer_write(out1[pos], in1[pos] * (cut * (1.0f - gate) + gate));
	    buffer_write(out2[pos], in2[pos] * (cut * (1.0f - gate) + gate));
	  } else if (op == -1) {
	    buffer_write(out1[pos], post_filter);
	    buffer_write(out2[pos], post_filter);
	  } else {
	    buffer_write(out1[pos], in1[pos]);
	    buffer_write(out2[pos], in2[pos]);
	  }
	}
	*(plugin_data->level) = CO_DB(env);

	plugin_data->env = env;
	plugin_data->gate = gate;
	plugin_data->state = state;
	plugin_data->hold_count = hold_count;
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


	gateDescriptor =
	 (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));

	if (gateDescriptor) {
		gateDescriptor->UniqueID = 1921;
		gateDescriptor->Label = "gate";
		gateDescriptor->Properties =
		 LADSPA_PROPERTY_HARD_RT_CAPABLE;
		gateDescriptor->Name =
		 D_("Gate");
		gateDescriptor->Maker =
		 "Steve Harris <steve@plugin.org.uk>";
		gateDescriptor->Copyright =
		 "GPL";
		gateDescriptor->PortCount = 11;

		port_descriptors = (LADSPA_PortDescriptor *)calloc(11,
		 sizeof(LADSPA_PortDescriptor));
		gateDescriptor->PortDescriptors =
		 (const LADSPA_PortDescriptor *)port_descriptors;

		port_range_hints = (LADSPA_PortRangeHint *)calloc(11,
		 sizeof(LADSPA_PortRangeHint));
		gateDescriptor->PortRangeHints =
		 (const LADSPA_PortRangeHint *)port_range_hints;

		port_names = (char **)calloc(11, sizeof(char*));
		gateDescriptor->PortNames =
		 (const char **)port_names;

		/* Parameters for LF key filter (Hz) */
		port_descriptors[GATE_LF_FC] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[GATE_LF_FC] =
		 D_("LF key filter (Hz)");
		port_range_hints[GATE_LF_FC].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_SAMPLE_RATE | LADSPA_HINT_DEFAULT_MINIMUM;
		port_range_hints[GATE_LF_FC].LowerBound = 0.0007f;
		port_range_hints[GATE_LF_FC].UpperBound = 0.1;

		/* Parameters for HF key filter (Hz) */
		port_descriptors[GATE_HF_FC] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[GATE_HF_FC] =
		 D_("HF key filter (Hz)");
		port_range_hints[GATE_HF_FC].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_SAMPLE_RATE | LADSPA_HINT_DEFAULT_MAXIMUM;
		port_range_hints[GATE_HF_FC].LowerBound = 0.005f;
		port_range_hints[GATE_HF_FC].UpperBound = 0.49;

		/* Parameters for Key level (dB) */
		port_descriptors[GATE_LEVEL] =
		 LADSPA_PORT_OUTPUT | LADSPA_PORT_CONTROL;
		port_names[GATE_LEVEL] =
		 D_("Key level (dB)");
		port_range_hints[GATE_LEVEL].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
		port_range_hints[GATE_LEVEL].LowerBound = -70;
		port_range_hints[GATE_LEVEL].UpperBound = +20;

		/* Parameters for Threshold (dB) */
		port_descriptors[GATE_THRESHOLD] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[GATE_THRESHOLD] =
		 D_("Threshold (dB)");
		port_range_hints[GATE_THRESHOLD].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MINIMUM;
		port_range_hints[GATE_THRESHOLD].LowerBound = -70;
		port_range_hints[GATE_THRESHOLD].UpperBound = +20;

		/* Parameters for Attack (ms) */
		port_descriptors[GATE_ATTACK] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[GATE_ATTACK] =
		 D_("Attack (ms)");
		port_range_hints[GATE_ATTACK].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_LOW;
		port_range_hints[GATE_ATTACK].LowerBound = 0.01;
		port_range_hints[GATE_ATTACK].UpperBound = 1000;

		/* Parameters for Hold (ms) */
		port_descriptors[GATE_HOLD] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[GATE_HOLD] =
		 D_("Hold (ms)");
		port_range_hints[GATE_HOLD].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_HIGH;
		port_range_hints[GATE_HOLD].LowerBound = 2;
		port_range_hints[GATE_HOLD].UpperBound = 2000;

		/* Parameters for Decay (ms) */
		port_descriptors[GATE_DECAY] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[GATE_DECAY] =
		 D_("Decay (ms)");
		port_range_hints[GATE_DECAY].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE;
		port_range_hints[GATE_DECAY].LowerBound = 2;
		port_range_hints[GATE_DECAY].UpperBound = 4000;

		/* Parameters for Range (dB) */
		port_descriptors[GATE_RANGE] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[GATE_RANGE] =
		 D_("Range (dB)");
		port_range_hints[GATE_RANGE].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MINIMUM;
		port_range_hints[GATE_RANGE].LowerBound = -90;
		port_range_hints[GATE_RANGE].UpperBound = 0;

		/* Parameters for Output select (-1 = key listen, 0 = gate, 1 = bypass) */
		port_descriptors[GATE_SELECT] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[GATE_SELECT] =
		 D_("Output select (-1 = key listen, 0 = gate, 1 = bypass)");
		port_range_hints[GATE_SELECT].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_INTEGER | LADSPA_HINT_DEFAULT_0;
		port_range_hints[GATE_SELECT].LowerBound = -1;
		port_range_hints[GATE_SELECT].UpperBound = 1;

		/* Parameters for Input */
		port_descriptors[GATE_INPUT] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		port_names[GATE_INPUT] =
		 D_("Input");
		port_range_hints[GATE_INPUT].HintDescriptor = 0;

		/* Parameters for Output */
		port_descriptors[GATE_OUTPUT] =
		 LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		port_names[GATE_OUTPUT] =
		 D_("Output");
		port_range_hints[GATE_OUTPUT].HintDescriptor = 0;

		gateDescriptor->activate = activateGate;
		gateDescriptor->cleanup = cleanupGate;
		gateDescriptor->connect_port = connectPortGate;
		gateDescriptor->deactivate = NULL;
		gateDescriptor->instantiate = instantiateGate;
		gateDescriptor->run = runGate;
		gateDescriptor->run_adding = runAddingGate;
		gateDescriptor->set_run_adding_gain = setRunAddingGainGate;
	}

	stereo_gateDescriptor =
	 (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));

	if (stereo_gateDescriptor) {
		stereo_gateDescriptor->UniqueID = 1922;
		stereo_gateDescriptor->Label = "stereo_gate";
		stereo_gateDescriptor->Properties =
		 LADSPA_PROPERTY_HARD_RT_CAPABLE;
		stereo_gateDescriptor->Name =
		 D_("Stereo Gate");
		stereo_gateDescriptor->Maker =
		 "Steve Harris <steve@plugin.org.uk>";
		stereo_gateDescriptor->Copyright =
		 "GPL";
		stereo_gateDescriptor->PortCount = 13;

		port_descriptors = (LADSPA_PortDescriptor *)calloc(13,
		 sizeof(LADSPA_PortDescriptor));
		stereo_gateDescriptor->PortDescriptors =
		 (const LADSPA_PortDescriptor *)port_descriptors;

		port_range_hints = (LADSPA_PortRangeHint *)calloc(13,
		 sizeof(LADSPA_PortRangeHint));
		stereo_gateDescriptor->PortRangeHints =
		 (const LADSPA_PortRangeHint *)port_range_hints;

		port_names = (char **)calloc(13, sizeof(char*));
		stereo_gateDescriptor->PortNames =
		 (const char **)port_names;

		/* Parameters for LF key filter (Hz) */
		port_descriptors[STEREO_GATE_LF_FC] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[STEREO_GATE_LF_FC] =
		 D_("LF key filter (Hz)");
		port_range_hints[STEREO_GATE_LF_FC].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_SAMPLE_RATE | LADSPA_HINT_DEFAULT_MINIMUM;
		port_range_hints[STEREO_GATE_LF_FC].LowerBound = 0.0007f;
		port_range_hints[STEREO_GATE_LF_FC].UpperBound = 0.1;

		/* Parameters for HF key filter (Hz) */
		port_descriptors[STEREO_GATE_HF_FC] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[STEREO_GATE_HF_FC] =
		 D_("HF key filter (Hz)");
		port_range_hints[STEREO_GATE_HF_FC].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_SAMPLE_RATE | LADSPA_HINT_DEFAULT_MAXIMUM;
		port_range_hints[STEREO_GATE_HF_FC].LowerBound = 0.005f;
		port_range_hints[STEREO_GATE_HF_FC].UpperBound = 0.49;

		/* Parameters for Key level (dB) */
		port_descriptors[STEREO_GATE_LEVEL] =
		 LADSPA_PORT_OUTPUT | LADSPA_PORT_CONTROL;
		port_names[STEREO_GATE_LEVEL] =
		 D_("Key level (dB)");
		port_range_hints[STEREO_GATE_LEVEL].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE;
		port_range_hints[STEREO_GATE_LEVEL].LowerBound = -70;
		port_range_hints[STEREO_GATE_LEVEL].UpperBound = +20;

		/* Parameters for Threshold (dB) */
		port_descriptors[STEREO_GATE_THRESHOLD] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[STEREO_GATE_THRESHOLD] =
		 D_("Threshold (dB)");
		port_range_hints[STEREO_GATE_THRESHOLD].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MINIMUM;
		port_range_hints[STEREO_GATE_THRESHOLD].LowerBound = -70;
		port_range_hints[STEREO_GATE_THRESHOLD].UpperBound = +20;

		/* Parameters for Attack (ms) */
		port_descriptors[STEREO_GATE_ATTACK] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[STEREO_GATE_ATTACK] =
		 D_("Attack (ms)");
		port_range_hints[STEREO_GATE_ATTACK].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_LOW;
		port_range_hints[STEREO_GATE_ATTACK].LowerBound = 0.01;
		port_range_hints[STEREO_GATE_ATTACK].UpperBound = 1000;

		/* Parameters for Hold (ms) */
		port_descriptors[STEREO_GATE_HOLD] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[STEREO_GATE_HOLD] =
		 D_("Hold (ms)");
		port_range_hints[STEREO_GATE_HOLD].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_HIGH;
		port_range_hints[STEREO_GATE_HOLD].LowerBound = 2;
		port_range_hints[STEREO_GATE_HOLD].UpperBound = 2000;

		/* Parameters for Decay (ms) */
		port_descriptors[STEREO_GATE_DECAY] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[STEREO_GATE_DECAY] =
		 D_("Decay (ms)");
		port_range_hints[STEREO_GATE_DECAY].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MIDDLE;
		port_range_hints[STEREO_GATE_DECAY].LowerBound = 2;
		port_range_hints[STEREO_GATE_DECAY].UpperBound = 4000;

		/* Parameters for Range (dB) */
		port_descriptors[STEREO_GATE_RANGE] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[STEREO_GATE_RANGE] =
		 D_("Range (dB)");
		port_range_hints[STEREO_GATE_RANGE].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_DEFAULT_MINIMUM;
		port_range_hints[STEREO_GATE_RANGE].LowerBound = -90;
		port_range_hints[STEREO_GATE_RANGE].UpperBound = 0;

		/* Parameters for Output select (-1 = key listen, 0 = gate, 1 = bypass) */
		port_descriptors[STEREO_GATE_SELECT] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
		port_names[STEREO_GATE_SELECT] =
		 D_("Output select (-1 = key listen, 0 = gate, 1 = bypass)");
		port_range_hints[STEREO_GATE_SELECT].HintDescriptor =
		 LADSPA_HINT_BOUNDED_BELOW | LADSPA_HINT_BOUNDED_ABOVE | LADSPA_HINT_INTEGER | LADSPA_HINT_DEFAULT_0;
		port_range_hints[STEREO_GATE_SELECT].LowerBound = -1;
		port_range_hints[STEREO_GATE_SELECT].UpperBound = 1;

		/* Parameters for Input 1 */
		port_descriptors[STEREO_GATE_IN1] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		port_names[STEREO_GATE_IN1] =
		 D_("Input 1");
		port_range_hints[STEREO_GATE_IN1].HintDescriptor = 0;

		/* Parameters for Input 2 */
		port_descriptors[STEREO_GATE_IN2] =
		 LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
		port_names[STEREO_GATE_IN2] =
		 D_("Input 2");
		port_range_hints[STEREO_GATE_IN2].HintDescriptor = 0;

		/* Parameters for Output 1 */
		port_descriptors[STEREO_GATE_OUT1] =
		 LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		port_names[STEREO_GATE_OUT1] =
		 D_("Output 1");
		port_range_hints[STEREO_GATE_OUT1].HintDescriptor = 0;

		/* Parameters for Output 2 */
		port_descriptors[STEREO_GATE_OUT2] =
		 LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
		port_names[STEREO_GATE_OUT2] =
		 D_("Output 2");
		port_range_hints[STEREO_GATE_OUT2].HintDescriptor = 0;

		stereo_gateDescriptor->activate = activateStereo_gate;
		stereo_gateDescriptor->cleanup = cleanupStereo_gate;
		stereo_gateDescriptor->connect_port = connectPortStereo_gate;
		stereo_gateDescriptor->deactivate = NULL;
		stereo_gateDescriptor->instantiate = instantiateStereo_gate;
		stereo_gateDescriptor->run = runStereo_gate;
		stereo_gateDescriptor->run_adding = runAddingStereo_gate;
		stereo_gateDescriptor->set_run_adding_gain = setRunAddingGainStereo_gate;
	}
}

void _fini() {
	if (gateDescriptor) {
		free((LADSPA_PortDescriptor *)gateDescriptor->PortDescriptors);
		free((char **)gateDescriptor->PortNames);
		free((LADSPA_PortRangeHint *)gateDescriptor->PortRangeHints);
		free(gateDescriptor);
	}
	if (stereo_gateDescriptor) {
		free((LADSPA_PortDescriptor *)stereo_gateDescriptor->PortDescriptors);
		free((char **)stereo_gateDescriptor->PortNames);
		free((LADSPA_PortRangeHint *)stereo_gateDescriptor->PortRangeHints);
		free(stereo_gateDescriptor);
	}

}
