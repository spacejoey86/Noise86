#include "plugin.hpp"


struct SeqOfChanges : Module {
	enum ParamId {
		ENUMS(PROBABILITIES, 8),
		PARAMS_LEN
	};
	enum InputId {
		CLK_INPUT,
		ENUMS(TRIGS, 8),
		INPUTS_LEN
	};
	enum OutputId {
		CV_OUT_OUTPUT,
		OUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		ENUMS(LIGHTS, 3),
		LIGHTS_LEN
	};

	dsp::TSchmittTrigger<float> resets[8];
	dsp::PulseGenerator trigGen;
	bool prevClock = false;
	int state = 0; // first three bits represent the previous three outputs


	SeqOfChanges() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PROBABILITIES + 0, 0.f, 1.f, 0.f, "Kun");
		configParam(PROBABILITIES + 1, 0.f, 1.f, 0.f, "Gen");
		configParam(PROBABILITIES + 2, 0.f, 1.f, 0.f, "Kan");
		configParam(PROBABILITIES + 3, 0.f, 1.f, 0.f, "Xun");
		configParam(PROBABILITIES + 4, 0.f, 1.f, 0.f, "Zhen");
		configParam(PROBABILITIES + 5, 0.f, 1.f, 0.f, "Li");
		configParam(PROBABILITIES + 6, 0.f, 1.f, 0.f, "Dui");
		configParam(PROBABILITIES + 7, 0.f, 1.f, 0.f, "Qian");

		configInput(CLK_INPUT, "Clock");
		configInput(TRIGS + 0, "Kun");
		configInput(TRIGS + 1, "Gen");
		configInput(TRIGS + 2, "Kan");
		configInput(TRIGS + 3, "Xun");
		configInput(TRIGS + 4, "Zhen");
		configInput(TRIGS + 5, "Li");
		configInput(TRIGS + 6, "Dui");
		configInput(TRIGS + 7, "Qian");

		configOutput(OUT_OUTPUT, "Trigger");
		configOutput(CV_OUT_OUTPUT, "CV");
	}

	void process(const ProcessArgs& args) override {
		bool resetThisTick = false;
		for (int i = 0; i < 8; i++) {
			if (resets[i].process(inputs[TRIGS + i].getVoltage(), 0.1f, 1.f)) {
				state = i;
				resetThisTick = true;
			}
		}

		if (!resetThisTick && (inputs[CLK_INPUT].getVoltage() > 3.f) && !prevClock) {
			bool nextTrig = random::uniform() < params[PROBABILITIES + state].getValue();
			state = state >> 1;
			if ((int) nextTrig) {
				state = state | 0b100;
			}

			if (nextTrig) {
				trigGen.trigger(1e-3f);
			}
		}
		prevClock = inputs[CLK_INPUT].getVoltage() > 3.f;

		bool trig = trigGen.process(args.sampleTime);
		outputs[OUT_OUTPUT].setVoltage(trig ? 10.f : 0.f);
		outputs[CV_OUT_OUTPUT].setVoltage((float) params[PROBABILITIES + state].getValue() * 10);

		lights[LIGHTS + 0].setBrightness(((state & 0b001) > 0) ? 1.f : 0.f);
		lights[LIGHTS + 1].setBrightness(((state & 0b010) > 0) ? 1.f : 0.f);
		lights[LIGHTS + 2].setBrightness(((state & 0b100) > 0) ? 1.f : 0.f);

	}
};


struct SeqOfChangesWidget : ModuleWidget {
	SeqOfChangesWidget(SeqOfChanges* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/SeqOfChanges.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(35.186, 21.304)), module, SeqOfChanges::PROBABILITIES + 0));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.442, 44.649)), module, SeqOfChanges::PROBABILITIES + 1));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(35.186, 58.919)), module, SeqOfChanges::PROBABILITIES + 2));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.442, 102.538)), module, SeqOfChanges::PROBABILITIES + 3));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(14.606, 44.503)), module, SeqOfChanges::PROBABILITIES + 4));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(35.186, 79.175)), module, SeqOfChanges::PROBABILITIES + 5));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(14.606, 102.521)), module, SeqOfChanges::PROBABILITIES + 6));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(35.186, 116.791)), module, SeqOfChanges::PROBABILITIES + 7));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.781, 57.107)), module, SeqOfChanges::CLK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.774, 21.39)), module, SeqOfChanges::TRIGS + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(46.442, 35.574)), module, SeqOfChanges::TRIGS + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.774, 58.971)), module, SeqOfChanges::TRIGS + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(46.442, 93.446)), module, SeqOfChanges::TRIGS + 3));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(14.826, 35.72)), module, SeqOfChanges::TRIGS + 4));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.774, 79.21)), module, SeqOfChanges::TRIGS + 5));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(14.606, 93.446)), module, SeqOfChanges::TRIGS + 6));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.774, 116.791)), module, SeqOfChanges::TRIGS + 7));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.781, 67.729)), module, SeqOfChanges::OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.534, 78.812)), module, SeqOfChanges::CV_OUT_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(55.803, 61.538)), module, SeqOfChanges::LIGHTS + 0));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(55.803, 69.047)), module, SeqOfChanges::LIGHTS + 1));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(55.803, 76.556)), module, SeqOfChanges::LIGHTS + 2));
	}
};


Model* modelSeqOfChanges = createModel<SeqOfChanges, SeqOfChangesWidget>("SeqOfChanges");
