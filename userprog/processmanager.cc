#include "processmanager.h"

ProcessManager::ProcessManager(int size){
	processIDs = new BitMap(size);
	lock = new Lock("ID Lock");
	totalPcbs = new pcb*[size];
	totalIDs = size;
}

ProcessManager::~ProcessManager() {
	delete lock;
	delete [] totalPcbs;
}

pcb 
*ProcessManager::createPcb() {
	int totalFreeIDs, newProcessID;
	pcb *newPcb;

	lock->Acquire();
	totalFreeIDs = processIDs->NumClear();
	lock->Release();

	if (totalFreeIDs == 0) {
		DEBUG('s', "No free slots for process.\n");
		return NULL;
	}
	
	lock->Acquire();
	newProcessID = processIDs->Find();
	newPcb = new pcb(newProcessID);
	totalPcbs[newProcessID] = newPcb;
	lock->Release();
	DEBUG('s',"Created a new process with ID [%d]\n", newPcb->processID);
return newPcb;
}

void
ProcessManager::removePcb(pcb *inputPCB) {
	lock->Acquire();
	processIDs->Clear(inputPCB->processID);
	totalPcbs[inputPCB->processID] = NULL;
	lock->Release();
}

pcb 
*ProcessManager::getPcb(int processID) {
	lock->Acquire();
	if (processIDs->Test(processID)) {
		pcb *foundPcb = totalPcbs[processID];
		DEBUG('s', "Found process we were looking for");
		lock->Release();
		return foundPcb;
	}
	
	DEBUG('s', "Didn't find the process we were looking for\n");
	lock->Release();
return NULL;
}

