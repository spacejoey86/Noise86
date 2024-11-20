#include "plugin.hpp"


struct RepeatBuddy : Module {
	enum ParamId {
		COUNTER_CV_ATTENUATOR_PARAM,
		DIVISION_ATTENUATOR_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		THRESHOLD_CV_INPUT,
		INCREASE_INPUT,
		RESET_INPUT,
		DIVISION_CV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		COMPARATOR_GATE_OUTPUT,
		COUNTER_CV_OUTPUT,
		DIVIDED_TRIGGER_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		THRESHOLD_LED_7_LIGHT,
		COUNTER_LED_7_LIGHT,
		THRESHOLD_LED_6_LIGHT,
		COUNTER_LED_6_LIGHT,
		THRESHOLD_LED_5_LIGHT,
		COUNTER_LED_5_LIGHT,
		THRESHOLD_LED_4_LIGHT,
		COUNTER_LED_4_LIGHT,
		THRESHOLD_LED_3_LIGHT,
		COUNTER_LED_3_LIGHT,
		THRESHOLD_LED_2_LIGHT,
		COUNTER_LED_2_LIGHT,
		THRESHOLD_LED_1_LIGHT,
		COUNTER_LED_1_LIGHT,
		LIGHTS_LEN
	};

	dsp::TSchmittTrigger<float> increase_schmitt,
		reset_schmitt;
	int counter = 0;
	dsp::PulseGenerator comparator_gate_gen;

	RepeatBuddy() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configInput(THRESHOLD_CV_INPUT, "Comparator threshold");
		configInput(INCREASE_INPUT, "Increase counter (clock)");
		configInput(RESET_INPUT, "Reset counter");
		configOutput(COMPARATOR_GATE_OUTPUT, "Comparator gate");

		configParam(COUNTER_CV_ATTENUATOR_PARAM, 0.f, 1.f, 1.f, "Counter CV attenuator");
		configOutput(COUNTER_CV_OUTPUT, "Counter CV");

		configParam(DIVISION_ATTENUATOR_PARAM, 0.f, 1.f, 0.f, "Division CV attenuator");
		configInput(DIVISION_CV_INPUT, "Division CV");
		configOutput(DIVIDED_TRIGGER_OUTPUT, "Divided");
	}

	void process(const ProcessArgs& args) override {
		// handle reset
		if (reset_schmitt.process(inputs[RESET_INPUT].getVoltage(), 0.1f, 1.f)) {
			counter = 0;
		}

		// increase counter
		if (increase_schmitt.process(inputs[INCREASE_INPUT].getVoltage(), 0.1f, 1.f)) {
			counter = clamp(counter + 1, 0, 10);
		}

		// get threshold value
		float threshold = 10.0;
		if (inputs[THRESHOLD_CV_INPUT].isConnected()) {
			threshold = clamp(inputs[THRESHOLD_CV_INPUT].getVoltage(), 1.0, 10.0);
		}

		// compare counter to threshold
		if ((float) counter >= threshold) {
			comparator_gate_gen.trigger(1e-3f);
			if (!inputs[RESET_INPUT].isConnected()) {
				counter = 0;
			}
		}
		outputs[COMPARATOR_GATE_OUTPUT].setVoltage(comparator_gate_gen.process(args.sampleTime) ? 10.f : 0.f);
		outputs[COUNTER_CV_OUTPUT].setVoltage(counter * params[COUNTER_CV_ATTENUATOR_PARAM].getValue());
	}
};


struct RepeatBuddyWidget : ModuleWidget {
	RepeatBuddyWidget(RepeatBuddy* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/RepeatBuddy.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 71.199)), module, RepeatBuddy::COUNTER_CV_ATTENUATOR_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 99.209)), module, RepeatBuddy::DIVISION_ATTENUATOR_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.434, 28.931)), module, RepeatBuddy::THRESHOLD_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.434, 42.936)), module, RepeatBuddy::INCREASE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.434, 56.941)), module, RepeatBuddy::RESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.434, 113.323)), module, RepeatBuddy::DIVISION_CV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.886, 57.194)), module, RepeatBuddy::COMPARATOR_GATE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.886, 85.204)), module, RepeatBuddy::COUNTER_CV_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.886, 113.214)), module, RepeatBuddy::DIVIDED_TRIGGER_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(12.21, 24.993)), module, RepeatBuddy::THRESHOLD_LED_7_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.53, 24.993)), module, RepeatBuddy::COUNTER_LED_7_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(12.21, 28.64)), module, RepeatBuddy::THRESHOLD_LED_6_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.53, 28.64)), module, RepeatBuddy::COUNTER_LED_6_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(12.21, 32.287)), module, RepeatBuddy::THRESHOLD_LED_5_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.53, 32.287)), module, RepeatBuddy::COUNTER_LED_5_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(12.21, 35.933)), module, RepeatBuddy::THRESHOLD_LED_4_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.53, 35.933)), module, RepeatBuddy::COUNTER_LED_4_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(12.21, 39.58)), module, RepeatBuddy::THRESHOLD_LED_3_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.53, 39.58)), module, RepeatBuddy::COUNTER_LED_3_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(12.21, 43.227)), module, RepeatBuddy::THRESHOLD_LED_2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.53, 43.227)), module, RepeatBuddy::COUNTER_LED_2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(12.21, 46.873)), module, RepeatBuddy::THRESHOLD_LED_1_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.53, 46.873)), module, RepeatBuddy::COUNTER_LED_1_LIGHT));
	}
};


Model* modelRepeatBuddy = createModel<RepeatBuddy, RepeatBuddyWidget>("RepeatBuddy");