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
		ENUMS(THRESHOLD_LIGHTS, 7),
		ENUMS(COUNTER_LIGHTS, 7),
		LIGHTS_LEN
	};

	dsp::TSchmittTrigger<float> increase_schmitt,
		reset_schmitt;
	int counter = 0;
	dsp::PulseGenerator comparator_gate_gen,
		divider_pulse_gen;

	RepeatBuddy() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		configInput(THRESHOLD_CV_INPUT, "Comparator threshold");
		configInput(INCREASE_INPUT, "Increase counter (clock)");
		configInput(RESET_INPUT, "Reset counter");
		configOutput(COMPARATOR_GATE_OUTPUT, "Comparator gate");

		configParam(COUNTER_CV_ATTENUATOR_PARAM, 0.f, 1.f, 1.f, "Counter CV attenuator");
		configOutput(COUNTER_CV_OUTPUT, "Counter CV");

		configParam(DIVISION_ATTENUATOR_PARAM, 0.f, 1.f, 0.1f, "Division CV attenuator");
		configInput(DIVISION_CV_INPUT, "Division CV");
		configOutput(DIVIDED_TRIGGER_OUTPUT, "Divided");
	}

	void processDivider(int counter, int division) {
		if (division != 0 && ((counter % (division)) == 0)) {
			divider_pulse_gen.trigger(1e-3f);
		}
	}

	void setCounterLights(int counter) {
		for (int i = 0; i < 7; i++) {
			if (counter > i) {
				lights[COUNTER_LIGHTS + i].setBrightness(1.f);
			} else {
				lights[COUNTER_LIGHTS + i].setBrightness(0.f);
			}
		}
	}

	void setThresholdLights(int threshold, int division) {
		for (int i = 0; i < 7; i++) {
			lights[THRESHOLD_LIGHTS + i].setBrightness((threshold >= i) * ((division != 0 && ((i % (division)) == 0)) ? 1.f : 0.2f));
		}
	}

	void process(const ProcessArgs& args) override {
		// get division value
		int division = 1;
		if (inputs[DIVISION_CV_INPUT].isConnected()) {
			division = std::round(rescale(clamp(inputs[DIVISION_CV_INPUT].getVoltage() * params[DIVISION_ATTENUATOR_PARAM].getValue(), 0.0, 10.0), 0.f, 10.f, 0.f, 8.f));
		} else {
			division = std::round(rescale(clamp(10.f * params[DIVISION_ATTENUATOR_PARAM].getValue(), 0.f, 10.f), 0.f, 10.f, 0.f, 8.f));
		}

		// handle reset
		if (reset_schmitt.process(inputs[RESET_INPUT].getVoltage(), 0.1f, 1.f)) {
			counter = 0;
			setCounterLights(counter);

			processDivider(counter, division);
		}

		// increase counter
		if (increase_schmitt.process(inputs[INCREASE_INPUT].getVoltage(), 0.1f, 1.f)) {
			counter = clamp(counter + 1, 0, 8);
			setCounterLights(counter);

			processDivider(counter, division);
		}

		// get threshold value
		float threshold = 8.0;
		if (inputs[THRESHOLD_CV_INPUT].isConnected()) {
			threshold = rescale(clamp(inputs[THRESHOLD_CV_INPUT].getVoltage(), 1.f, 10.f), 0.f, 10.f, 0.f, 8.f);
		}

		// compare counter to threshold
		if ((float) counter >= threshold) {
			comparator_gate_gen.trigger(1e-3f);
			if (!inputs[RESET_INPUT].isConnected()) {
				counter = 0;
				setCounterLights(counter);
				processDivider(counter, division);
			}
		}

		setThresholdLights(threshold, division);
		outputs[COMPARATOR_GATE_OUTPUT].setVoltage(comparator_gate_gen.process(args.sampleTime) ? 10.f : 0.f);
		outputs[COUNTER_CV_OUTPUT].setVoltage(rescale(counter, 0.f, 7.f, 0.f, 10.f) * params[COUNTER_CV_ATTENUATOR_PARAM].getValue());
		outputs[DIVIDED_TRIGGER_OUTPUT].setVoltage(divider_pulse_gen.process(args.sampleTime) ? 10.f : 0.f);
	}
};


struct RepeatBuddyWidget : ModuleWidget {
	RepeatBuddyWidget(RepeatBuddy* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/RepeatBuddy.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 71.199)), module, RepeatBuddy::COUNTER_CV_ATTENUATOR_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 99.209)), module, RepeatBuddy::DIVISION_ATTENUATOR_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.434, 28.931)), module, RepeatBuddy::THRESHOLD_CV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.434, 42.936)), module, RepeatBuddy::INCREASE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.434, 56.941)), module, RepeatBuddy::RESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.434, 113.323)), module, RepeatBuddy::DIVISION_CV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.886, 57.194)), module, RepeatBuddy::COMPARATOR_GATE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.886, 85.204)), module, RepeatBuddy::COUNTER_CV_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.886, 113.214)), module, RepeatBuddy::DIVIDED_TRIGGER_OUTPUT));

		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(17.53, 46.873)), module, RepeatBuddy::COUNTER_LIGHTS + 0));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(17.53, 43.227)), module, RepeatBuddy::COUNTER_LIGHTS + 1));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(17.53, 39.58)), module, RepeatBuddy::COUNTER_LIGHTS + 2));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(17.53, 35.933)), module, RepeatBuddy::COUNTER_LIGHTS + 3));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(17.53, 32.287)), module, RepeatBuddy::COUNTER_LIGHTS + 4));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(17.53, 28.64)), module, RepeatBuddy::COUNTER_LIGHTS + 5));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(17.53, 24.993)), module, RepeatBuddy::COUNTER_LIGHTS + 6));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(12.21, 46.873)), module, RepeatBuddy::THRESHOLD_LIGHTS + 0));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(12.21, 43.227)), module, RepeatBuddy::THRESHOLD_LIGHTS + 1));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(12.21, 39.58)), module, RepeatBuddy::THRESHOLD_LIGHTS + 2));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(12.21, 35.933)), module, RepeatBuddy::THRESHOLD_LIGHTS + 3));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(12.21, 32.287)), module, RepeatBuddy::THRESHOLD_LIGHTS + 4));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(12.21, 28.64)), module, RepeatBuddy::THRESHOLD_LIGHTS + 5));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(12.21, 24.993)), module, RepeatBuddy::THRESHOLD_LIGHTS + 6));
	}
};


Model* modelRepeatBuddy = createModel<RepeatBuddy, RepeatBuddyWidget>("RepeatBuddy");
