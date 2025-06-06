#include "plugin.hpp"
#include <cmath>

struct DualLFO : Module {
	enum ParamId {
		BOTH_A_PARAM,
		CV_A_PARAM,
		FREQ_A_PARAM,
		BOTH_B_PARAM,
		CV_B_PARAM,
		FREQ_B_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		BOTH_INPUT,
		CV_A_INPUT,
		VOCT_A_INPUT,
		CV_B_INPUT,
		VOCT_B_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		SQUARE_A_OUTPUT,
		TRI_A_OUTPUT,
		SQUARE_B_OUTPUT,
		TRI_B_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	float phase = 0.f;

	DualLFO() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(BOTH_A_PARAM, -1.f, 1.f, 0.f, "Both -> A CV");
		configParam(CV_A_PARAM, -1.f, 1.f, 0.f, "A CV");
		configParam(FREQ_A_PARAM, 0.001f, 5000.f, 0.5f, "A Frequency", " Hz");
		configParam(BOTH_B_PARAM, -1.f, 1.f, 0.f, "Both -> B CV");
		configParam(CV_B_PARAM, -1.f, 1.f, 0.f, "B CV");
		configParam(FREQ_B_PARAM, 0.001f, 5000.f, 0.5f, "B Frequency", " Hz", dsp::FREQ_SEMITONE, dsp::FREQ_C4);

		configInput(BOTH_INPUT, "Both frequencies CV");
		configInput(CV_A_INPUT, "Oscillator A frequency CV");
		configInput(VOCT_A_INPUT, "Oscillator A V/Oct CV");
		configInput(CV_B_INPUT, "Oscillator B frequency CV");
		configInput(VOCT_B_INPUT, "Oscillator B V/Oct CV");

		configOutput(SQUARE_A_OUTPUT, "Oscillator A Square");
		configOutput(TRI_A_OUTPUT, "Oscillator A Triangle");
		configOutput(SQUARE_B_OUTPUT, "Oscillator B Square");
		configOutput(TRI_B_OUTPUT, "Oscillator B Triangle");
	}

	void process(const ProcessArgs& args) override {
		float freq = params[FREQ_A_PARAM].getValue();
		freq *= dsp::exp2_taylor5(inputs[VOCT_A_INPUT].getVoltage())
			+ (inputs[BOTH_INPUT].getVoltage() * params[BOTH_A_PARAM].getValue())
			+ (inputs[CV_A_INPUT].getVoltage() * params[CV_A_PARAM].getValue());
		freq = clamp(freq, 0.f, 8000.f);
		phase += args.sampleTime * (freq);
		outputs[TRI_B_OUTPUT].setVoltage(freq);
		phase -= std::floor(phase);

		bool square_a = phase > 0.5f;
		outputs[SQUARE_A_OUTPUT].setVoltage(square_a ? 10.f : 0.f);
		outputs[TRI_A_OUTPUT].setVoltage(20.f * (phase - 0.5f) * (square_a ? -1.f : 1.f) + 5.f);
	}
};


struct DualLFOWidget : ModuleWidget {
	DualLFOWidget(DualLFO* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/DualLFO.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(6.916, 26.088)), module, DualLFO::BOTH_A_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(33.724, 26.088)), module, DualLFO::CV_A_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(60.531, 26.088)), module, DualLFO::FREQ_A_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(6.916, 48.516)), module, DualLFO::BOTH_B_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(33.724, 48.516)), module, DualLFO::CV_B_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(60.531, 48.516)), module, DualLFO::FREQ_B_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.916, 102.395)), module, DualLFO::BOTH_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20.32, 102.395)), module, DualLFO::CV_A_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(33.724, 102.395)), module, DualLFO::VOCT_A_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20.32, 115.799)), module, DualLFO::CV_B_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(33.724, 115.799)), module, DualLFO::VOCT_B_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(47.128, 102.395)), module, DualLFO::SQUARE_A_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(60.531, 102.395)), module, DualLFO::TRI_A_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(47.128, 115.799)), module, DualLFO::SQUARE_B_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(60.531, 115.799)), module, DualLFO::TRI_B_OUTPUT));
	}
};


Model* modelDualLFO = createModel<DualLFO, DualLFOWidget>("DualLFO");