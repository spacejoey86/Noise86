#include "plugin.hpp"


struct SeqOfChanges : Module {
	enum ParamId {
		ENUMS(PROBABILITIES, 8),
		PARAMS_LEN
	};
	enum InputId {
		CLK_INPUT,
		RESET_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		ENUMS(LIGHTS, 3),
		LIGHTS_LEN
	};

	dsp::TSchmittTrigger<float> reset;
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
		configInput(RESET_INPUT, "Reset");
		configOutput(OUT_OUTPUT, "Trigger");
	}

	void process(const ProcessArgs& args) override {
		if (reset.process(inputs[RESET_INPUT].getVoltage(), 0.1f, 1.f)) {
			state = 0;
		}
		else if ((inputs[CLK_INPUT].getVoltage() > 3.f) && !prevClock) {
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

		// state = 0b001;
		lights[LIGHTS + 0].setBrightness(((state & 0b001) > 0) ? 1.f : 0.f);
		lights[LIGHTS + 1].setBrightness(((state & 0b010) > 0) ? 1.f : 0.f);
		lights[LIGHTS + 2].setBrightness(((state & 0b100) > 0) ? 1.f : 0.f);

		// for (int i = 0; i < 3; i++) {
		// 	bool light_state = (state >> i) & 0b001;
		// 	lights[LIGHTS + i].setBrightness(light_state ? 1.f : 0.f);
		// }

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

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.101, 83.598)), module, SeqOfChanges::PROBABILITIES + 0));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.212, 110.773)), module, SeqOfChanges::PROBABILITIES + 1));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.688, 120.694)), module, SeqOfChanges::PROBABILITIES + 2));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(14.949, 83.536)), module, SeqOfChanges::PROBABILITIES + 3));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.252, 97.134)), module, SeqOfChanges::PROBABILITIES + 4));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.688, 74.21)), module, SeqOfChanges::PROBABILITIES + 5));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(52.708, 97.134)), module, SeqOfChanges::PROBABILITIES + 6));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.296, 110.602)), module, SeqOfChanges::PROBABILITIES + 7));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.458, 25.393)), module, SeqOfChanges::CLK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.828, 25.393)), module, SeqOfChanges::RESET_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(44.742, 25.305)), module, SeqOfChanges::OUT_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(30.48, 38.308)), module, SeqOfChanges::LIGHTS + 0));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(30.48, 49.802)), module, SeqOfChanges::LIGHTS + 1));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(30.48, 61.296)), module, SeqOfChanges::LIGHTS + 2));
	}
};


Model* modelSeqOfChanges = createModel<SeqOfChanges, SeqOfChangesWidget>("SeqOfChanges");
