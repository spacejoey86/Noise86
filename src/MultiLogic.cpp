#include "plugin.hpp"


struct MultiLogic : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		ENUMS(INPUTS, 9),
		INPUTS_LEN
	};
	enum OutputId {
		ENUMS(QOUT, 3),
		ENUMS(NQOUT, 3),
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	MultiLogic() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(INPUTS + 0, "In 1");
		configInput(INPUTS + 1, "In 2");
		configInput(INPUTS + 2, "In 3");
		configInput(INPUTS + 3, "In 4");
		configInput(INPUTS + 4, "In 5");
		configInput(INPUTS + 5, "In 6");
		configInput(INPUTS + 6, "In 7");
		configInput(INPUTS + 7, "In 8");
		configInput(INPUTS + 8, "In 9");
		configOutput(QOUT + 0, "Q");
		configOutput(QOUT + 1, "Q");
		configOutput(QOUT + 2, "Q");
		configOutput(NQOUT + 0, "Q'");
		configOutput(NQOUT + 1, "Q'");
		configOutput(NQOUT + 2, "Q'");
	}

	void process(const ProcessArgs& args) override {
		for (int LGate = 0; LGate < 3; LGate++) {
			bool result;
			if (inputs[INPUTS + 0 + 3*LGate].isConnected() &&
				inputs[INPUTS + 1 + 3*LGate].isConnected() &&
				! inputs[INPUTS + 2 + 3*LGate].isConnected()) {
				//XOR gate
				result = (inputs[INPUTS + 0 + 3*LGate].getVoltage() > 2.f) != (inputs[INPUTS + 1 + 3*LGate].getVoltage() > 2.f);

			} else if (! inputs[INPUTS + 0 + 3*LGate].isConnected() &&
						inputs[INPUTS + 1 + 3*LGate].isConnected() &&
						inputs[INPUTS + 2 + 3*LGate].isConnected()) {
				//AND gate
				result = (inputs[INPUTS + 1 + 3*LGate].getVoltage() > 2.f) && (inputs[INPUTS + 2 + 3*LGate].getVoltage() > 2.f);
			} else {
				//OR gate
				result = (inputs[INPUTS + 0 + 3*LGate].getVoltage() > 2.f) || (inputs[INPUTS + 1 + 3*LGate].getVoltage() > 2.f) || (inputs[INPUTS + 2 + 3*LGate].getVoltage() > 2.f);
			}
			outputs[QOUT + LGate].setVoltage(result ? 10.f : 0.f);
			outputs[NQOUT + LGate].setVoltage(result ? 0.f : 10.f);
		}
	}
};


struct MultiLogicWidget : ModuleWidget {
	MultiLogicWidget(MultiLogic* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/MultiLogic.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.525, 13.302)), module, MultiLogic::INPUTS + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.525, 26.705)), module, MultiLogic::INPUTS + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.525, 40.109)), module, MultiLogic::INPUTS + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.525, 52.171)), module, MultiLogic::INPUTS + 3));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.525, 65.575)), module, MultiLogic::INPUTS + 4));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.525, 78.979)), module, MultiLogic::INPUTS + 5));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.525, 91.041)), module, MultiLogic::INPUTS + 6));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.525, 104.445)), module, MultiLogic::INPUTS + 7));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.525, 117.849)), module, MultiLogic::INPUTS + 8));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.439, 20.004)), module, MultiLogic::NQOUT + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.439, 33.407)), module, MultiLogic::QOUT + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.439, 58.873)), module, MultiLogic::NQOUT + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.439, 72.277)), module, MultiLogic::QOUT + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.439, 97.743)), module, MultiLogic::NQOUT + 2));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.439, 111.147)), module, MultiLogic::QOUT + 2));
	}
};


Model* modelMultiLogic = createModel<MultiLogic, MultiLogicWidget>("MultiLogic");