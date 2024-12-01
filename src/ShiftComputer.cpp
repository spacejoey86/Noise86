#include "plugin.hpp"


struct ShiftComputer : Module {
	enum ParamId {
		ENUMS(ATTENS, 10),
		PARAMS_LEN
	};
	enum InputId {
		EXTERNAL_SOURCE_INPUT,
		SOURCE_SELECT_INPUT,
		DESTINATION_SELECT_INPUT,
		SAMPLE_TRIG_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		ENUMS(STAGE,10),
		OUTPUTS_LEN
	};
	enum LightId {
		ENUMS(SOURCE, 10),
		ENUMS(DESTINATION, 10),
		LIGHTS_LEN
	};

	dsp::TSchmittTrigger<float> sample_trig_schmitt;
	float sampled_voltages [10] = {10.f, 10.f, 10.f, 10.f, 10.f, 10.f, 10.f, 10.f, 10.f, 10.f};
	int previous_destination = 0;

	ShiftComputer() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(ATTENS + 0, 0.f, 1.f, 1.f, "Stage 1 output attenuator");
		configParam(ATTENS + 1, 0.f, 1.f, 1.f, "Stage 2 output attenuator");
		configParam(ATTENS + 2, 0.f, 1.f, 1.f, "Stage 3 output attenuator");
		configParam(ATTENS + 3, 0.f, 1.f, 1.f, "Stage 4 output attenuator");
		configParam(ATTENS + 4, 0.f, 1.f, 1.f, "Stage 5 output attenuator");
		configParam(ATTENS + 5, 0.f, 1.f, 1.f, "Stage 6 output attenuator");
		configParam(ATTENS + 6, 0.f, 1.f, 1.f, "Stage 7 output attenuator");
		configParam(ATTENS + 7, 0.f, 1.f, 1.f, "Stage 8 output attenuator");
		configParam(ATTENS + 8, 0.f, 1.f, 1.f, "Stage 9 output attenuator");
		configParam(ATTENS + 9, 0.f, 1.f, 1.f, "Stage 10 output attenuator");

		configInput(EXTERNAL_SOURCE_INPUT, "External source");
		configInput(SOURCE_SELECT_INPUT, "Source select CV");
		configInput(DESTINATION_SELECT_INPUT, "Destination select CV");
		configInput(SAMPLE_TRIG_INPUT, "Sample trigger");

		configOutput(STAGE + 0, "Stage 1");
		configOutput(STAGE + 1, "Stage 2");
		configOutput(STAGE + 2, "Stage 3");
		configOutput(STAGE + 3, "Stage 4");
		configOutput(STAGE + 4, "Stage 5");
		configOutput(STAGE + 5, "Stage 6");
		configOutput(STAGE + 6, "Stage 7");
		configOutput(STAGE + 7, "Stage 8");
		configOutput(STAGE + 8, "Stage 9");
		configOutput(STAGE + 9, "Stage 10");
	}

	void process(const ProcessArgs& args) override {
		// calculate source and destination selection
		int selected_source = 0;
		if (inputs[SOURCE_SELECT_INPUT].isConnected()) {
			selected_source = 9 - (int) clamp(std::round(inputs[SOURCE_SELECT_INPUT].getVoltage()), 0.f, 9.f);
		}

		int selected_destination = selected_source;
		if (inputs[DESTINATION_SELECT_INPUT].isConnected()) {
			selected_destination = 9 - (int) clamp(std::round(inputs[DESTINATION_SELECT_INPUT].getVoltage()), 0.f, 9.f);
		}


		// sample
		if ((!inputs[SAMPLE_TRIG_INPUT].isConnected() && selected_destination != previous_destination) ||
			sample_trig_schmitt.process(inputs[SAMPLE_TRIG_INPUT].getVoltage(), 0.1f, 1.0f)) {
			if (selected_source == 0) {
				if (inputs[EXTERNAL_SOURCE_INPUT].isConnected()) {
					sampled_voltages[selected_destination] = inputs[EXTERNAL_SOURCE_INPUT].getVoltage();
				} else {
					sampled_voltages[selected_destination] = 10.f;
				}
			} else {
				sampled_voltages[selected_destination] = sampled_voltages[selected_source - 1];
			}

		}

		previous_destination = selected_destination;

		// update outputs
		for (int i = 0; i < 10; i++) {
			lights[SOURCE + i].setBrightness(i == selected_source ? 1.0f : 0.0f);
			lights[DESTINATION + i].setBrightness(i == selected_destination ? 1.0f : 0.0f);
			outputs[STAGE + i].setVoltage(sampled_voltages[i] * params[ATTENS + i].getValue());
		}
	}
};


struct ShiftComputerWidget : ModuleWidget {
	ShiftComputerWidget(ShiftComputer* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/ShiftComputer.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(5.625, 38.996)), module, ShiftComputer::ATTENS + 0));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.422, 47.693)), module, ShiftComputer::ATTENS + 1));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(5.625, 56.39)), module, ShiftComputer::ATTENS + 2));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.422, 65.087)), module, ShiftComputer::ATTENS + 3));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(5.625, 73.784)), module, ShiftComputer::ATTENS + 4));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.422, 82.481)), module, ShiftComputer::ATTENS + 5));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(5.625, 91.178)), module, ShiftComputer::ATTENS + 6));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.422, 99.875)), module, ShiftComputer::ATTENS + 7));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(5.625, 108.572)), module, ShiftComputer::ATTENS + 8));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.422, 119.754)), module, ShiftComputer::ATTENS + 9));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(5.625, 21.602)), module, ShiftComputer::EXTERNAL_SOURCE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.422, 21.602)), module, ShiftComputer::SOURCE_SELECT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.218, 21.602)), module, ShiftComputer::DESTINATION_SELECT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(35.015, 21.602)), module, ShiftComputer::SAMPLE_TRIG_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.218, 38.996)), module, ShiftComputer::STAGE + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.015, 47.693)), module, ShiftComputer::STAGE + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.218, 56.39)), module, ShiftComputer::STAGE + 2));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.015, 65.087)), module, ShiftComputer::STAGE + 3));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.218, 73.784)), module, ShiftComputer::STAGE + 4));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.015, 82.481)), module, ShiftComputer::STAGE + 5));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.218, 91.178)), module, ShiftComputer::STAGE + 6));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.015, 99.875)), module, ShiftComputer::STAGE + 7));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.218, 108.572)), module, ShiftComputer::STAGE + 8));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.015, 119.754)), module, ShiftComputer::STAGE + 9));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(5.625, 30.299)), module, ShiftComputer::SOURCE + 0));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.422, 38.996)), module, ShiftComputer::SOURCE + 1));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(5.625, 47.693)), module, ShiftComputer::SOURCE + 2));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.422, 56.39)), module, ShiftComputer::SOURCE + 3));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(5.625, 65.087)), module, ShiftComputer::SOURCE + 4));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.422, 73.784)), module, ShiftComputer::SOURCE + 5));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(5.625, 82.481)), module, ShiftComputer::SOURCE + 6));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.422, 91.178)), module, ShiftComputer::SOURCE + 7));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(5.625, 99.875)), module, ShiftComputer::SOURCE + 8));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.422, 108.572)), module, ShiftComputer::SOURCE + 9));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(35.015, 38.996)), module, ShiftComputer::DESTINATION + 0));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(25.218, 47.693)), module, ShiftComputer::DESTINATION + 1));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(35.015, 56.39)), module, ShiftComputer::DESTINATION + 2));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(25.218, 65.087)), module, ShiftComputer::DESTINATION + 3));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(35.015, 73.784)), module, ShiftComputer::DESTINATION + 4));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(25.218, 82.481)), module, ShiftComputer::DESTINATION + 5));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(35.015, 91.178)), module, ShiftComputer::DESTINATION + 6));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(25.218, 99.875)), module, ShiftComputer::DESTINATION + 7));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(35.015, 108.572)), module, ShiftComputer::DESTINATION + 8));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(25.218, 119.754)), module, ShiftComputer::DESTINATION + 9));
	}
};


Model* modelShiftComputer = createModel<ShiftComputer, ShiftComputerWidget>("ShiftComputer");