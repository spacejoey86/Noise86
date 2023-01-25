#include "plugin.hpp"


struct TransQuant : Module {
	enum ParamId {
		TRANSPOSE_PARAM,
		INTERVAL_PARAM,
		MODE_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		UP_INPUT,
		DOWN_INPUT,
		TRANSPOSE_INPUT,
		INTERVAL_INPUT,
		MODE_INPUT,
		ENUMS(QUANT_INS, 3),
		INPUTS_LEN
	};
	enum OutputId {
		ENUMS(TRIGS, 3),
		ENUMS(OUTS, 3),
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};
	
	dsp::PulseGenerator trigGens[3];
	dsp::TSchmittTrigger<float> up, down;
	float prevs[3];
	
	float internal_transpose;
	
	//chromatic scale, 3dp
	// first note is the last note an octave down, added 0 on the end to terminate search
	float scales[9][14] = {
			{-0.083, 0.000, 0.083, 0.167, 0.250, 0.333, 0.417, 0.500, 0.583, 0.667, 0.750, 0.833, 0.917, 0.000}, // Chromatic
			{-0.083, 0.000, 0.167, 0.250, 0.333, 0.417, 0.583, 0.750, 0.833, 0.917, 0.000}, // Blues
			{-0.083, 0.000, 0.167, 0.333, 0.417, 0.583, 0.750, 0.917, 0.000}, // Major/Ionian
			{-0.167, 0.000, 0.167, 0.250, 0.417, 0.583, 0.750, 0.833, 0.000}, //Dorian
			{-0.167, 0.000, 0.083, 0.250, 0.417, 0.583, 0.667, 0.833, 0.000}, //Phrygian
			{-0.083, 0.000, 0.167, 0.333, 0.500, 0.583, 0.667, 0.833, 0.000}, //Lydian
			{-0.167, 0.000, 0.167, 0.333, 0.417, 0.583, 0.750, 0.833, 0.000}, //Mixolydian
			{-0.167, 0.000, 0.167, 0.250, 0.417, 0.583, 0.667, 0.833, 0.000}, //Minor / Aolian
			{-0.167, 0.000, 0.083, 0.250, 0.417, 0.500, 0.667, 0.833, 0.000}  //Locrian
		};

	TransQuant() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		
		configParam(TRANSPOSE_PARAM, 0.f, 11.f, 0.f, "Transpose");
		paramQuantities[TRANSPOSE_PARAM]->snapEnabled = true;
		configParam(INTERVAL_PARAM, 0.f, 20.f, 5.f, "Interval");
		paramQuantities[INTERVAL_PARAM]->snapEnabled = true;
		configSwitch(MODE_PARAM, 0.f, 8.f, 0.f, "Mode",
			{"Chromatic", "Blues", "Major/Ionian", "Dorian", "Phrygian", "Lydian", "Mixolydian", "Minor/Aolian", "Locrian"});
		paramQuantities[MODE_PARAM]->snapEnabled = true;
		
		configInput(UP_INPUT, "Transpose up trig");
		configInput(DOWN_INPUT, "Transpose down trig");
		configInput(TRANSPOSE_INPUT, "Transpose CV");
		configInput(INTERVAL_INPUT, "Interval CV");
		configInput(MODE_INPUT, "Mode CV");
		configInput(QUANT_INS + 0, "CHNL 1");
		configInput(QUANT_INS + 1, "CHNL 2");
		configInput(QUANT_INS + 2, "CHNL 3");
		
		configOutput(TRIGS + 0, "CHNL 1 TRG");
		configOutput(TRIGS + 1, "CHNL 2 TRG");
		configOutput(TRIGS + 2, "CHNL 3 TRG");
		configOutput(OUTS + 0, "CHNL 1");
		configOutput(OUTS + 1, "CHNL 2");
		configOutput(OUTS + 2, "CHNL 3");
		
		configBypass(QUANT_INS + 0, OUTS + 0);
		configBypass(QUANT_INS + 1, OUTS + 1);
		configBypass(QUANT_INS + 2, OUTS + 2);
	}

	void process(const ProcessArgs& args) override {
		//scale input
		int scale = params[MODE_PARAM].getValue();
		if (inputs[MODE_INPUT].isConnected()) {
			scale = (int)inputs[MODE_INPUT].getVoltage();
			if (scale < 0) {
				scale = 0;
			}
			else if (scale > 8) {
				scale = 8;
			}
		}
		
		//transpose triggers
		float interval = params[INTERVAL_PARAM].getValue();
		if (inputs[INTERVAL_INPUT].isConnected()) {
			interval = inputs[INTERVAL_INPUT].getVoltage()*12;
		}
		if (up.process(inputs[UP_INPUT].getVoltage(), 0.1f, 1.f)) {
			internal_transpose += interval;
		}
		if (down.process(inputs[DOWN_INPUT].getVoltage(), 0.1f, 1.f)) {
			internal_transpose -= interval;
		}
		
		//sort out transposition inputs
		float transpose_in = inputs[TRANSPOSE_INPUT].getVoltage() + params[TRANSPOSE_PARAM].getValue()/12 + internal_transpose/12;
		float transpose = transpose_in - (int)floor(transpose_in);
		
		
		//main input
		for (int chnl=0; chnl < 3; chnl++) {
			float in = inputs[QUANT_INS + chnl].getVoltage();
			if (transpose >= 0.5) { //transpose input, going either up or down depending which is closer
				in -= transpose;
			}
			else {
				in += transpose;
			}
			//seperate into oct and note components
			int oct = (int)floor(in);
			float note = in - oct;
			
			//quantize to the scale
			int scale_index = 0;
			float diff = scales[scale][scale_index] - note;
			while (abs(scales[scale][scale_index + 1] - note) <= abs(diff)) {
				scale_index += 1;
				diff = scales[scale][scale_index] - note;
			}
			if (scale_index == 0) {
				oct -= 1;
			}
			float quant = scales[scale][scale_index] + oct;
			
			//process trigger out
			if (quant != prevs[chnl]) {
				trigGens[chnl].trigger(1e-3f);
			}
			bool trig = trigGens[chnl].process(args.sampleTime);
			outputs[TRIGS + chnl].setVoltage(trig ? 10.f : 0.f);
			prevs[chnl] = quant;
			
			//main output
			outputs[OUTS + chnl].setVoltage(quant);
		}
		
	}
};


struct TransQuantWidget : ModuleWidget {
	TransQuantWidget(TransQuant* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/transquant.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(33.724, 36.236)), module, TransQuant::TRANSPOSE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(33.724, 51.87)), module, TransQuant::INTERVAL_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(33.724, 67.579)), module, TransQuant::MODE_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.916, 20.749)), module, TransQuant::UP_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(33.724, 20.749)), module, TransQuant::DOWN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.916, 36.236)), module, TransQuant::TRANSPOSE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.916, 51.87)), module, TransQuant::INTERVAL_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.916, 67.579)), module, TransQuant::MODE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.916, 88.992)), module, TransQuant::QUANT_INS + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.916, 102.395)), module, TransQuant::QUANT_INS + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.916, 115.799)), module, TransQuant::QUANT_INS + 2));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(20.32, 88.992)), module, TransQuant::TRIGS + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(20.32, 102.395)), module, TransQuant::TRIGS + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(20.32, 115.799)), module, TransQuant::TRIGS + 2));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.724, 88.992)), module, TransQuant::OUTS + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.724, 102.395)), module, TransQuant::OUTS + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.724, 115.799)), module, TransQuant::OUTS + 2));
	}
};


Model* modelTransQuant = createModel<TransQuant, TransQuantWidget>("TransQuant");
