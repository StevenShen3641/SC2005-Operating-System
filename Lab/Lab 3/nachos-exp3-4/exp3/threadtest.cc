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

//----------------------------------------------------------------------
// Inc and Dec execute value++ and value-- for twice.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

int value=0;
//case 0
void Inc(_int which)
{
	int a=value;
	a++;
	value=a;
	printf("**** Inc thread %d new value %d\n", (int) which, value);
}

void Dec(_int which)
{
	int a=value;
	a--;
	value=a;
	printf("**** Dec thread %d new value %d\n", (int) which, value);
}

//exercise 1: two Inc threads and two Dec threads, and implement the interleaving
//so that value=targetV when all the four threads ends.


//targetV=1;
//After executing TestValueOne(), the value should be one.
//1. implement the new version of Inc: Inc_v1
void Inc_v1(_int which)
{
	int a = value;
	currentThread -> Yield();
	a++;
	value = a;
	printf("**** Inc thread %d new value %d\n", (int)which, value);
}

//2. implement the new version of Dec: Dec_v1
void Dec_v1(_int which)
{
	int a = value;
	a--;
	value = a;
	printf("**** Dec thread %d new value %d\n", (int)which, value);
}

//3. implement TestValueOne by create two threads with Inc_v1 and two threads with Dec_v1
// you should pass the checking at the end, printing "congratulations! passed."
void TestValueOne()
{
	value=0;
	printf("enter TestValueOne, value=%d...\n", value);
	//1. fill your code here.
	Thread *t1 = new Thread("Increase_1");
	Thread *t2 = new Thread("Increase_2");
	Thread *t3 = new Thread("Decrease_1");
	Thread *t4 = new Thread("Decrease_2");
	t1->Fork(Inc_v1, 1, 1);
	t2->Fork(Inc_v1, 2, 1);
	t3->Fork(Dec_v1, 3, 1);
	t4->Fork(Dec_v1, 4, 1);
	currentThread->Join(t1);
	currentThread->Join(t2);
	currentThread->Join(t3);
	currentThread->Join(t4);

	//2. checking the value. you should not modify the code or add any code lines behind
	//this section.
	if(value==1)
	    printf("congratulations! passed.\n");
	else
		printf("value=%d, failed.\n", value);
}


//targetV=-2;
//After executing TestValueMinusTwo(), the value should be -2.
//1. implement the new version of Inc: Inc_v2
void Inc_v2(_int which)
{
	int a = value;
	a++;
	value = a;
	printf("**** Inc thread %d new value %d\n", (int)which, value);
}

//2. implement the new version of Dec: Dec_v2
void Dec_v2(_int which)
{
	int a = value;
	currentThread->Yield();
	a--;
	value = a;
	printf("**** Dec thread %d new value %d\n", (int)which, value);
}

//3. implement TestValueMinusOne by create two threads with Inc_v2 and two threads with Dec_v2
// you should pass the checking at the end, printing "congratulations! passed."
void TestValueMinusOne()
{
	value=0;
	printf("enter TestValueMinusOne, value=%d...\n", value);

	Thread *t1 = new Thread("Decrease_1");
	Thread *t2 = new Thread("Decrease_2");
	Thread *t3 = new Thread("Increase_1");
	Thread *t4 = new Thread("Increase_2");
	t1->Fork(Dec_v1, 1, 1);
	t2->Fork(Dec_v1, 2, 1);
	t3->Fork(Inc_v1, 3, 1);
	t4->Fork(Inc_v1, 4, 1);
	currentThread->Join(t1);
	currentThread->Join(t2);
	currentThread->Join(t3);
	currentThread->Join(t4);

	//2. checking the value. you should not modify the code or add any code lines behind
	//this section.
	if(value==-1)
		printf("congratulations! passed.\n");
	else
		printf("value=%d, failed.\n", value);
}


//Exercise 2: offer an implementation of Inc and Dec so that
//no matter what kind of interleaving occurs, the result value should be consistent.

//1. Declare any paramters here.

// create semaphore and lock, but we use lock here
Semaphore *semaphore = new Semaphore("Semaphore", 1); 
Lock *lock = new Lock("Lock");

//2. implement the new version of Inc: Inc_Consistent
void Inc_Consistent(_int which)
{
	lock->Acquire();
	int a = value;
	a++;
	value = a;
	printf("**** Inc thread %d new value %d\n", (int)which, value);
	lock->Release();
}

//3. implement the new version of Dec: Dec_Consistent
void Dec_Consistent(_int which)
{
	lock->Acquire();
	int a = value;
	a--;
	value = a;
	printf("**** Dec thread %d new value %d\n", (int)which, value);
	lock->Release();
}

//4. implement TestValueMinusOne by create two threads with Inc_Consistent and two threads with Dec_Consistent
// you should pass the checking at the end, printing "congratulations! passed."
void TestConsistency()
{
	value=0;
	printf("enter TestConsistency, value=%d...\n", value);

	Thread *t1 = new Thread("Increase_1");
	Thread *t2 = new Thread("Increase_2");
	Thread *t3 = new Thread("Decrease_1");
	Thread *t4 = new Thread("Decrease_2");
	t1->Fork(Inc_Consistent, 1, 1);
	t2->Fork(Inc_Consistent, 2, 1);
	t3->Fork(Dec_Consistent, 3, 1);
	t4->Fork(Dec_Consistent, 4, 1);
	currentThread->Join(t1);
	currentThread->Join(t2);
	currentThread->Join(t3);
	currentThread->Join(t4);


	//2. checking the value. you should not modify the code or add any code lines behind
	//this section.
	if(value==0)
		printf("congratulations! passed.\n");
	else
		printf("value=%d, failed.\n", value);
}

//select the function that you want to test.
void
ThreadTest()
{
	int loopTimes=0;
    DEBUG('t', "Entering SimpleTest");
	//for exercise 1.
    TestValueOne();
    TestValueMinusOne();
    //for exercise 2.
    TestConsistency();
}
