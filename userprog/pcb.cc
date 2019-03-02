#include "pcb.h"

pcb::pcb(Thread *input){
        processThread = input;
	parent_process = 0;
        processID = getID();
        lock = new Lock("Process ID lock");
        IDMap = new BitMap(32);
	children = new List();
}

pcb::~pcb(){
        clearID();
        delete lock;
}

int
pcb::getID(){
	lock->Acquire();
	int IDNum = IDMap->Find();
	lock->Release();
return IDNum;
}

void 
pcb::clearID(){
	lock->Acquire();
	IDMap->Clear(processID);
	lock->Release();
}

void
pcb::setParentToNull(){
	if (!children->IsEmpty()){
			
	}
}

void
pcb::removeChild(){}
