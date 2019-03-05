#include "copyright.h"
#include "memorymanager.h"

MemoryManager::MemoryManager(){
	virtMem = new BitMap(NumPhysPages);
	lock = new Lock("virtual page lock");
}

MemoryManager::~MemoryManager(){
	delete virtMem;
	delete lock;
}

int 
MemoryManager::getPage(){
	lock->Acquire();
	int pageNum = virtMem->Find();
	lock->Release();
return pageNum;
}

void
MemoryManager::clearPage(int pageId){
        lock->Acquire();
        virtMem->Clear(pageId);
        lock->Release();
}

int
MemoryManager::getNumFreePages(){
        lock->Acquire();
        int freePagesNum = virtMem->NumClear();
        lock->Release();
return freePagesNum;
}
