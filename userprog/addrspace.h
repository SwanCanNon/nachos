// addrspace.h 
//	Data structures to keep track of executing user programs 
//	(address spaces).
//
//	For now, we don't keep any information about address spaces.
//	The user level CPU state is saved and restored in the thread
//	executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "pcb.h"
 
class pcb;
 
#define UserStackSize		1024 	// increase this as necessary!
class AddrSpace {
  public:
    AddrSpace(OpenFile *executable);	// Create an address space,
					// initializing it with the program
					// stored in the file "executable"
    AddrSpace(); 			// Create empty objet to be built to be initialized on fork    

    ~AddrSpace();			// De-allocate an address space

    void InitRegisters();		// Initialize user-level CPU registers,
					// before jumping to user cod
    void SaveState();			// Save/restore address space-specific
    
    void RestoreState();		// info on a context switch 
    
    void LoadCode(int virtualAddr, int size, int fileAddr, OpenFile* file);

    void ReleaseMemory();
	
    void Translation(int virtualAddress, int * physicalAdress, int size);

    bool ReplaceContent(OpenFile *executable); // New function for exec syscall, same as construtor but diff executable
	
    void SaveRegisters(); // New function for fork syscall
 
    void RestoreRegisters();
    
    AddrSpace* Copy(); // Copy address space for fork syscall
    
    pcb *PCB;
	
  private:
    TranslationEntry *pageTable;	// Assume linear page table translation
					// for now!
    unsigned int numPages;		// Number of pages in the virtual 
					// address space
    int tempRegs[NumTotalRegs];
};

#endif // ADDRSPACE_H
