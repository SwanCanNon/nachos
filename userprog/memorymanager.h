#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "bitmap.h"
#include "synch.h"

class Lock;
class BitMap;

class MemoryManager {
        public:
                MemoryManager();
                ~MemoryManager();
                int getPage();
                void clearPage(int pageId);
		int getNumFreePages();
        private:
                BitMap *virtMem;
                Lock *lock;
};

#endif //MEMORYMANAGER_H
