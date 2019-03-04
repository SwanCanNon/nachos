#ifndef PCB_H
#define PCB_H
#include "list.h"

class Thread;
class List;

class pcb {
        public:
                pcb(int processID);
                ~pcb();
		void setChildrenParentToNull();
		void removeChild(int exit);
		void addChild(pcb *inputProcess);
	
                Thread *processThread;
                int processID;
                pcb *parent_process;
		List *children;
		int exitValue;
};

void setParentToNull(int input);

#endif //PCB_H
