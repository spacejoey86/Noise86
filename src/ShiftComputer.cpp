#include "plugin.hpp"

struct ShiftComputer : Module
{
	enum ParamId
	{
		ENUMS(ATTENS, 10),
		PARAMS_LEN
	};
	enum InputId
	{
		EXTERNAL_SOURCE,

		SOURCE_UP,
		DESTINATION_UP,
		SOURCE_DOWN,
		DESTINATION_DOWN,
		SOURCE_RESET,
		DESTINATION_RESET,

		SAMPLE_TRIG,
		INPUTS_LEN
	};
	enum OutputId
	{
		ENUMS(GATE, 10),
		POLY_OUTPUT,
		ENUMS(STAGE, 10),
		OUTPUTS_LEN
	};
	enum LightId
	{
		ENUMS(SOURCE, 10),
		ENUMS(DESTINATION, 10),
		LIGHTS_LEN
	};

	dsp::TSchmittTrigger<float> source_up_schmitt, destination_up_schmitt,
		source_down_schmitt, destination_down_schmitt,
		source_reset_schmitt, destination_reset_schmitt, sample_trig_schmitt;
	float sampled_voltages[10] = {10.f, 10.f, 10.f, 10.f, 10.f, 10.f, 10.f, 10.f, 10.f, 10.f};
	int selected_source = 9;
	int selected_destination = 7;

	ShiftComputer()
	{
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

		configInput(EXTERNAL_SOURCE, "External source");
		configInput(SOURCE_UP, "Source up trigger");
		configInput(DESTINATION_UP, "Destination up trigger");
		configInput(SOURCE_DOWN, "Source down trigger");
		configInput(DESTINATION_DOWN, "Destination down trigger");
		configInput(SOURCE_RESET, "Source reset trigger");
		configInput(DESTINATION_RESET, "Destination reset trigger");
		configInput(SAMPLE_TRIG, "Sample trigger");

		configOutput(POLY_OUTPUT, "Polyphonic");
		configOutput(GATE + 0, "Gate 1");
		configOutput(GATE + 1, "Gate 2");
		configOutput(GATE + 2, "Gate 3");
		configOutput(GATE + 3, "Gate 4");
		configOutput(GATE + 4, "Gate 5");
		configOutput(GATE + 5, "Gate 6");
		configOutput(GATE + 6, "Gate 7");
		configOutput(GATE + 7, "Gate 8");
		configOutput(GATE + 8, "Gate 9");
		configOutput(GATE + 9, "Gate 10");
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

	void process(const ProcessArgs &args) override
	{
		// calculate source and destination selection
		bool up = source_up_schmitt.process(inputs[SOURCE_UP].getVoltage(), 0.1f, 1.0f);
		bool down = source_down_schmitt.process(inputs[SOURCE_DOWN].getVoltage(), 0.1f, 1.0f);
		bool reset = source_reset_schmitt.process(inputs[SOURCE_RESET].getVoltage(), 0.1f, 1.0f);

		selected_source = (selected_source - up + 10) % 10;
		selected_source = (selected_source + down) % 10;
		if (reset) {
			selected_source = 9;
		}

		if (inputs[DESTINATION_UP].isConnected())
		{
			selected_destination = (selected_destination - destination_up_schmitt.process(inputs[DESTINATION_UP].getVoltage(), 0.1f, 1.0f) + 10) % 10;
		} else {
			selected_destination = (selected_destination - up + 10) % 10;
		}

		if (inputs[DESTINATION_DOWN].isConnected())
		{
			selected_destination = (selected_destination + destination_down_schmitt.process(inputs[DESTINATION_DOWN].getVoltage(), 0.1f, 1.0f)) % 10;
		} else {
			selected_destination = (selected_destination + down) % 10;
		}

		if (inputs[DESTINATION_RESET].isConnected()) {
			if (destination_reset_schmitt.process(inputs[DESTINATION_RESET].getVoltage(), 0.1f, 1.0f)) {
				selected_destination = 9;
			}
		} else {
			if (reset) {
				selected_destination = 9;
			}
		}

		// sample
		if (sample_trig_schmitt.process(inputs[SAMPLE_TRIG].getVoltage(), 0.1f, 1.0f))
		{
			if (selected_source == 0)
			{
				if (inputs[EXTERNAL_SOURCE].isConnected())
				{
					sampled_voltages[selected_destination] = inputs[EXTERNAL_SOURCE].getVoltage();
				}
				else
				{
					sampled_voltages[selected_destination] = 10.f;
				}
			}
			else
			{
				sampled_voltages[selected_destination] = sampled_voltages[selected_source - 1];
			}
		}

		// update outputs
		outputs[POLY_OUTPUT].setChannels(10);
		for (int i = 0; i < 10; i++)
		{
			lights[SOURCE + i].setBrightness(i == selected_source ? 1.0f : 0.0f);
			lights[DESTINATION + i].setBrightness(i == selected_destination ? 1.0f : 0.0f);
			float output_val = sampled_voltages[i] * params[ATTENS + i].getValue();
			outputs[GATE + i].setVoltage(i == selected_destination ? 10.0f : 0.0f);
			outputs[STAGE + i].setVoltage(output_val);
			outputs[POLY_OUTPUT].setVoltage(output_val, i);
		}
	}
};

struct ShiftComputerWidget : ModuleWidget
{
	ShiftComputerWidget(ShiftComputer *module)
	{
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/ShiftComputer.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(6.352, 41.481)), module, ShiftComputer::ATTENS + 0));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(16.149, 50.178)), module, ShiftComputer::ATTENS + 1));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(6.352, 58.875)), module, ShiftComputer::ATTENS + 2));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(16.149, 67.572)), module, ShiftComputer::ATTENS + 3));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(6.352, 76.269)), module, ShiftComputer::ATTENS + 4));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(16.149, 84.966)), module, ShiftComputer::ATTENS + 5));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(6.352, 93.663)), module, ShiftComputer::ATTENS + 6));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(16.149, 102.36)), module, ShiftComputer::ATTENS + 7));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(6.352, 111.057)), module, ShiftComputer::ATTENS + 8));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(16.149, 119.754)), module, ShiftComputer::ATTENS + 9));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.352, 21.602)), module, ShiftComputer::EXTERNAL_SOURCE));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.149, 21.602)), module, ShiftComputer::SOURCE_UP));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.945, 21.602)), module, ShiftComputer::DESTINATION_UP));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(35.742, 21.602)), module, ShiftComputer::SOURCE_DOWN));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(45.538, 21.602)), module, ShiftComputer::DESTINATION_DOWN));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(55.335, 21.602)), module, ShiftComputer::SOURCE_RESET));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(65.131, 21.602)), module, ShiftComputer::DESTINATION_RESET));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(74.928, 21.602)), module, ShiftComputer::SAMPLE_TRIG));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.928, 32.784)), module, ShiftComputer::POLY_OUTPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.742, 41.481)), module, ShiftComputer::GATE + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.538, 50.178)), module, ShiftComputer::GATE + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.742, 58.875)), module, ShiftComputer::GATE + 2));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.538, 67.572)), module, ShiftComputer::GATE + 3));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.742, 76.269)), module, ShiftComputer::GATE + 4));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.538, 84.966)), module, ShiftComputer::GATE + 5));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.742, 93.663)), module, ShiftComputer::GATE + 6));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.538, 102.36)), module, ShiftComputer::GATE + 7));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.742, 111.057)), module, ShiftComputer::GATE + 8));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.538, 119.754)), module, ShiftComputer::GATE + 9));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(65.131, 41.481)), module, ShiftComputer::STAGE + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.928, 50.178)), module, ShiftComputer::STAGE + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(65.131, 58.875)), module, ShiftComputer::STAGE + 2));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.928, 67.572)), module, ShiftComputer::STAGE + 3));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(65.131, 76.269)), module, ShiftComputer::STAGE + 4));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.928, 84.966)), module, ShiftComputer::STAGE + 5));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(65.131, 93.663)), module, ShiftComputer::STAGE + 6));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.928, 102.36)), module, ShiftComputer::STAGE + 7));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(65.131, 111.057)), module, ShiftComputer::STAGE + 8));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.928, 119.754)), module, ShiftComputer::STAGE + 9));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(6.352, 32.784)), module, ShiftComputer::SOURCE + 0));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(16.149, 41.481)), module, ShiftComputer::SOURCE + 1));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(6.352, 50.178)), module, ShiftComputer::SOURCE + 2));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(16.149, 58.875)), module, ShiftComputer::SOURCE + 3));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(6.352, 67.572)), module, ShiftComputer::SOURCE + 4));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(16.149, 76.269)), module, ShiftComputer::SOURCE + 5));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(6.352, 84.966)), module, ShiftComputer::SOURCE + 6));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(16.149, 93.663)), module, ShiftComputer::SOURCE + 7));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(6.352, 102.36)), module, ShiftComputer::SOURCE + 8));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(16.149, 111.057)), module, ShiftComputer::SOURCE + 9));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(74.928, 41.481)), module, ShiftComputer::DESTINATION + 0));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(65.131, 50.178)), module, ShiftComputer::DESTINATION + 1));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(74.928, 58.875)), module, ShiftComputer::DESTINATION + 2));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(65.131, 67.572)), module, ShiftComputer::DESTINATION + 3));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(74.928, 76.269)), module, ShiftComputer::DESTINATION + 4));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(65.131, 84.966)), module, ShiftComputer::DESTINATION + 5));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(74.928, 93.663)), module, ShiftComputer::DESTINATION + 6));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(65.131, 102.36)), module, ShiftComputer::DESTINATION + 7));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(74.928, 111.057)), module, ShiftComputer::DESTINATION + 8));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(65.131, 119.754)), module, ShiftComputer::DESTINATION + 9));
	}
};

Model *modelShiftComputer = createModel<ShiftComputer, ShiftComputerWidget>("ShiftComputer");