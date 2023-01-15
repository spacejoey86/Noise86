#include "plugin.hpp"


struct MultiLogic : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		G1IN1_INPUT,
		G1IN2_INPUT,
		G1IN3_INPUT,
		G2IN1_INPUT,
		G2IN2_INPUT,
		G2IN3_INPUT,
		G3IN1_INPUT,
		G3IN2_INPUT,
		G3IN3_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		G1G__OUTPUT,
		G1Q_OUTPUT,
		G2Q__OUTPUT,
		G2Q_OUTPUT,
		G3Q__OUTPUT,
		G3Q_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	MultiLogic() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(G1IN1_INPUT, "");
		configInput(G1IN2_INPUT, "");
		configInput(G1IN3_INPUT, "");
		configInput(G2IN1_INPUT, "");
		configInput(G2IN2_INPUT, "");
		configInput(G2IN3_INPUT, "");
		configInput(G3IN1_INPUT, "");
		configInput(G3IN2_INPUT, "");
		configInput(G3IN3_INPUT, "");
		configOutput(G1G__OUTPUT, "");
		configOutput(G1Q_OUTPUT, "");
		configOutput(G2Q__OUTPUT, "");
		configOutput(G2Q_OUTPUT, "");
		configOutput(G3Q__OUTPUT, "");
		configOutput(G3Q_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
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

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.525, 13.302)), module, MultiLogic::G1IN1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.525, 26.705)), module, MultiLogic::G1IN2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.525, 40.109)), module, MultiLogic::G1IN3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.525, 52.171)), module, MultiLogic::G2IN1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.525, 65.575)), module, MultiLogic::G2IN2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.525, 78.979)), module, MultiLogic::G2IN3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.525, 91.041)), module, MultiLogic::G3IN1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.525, 104.445)), module, MultiLogic::G3IN2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.525, 117.849)), module, MultiLogic::G3IN3_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.439, 20.004)), module, MultiLogic::G1G__OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.439, 33.407)), module, MultiLogic::G1Q_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.439, 58.873)), module, MultiLogic::G2Q__OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.439, 72.277)), module, MultiLogic::G2Q_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.439, 97.743)), module, MultiLogic::G3Q__OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.439, 111.147)), module, MultiLogic::G3Q_OUTPUT));
	}
};


Model* modelMultiLogic = createModel<MultiLogic, MultiLogicWidget>("MultiLogic");