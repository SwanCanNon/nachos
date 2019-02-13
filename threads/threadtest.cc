// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "synch.h"
// testnum is set in main.cc
int testnum = 1;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

//Version of SimpleThread where a lock is used to synchronize threads
#if defined(CHANGED) && defined(HW1_LOCKS)
int SharedVariable;
Lock *myLock;
int barrier;

void SimpleThread(int which) 
{
    DEBUG('t', "Entering SimpleThread - with locks");
    int num, val;
    
    if(myLock == NULL)
    {
        myLock = new Lock("myLock");
    }

    for (num = 0; num < 5; num++) {
        myLock->Acquire();
        val = SharedVariable;
        printf("*** thread %d sees value %d\n", which, val);
        currentThread->Yield();
        SharedVariable = val+1;
        myLock->Release();
        currentThread->Yield();
    }
    //Barrier is required to ensure all threads reach same final value
    barrier--;

    while (barrier != 0) {
        currentThread->Yield();
    }

    val = SharedVariable;
    printf("Thread %d sees final value %d\n", which, val);
}
//Version of SimpleThread where a semaphore is used to synchronize threads
#elif defined(CHANGED) && defined(HW1_SEMAPHORES)
int SharedVariable;
Semaphore *sem;
int barrier;

void SimpleThread(int which) 
{
    DEBUG('t', "Entering SimpleThread - with semaphore");
    int num, val;
    
    if(sem == NULL)
    {
        sem = new Semaphore("mySem", 1);
    }

    for (num = 0; num < 5; num++) {
        sem->P();
        val = SharedVariable;
        printf("*** thread %d sees value %d\n", which, val);
        currentThread->Yield();
        SharedVariable = val+1;
        sem->V();
        currentThread->Yield();
    }
    //Barrier is required to ensure all threads reach same final value
    barrier--;

    while (barrier != 0) {
        currentThread->Yield();
    }

    val = SharedVariable;
    printf("Thread %d sees final value %d\n", which, val);
}

//Unsynchronized version of modified SimpleThread
#elif defined(CHANGED) && defined(THREADS)
int SharedVariable;

void SimpleThread(int which) 
{
    DEBUG('t', "Entering SimpleThread");
    int num, val;
    for (num = 0; num < 5; num++) {
        val = SharedVariable;
        printf("*** thread %d sees value %d\n", which, val);
        currentThread->Yield();
        SharedVariable = val+1;
        currentThread->Yield();
    }
    val = SharedVariable;
    printf("Thread %d sees final value %d\n", which, val);
}

//Original SimpleThread
#else
void
SimpleThread(int which)
{
    DEBUG('t', "Entering SimpleThread");
    int num;
    
    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}
#endif
//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------
#if defined(CHANGED) && defined(THREADS)
void ThreadTest(int n) 
{
    DEBUG('t', "Entering ThreadTest");

    int i;
    //Barrier value is set here, to allow for changing number of threads
    #if defined(HW1_SEMAPHORES) || defined (HW1_LOCKS)
    barrier = n;
    #endif
    for(i = 0; i < n; i++) 
    {
        Thread *t = new Thread("forked thread");

        t->Fork(SimpleThread, i + 1);
    }

}
#else
void
ThreadTest()
{
    switch (testnum) {
    case 1:
	ThreadTest1();
	break;
    default:
	printf("No test specified.\n");
	break;
    }
}
#endif
