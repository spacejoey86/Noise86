#include "plugin.hpp"


struct RepeatBuddy : Module {
	enum ParamId {
		COUNTER_CV_ATTENUATOR_PARAMETER_PARAM,
		DIVISION_ATTENUATOR_PARAMETER_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		THRESHOLD_CV_INPUT_INPUT,
		INCREASE_TRIGGER_INPUT_INPUT,
		COUNTER_RESET_INPUT_INPUT,
		DIVISION_CV_INPUT_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		COMPARATOR_GATE_OUTPUT_OUTPUT,
		COUNTER_CV_OUTPUT_OUTPUT,
		DIVIDED_TRIGGER_OUTPUT_OUTPUT,
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

	RepeatBuddy() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(COUNTER_CV_ATTENUATOR_PARAMETER_PARAM, 0.f, 1.f, 0.f, "");
		configParam(DIVISION_ATTENUATOR_PARAMETER_PARAM, 0.f, 1.f, 0.f, "");
		configInput(THRESHOLD_CV_INPUT_INPUT, "");
		configInput(INCREASE_TRIGGER_INPUT_INPUT, "");
		configInput(COUNTER_RESET_INPUT_INPUT, "");
		configInput(DIVISION_CV_INPUT_INPUT, "");
		configOutput(COMPARATOR_GATE_OUTPUT_OUTPUT, "");
		configOutput(COUNTER_CV_OUTPUT_OUTPUT, "");
		configOutput(DIVIDED_TRIGGER_OUTPUT_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
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

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 71.199)), module, RepeatBuddy::COUNTER_CV_ATTENUATOR_PARAMETER_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.16, 99.209)), module, RepeatBuddy::DIVISION_ATTENUATOR_PARAMETER_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.434, 28.931)), module, RepeatBuddy::THRESHOLD_CV_INPUT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.434, 42.936)), module, RepeatBuddy::INCREASE_TRIGGER_INPUT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.434, 56.941)), module, RepeatBuddy::COUNTER_RESET_INPUT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.434, 113.323)), module, RepeatBuddy::DIVISION_CV_INPUT_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.886, 57.194)), module, RepeatBuddy::COMPARATOR_GATE_OUTPUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.886, 85.204)), module, RepeatBuddy::COUNTER_CV_OUTPUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(14.886, 113.214)), module, RepeatBuddy::DIVIDED_TRIGGER_OUTPUT_OUTPUT));

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