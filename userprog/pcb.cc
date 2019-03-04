#include "pcb.h"

pcb::pcb(int ID){
	parent_process = 0;
        processID = ID;
	children = new List();
}

pcb::~pcb(){ 
	delete children;
}

void
setParentToNull(int input){
	pcb *input_process = (pcb *)input;
	input_process->parent_process = NULL;
}

void
pcb::setChildrenParentToNull(){
	if(!children->IsEmpty()){
	   children->Mapcar(setParentToNull);
	}
}

void
pcb::removeChild(int exit){
	if(parent_process != NULL){
	   children->SortedRemove(&parent_process->processID);	
	   parent_process->exitValue = exit; 	
	}	
}

void
pcb::addChild(pcb *inputProcess){
	children->SortedInsert((void *)inputProcess, inputProcess->processID);
}
