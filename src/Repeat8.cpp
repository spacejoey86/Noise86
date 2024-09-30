#include "plugin.hpp"


struct Repeat8 : Module {
	enum ParamId {
		ENUMS(STAGE_CV, 8),
		ENUMS(REPEAT_CV, 8),
		ONE_SHOT_MODE_PARAM,
		CV_SCALE_PARAM,
		STEP_FORWARD_PARAM,
		STEP_BACKWARD_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		LENGTH_RESET_INPUT,
		CLK_INPUT,
		HARD_RESET_INPUT,
		COUNTER_INCREASE_INPUT,
		COUNTER_RESET_INPUT,
		ENUMS(COMPARATOR_GATE_INPUTS, 3),
		INPUTS_LEN
	};
	enum OutputId {
		ENUMS(STAGE_GATE_OUTPUTS, 8),
		CV_1_OUTPUT,
		CV_2_OUTPUT,
		ENUMS(REPEAT_GATE_OUTPUTS, 3),
		COUNTER_CV_OUTPUT,
		COMPARATOR_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		ENUMS(STAGE_LIGHTS, 8),
		STEP_FORWARD_LIGHT,
		STEP_BACKWARD_LIGHT,
		LIGHTS_LEN
	};

	dsp::TSchmittTrigger<float> clock_schmitt,
		backward_schmitt,
		forward_schmitt,
		length_reset_schmitt,
		hard_reset_schmitt,
		increase_schmitt,
		counter_reset_schmitt;
	int stage = 0;
	bool running = true;
	bool prev_one_shot_mode = false;

	int counter = 0;
	dsp::PulseGenerator comparator_gate_gen;

	Repeat8() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(STAGE_CV + 0, 0.f, 10.f, 0.f, "Stage 1 CV");
		configParam(STAGE_CV + 1, 0.f, 10.f, 0.f, "Stage 2 CV");
		configParam(STAGE_CV + 2, 0.f, 10.f, 0.f, "Stage 3 CV");
		configParam(STAGE_CV + 3, 0.f, 10.f, 0.f, "Stage 4 CV");
		configParam(STAGE_CV + 4, 0.f, 10.f, 0.f, "Stage 5 CV");
		configParam(STAGE_CV + 5, 0.f, 10.f, 0.f, "Stage 6 CV");
		configParam(STAGE_CV + 6, 0.f, 10.f, 0.f, "Stage 7 CV");
		configParam(STAGE_CV + 7, 0.f, 10.f, 0.f, "Stage 8 CV");

		configParam(REPEAT_CV + 0, 1.f, 8.f, 1.f, "Stage 1 repeats");
		paramQuantities[REPEAT_CV + 0]->snapEnabled = true;
		configParam(REPEAT_CV + 1, 1.f, 8.f, 1.f, "Stage 2 repeats");
		paramQuantities[REPEAT_CV + 1]->snapEnabled = true;
		configParam(REPEAT_CV + 2, 1.f, 8.f, 1.f, "Stage 3 repeats");
		paramQuantities[REPEAT_CV + 2]->snapEnabled = true;
		configParam(REPEAT_CV + 3, 1.f, 8.f, 1.f, "Stage 4 repeats");
		paramQuantities[REPEAT_CV + 3]->snapEnabled = true;
		configParam(REPEAT_CV + 4, 1.f, 8.f, 1.f, "Stage 5 repeats");
		paramQuantities[REPEAT_CV + 4]->snapEnabled = true;
		configParam(REPEAT_CV + 5, 1.f, 8.f, 1.f, "Stage 6 repeats");
		paramQuantities[REPEAT_CV + 5]->snapEnabled = true;
		configParam(REPEAT_CV + 6, 1.f, 8.f, 1.f, "Stage 7 repeats");
		paramQuantities[REPEAT_CV + 6]->snapEnabled = true;
		configParam(REPEAT_CV + 7, 1.f, 8.f, 1.f, "Stage 8 repeats");
		paramQuantities[REPEAT_CV + 7]->snapEnabled = true;

		configOutput(STAGE_GATE_OUTPUTS + 0, "Stage 1 gate");
		outputs[STAGE_GATE_OUTPUTS + 0].setVoltage(10.f);
		lights[STAGE_LIGHTS + 0].setBrightness(1.f);
		configOutput(STAGE_GATE_OUTPUTS + 1, "Stage 2 gate");
		configOutput(STAGE_GATE_OUTPUTS + 2, "Stage 3 gate");
		configOutput(STAGE_GATE_OUTPUTS + 3, "Stage 4 gate");
		configOutput(STAGE_GATE_OUTPUTS + 4, "Stage 5 gate");
		configOutput(STAGE_GATE_OUTPUTS + 5, "Stage 6 gate");
		configOutput(STAGE_GATE_OUTPUTS + 6, "Stage 7 gate");
		configOutput(STAGE_GATE_OUTPUTS + 7, "Stage 8 gate");

		configSwitch(ONE_SHOT_MODE_PARAM, 0.f, 1.f, 1.f, "One shot mode",
			{"One shot", "Repeating"});
		paramQuantities[ONE_SHOT_MODE_PARAM]->snapEnabled = true;
		configParam(CV_SCALE_PARAM, 0.f, 1.f, 0.2f, "CV output attenuator");
		configParam(STEP_FORWARD_PARAM, 0.f, 1.f, 0.f, "Step forward");
		configParam(STEP_BACKWARD_PARAM, 0.f, 1.f, 0.f, "Step backward");

		configInput(LENGTH_RESET_INPUT, "Length reset");
		configInput(CLK_INPUT, "Clock");
		configInput(HARD_RESET_INPUT, "Hard reset");
		configInput(COUNTER_INCREASE_INPUT, "Counter increase (main clock)");
		configInput(COUNTER_RESET_INPUT, "Counter reset (main reset)");
		configInput(COMPARATOR_GATE_INPUTS + 0, "Comparator gate 1");
		configInput(COMPARATOR_GATE_INPUTS + 1, "Comparator gate 2");
		configInput(COMPARATOR_GATE_INPUTS + 2, "Comparator gate 3");

		configOutput(CV_1_OUTPUT, "Main CV out");
		configOutput(CV_2_OUTPUT, "Repeat CV out");
		configOutput(REPEAT_GATE_OUTPUTS + 0, "Repeat gate 1");
		configOutput(REPEAT_GATE_OUTPUTS + 1, "Repeat gate 2");
		configOutput(REPEAT_GATE_OUTPUTS + 2, "Repeat gate 3");
		configOutput(COUNTER_CV_OUTPUT, "Counter CV");
		configOutput(COMPARATOR_OUTPUT, "Comparator gate");
	}

	void process(const ProcessArgs& args) override {
		//Sequencer part of the module
		// check if repeat mode has been enabled
		if (!prev_one_shot_mode && params[ONE_SHOT_MODE_PARAM].getValue() == 1.0f) {
			running = true;
		}
		prev_one_shot_mode = params[ONE_SHOT_MODE_PARAM].getValue();

		// handle both reset inputs
		if (length_reset_schmitt.process(inputs[LENGTH_RESET_INPUT].getVoltage(), 0.1f, 1.f)) {
			outputs[STAGE_GATE_OUTPUTS + stage].setVoltage(0.f);
			lights[STAGE_LIGHTS + stage].setBrightness(0.f);
			stage = 0;
			outputs[STAGE_GATE_OUTPUTS + 0].setVoltage(10.f);
			lights[STAGE_LIGHTS + 0].setBrightness(1.f);
			if (params[ONE_SHOT_MODE_PARAM].getValue() == 0.0f) {
				running = false;
			} else {
				running = true;
			}
		}
		if (hard_reset_schmitt.process(inputs[HARD_RESET_INPUT].getVoltage(), 0.1f, 1.f)) {
			running = true;
			outputs[STAGE_GATE_OUTPUTS + stage].setVoltage(0.f);
			lights[STAGE_LIGHTS + stage].setBrightness(0.f);
			stage = 0;
			outputs[STAGE_GATE_OUTPUTS + 0].setVoltage(10.f);
			lights[STAGE_LIGHTS + 0].setBrightness(1.f);
		}

		// handle clock input
		bool clock = false;
		if (inputs[CLK_INPUT].isConnected()) {
			clock = clock_schmitt.process(inputs[CLK_INPUT].getVoltage(), 0.1f, 1.f);
		} else {
			clock = clock_schmitt.process(outputs[COMPARATOR_OUTPUT].getVoltage(), 0.1f, 1.f);
		}
		// handle manual step buttons
		if (backward_schmitt.process(params[STEP_BACKWARD_PARAM].getValue(), 0.1f, 1.f)) {
			outputs[STAGE_GATE_OUTPUTS + stage].setVoltage(0.f);
			lights[STAGE_LIGHTS + stage].setBrightness(0.f);
			stage = (stage + 7) % 8;
			outputs[STAGE_GATE_OUTPUTS + stage].setVoltage(10.f);
			lights[STAGE_LIGHTS + stage].setBrightness(1.f);
		}
		if (forward_schmitt.process(params[STEP_FORWARD_PARAM].getValue(), 0.1f, 1.f)) {
			outputs[STAGE_GATE_OUTPUTS + stage].setVoltage(0.f);
			lights[STAGE_LIGHTS + stage].setBrightness(0.f);
			stage = (stage + 1) % 8;
			outputs[STAGE_GATE_OUTPUTS + stage].setVoltage(10.f);
			lights[STAGE_LIGHTS + stage].setBrightness(1.f);
		}
		// update sequencer
		if (running && clock) {
			outputs[STAGE_GATE_OUTPUTS + stage].setVoltage(0.f);
			lights[STAGE_LIGHTS + stage].setBrightness(0.f);
			stage = (stage + 1) % 8;
			if (params[ONE_SHOT_MODE_PARAM].getValue() == 0 && (stage == 0)) {
				running = false;
			}
			outputs[STAGE_GATE_OUTPUTS + stage].setVoltage(10.f);
			lights[STAGE_LIGHTS + stage].setBrightness(1.f);
		}
		outputs[CV_1_OUTPUT].setVoltage(params[STAGE_CV + stage].getValue()
			* params[CV_SCALE_PARAM].getValue());

		int repeat = params[REPEAT_CV + stage].getValue();
		outputs[CV_2_OUTPUT].setVoltage((float) repeat);
		outputs[REPEAT_GATE_OUTPUTS + 0].setVoltage((float) (((repeat - 1) & 0b001) > 0) * 10);
		outputs[REPEAT_GATE_OUTPUTS + 1].setVoltage((float) (((repeat - 1) & 0b010) > 0) * 10);
		outputs[REPEAT_GATE_OUTPUTS + 2].setVoltage((float) (((repeat - 1) & 0b100) > 0) * 10);


		//Counter and Comparator part of module
		// handle reset
		if (counter_reset_schmitt.process(inputs[COUNTER_RESET_INPUT].getVoltage(), 0.1f, 1.f)) {
			counter = 0;
		}

		// increase counter
		if (increase_schmitt.process(inputs[COUNTER_INCREASE_INPUT].getVoltage(), 0.1f, 1.f)) {
			counter = clamp(counter + 1, 0, 10);
		}

		// calculate binary input
		int threshold = 0;
		for (int i = 0; i < 3; i++) {
			if (inputs[COMPARATOR_GATE_INPUTS + i].isConnected()) {
				threshold += (inputs[COMPARATOR_GATE_INPUTS + i].getVoltage() > 2.f) << i;
			} else {
				threshold += (repeat - 1) & (0b001 << i);
			}
		}

		// compare counter to threshold
		if (counter > threshold) {
			comparator_gate_gen.trigger(1e-3f);
			if (!inputs[COUNTER_RESET_INPUT].isConnected()) {
				counter = 0;
			}
		}
		outputs[COMPARATOR_OUTPUT].setVoltage(comparator_gate_gen.process(args.sampleTime) ? 10.f : 0.f);
		outputs[COUNTER_CV_OUTPUT].setVoltage(counter);
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

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.662,  14.488)), module, Repeat8::STAGE_CV + 0));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(25.033,  14.488)), module, Repeat8::STAGE_CV + 1));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(39.404,  14.488)), module, Repeat8::STAGE_CV + 2));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(53.775,  14.488)), module, Repeat8::STAGE_CV + 3));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(68.145,  14.488)), module, Repeat8::STAGE_CV + 4));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(82.516,  14.488)), module, Repeat8::STAGE_CV + 5));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(96.887,  14.488)), module, Repeat8::STAGE_CV + 6));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(111.258, 14.488)), module, Repeat8::STAGE_CV + 7));

		addParam(createLightParamCentered<VCVLightSlider<RedLight>>(mm2px(Vec(10.508,  36.192)), module, Repeat8::REPEAT_CV + 0, Repeat8::STAGE_LIGHTS + 0));
		addParam(createLightParamCentered<VCVLightSlider<RedLight>>(mm2px(Vec(25.033,  36.192)), module, Repeat8::REPEAT_CV + 1, Repeat8::STAGE_LIGHTS + 1));
		addParam(createLightParamCentered<VCVLightSlider<RedLight>>(mm2px(Vec(39.404,  36.192)), module, Repeat8::REPEAT_CV + 2, Repeat8::STAGE_LIGHTS + 2));
		addParam(createLightParamCentered<VCVLightSlider<RedLight>>(mm2px(Vec(53.775,  36.192)), module, Repeat8::REPEAT_CV + 3, Repeat8::STAGE_LIGHTS + 3));
		addParam(createLightParamCentered<VCVLightSlider<RedLight>>(mm2px(Vec(68.145,  36.192)), module, Repeat8::REPEAT_CV + 4, Repeat8::STAGE_LIGHTS + 4));
		addParam(createLightParamCentered<VCVLightSlider<RedLight>>(mm2px(Vec(82.516,  36.192)), module, Repeat8::REPEAT_CV + 5, Repeat8::STAGE_LIGHTS + 5));
		addParam(createLightParamCentered<VCVLightSlider<RedLight>>(mm2px(Vec(96.887,  36.192)), module, Repeat8::REPEAT_CV + 6, Repeat8::STAGE_LIGHTS + 6));
		addParam(createLightParamCentered<VCVLightSlider<RedLight>>(mm2px(Vec(111.258, 36.192)), module, Repeat8::REPEAT_CV + 7, Repeat8::STAGE_LIGHTS + 7));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(10.662,  58.425)), module, Repeat8::STAGE_GATE_OUTPUTS + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.033,  58.425)), module, Repeat8::STAGE_GATE_OUTPUTS + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.404,  58.425)), module, Repeat8::STAGE_GATE_OUTPUTS + 2));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(53.775,  58.425)), module, Repeat8::STAGE_GATE_OUTPUTS + 3));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(68.145,  58.425)), module, Repeat8::STAGE_GATE_OUTPUTS + 4));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(82.516,  58.425)), module, Repeat8::STAGE_GATE_OUTPUTS + 5));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(96.887,  58.425)), module, Repeat8::STAGE_GATE_OUTPUTS + 6));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(111.258, 58.425)), module, Repeat8::STAGE_GATE_OUTPUTS + 7));
		
		addParam(createParamCentered<CKSS>(mm2px(Vec(10.662, 78.012)), module, Repeat8::ONE_SHOT_MODE_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(25.033, 78.012)), module, Repeat8::CV_SCALE_PARAM));
		addParam(createLightParamCentered<VCVLightBezel<RedLight>>(mm2px(Vec(25.033, 97.6)), module, Repeat8::STEP_FORWARD_PARAM, Repeat8::STEP_FORWARD_LIGHT));
		addParam(createLightParamCentered<VCVLightBezel<RedLight>>(mm2px(Vec(10.662, 97.6)), module, Repeat8::STEP_BACKWARD_PARAM, Repeat8::STEP_BACKWARD_LIGHT));

		//Main sequencer jacks
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(53.775, 97.6)), module, Repeat8::CLK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(68.145, 78.012)), module, Repeat8::LENGTH_RESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(68.145, 97.6)), module, Repeat8::HARD_RESET_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.404, 78.012)), module, Repeat8::CV_1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.404, 97.6)), module, Repeat8::CV_2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(82.516, 97.6)), module, Repeat8::REPEAT_GATE_OUTPUTS + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(96.887, 97.6)), module, Repeat8::REPEAT_GATE_OUTPUTS + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(111.258, 97.6)), module, Repeat8::REPEAT_GATE_OUTPUTS + 2));

		// Counter jacks
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.033, 117.187)), module, Repeat8::COUNTER_INCREASE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(68.145, 117.187)), module, Repeat8::COUNTER_RESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(82.516, 117.187)), module, Repeat8::COMPARATOR_GATE_INPUTS + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(96.887, 117.187)), module, Repeat8::COMPARATOR_GATE_INPUTS + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(111.258, 117.187)), module, Repeat8::COMPARATOR_GATE_INPUTS + 2));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.404, 117.187)), module, Repeat8::COUNTER_CV_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(53.775, 117.187)), module, Repeat8::COMPARATOR_OUTPUT));
	}
};


Model* modelRepeat8 = createModel<Repeat8, Repeat8Widget>("Repeat8");