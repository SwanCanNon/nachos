#include "synch.h"
#include "bitmap.h"
class MemoryManager {
        public:
                MemoryManager (int numTotalPages);
                ~MemoryManager ();
                int getPage();
                void clearPage (int pageId);
        private:
                BitMap *virtMem;
                Lock *lock;
}
