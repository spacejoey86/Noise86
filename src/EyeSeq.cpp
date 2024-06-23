#include "plugin.hpp"


struct EyeSeq : Module {
	enum ParamId {
		ENUMS(DIVS, 6),
		ENUMS(OFFSETS, 6),
		PARAMS_LEN
	};
	enum InputId {
		CLK_INPUT,
		RESET_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		ENUMS(LIGHTS, 6),
		LIGHTS_LEN
	};

	dsp::TSchmittTrigger<float> reset;
	bool prevClock = false;
	int step = 0;

	EyeSeq() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(DIVS + 0, 2.f, 16.f, 2.f, "Div 1");
		paramQuantities[DIVS + 0]->snapEnabled = true;
		configParam(DIVS + 1, 2.f, 16.f, 3.f, "Div 2");
		paramQuantities[DIVS + 1]->snapEnabled = true;
		configParam(DIVS + 2, 2.f, 16.f, 4.f, "Div 3");
		paramQuantities[DIVS + 2]->snapEnabled = true;
		configParam(DIVS + 3, 2.f, 16.f, 5.f, "Div 4");
		paramQuantities[DIVS + 3]->snapEnabled = true;
		configParam(DIVS + 4, 2.f, 16.f, 8.f, "Div 5");
		paramQuantities[DIVS + 4]->snapEnabled = true;
		configParam(DIVS + 5, 2.f, 16.f, 16.f, "Div 6");
		paramQuantities[DIVS + 5]->snapEnabled = true;
		configParam(OFFSETS + 0, -1.f, 1.f, 0.f, "Offset 1");
		configParam(OFFSETS + 1, -1.f, 1.f, 0.f, "Offset 2");
		configParam(OFFSETS + 2, -1.f, 1.f, 0.f, "Offset 3");
		configParam(OFFSETS + 3, -1.f, 1.f, 0.f, "Offset 4");
		configParam(OFFSETS + 4, -1.f, 1.f, 0.f, "Offset 5");
		configParam(OFFSETS + 5, -1.f, 1.f, 0.f, "Offset 6");
		configInput(CLK_INPUT, "Clock");
		configInput(RESET_INPUT, "Reset");
		configOutput(OUTPUT_OUTPUT, "Output");
	}

	void process(const ProcessArgs& args) override {
		if (reset.process(inputs[RESET_INPUT].getVoltage(), 0.1f, 1.f)) {
			step = 0;
		}
		else if ((inputs[CLK_INPUT].getVoltage() > 3.f) && !prevClock) {
			step += 1;
			if (step > 720720) { //LCM of the numbers 1 to 16 is 720720
				step = 1;
			}
		}
		prevClock = inputs[CLK_INPUT].getVoltage() > 3.f;

		float output = 0;
		for (int i = 0; i < 6; i++) {
			if (step % (int) params[DIVS + i].getValue() * 2 < (int) params[DIVS + i].getValue()) {
				lights[LIGHTS + i].setBrightness(1.f);
				output += params[OFFSETS + i].getValue();
			} else {
				lights[LIGHTS + i].setBrightness(0.f);
			}
		}

		outputs[OUTPUT_OUTPUT].setVoltage(output);
	}
};


struct EyeSeqWidget : ModuleWidget {
	EyeSeqWidget(EyeSeq* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/EyeSeq.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.423, 40.143)), module, EyeSeq::DIVS + 0));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.057, 40.143)), module, EyeSeq::OFFSETS + 0));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.423, 53.071)), module, EyeSeq::DIVS + 1));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.057, 53.071)), module, EyeSeq::OFFSETS + 1));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.423, 65.999)), module, EyeSeq::DIVS + 2));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.057, 65.999)), module, EyeSeq::OFFSETS + 2));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.423, 78.927)), module, EyeSeq::DIVS + 3));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.057, 78.927)), module, EyeSeq::OFFSETS + 3));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.423, 91.855)), module, EyeSeq::DIVS + 4));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.057, 91.855)), module, EyeSeq::OFFSETS + 4));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.423, 104.783)), module, EyeSeq::DIVS + 5));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.057, 104.783)), module, EyeSeq::OFFSETS + 5));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.423, 24.506)), module, EyeSeq::CLK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(23.057, 24.506)), module, EyeSeq::RESET_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.057, 116.986)), module, EyeSeq::OUTPUT_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.24, 40.143)), module, EyeSeq::LIGHTS + 0));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.24, 53.071)), module, EyeSeq::LIGHTS + 1));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.24, 65.999)), module, EyeSeq::LIGHTS + 2));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.24, 78.927)), module, EyeSeq::LIGHTS + 3));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.24, 91.855)), module, EyeSeq::LIGHTS + 4));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.24, 104.783)), module, EyeSeq::LIGHTS + 5));
	}
};


Model* modelEyeSeq = createModel<EyeSeq, EyeSeqWidget>("EyeSeq");