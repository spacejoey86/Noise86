#include "plugin.hpp"


struct SeqOfChanges : Module {
	enum ParamId {
		LI_PARAM,
		XUN_PARAM,
		KUN_PARAM,
		ZHEN_PARAM,
		DUI_PARAM,
		GEN_PARAM,
		QIAN_PARAM,
		KAN_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CLK_INPUT,
		RESET_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHT_1_LIGHT,
		LIGHT_2_LIGHT,
		LIGHT_3_LIGHT,
		LIGHTS_LEN
	};

	SeqOfChanges() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(LI_PARAM, 0.f, 1.f, 0.f, "");
		configParam(XUN_PARAM, 0.f, 1.f, 0.f, "");
		configParam(KUN_PARAM, 0.f, 1.f, 0.f, "");
		configParam(ZHEN_PARAM, 0.f, 1.f, 0.f, "");
		configParam(DUI_PARAM, 0.f, 1.f, 0.f, "");
		configParam(GEN_PARAM, 0.f, 1.f, 0.f, "");
		configParam(QIAN_PARAM, 0.f, 1.f, 0.f, "");
		configParam(KAN_PARAM, 0.f, 1.f, 0.f, "");
		configInput(CLK_INPUT, "");
		configInput(RESET_INPUT, "");
		configOutput(OUT_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct SeqOfChangesWidget : ModuleWidget {
	SeqOfChangesWidget(SeqOfChanges* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/SeqOfChanges.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.688, 74.21)), module, SeqOfChanges::LI_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(14.949, 83.536)), module, SeqOfChanges::XUN_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.101, 83.598)), module, SeqOfChanges::KUN_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.252, 97.134)), module, SeqOfChanges::ZHEN_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(52.708, 97.134)), module, SeqOfChanges::DUI_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.212, 110.773)), module, SeqOfChanges::GEN_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.296, 110.602)), module, SeqOfChanges::QIAN_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(30.688, 120.694)), module, SeqOfChanges::KAN_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.458, 25.393)), module, SeqOfChanges::CLK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.828, 25.393)), module, SeqOfChanges::RESET_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(44.742, 25.305)), module, SeqOfChanges::OUT_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(30.48, 38.308)), module, SeqOfChanges::LIGHT_1_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(30.48, 49.802)), module, SeqOfChanges::LIGHT_2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(30.48, 61.296)), module, SeqOfChanges::LIGHT_3_LIGHT));
	}
};


Model* modelSeqOfChanges = createModel<SeqOfChanges, SeqOfChangesWidget>("SeqOfChanges");