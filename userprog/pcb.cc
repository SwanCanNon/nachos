
#include "synch.h"
class Thread;
class pcb {
        public:
                pcb (Thread *input);
                ~pcb ();
                int getID();
        private:
                int MAX_FILES;
                Thread *processThread;
                int processID;
                pcb * parent_process;
}
