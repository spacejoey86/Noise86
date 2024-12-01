#include "plugin.hpp"


Plugin* pluginInstance;


void init(Plugin* p) {
	pluginInstance = p;

	// Add modules here
	// p->addModel(modelMyModule);
	p->addModel(modelTransQuant);
	p->addModel(modelMarkovDrummer);
	p->addModel(modelMultiLogic);
	p->addModel(modelEyeSeq);
	p->addModel(modelSeqOfChanges);
	p->addModel(modelRepeatBuddy);
	p->addModel(modelShiftComputer);

	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
