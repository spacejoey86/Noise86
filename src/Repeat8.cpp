#include "plugin.hpp"


struct Repeat8 : Module {
	enum ParamId {
		STEP_FORWARDS_PARAM,
		STAGE_1_CV_PARAM,
		STAGE_2_CV_PARAM,
		STAGE_3_CV_PARAM,
		STAGE_4_CV_PARAM,
		STAGE_5_CV_PARAM,
		STAGE_6_CV_PARAM,
		STAGE_7_CV_PARAM,
		STAGE_8_CV_PARAM,
		STEP_BACKWARDS_PARAM,
		STAGE_1_REPEATS_PARAM,
		STAGE_2_REPEATS_PARAM,
		STAGE_3_REPEATS_PARAM,
		STAGE_4_REPEATS_PARAM,
		STAGE_5_REPEATS_PARAM,
		STAGE_6_REPEATS_PARAM,
		STAGE_7_REPEATS_PARAM,
		STAGE_8_REPEATS_PARAM,
		ONE_SHOT_MODE_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CLK_INPUT,
		RESET_1_INPUT,
		RESET_2_INPUT,
		COMPARATOR_GATE_1_INPUT,
		COMPARATOR_GATE_2_INPUT,
		COMPARATOR_GATE_3_INPUT,
		COUNTER_INCREASE_INPUT,
		COUNTER_RESET_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		COMPARATOR_OUTPUT_OUTPUT,
		STAGE_1_GATE_OUTPUT,
		STAGE_2_GATE_OUTPUT,
		STAGE_3_GATE_OUTPUT,
		STAGE_4_GATE_OUTPUT,
		STAGE_5_GATE_OUTPUT,
		STAGE_6_GATE_OUTPUT,
		STAGE_7_GATE_OUTPUT,
		STAGE_8_GATE_OUTPUT,
		CV_OUTPUT_1_OUTPUT,
		CV_OUTPUT_2_OUTPUT,
		REPEAT_GATE_1_OUTPUT,
		REPEAT_GATE_2_OUTPUT,
		REPEAT_GATE_3_OUTPUT,
		COUNTER_CV_OUTPUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Repeat8() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(STEP_FORWARDS_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STAGE_1_CV_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STAGE_2_CV_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STAGE_3_CV_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STAGE_4_CV_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STAGE_5_CV_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STAGE_6_CV_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STAGE_7_CV_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STAGE_8_CV_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STEP_BACKWARDS_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STAGE_1_REPEATS_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STAGE_2_REPEATS_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STAGE_3_REPEATS_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STAGE_4_REPEATS_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STAGE_5_REPEATS_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STAGE_6_REPEATS_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STAGE_7_REPEATS_PARAM, 0.f, 1.f, 0.f, "");
		configParam(STAGE_8_REPEATS_PARAM, 0.f, 1.f, 0.f, "");
		configParam(ONE_SHOT_MODE_PARAM, 0.f, 1.f, 0.f, "");
		configInput(CLK_INPUT, "");
		configInput(RESET_1_INPUT, "");
		configInput(RESET_2_INPUT, "");
		configInput(COMPARATOR_GATE_1_INPUT, "");
		configInput(COMPARATOR_GATE_2_INPUT, "");
		configInput(COMPARATOR_GATE_3_INPUT, "");
		configInput(COUNTER_INCREASE_INPUT, "");
		configInput(COUNTER_RESET_INPUT, "");
		configOutput(COMPARATOR_OUTPUT_OUTPUT, "");
		configOutput(STAGE_1_GATE_OUTPUT, "");
		configOutput(STAGE_2_GATE_OUTPUT, "");
		configOutput(STAGE_3_GATE_OUTPUT, "");
		configOutput(STAGE_4_GATE_OUTPUT, "");
		configOutput(STAGE_5_GATE_OUTPUT, "");
		configOutput(STAGE_6_GATE_OUTPUT, "");
		configOutput(STAGE_7_GATE_OUTPUT, "");
		configOutput(STAGE_8_GATE_OUTPUT, "");
		configOutput(CV_OUTPUT_1_OUTPUT, "");
		configOutput(CV_OUTPUT_2_OUTPUT, "");
		configOutput(REPEAT_GATE_1_OUTPUT, "");
		configOutput(REPEAT_GATE_2_OUTPUT, "");
		configOutput(REPEAT_GATE_3_OUTPUT, "");
		configOutput(COUNTER_CV_OUTPUT_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct Repeat8Widget : ModuleWidget {
	Repeat8Widget(Repeat8* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Repeat8.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(-97.6, 10.662)), module, Repeat8::STEP_FORWARDS_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.662, 19.25)), module, Repeat8::STAGE_1_CV_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(25.033, 19.25)), module, Repeat8::STAGE_2_CV_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(39.404, 19.25)), module, Repeat8::STAGE_3_CV_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(53.775, 19.25)), module, Repeat8::STAGE_4_CV_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(68.145, 19.25)), module, Repeat8::STAGE_5_CV_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(82.516, 19.25)), module, Repeat8::STAGE_6_CV_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(96.887, 19.25)), module, Repeat8::STAGE_7_CV_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(111.258, 19.25)), module, Repeat8::STAGE_8_CV_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(-97.6, 25.033)), module, Repeat8::STEP_BACKWARDS_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.508, 38.838)), module, Repeat8::STAGE_1_REPEATS_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(25.033, 38.838)), module, Repeat8::STAGE_2_REPEATS_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(39.404, 38.838)), module, Repeat8::STAGE_3_REPEATS_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(53.775, 38.838)), module, Repeat8::STAGE_4_REPEATS_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(68.145, 38.838)), module, Repeat8::STAGE_5_REPEATS_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(82.516, 38.838)), module, Repeat8::STAGE_6_REPEATS_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(96.887, 38.838)), module, Repeat8::STAGE_7_REPEATS_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(111.258, 38.838)), module, Repeat8::STAGE_8_REPEATS_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.662, 78.012)), module, Repeat8::ONE_SHOT_MODE_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(-97.6, 53.775)), module, Repeat8::CLK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(-97.6, 68.145)), module, Repeat8::RESET_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(-78.012, 68.145)), module, Repeat8::RESET_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(-117.187, 82.516)), module, Repeat8::COMPARATOR_GATE_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(-117.187, 96.887)), module, Repeat8::COMPARATOR_GATE_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(-117.187, 111.258)), module, Repeat8::COMPARATOR_GATE_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.033, 117.187)), module, Repeat8::COUNTER_INCREASE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(68.145, 117.187)), module, Repeat8::COUNTER_RESET_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(117.187, -53.775)), module, Repeat8::COMPARATOR_OUTPUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.662, 58.425)), module, Repeat8::STAGE_1_GATE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.033, 58.425)), module, Repeat8::STAGE_2_GATE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.404, 58.425)), module, Repeat8::STAGE_3_GATE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(53.775, 58.425)), module, Repeat8::STAGE_4_GATE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(68.145, 58.425)), module, Repeat8::STAGE_5_GATE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(82.516, 58.425)), module, Repeat8::STAGE_6_GATE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(96.887, 58.425)), module, Repeat8::STAGE_7_GATE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(111.258, 58.425)), module, Repeat8::STAGE_8_GATE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.404, 78.012)), module, Repeat8::CV_OUTPUT_1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.404, 97.6)), module, Repeat8::CV_OUTPUT_2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(82.516, 97.6)), module, Repeat8::REPEAT_GATE_1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(96.887, 97.6)), module, Repeat8::REPEAT_GATE_2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(111.258, 97.6)), module, Repeat8::REPEAT_GATE_3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.404, 117.187)), module, Repeat8::COUNTER_CV_OUTPUT_OUTPUT));
	}
};


Model* modelRepeat8 = createModel<Repeat8, Repeat8Widget>("Repeat8");