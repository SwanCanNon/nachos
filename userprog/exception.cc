// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------
void 
SyscallYield() {
        AddrSpace *currentAddrSpace = currentThread->space;
        printf("System Call: %d invoked Yield", currentAddrSpace->PCB->processID);
        currentThread->Yield();
}

void 
SyscallExit() {
 	AddrSpace *currentAddrSpace = currentThread->space;
 	pcb *currentProcess = currentAddrSpace->PCB;
 	int exitValue = machine->ReadRegister(4);
	
 	currentProcess->setChildrenParentToNull();
	currentProcess->removeChild(exitValue);
        processManager->removePcb(currentProcess);
	
	printf("Process [%d] exits with status [%d].\n", currentProcess->processID, exitValue);
		
	currentAddrSpace->ReleaseMemory();
 	
	delete currentAddrSpace;
	delete currentProcess;
	
	currentThread->Finish();
 }

void 
ThreadFunction(int which){
	currentThread->space->RestoreRegisters();
	currentThread->space->RestoreState(); // restore mem
	machine->Run();
}

void 
SyscallFork() {
 	AddrSpace *currentAddrSpace = currentThread->space;
        printf("System Call: %d invoked Fork", currentAddrSpace->PCB->processID);
	
	currentAddrSpace->SaveRegisters();
	
	AddrSpace *forkedAddrSpace = currentAddrSpace->Copy();
	Thread *forkedThread = new Thread("Forked thread");
	pcb *forkedPcb = processManager->createPcb();

	if (forkedAddrSpace == NULL || forkedThread == NULL || forkedPcb == NULL){
		DEBUG('s', "Unable to fork because space, thread or pcb is null.\n");
                machine->WriteRegister(2, -1);
                delete forkedAddrSpace;
		delete forkedPcb;
		delete forkedThread;
                return;
	}
	
	forkedAddrSpace->PCB = forkedPcb;
	forkedThread->space = forkedAddrSpace;
	forkedPcb->processThread = forkedThread;
	
	//copy registers
	int processAddr = machine->ReadRegister(4);
	machine->WriteRegister(PCReg, processAddr);
	
	forkedThread->Fork(ThreadFunction, 1);
	
	currentAddrSpace->RestoreRegisters(); 
 
	machine->WriteRegister(2, forkedPcb->processID);
	machine->WriteRegister(NextPCReg, processAddr + 4);
	machine->WriteRegister(PrevPCReg, processAddr + 4);
 }

void 
SyscallExec() {
 	AddrSpace *currentAddrSpace = currentThread->space;
        printf("System Call: %d invoked Exec", currentAddrSpace->PCB->processID);
	
	char symbol;
	char eos = '\0';
	int counter = 0;
	int physicalAddr = 0;
	int path = machine->ReadRegister(4);
	char *fileName = new char[256];
	
	printf("Exec Program: [%d] loading [%c].\n", currentAddrSpace->PCB->processID, fileName);
	
	while (symbol != eos && counter < 256){
		currentAddrSpace->Translation(path, &physicalAddr, 1);
		if (physicalAddr != 0){
			bcopy(machine->mainMemory + physicalAddr, &symbol, 1);
			fileName[counter] = symbol;
			counter++;
			path = path + 4;
		}
	}

	OpenFile *executable = fileSystem->Open(fileName);

	if (executable == NULL){
		DEBUG('s', "Unable to execute because file is null.\n");
		machine->WriteRegister(2, -1);
		delete []fileName;
		return;
	}

	if (!currentAddrSpace->ReplaceContent(executable)){
                DEBUG('s', "Unable to replace current file on memory with executable.\n");
                machine->WriteRegister(2, -1);
                delete []fileName;
                return;
	}

	currentAddrSpace->InitRegisters();
	machine->WriteRegister(2, 1);
	
	delete executable;
	delete []fileName;
 }

void 
SyscallJoin() {
 	AddrSpace *currentAddrSpace = currentThread->space;
	pcb *currentProcess = currentAddrSpace->PCB;
        printf("System Call: %d invoked Join", currentAddrSpace->PCB->processID);
	int joinID = machine->ReadRegister(4);  
	
	if (joinID >= processManager->totalIDs || joinID < 0){
		DEBUG('s', "Join process failed, invalid ID.\n");
                machine->WriteRegister(2, -1);
                return;
	}

	pcb *joinPcb = processManager->getPcb(joinID);

	if (joinPcb == NULL){
		DEBUG('s', "Join process was not found to be in process manager.\n");
        	machine->WriteRegister(2, -1);
		return;
	}

	if (joinPcb->parent_process->processID == currentProcess->processID){
		while(!processManager->isFree(joinID)) {
  			DEBUG('s', "Join process is not finished.\n");
			SyscallYield();
		}
		machine->WriteRegister(2, joinPcb->exitValue);
	}
	
	DEBUG('s', "Join process not child of current process.\n");
	machine->WriteRegister(2, -1);
}

void 
SyscallKill() {
 	AddrSpace *currentAddrSpace = currentThread->space;
 	pcb *currentProcess = currentAddrSpace->PCB;
        printf("System Call: %d invoked Kill", currentAddrSpace->PCB->processID);

	int processKillID = machine->ReadRegister(4);

	if (processKillID >= processManager->totalIDs || processKillID){
                DEBUG('s', "Unable to kill process, process ID is invalid.\n");
                machine->WriteRegister(2, -1);
                return;
        }

	pcb *processToKill = processManager->getPcb(processKillID);

	if(processToKill == NULL){
		DEBUG('s', "Unable to kill process because it doesn't exist\n");
		printf("Process [%d] is unable to kill process [%d]: doesn't exist.\n", currentProcess->processID, processKillID);

		machine->WriteRegister(2, -1);
		return;
	}

	else {
		if(processKillID == currentProcess->processID){
			DEBUG('s', "Process to be killed is the current process. Call exit syscall\n");
                	SyscallExit();
			machine->WriteRegister(2, 0);
			return;
		}

		Thread *threadToKill = processToKill->processThread;
                AddrSpace *addrSpaceToKill = threadToKill->space;

		processToKill->setChildrenParentToNull();
		processToKill->removeChild(-1);
		processManager->removePcb(processToKill);
		addrSpaceToKill->ReleaseMemory();
		scheduler->RemoveThread(threadToKill);

		 printf("Process [%d] killed process [%d].\n", currentProcess->processID, processKillID);

		delete processToKill;
		delete addrSpaceToKill;
		delete threadToKill;

		machine->WriteRegister(2, 0);
	}
  }

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
/*
    if ((which == SyscallException) && (type == SC_Halt)) {
	DEBUG('a', "Shutdown, initiated by user program.\n");
   	interrupt->Halt();
    } else {
	printf("Unexpected user mode exception %d %d\n", which, type);
	ASSERT(FALSE);
    }*/

    switch (which)
    {
    	case SyscallException:
    		switch(type)
    		{
    			case SC_Halt:	//moved previous code into switch case
    			{
	    			DEBUG('a', "Shutdown, initiated by user program.\n");
	   				interrupt->Halt();
	   				break;
   				}

   				case SC_Exit:
   				{
   					DEBUG('a', "Requesting exit syscall.\n");
   					SyscallExit();
					break;
   				}

   				case SC_Exec:
   				{
   					DEBUG('a', "Requesting exec syscall.\n");
   					SyscallExec();
					break;
   				}

   				case SC_Join:
   				{
   					DEBUG('a', "Requesting join syscall.\n");
   					SyscallJoin();
					break;
   				}

   				case SC_Open:
   				{
   					DEBUG('a', "Requesting open syscall.\n");
   					break;
   				}

   				case SC_Read:
   				{
   					DEBUG('a', "Requesting read syscall.\n");
   					break;
   				}

   				case SC_Write:
   				{
   					DEBUG('a', "Requesting write syscall.\n");
   					break;
   				}

   				case SC_Close:
   				{
   					DEBUG('a', "Requesting close syscall.\n");
   					break;
   				}

   				case SC_Fork:
   				{
   					DEBUG('a', "Requesting fork syscall.\n");
   					SyscallFork();
					break;
   				}

   				case SC_Yield:
   				{
            				DEBUG('a', "Requesting a yield syscall.\n");
            				SyscallYield();
            				break;
   				}

          default: ASSERT(FALSE);

          IncrementPC();
    	}


    }
}
