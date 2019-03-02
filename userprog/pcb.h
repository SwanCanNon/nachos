#ifndef PCB_H
#define PCB_H

#include "bitmap.h"
#include "synch.h"

class Lock;
class BitMap;
class Thread;
class List;

class pcb {
        public:
                pcb(Thread *input);
                ~pcb();
                int getID();
                void clearID();
		void setParentToNull();
		void removeChild();
	
                Thread *processThread;
                int processID;
                pcb *parent_process;
                BitMap *IDMap;
                Lock *lock;
		List *children;
};
#endif //PCB_H
