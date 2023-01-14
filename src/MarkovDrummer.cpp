#include "plugin.hpp"


struct MarkovDrummer : Module {
	enum ParamId {
		ENUMS(PARAMS, 16),
		PARAMS_LEN
	};
	enum InputId {
		CLK_INPUT,
		RESET_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		ENUMS(OUTS, 8),
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	dsp::PulseGenerator trigGens[8];
	dsp::TSchmittTrigger<float> clock, reset;
	int step = 0;

	MarkovDrummer() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PARAMS + 0, 0.f, 1.f, 0.f, "2→1");
		configParam(PARAMS + 1, 0.f, 1.f, 1.f, "2→3");
		configParam(PARAMS + 2, 0.f, 1.f, 0.f, "2→5");
		configParam(PARAMS + 3, 0.f, 1.f, 0.f, "2→7");
		configParam(PARAMS + 4, 0.f, 1.f, 0.f, "4→1");
		configParam(PARAMS + 5, 0.f, 1.f, 0.f, "4→3");
		configParam(PARAMS + 6, 0.f, 1.f, 1.f, "4→5");
		configParam(PARAMS + 7, 0.f, 1.f, 0.f, "4→7");
		configParam(PARAMS + 8, 0.f, 1.f, 0.f, "6→1");
		configParam(PARAMS + 9, 0.f, 1.f, 0.f, "6→3");
		configParam(PARAMS + 10, 0.f, 1.f, 0.f, "6→5");
		configParam(PARAMS + 11, 0.f, 1.f, 1.f, "6→7");
		configParam(PARAMS + 12, 0.f, 1.f, 1.f, "8→1");
		configParam(PARAMS + 13, 0.f, 1.f, 0.f, "8→3");
		configParam(PARAMS + 14, 0.f, 1.f, 0.f, "8→5");
		configParam(PARAMS + 15, 0.f, 1.f, 0.f, "8→7");
		configInput(CLK_INPUT, "Clock");
		configInput(RESET_INPUT, "Reset");
		configOutput(OUTS + 0, "Trig 1");
		configOutput(OUTS + 1, "Trig 2");
		configOutput(OUTS + 2, "Trig 3");
		configOutput(OUTS + 3, "Trig 4");
		configOutput(OUTS + 4, "Trig 5");
		configOutput(OUTS + 5, "Trig 6");
		configOutput(OUTS + 6, "Trig 7");
		configOutput(OUTS + 7, "Trig 8");
	}

	void process(const ProcessArgs& args) override {
		if (reset.process(inputs[RESET_INPUT].getVoltage(), 0.1f, 1.f)) {
			step = 0;
			trigGens[step].trigger(1e-3f);
		} else if (clock.process(inputs[CLK_INPUT].getVoltage(), 0.1f, 1.f)) {
			if (step % 2 == 0) {
				step += 1;
			} else {
				float chanceSum = 0;
				for (int row = 0; row < 4; row++) {
					chanceSum += params[PARAMS + 2 * (step - 1) + row].getValue();
				}
				float choice = random::uniform()*chanceSum; //choose a random number from 0 to chanceSum, inclusive of 0

				chanceSum = 0;
				for (int row = 0; row < 4; row++) {
					//step is 1, 3, 5 or 7
					chanceSum += params[PARAMS + 2 * (step - 1) + row].getValue();
					if (choice <= chanceSum) {
						step = row * 2;
						break;
					}
				}
			}
			trigGens[step].trigger(1e-3f);
		}

		// Output
		for (int out = 0; out < 8; out++) {
			bool trig = trigGens[out].process(args.sampleTime);
			outputs[OUTS + out].setVoltage(trig ? 10.f : 0.f);
		}
		
	}
};


struct MarkovDrummerWidget : ModuleWidget {
	MarkovDrummerWidget(MarkovDrummer* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/MarkovDrummer.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(16.458, 42.19)), module, MarkovDrummer::PARAMS + 0));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(16.458, 56.337)), module, MarkovDrummer::PARAMS + 1));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(16.458, 70.485)), module, MarkovDrummer::PARAMS + 2));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(16.458, 84.632)), module, MarkovDrummer::PARAMS + 3));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.828, 42.19)), module, MarkovDrummer::PARAMS + 4));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.828, 56.337)), module, MarkovDrummer::PARAMS + 5));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.828, 70.485)), module, MarkovDrummer::PARAMS + 6));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.828, 84.632)), module, MarkovDrummer::PARAMS + 7));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(45.199, 42.19)), module, MarkovDrummer::PARAMS + 8));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(45.199, 56.337)), module, MarkovDrummer::PARAMS + 9));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(45.199, 70.485)), module, MarkovDrummer::PARAMS + 10));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(45.199, 84.632)), module, MarkovDrummer::PARAMS + 11));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(59.57, 42.19)), module, MarkovDrummer::PARAMS + 12));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(59.57, 56.337)), module, MarkovDrummer::PARAMS + 13));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(59.57, 70.485)), module, MarkovDrummer::PARAMS + 14));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(59.57, 84.632)), module, MarkovDrummer::PARAMS + 15));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.458, 25.393)), module, MarkovDrummer::CLK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.828, 25.393)), module, MarkovDrummer::RESET_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(16.458, 98.78)), module, MarkovDrummer::OUTS + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(30.828, 98.78)), module, MarkovDrummer::OUTS + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.199, 98.78)), module, MarkovDrummer::OUTS + 2));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(59.57, 98.78)), module, MarkovDrummer::OUTS + 3));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(16.458, 112.927)), module, MarkovDrummer::OUTS + 4));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(30.828, 112.927)), module, MarkovDrummer::OUTS + 5));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.199, 112.927)), module, MarkovDrummer::OUTS + 6));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(59.57, 112.927)), module, MarkovDrummer::OUTS + 7));
	}
};


Model* modelMarkovDrummer = createModel<MarkovDrummer, MarkovDrummerWidget>("MarkovDrummer");