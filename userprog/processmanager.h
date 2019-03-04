#ifndef PROCESSMGR_H
#define PROCESSMGR_H
#define BITMAP_SIZE 3

#include "pcb.h"
#include "bitmap.h"
#include "synch.h"

class Lock;

class ProcessManager{
 public:
  	ProcessManager(int size = BITMAP_SIZE);
  	~ProcessManager();
  
 	pcb *createPcb();
  	void removePcb(pcb *inputProcess);
  	int getNumFreePcb();
  	bool isFree(int processID);
  	pcb *getPcb(int processID);

  	int totalIDs;  
  	BitMap *processIDs;
  	pcb **totalPcbs;
  	Lock *lock;
  
};
#endif
