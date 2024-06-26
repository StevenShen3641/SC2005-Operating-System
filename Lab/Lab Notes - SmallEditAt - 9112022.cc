NACHOS NOTES : SC2005 : NANYANG TECHNOLOGICAL UNIVERSITY : IDEALLY VIEW WITH MS PGOTHIC FONT SIZE 12 UTF-8 : BEST VIEWED IN NOTEPAD++
　　　　　　　　　　　　　　　　　　 　 　 ,.､
　　　　 　 　　 　_,,... -─-- ､.,_　　　/ヽ,ヽ,
　　　　　　,.. '"´　　　　　　　　　｀ヽ/::::/ヽ､>
　　　　 ／ 　　　　　　 　 　 　 　　/:::::::::／ヽ.
　　　 ,.' 　 ,　　　　　　　　　　　 ヽ!｀ヾイ:::::::::::!
　 　 ,' 　 /　　/ __ ! ./　|　/|__」_　',___!/Y´i￣
　　 i.　 ,' 　 7´i　/|/ 　 !/　!　 ﾉ｀ !　|　:　 !
　　 i 　 !　 /ｧ'´｀ヽ　　　　ｧ'"´ ｀ヽ!　|.　　 |
　　 ! 　 !　/i 　 （）　　　　　（）　 　ﾚ' |　 　 |
　　.レﾍﾚ',! `　　　　　　　 　　　- '/　!　　　|
　　 　 /⊂⊃　 　rｧ─- ､　　　⊂⊃.!　 i　 |
　　　 ,'　 ﾄ､,　　　!　　　　）　　 /.　 / 　 !　 !
　　　 i　ノイ i＞ .､.,,______　 　イi　 /　　ﾊ.　|
　 　  .レ'´ ﾚ'ヽ. 　.,.ｲ〈 i____,.ノ ,|／ﾚﾍ/　 V
　　　　　　　 　｀7 i::ヽﾚム　/:::::::/｀ヽ.
　　　　　　　　 /　 !::::::くﾝ-'::::::::/ 　 　 ':,
　　　　　　　／ 　,'::::::::::::::::::::::::i　　　 　 i

"I paid afew grand for this?" 
 


GENERAL NOTES:
/*
- Universal pointers used by many .cc files: see in system.cc

IE:
Thread *currentThread;			// the thread we are running now
Thread *threadToBeDestroyed;  		// the thread that just finished
Scheduler *scheduler;			// the ready list
Interrupt *interrupt;			// interrupt status
Statistics *stats;			// performance metrics
Timer *timer			// the hardware timer device,
					// for invoking context switches
^ All in system.cc

Use these pointers in place of 'this' when calling outside of function's usual place.

IE:
original in timer.cc:  interrupt->Schedule(TimerHandler, (_int) this, TimeOfNextInterrupt(), TimerInt); 
New: (void) interrupt->Schedule(TimerHandler, (_int)timer <-- universal pointer ,400,TimerInt);


- If need be, copy functions into other .cc file portions between '#ifdef CHANGED' and '#endif'.

IE:
#ifdef CHANGED
static void TimerHandler(_int arg)
{ Timer *p = (Timer *)arg; p->TimerExpired(); } <-- copied function
....
#endif

- WIP

*/


<EXPERIMENT 1>: // EXPERIMENT 1
FUNCTIONS TO BE STUDIED FOR EXPERIMENT 1:
/* 
thread.cc: Fork(),Thread(),Yield()
scheduler.cc: contains functions used by Fork() and Yield()
threadtest.cc: simplethread()

*/

// THEORY
void
SimpleThread(_int which)
{
    int num;
    
    for (num = 0; num < 3; num++) {
	printf("*** thread %d looped %d times\n", (int) which, num); // runs a printf saying which current thread has looped how many times
        currentThread->Yield(); // yields the current thread, this means:
		// 1. The first thread in the ready Q is fetched. (nextThread = scheduler->FindNextToRun();)
		// 2. current thread is put back to end of ready queue (scheduler->ReadyToRun(this); , see scheduler.cc)
		// 3. The fetched thread is run. (scheduler->Run(nextThread);) 
		
		// repeat untill each thread's simplethread function has run 3 times.
		/*
		like so: (main thread = 0 ,  '->' = yield())
		
		0 -> 1 -> 2 -> 0 -> 1 -> 2 -> 0 -> 1 -> 2
		
		see table.csv and output.txt
		
		*/		
    }
}


Thread *t1 = new Thread("child1"); // constructs a thread named 'child1'
t1->Fork(SimpleThread, 1, 0); 
// child1 thread, calling fork function, has the function simplethread queued into ready queue to run with arguement of '1' passed into function simplethread.
// the '0' ^ above in Fork() represents if a thread should be joined or not ie: wait untill t1 finishes executing, '0' == do not join, '1' join.
// see: Thread.cc Fork()

Thread *t2 = new Thread("child2"); // repeat with child2
    t2->Fork(SimpleThread, 2, 0);
	
SimpleThread(0); // main thread is now to run SimpleThread function with arguement 0 (ie: main thread is called thread 0)


// NOTES ON HOW EXPERIMENT 1 ENDS:
1.  With the last forked procedure yielding to the main thread 0 in the third loop, finish() is called. This runs: DEBUG('t', "Finishing thread %s #%i\n", getName(), pid);
2.  It then sets threadToBeDestroyed = currentThread; which is itself.
3.  It then runs sleep().
4.  In sleep(): DEBUG('t', "Sleeping thread %s #%i\n", getName(), pid); is run.
5. Since thread 1 n 2 are still in the Q, the while loop will not be triggered and it skips to: scheduler->Run(nextThread).
6. Under scheduler->Run(nextThread), DEBUG('t', "Switching from thread %s #%i to thread %s #%i\n",oldThread->getName(), oldThread->pid, nextThread->getName(),nextThread->pid); and 
DEBUG('t', "Now in thread %s #%i\n", currentThread->getName(), currentThread->pid); are run, this means the current thread is now 1.
7. Then:  delete threadToBeDestroyed; is run, which calls the thread destructor on thread 0.
8. This deletes thread 0, as shown with: DEBUG('t', "Deleting thread %s #%i\n", name, pid); running.
9. repeat steps 1-8 with thread 1.
10. At thread 2, the aforementioned while loop will be triggered. This runs: interrupt->Idle().
11.  Under Idle{}, this will run: DEBUG('i', "Machine idling; checking for interrupts.\n"); (Here no interrupts, interrupts are used in exp2)
12. Since there are no interrupts, these will run: DEBUG('i', "Machine idle.  No interrupts to do.\n");
    printf("No threads ready or runnable, and no pending interrupts.\n");
    printf("Assuming the program completed.\n");
13. This signifies there is nothing left and the program ends.
* Note that interrupts: on -> off means whether we allow interrupts happen or not.

// HOW CODE RUNS:
void
SimpleThread(_int which)
{
    int num;
    
    for (num = 0; num < 3; num++) {
	printf("*** thread %d looped %d times\n", (int) which, num); 
        currentThread->Yield();
		
		/*
		Tick 30: T0 runs printf("*** thread %d looped %d times\n", (int) which, num); , yield() queue's T0 (Sends it to the back of the Q) and fetches first item in Q which is thread T1.
		Tick 40: T1 runs printf("*** thread %d looped %d times\n", (int) which, num); , yield() queue's T1 and fetches first item in Q which is thread T2
		Tick 50: T2 runs printf("*** thread %d looped %d times\n", (int) which, num); , yield() queue's T2 and fetches first item in Q which is thread T0

		Tick 60: T0 runs printf("*** thread %d looped %d times\n", (int) which, num); , yield() queue's T0 and fetches first item in Q which is thread T1.
		Tick 70: T1 runs printf("*** thread %d looped %d times\n", (int) which, num); , yield() queue's T1 and fetches first item in Q which is thread T2
		Tick 80: T2 runs printf("*** thread %d looped %d times\n", (int) which, num); , yield() queue's T2 and fetches first item in Q which is thread T0
		
		Tick 90: T0 runs printf("*** thread %d looped %d times\n", (int) which, num); , yield() queue's T0 and fetches first item in Q which is thread T1.
		Tick 100: T1 runs printf("*** thread %d looped %d times\n", (int) which, num); , yield() queue's T1 and fetches first item in Q which is thread T2
		Tick 110: T2 runs printf("*** thread %d looped %d times\n", (int) which, num); , yield() queue's T2 and fetches first item in Q which is thread T0
		
		After 3 loops:
		Tick 120: T0 finish()->sleep()->(switch to T1)->delete T0
		Tick 130: T1 finish()->sleep()->(switch to T2)->delete T1
		Tick 140: T2 finish()->sleep()->idle()->nointerrupts = Kill Program.
		*/
    }
}

//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest");
    // each q will Q the function n id ie: simplethread , 1
    Thread *t1 = new Thread("child1");
    t1->Fork(SimpleThread, 1, 0); // Tick 10 Q T1
    Thread *t2 = new Thread("child2");
    t2->Fork(SimpleThread, 2, 0); // Tick 20 Q T2
    SimpleThread(0); // Tick 30 Run simplethread with main thread of id '0',    main_thread will be referred to as T0
}



<EXPERIMENT 2>: 
　　　　　　　　　　__,.､__,.､_
　　　　　　　 _ｒ'"7_,.!-‐'-.､7-､_
　　　　　　ｒ´ >'"　　　　　 ￣｀ヾ!､
　　　　　ｒゝ'´/　　 /i.　　 i　､　　Y
. /|　　 /Y　./　/メ､ﾊ　 /|　ﾊ　　 |
｜|　／ ﾉ／! イ　○ レ'　ﾚ_,. ﾚ'ヽ,ゝ
ｒ'-'┐/　_,ノ ﾉ|　 "　　　　 Ｏ |ｿ|　　　/|
｀l^ヽ､ﾍ⌒ヽr,ﾍ.　　/´｀ヽ 　"从 ',　 / .|    <Now i've lost it, I know I can kill, the truth lies behind the GPA>
　ヽ､_ﾉ7 　(Y,/」＞!､_,／_,. イﾉﾍ〉ｒ/／
　　　｀ｰ-∠ﾄ､7:::|／ﾑヽﾉ:::i´｀7"´__ﾝ､>
　　　ノ＼_rﾍく:::::::::::ﾊ::::::::/-‐'￣
　　 ｒ'ｱ'"::::｀''ｰｧ二ﾆ=-､ｲ､7
　ﾉ7:::::／::::::::/|} 　ノ／⌒ヽ>､
　}」／::::::::::::/:::}|___,ンi二二!-'
　 iヽ､_:::__ンﾆ='-‐=-'!､__.ノ
　 ｀7二｀／´
　　〈__ン

// EXPERIMENT 2
FUNCTIONS TO BE STUDIED FOR EXPERIMENT 2:
// Thread.cc, thread.h
Finish()
Yield()

//system.cc, timer.cc and timer.h
TimerInterruptHandler() <--system.cc
TimerExpired()
TimeOfNextInterrupt()

// interrupt.h and interrupt.cc
Schedule()
OneTick();
CheckIfDue()
YieldOnReturn()

// How functions interact with one another, based on part b):
When a timer interrupt is to occur (ie: at 40):
1. OneTick() is run, which in turn runs CheckIfDue() as apart of a while loop. 
2. CheckIfDue() then runs: DEBUG('i', "Invoking interrupt handler for the %s at time %d\n", intTypeNames[toOccur->type], toOccur->when).
3. As CheckIfDue() runs, it will eventually call the TimerHandler() from timer.cc with: (*(toOccur->handler))(toOccur->arg).
4. TimerHandler() then calls TimerExpired()
5. TimerExpired() then schedules the next Timer interrupt at present_time+TimeOfNextInterrupt(), this is shown when the interrupt->Schedule(..) section runs: DEBUG('i', "Scheduling interrupt handler the %s at time = %d\n", intTypeNames[type], when);
6. TimerExpired() then also calls TimerInterruptHandler() from system.cc with: (*handler)(arg);
7. TimerInterruptHandler() then calls YieldOnReturn(), which sets yieldOnReturn to true.
8. Since nothing further is called, we jump all the way back to CheckIfDue(), which then returns true.
9. As CheckIfDue() returns true, the while loop in part 1. will loop once, then in the next loop CheckIfDue() returns false since the pending interrupt has already been delivered. This breaks the while loop.
10.  Since yieldOnReturn has been set to true, we enter the if else section in OneTick(), which then runs yield().
11. When yield() is run, this sends the main thread to the ready queue and fetches the first thread in the ready Q to be run.
12. This allows for the forked threads to be run essentially.
13. When each forked thread ends, it runs finish(), which resets the next timer interrupt, and then calls sleep()
14. sleep() then calls for the next thread in the queue to be run. 
(I have no idea why, but only the last thread to be called to sleep before main is specifically deleted by a destructor. probably due to how scheduler.cc->Run()->SWITCH(oldThread, nextThread); snippet works.)
(It is a mystery)
15. This process is repeated untill the main thread is run again.
16. Once at the main thread, the next bunch of threads will be forked. (Since we are now back at the main section of the code, see bottom for better explaination)
17. At some point the next timer interrupt will be due to occur, then we go back to step 1. repeat.


PART A):

// THEORY

// Because part A wants us to trigger an interrupt every 40 seconds, this means at every 40 second interval:
// The interrupt will be triggered
// when first interrupt is triggered, all queued thread's functions (Simplethread) will be run one by one.
// The second interrupt will be scheduled 40 seconds later.
// each run thanks to for loop 'for (num = 0; num < 1000000; num++) {}' will take 10 ticks.
// thus for each 10 tick one thread will be completed
// At the second interrupt, we will jump back into the main code, where will fork the next 3 threads to Q (add thread's functions to Q).
// The third interrupt will be scheduled 40 seconds later.
// Each addition to the Q takes 10 ticks.
// At the third interrupt, same thing happens as the first.
// At the fourth interrupt, same thing happens as the second.
// Repeat untill we reach SimpleThread(0).
/*
Tick 10 - 30: Fork (queue) threads 1-3 and their functions (SimpleThread)
Tick 40: invokve interrupt, schedule next interrupt, switch to running queued threads (1-3) by calling yield().
Tick 50-70: run queued threads (1-3), at last thread, finish() calls sleep(), which switches us back to the main program for forking threads 4-6.
Tick 80: invokve interrupt, schedule next interrupt, call yield() <- waste
Tick 90-110:  fork threads 4-6
Tick 120: invokve interrupt, schedule next interrupt, switch to running queued threads (4-6) by calling yield().
Tick 130-150: run queued threads (4-6), at last thread, finish() calls sleep(), which switches us back to the main program for forking threads 7-9.
Tick 160: invokve interrupt, schedule next interrupt, call yield() <- waste
Tick 170-190: fork threads 7-9
Tick 200: invokve interrupt, schedule next interrupt, switch to running queued threads(7-9) by calling yield().
Tick 210-230: run queued threads(7-9), at last thread, finish() calls sleep(), which switches us back to the main program.
Tick 240: invokve interrupt, schedule next interrupt, call yield() <- waste
Tick 250: run main thread with function SimpleThread ( SimpleThread(0);). Since nothing left, program completed
*/

// CODE TO BE EDITED

//Under system.cc, set randomYield to TRUE:
 bool randomYield = TRUE; // timer is activated by this? i changed this.
 // This will permanently activate the function we need in Timer.cc
 
 // Next, in Timer.cc copy this section in Timer() into TimerExpired().
  interrupt->Schedule(TimerHandler, (_int) this, TimeOfNextInterrupt(), 
		TimerInt); 
// This will ensure TimerExpired will always schedule the next interrupt after a current interrupt is invoked..

// FInally, under TimeOfNextInterrupt(), set return to permanently 40, or comment out the section under if(randomize) and set it to permanently return 40, like so:
if (randomize)
	return 40; // 1 + (Random() % (TimerTicks * 2)); <- original, changed to return 40
// since randomize = true for timer to be activated
    else
	return TimerTicks; 
// This ensures, since random is set to True pemanently, that 40 ticks will always be returned when timer expires in an interrupt invocation.


PART B):

// THEORY

// In part b), whenever a thread is finished running (ie:  printf("Thread %d Completed.\n",(int)which);), they want the scheduled timer interrupt to be reset to the current tick + 40 ticks, ie:
// if in tick 50, thread 1 finishes. the original scheduled timer interrupt at 80 seconds will now be at 90 seconds..
// This saves on ticks, as when the last thread that is not 'main thread' in Q runs, it also resets the scheduled next interrupt time, that means in the next 10 Ticks:
// it can go straight to forking, without the need to spend a whole 10 Ticks invoke interrupt, schedule the next interrupt and running yield() . like in part a)
// thus overall shorter time needed
/*
Tick 10-30: Fork 1-3
Tick 40: Invoke Interrupt, next at 80, switch to running queued threads by calling yield().
Tick 50: Finish thread1, reset interrupt to 40 ticks later (Interrupt at 90)
Tick 60: Finish thread2, Interrupt reset to 100
Tick 70: Finish thread3, Interrupt reset to 110, at last thread, finish() calls sleep(), which switches us back to the main program for forking threads
Tick 80 - 100: Fork 4-6                                  <-- Tick of 10 saved here
Tick 110: Invoke Interrupt, next at 150, switch to running queued threads by calling yield().
Tick 120: Finish thread4, Interrupt reset to 160
Tick 130: Finish thread5, Interrupt reset to 170
Tick 140: Finish thread6, Interrupt reset to 180, at last thread, finish() calls sleep(), which switches us back to the main program for forking threads
Tick 150-170:  Fork 7-9
Tick 180: Invoke Interrupt, next at 220, switch to running queued threads by calling yield().
Tick 190: Finish a thread7, Interrupt reset to 230
Tick 200: Finish a thread8, Interrupt reset to 240
Tick 210: Finish a thread9, Interrupt reset to 250, at last thread, finish() calls sleep(), which switches us back to the main program.
Tick 220: Run main thread with function SimpleThread ( SimpleThread(0);). Since nothing left, program completed
<30 ticks saved!>

*/  

// CODE TO BE EDITED

// Under threads.cc, find the finish() function portion, then:
// Step 1. add in this code chunk after #ifdef CHANGED:
#ifdef CHANGED			
static void TimerHandler(_int arg)
{ Timer *p = (Timer *)arg; p->TimerExpired(); }
// This is a copy of a function that we will need to use.

// Step 2.  Under the finish() function, under both if else results, insert this one line above sleep():
(void) interrupt->Schedule(TimerHandler, (_int)timer ,400,TimerInt);
// This snippet will tell the universal interrupt pointer (interrupt->) to :
// 1. Feed the universal timer pointer (timer) to TimerHandler().
// 2. TimerHandler() (using universal timer pointer)  signals that the current interrupt is expired and due to be replaced.
// 3. That the next interrupt is to be 400 seconds from now, and that it is Type TimerInt (you may simply set this to 40 seconds instead)


// Under interrupt.cc, check the schedule() function:
// Add this snippet if you used 400 seconds from now:
 if(fromNow == 400){
    delete pending->Remove();
    fromNow = fromNow / 10;} // '400' signifies that this schedule call was from a  thread finishing, and thus must be corrected.
	
// Add this if you used 40 seconds:
delete pending->Remove(); // This deletes the present scheduled interrupt.

// The rest of the schedule() function will schedule a new interrupt at present time + 40, thus fulfilling our requirements.


// HOW CODE RUNS:
Part A)

void
SimpleThread(_int which)
{
    int num;
    
    for (num = 0; num < 1000000; num++) {}
    printf("Thread %d Completed.\n",(int)which);
}

void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest"); // Interrupt Scheduled at 40
    
    Thread *t1 = new Thread("child1");
    t1->Fork(SimpleThread, 1, 0); // Tick 10 Q T1
    Thread *t2 = new Thread("child2");
    t2->Fork(SimpleThread, 2, 0); // Tick 20 Q T2
    Thread *t3 = new Thread("child3");
    t3->Fork(SimpleThread, 3, 0); // Tick 30 Q T3
	
	// TICK 40 - INTERRUPT - NEXT SCHEDULED AT 80
	// YIELD() RUNS:  T1 <- FIRST ITEM IN Q RETRIEVED, MAIN_THREAD ADDED TO BACK OF Q, RUNS T1
	// T1 RUN SIMPLETHREAD FUNCTION, FINISH(), SLEEP(), PASS ON TO T2
	// T2 RUN SIMPLETHREAD FUNCTION, FINISH(), SLEEP(), PASS ON TO T3
	// T3 RUN SIMPLETHREAD FUNCTION, FINISH(), SLEEP(), T3 DELETED, PASS ON TO MAIN_THREAD
	
	// PROGRAM RETURNS BACK TO HERE
	// TICK 80 - INTERRUPT - NEXT SCHEDULED AT 120
	
    Thread *t4 = new Thread("child4");
    t4->Fork(SimpleThread, 4, 0); // Tick 90 Q T4
    Thread *t5 = new Thread("child5");
    t5->Fork(SimpleThread, 5, 0); // Tick 100 Q T5
    Thread *t6 = new Thread("child6");
    t6->Fork(SimpleThread, 6, 0); // Tick 110 Q T6
	
	// TICK 120 - INTERRUPT - NEXT SCHEDULED AT 160
	// YIELD() RUNS:  T4 <- FIRST ITEM IN Q RETRIEVED, MAIN_THREAD ADDED TO BACK OF Q, RUNS T4
	// T4 RUN SIMPLETHREAD FUNCTION, FINISH(), SLEEP(), PASS ON TO T5
	// T5 RUN SIMPLETHREAD FUNCTION, FINISH(), SLEEP(), PASS ON TO T6
	// T6 RUN SIMPLETHREAD FUNCTION, FINISH(), SLEEP(), T6 DELETED, PASS ON TO MAIN_THREAD
	
	// PROGRAM RETURNS BACK TO HERE
	// TICK 160 - INTERRUPT - NEXT SCHEDULED AT 200
	
    Thread *t7 = new Thread("child7");
    t7->Fork(SimpleThread, 7, 0); // Tick 170 Q T7
    Thread *t8 = new Thread("child8");
    t8->Fork(SimpleThread, 8, 0); // Tick 180 Q T8
    Thread *t9 = new Thread("child9");
    t9->Fork(SimpleThread, 9, 0); // Tick 190 Q T9
	
	
	// TICK 200 - INTERRUPT - NEXT SCHEDULED AT 240
	// YIELD() RUNS:  T7 <- FIRST ITEM IN Q RETRIEVED, MAIN_THREAD ADDED TO BACK OF Q, RUNS T7
	// T7 RUN SIMPLETHREAD FUNCTION, FINISH(), SLEEP(), PASS ON TO T8
	// T8 RUN SIMPLETHREAD FUNCTION, FINISH(), SLEEP(), PASS ON TO T9
	// T9 RUN SIMPLETHREAD FUNCTION, FINISH(), SLEEP(), T9 DELETED, PASS ON TO MAIN_THREAD
	
	// PROGRAM RETURNS BACK TO HERE
	// TICK 240 - INTERRUPT - NEXT SCHEDULED AT 280


    SimpleThread(0); // Tick 250: run this, program ends. as idle determines no threads, interrupts or anything in Q left.
}

Part B)

void
SimpleThread(_int which)
{
    int num;
    
    for (num = 0; num < 1000000; num++) {}
    printf("Thread %d Completed.\n",(int)which);
}

void
ThreadTest()
{
    DEBUG('t', "Entering SimpleTest"); // Interrupt Scheduled at 40
    
    Thread *t1 = new Thread("child1");
    t1->Fork(SimpleThread, 1, 0); // Tick 10 Q T1
    Thread *t2 = new Thread("child2");
    t2->Fork(SimpleThread, 2, 0); // Tick 20 Q T2
    Thread *t3 = new Thread("child3");
    t3->Fork(SimpleThread, 3, 0); // Tick 30 Q T3
	
	// TICK 40 - INTERRUPT - NEXT SCHEDULED AT 80
	// YIELD() RUNS:  T1 <- FIRST ITEM IN Q RETRIEVED, MAIN_THREAD ADDED TO BACK OF Q, RUNS T1
	// TICK 50 - T1 RUN SIMPLETHREAD FUNCTION, FINISH(), SLEEP(), PASS ON TO T2, NEXT INTERRUPT SCHEDULED AT 90
	// TICK 60 - T2 RUN SIMPLETHREAD FUNCTION, FINISH(), SLEEP(), PASS ON TO T3, NEXT INTERRUPT SCHEDULED AT 100
	// TICK 70 - T3 RUN SIMPLETHREAD FUNCTION, FINISH(), SLEEP(), T3 DELETED, PASS ON TO MAIN_THREAD, NEXT INTERRUPT SCHEDULED AT 110
	
	// PROGRAM RETURNS BACK TO HERE
	// nothing runs here: ticks saved
	
    Thread *t4 = new Thread("child4");
    t4->Fork(SimpleThread, 4, 0); // Tick 80 Q T4
    Thread *t5 = new Thread("child5");
    t5->Fork(SimpleThread, 5, 0); // Tick 90 Q T5
    Thread *t6 = new Thread("child6");
    t6->Fork(SimpleThread, 6, 0); // Tick 100 Q T6
	
	// TICK 110 - INTERRUPT - NEXT SCHEDULED AT 150
	// YIELD() RUNS:  T4 <- FIRST ITEM IN Q RETRIEVED, MAIN_THREAD ADDED TO BACK OF Q, RUNS T4
	// TICK 120 - T4 RUN SIMPLETHREAD FUNCTION, FINISH(), SLEEP(), PASS ON TO T5, NEXT INTERRUPT SCHEDULED AT 160
	// TICK 130 - T5 RUN SIMPLETHREAD FUNCTION, FINISH(), SLEEP(), PASS ON TO T6, NEXT INTERRUPT SCHEDULED AT 170
	// TICK 140 - T6 RUN SIMPLETHREAD FUNCTION, FINISH(), SLEEP(), T6 DELETED, PASS ON TO MAIN_THREAD, NEXT INTERRUPT SCHEDULED AT 180
	
	// PROGRAM RETURNS BACK TO HERE
	// nothing runs here: ticks saved
	
    Thread *t7 = new Thread("child7");
    t7->Fork(SimpleThread, 7, 0); // Tick 150 Q T7
    Thread *t8 = new Thread("child8");
    t8->Fork(SimpleThread, 8, 0); // Tick 160 Q T8
    Thread *t9 = new Thread("child9");
    t9->Fork(SimpleThread, 9, 0); // Tick 170 Q T9
	
	
	// TICK 180 - INTERRUPT - NEXT SCHEDULED AT 220
	// YIELD() RUNS:  T7 <- FIRST ITEM IN Q RETRIEVED, MAIN_THREAD ADDED TO BACK OF Q, RUNS T7
	// TICK 190 - T7 RUN SIMPLETHREAD FUNCTION, FINISH(), SLEEP(), PASS ON TO T8, NEXT INTERRUPT SCHEDULED AT 230
	// TICK 200 - T8 RUN SIMPLETHREAD FUNCTION, FINISH(), SLEEP(), PASS ON TO T9, NEXT INTERRUPT SCHEDULED AT 240
	// TICK 210 - T9 RUN SIMPLETHREAD FUNCTION, FINISH(), SLEEP(), T9 DELETED, PASS ON TO MAIN_THREAD, NEXT INTERRUPT SCHEDULED AT 250
	
	// PROGRAM RETURNS BACK TO HERE
	// nothing runs here: ticks saved


    SimpleThread(0); // Tick 220: run this, program ends. as idle determines no threads, interrupts or anything in Q left.
}

<EXPERIMENT 3>: // EXPERIMENT 3
　　　　　　　_,,..-=ﾆ二7
　　　 　r'ｱ´
　　　　ﾉノ　　 r'ｱ'"｀ヽ.,_　_,,..-=-､　　　 _,. -rｧ
　　　r'ｧ⌒ヽ､i7::::::::;＞''"´::￣￣｀"''＜´:::::::::!(
　　 .||　　　　r!:::／::::::::::::::::::::::::::::::::::::::::ヽ:::::/i'
　　 ||　 　 　 ^Y:::::;:::::i:::::::::/i::::::i:::::::;:::::::;::::Y (_
　　 ||　　　　./i::::::i:::/!--/　|:::;ﾊ_!_::i::::::i:::::::i r'ヽ.
　　 !!　　　 く:::L__ﾊ/-‐‐'　 ﾚ'　_!__;ﾊ::::ﾊ:::::|_,ゝ:::',
　　 ',',ヽ.　 　ヽﾍ「7""　　　 　　 　 ｀ﾚ7´）/:::iヽ;:::i
　　 i´｀とﾝ' ´｀ヽ!,人　　「'￣ ｀i　""7_/'´':::::::!　i:::!　　　　"What the fuck is going on"
　　 ヽ.,_//」 ､_,ノ:::::ﾉ＞.､､,___,ﾉ_,,.イ:::!､__!7ノ__. ﾚ'
　　　　 i　　　ゝ-ｧ'/　/)_iヽ／　/(/ゝ､.,_ノ　　￣「iｰ-､
　　　　ﾉ〈)　　　 ｀　 /::::ソ^ヽ､/」::_r' _/ /」 　 　 　|つ-'
　　　<.,　　_____,,,... イ::::くr-､_」:::::::::Y^ヽ､　　　　[] ',
　　　 　￣　　ﾚ' 　 l＞-､::;;_______;;::」〉'ノヽ.　　　__　〉
　　　（(　　　　r'ｱ'"／:::/ i　ヽ;::::ヽ｀''::ｰｧ､`''ｰ-┴'"
　　　　　　　 ｒ'i:::／::::::/　.l　 　';:::::::::::::::::!,」
　　　　　　　　`ゝ､:::::/　　l　　　ヽ;::::::::::/」
　　　　　　　　　 └へ>､,_!_______,,..>ｧﾆﾝ!
　　　　　　　　　　　　'r-- 'i　　 ￣ヽ,_ノ
　　　　　　　　　　　　ヽ二ﾉ


PART 1)
TASK: After running 2 INC and 2DEC, the final result should be 1 due to race conditions..

//CODE
void Inc_v1(_int which)
{
	//fill your code
 int a=value;
	a++;
 currentThread->Yield(); // context switches here to next thread in Q (CS)
	value=a;
	printf("**** Inc thread %d new value %d\n", (int) which, value);
}

void Dec_v1(_int which)
{
	//fill your code
 int a=value;
	a--;
 currentThread->Yield(); // context switches here to next thread in Q (CS)
	value=a;
	printf("**** Dec thread %d new value %d\n", (int) which, value);
}

void TestValueOne()
{
	value=0;
	printf("enter TestValueOne, value=%d...\n", value);
	//1. fill your code here.
    Thread *t1 = new Thread("child1"); // thread is created
    t1->Fork(Inc_v1, 1, 0); // fork(function to be run, thread id given, not to be joined)
    Thread *t2 = new Thread("child2");// thread is created
    t2->Fork(Dec_v1, 2, 0);// fork(function to be run, thread id given, not to be joined)
    Thread *t3 = new Thread("child3");// thread is created
    t3->Fork(Dec_v1, 3, 0);//fork(function to be run, thread id given, not to be joined)
    Thread *t4 = new Thread("child4");// thread is created
    t4->Fork(Inc_v1, 4, 1); // fork(function to be run, thread id given, to be joined)
    currentThread->Join(t4); // join current thread (CT) to t4
	// this means CT progress will be paused untill t4 is finished

	//2. checking the value. you should not modify the code or add any code lines behind
	//this section.
	if(value==1)
	    printf("congratulations! passed.\n");
	else
		printf("value=%d, failed.\n", value);
}//CODE

// THEORY:
Based on the pattern, the Q looks like this:
t1,t2,t3.t4,

since CT is joined to t4:, this means untill t4 is finished (where it runs printf),  the rest of the code will not be run, ie: if(value==1) will not be checked.

finish() moves to next thread in Q and deletes current thread.

runs like so: (CS: context switch)
1. t1: a=1, CS->t2, queue t1, queue now looks like: t2,t3,t4,t1
2. t2: a=-1, CS->t3 queue t2, queue now looks like: t3,t4,t1,t2
3. t3: a=-1, CS->t4, queue t3, queue now looks like: t4,t1,t2,t3
4. t4: a=1, CS->t1, queue t4, queue now looks like: t1,t2,t3,t4
5. t1: value=1, run printf(), finish(t1)->CS->t2, queue now looks like: t2,t3,t4
6. t2: value=-1, run printf(), finish(t2)->CS->t3, queue now looks like: t3,t4
7. t3: value=-1, run printf(), finish(t3)->CS->t4, queue now looks like: t4
8. t4: value=1, run printf(), finish(t4)->CS->CT, queue now looks like: 
9. since t4 is finished, were now back in CT, now the rest of code will be run, ie: if(value==1).

for TestValueMinusOne, swap the inc & dec running function patterns:

//CODE
Thread *t1 = new Thread("child1");
 t1->Fork(Dec_v2, 1, 0);
Thread *t2 = new Thread("child2");
t2->Fork(Inc_v2, 2, 0);
Thread *t3 = new Thread("child3");
t3->Fork(Inc_v2, 3, 0);
Thread *t4 = new Thread("child4");
t4->Fork(Dec_v2, 4, 1); // indicate item is to be joined changed
currentThread->Join(t4);
//CODE

or:
-1
1
1
-1

thus it runs like so:
1. t1: a=-1, CS->t2, queue t1, queue now looks like: t2,t3,t4,t1
2. t2: a=1, CS->t3 queue t2, queue now looks like: t3,t4,t1,t2
3. t3: a=1, CS->t4, queue t3, queue now looks like: t4,t1,t2,t3
4. t4: a=-1, CS->t1, queue t4, queue now looks like: t1,t2,t3,t4
5. t1: value=-1, run printf(), finish(t1)->CS->t2, queue now looks like: t2,t3,t4
6. t2: value=1, run printf(), finish(t2)->CS->t3, queue now looks like: t3,t4
7. t3: value=1, run printf(), finish(t3)->CS->t4, queue now looks like: t4
8. t4: value=-1, run printf(), finish(t4)->CS->CT, queue now looks like: 
9. since t4 is finished, were now back in CT, now the rest of code will be run, ie: if(value==1).


PART 2):
To Ensure Consistency in increment & decrement, semaphores are used.

//CODE
Semaphore *S = new Semaphore("S1",1); // Create semaphore S with name "S1" and init val '1'
//2. implement the new version of Inc: Inc_Consistent
void Inc_Consistent(_int which)
{
	//fill your code
 S->P(); // Wait() equivalent
  int a=value;
	a++;
 currentThread->Yield(); // changed
	value=a;
	printf("**** Inc thread %d new value %d\n", (int) which, value);
 S->V(); // Signal() Equivalent
}

//3. implement the new version of Dec: Dec_Consistent
void Dec_Consistent(_int which)
{
	//fill your code
 
  S->P(); // Wait() equivalent
  int a=value;
	a--;
 currentThread->Yield(); // changed
	value=a;
	printf("**** Dec thread %d new value %d\n", (int) which, value);
 S->V(); // Signal() Equivalent
}

//4. implement TestValueMinusOne by create two threads with Inc_Consistent and two threads with Dec_Consistent
// you should pass the checking at the end, printing "congratulations! passed."
void TestConsistency()
{
	value=0;
	printf("enter TestConsistency, value=%d...\n", value);

	//fill your code
 Thread *t1 = new Thread("child1");
    t1->Fork(Inc_Consistent, 1, 0);
    Thread *t2 = new Thread("child2");
    t2->Fork(Dec_Consistent, 2, 0);
    Thread *t3 = new Thread("child3");
    t3->Fork(Dec_Consistent, 3, 0);
    Thread *t4 = new Thread("child4");
    t4->Fork(Inc_Consistent, 4, 1); // indicate item is to be joined changed
    currentThread->Join(t4);

	//2. checking the value. you should not modify the code or add any code lines behind
	//this section.
	if(value==0)
		printf("congratulations! passed.\n");
	else
		printf("value=%d, failed.\n", value);
}//CODE

//THEORY
Semaphore's ->P() is equvalent to wait().

Semaphore's ->V() is equvalent to signal().

Due to semaphore inclusion, it runs like so:

1. t1: passes S->P() and enters critical , a=1, CS->t2, queue t1, queue is now: t2,t3,t4,t1
2. t2: fails S->P() as t1 - critical , sleep t2 thus CS->t3, queue t2, queue is now: t3,t4,t1,t2
3. t3: fails S->P() as t1 - critical , sleep t3 thus CS->t4, queue t3, queue is now: t4,t1,t2,t3
4. t4: fails S->P() as t1 - critical , sleep t4 thus CS->t1, queue t4, queue is now: t1,t2,t3,t4
5. t1: value=1, printf,  passes S->V() and exits critical, finish()->CS->t2, queue is now: t2,t3,t4

6. t2: passes S->P() and enters critical , a=0, CS->t3, queue t2, queue is now: t3,t4,t2
7. t3: fails S->P() as t2 - critical , sleep t3 thus CS->t4, queue t3, queue is now: t4,t2,t3
8. t4: fails S->P() as t2 - critical , sleep t4 thus CS->t2, queue t4, queue is now: t2,t3,t4
9. t2: value = 0, printf, passes S->V() and exits critical, finish()->CS->t3, queue is now: t3,t4

10. t3: passes S->P() and enters critical , a=-1, CS->t4, queue t3, queue is now: t4,t3
11. t4: fails S->P() as t3 - critical , sleep t4 thus CS->t3 queue t4, queue is now: t3,t4
12. t3: value=-1, printf, passes S->V() and exits critical, finish()->CS->t4, queue is now: t4


13. t4: passes S->P() and enters critical , a=0, no usual CS + Queue from -> yield as no other threads in Q left.
14. t4:  value=0, printf, passes S->V() and exits critical, finish()->CT
15. since t4 is finished, were now back in CT, now the rest of code will be run, ie: if(value==1).

// FUNCTIONS TO REVISE:
Thread()
Fork()
Yield()
Sleep()
Finish()
Join()

Nachos Semaphores Functions
Nachos Lock Functions
^ Both are in synch.cc

<EXPERIMENT 4>: // EXPERIMENT 4
　　　　　　　　　　 ,. -ｰ- ､　　　　　　　　　　　　 　 　 ,. -ｰ- ､
　　 '　　 　 　 　 /.,riljljljh, ヽ _ ,,, .. --ー.ー-- .. ,,, _ /.,riljljljh, ヽ　　　　　 /
　 　 '　　　 　 　 ! il|l|l|l|l|l|l! i . . . . . . . . . . . . . . . . . . ! il|l|l|l|l|l|l! i 　 　 　 /
　　　 '.　　　　　 ヽヾ!ilililiﾂ ,'. . . . . . . . . . . . . . . . . . .ヽヾ!ilililiﾂ ,'　　　　/　　　　　 ／
　　　　', 　　 　 　 `'‐-,-‐": : : . . . . . . . . . . . . . . . : : :`'‐-,-‐"　　　　/　　　　 ／
　　　　 ', 　 　　　 　 ./ : : : : : : . . . . . . . . . . . . . . : : : : : : ﾍ.　 　 　 　 　　　／
　　　　　',　 　 　 　 / : : : : : : : : : : : : : : : : : : : : : : : : : : : : ヽ　　　　　　／
　　　　　 ',　　　 　./ __,,..　　..-..--..─―..―..ｰ..-..　　､､..__: :ヽ　　　　　 _,､∧／
＼ 　　　　　_,,. r;:'' "; ;:;:- '"´￣｀ヽ､::::-─-　'"´￣　｀ヽ､; ; ｀`;ヽ.､　　「
　　　 　 ,.r'" ; ; ; _:;ア´　　　　　　　　　　　　　　　　 　 　 ヽ､ ; ; ; ; ＜ 　　I HATE NACHOS
　　　 , ' ; ; ; ; ; ｱ´/　 , '´　　　/　　　　 i　　　 ',　　　 i　　　Y; ; ; ; ; ; ＞.　I HATE NACHOS
..,,_　 i ; ; ; ; ; ;:/　/　 / 　 i.　､,'　　ﾊ　　,ﾊ　　　,i　　　ﾊ_　 　 iヽ;: ; ; ; ;> 　I HATE NACHOS
　　　 ヽ､; ; ; ﾉ　,' 　.i　 　ﾊ　 i＼/　',　/　i　　/ i ,.イ´./i　　　!　 i; ;／　　 I HATE NACHOS
_____ 　　　｀ｲ 　 i 　 i 　./　ｧ'"￣`ヽｰ/　　|　/,ｧ''"￣`ヽﾊ　　ﾊ　∠_. 　 　I HATE NACHOS
　　　 ∧　'"　／|　ﾉ　,ハｲ 　 i'´'`i 　　　　ﾚ' 　i'´'`i. 　 ﾄ|　/ﾆi,_ 　ヽ7　　I HATE NACHOS
＼∧/　 Vi／　 |_,. -‐ｧi/　 　 !__,ﾘ　 　 　 　 　 !__,ﾘ 　 ' ﾚ' ￣!ヾ'､ ./へ
　NO　　MORE　　　./=i.　 `'' ｰ-　　　　　　　　 -‐ ''´　 ｉ=＝!i　!i 　i　　＼/V＼/
NO　　MORE 　　.∠,　i. ""　　　　　　 `　　　 　　　""！　 .i!ｌ　!i.　', 　 　 　─---
 NO　　MORE　 　 ./　 i.　　 　 　 ,.'´￣￣`ヽ 　 　 　 .ｨ　　.!!l　 !i.　ヽ.
 NO　　MORE　　 ./_ ,　ﾄ.　　　　 i.,.-ー─-､.ｉ　　　 ,.ｲ!　　 .!ヾ='<　　 ', 　　　-　..,,__
NO　　MORE 　 　.／ 　!::::ヽ　　  ヽ,　　　　ノ 　 ノ'':::::i.　　　', ‐ ､　ヽ 　i　　　　　　
　MORE 　　　　　く　　　i::::::::｀ｉ丶.､.,,￣￣ ,.. イ::::::::::::i 　　　,j 　　ヽ､ﾍ/ 　 　 ＼
NMORE　　　　　　 .>　　.i:::::::::::ヽ､.,__￣￣__,.ノ:::::::::::::i.　　 ,ｨ'. 　 　　ヽ.　　',　　　＼
　　　　!!!　　　　　 ＜.ヽ 　 !:::::::::::::::::::::￣￣::::::::::::::::::::::!　　/ 　　　　　　ヽ, 　',


　　 ＼　　　　　　　',　　　　　|　　　　 　 /　　　　　　　 ／　　　　　　,　'
　　　　＼　　　　　　',　　　　 |　　　　　/　　　　　 　 ／　　　　　,　'
　　　　　 ＼　　　　　　　　　　　　　　　　　　 　　 ／　　　　 ,　'
　　　　　　　　 ｧ' ⌒ヽ._,. --──-- ｧ' ⌒ヽ.　　　　　　　,　'
`　､　　　　 　 ! （:::）　i　　　 　　　　.!　（:::） i
　　　　　　　　 ':､.,__,.ノ　　　 　　　　 ':､.,__,.ノ
　　　　　　　　　/ 　　　　　　　　　　　　 　',　　　　　　　　--　　──
　　　　　　　　_/-‐ﾆ=--────-=ﾆ､..,,_';､.,_
　　　　　,..-'´ｧ''"´　 　　　　　　　　　　　　 ｀"ヽ｀ヽ､
─--r'"´:::::::/ .　　| ､　　!∧　　./!__」イ´ . 　 |::::::::::::::｀ヽ.
　　　ヽ､::::::/　'､　 !　,＞t､　＼/'´l'ﾊ｀ヽ./　∧::::::::::::::::::ノ
　　 　　 ｀ヽ 　 ＼| 7　 l,ﾊ　 　　　!_ｿ　ﾉレ'　 ＼::::- ''´
　 　　　　　ヽ　 /ﾚﾍ、 ゝ'　　' 　　　　⊂⊃　 !, '⌒ヽ.                <Nachos are to  be eaten, not compiled you nutjob!>
　　　　　　　ﾚイ ⊂⊃　　ｒｧ'´￣｀ヽ. u /　!　7　 　　|
　　　　　　　　/ 　,ﾊ、 　　! 　　 　 ﾉ　/　/　,'　 　　/
　　　　　　　　|　　　　＞ ､.,_　　 _,. イ'|／　./　　　/
　　　　　　　　|　 /ヽ＿,.. イ「こ__／ /＿___/　 　　,'-､_
　　　　　　　　レ´／＼::::::::::::::::::::::::::::::::_／　　 　/｀ヽ._ ＼
　　　　　　　　/´`'ｰ--|:::ﾄ∧ﾉ::i´￣￣　　　　　,'　　 / ｀ヽヽ､
　　 　　　　,ｒ/　　 　　r）/o-o､'､＿_,.､ 　 　 ｰ! 　 /　　/　）Y
　　　　　　/､＿＿__,.ｲ7:::/￣i´:::::::|　/ヽ.,＿　`ｰヽ._／ ／| .|
　　　　 ／　　　　　　|（`'っ .!ｿ:::::::::', '､:::::`ー｀ヽ､_ 　｀ン::/ /
　　　 /　　　　　　　/:ゝ､.つと）:::::::::＼＼:::::::::::::::::::￣:::／／


//THEORY:
Key Functions:
VPNToPhyPage()
InsertToTLB()
lrualgorithm()

Slide 14: Diagram for how functions work together with thought process.

In general:
1. check tlb for translation.
2. not in tlb? check iverse page table
3. in ipt! update tlb. not in ipt! call backing store.
4. tell backing store to update w direct insert if there are empty frames or LRU otherwise, ipt then updates tlb

Process:

<Type: No valid entry in tlb or ipt, nothing to replace in ipt since there are empty frames>

1. No valid page found in TLB.

Translate 0x0, read: *** no valid TLB entry found for this virtual page 0!

2. Runs VpnToPhyPage() to list Inverse page table entries and determine if page can be found here: (here 'frame '/'IPT: ' is interchangeable w phyPage)

...IPT: 0 <frame 0>, pid: 0 vpn: 0 last_used: 0  validtity: 0 
...IPT: 1 <frame 1>, pid: 0 vpn: 0 last_used: 0  validtity: 0 
...IPT: 2 <frame 2>, pid: 0 vpn: 0 last_used: 0  validtity: 0 
...IPT: 3, <frame 3> pid: 0 vpn: 0 last_used: 0  validtity: 0 
.....IPT w matching pid and vpn missing! <- not found.

3. Run DoPageIn() to move page from disk store to TLB n IPT

paging in: pid 0, phyPage 0, vpn 0

4. Run InsertToTlb() , lists tlb pre update:

TLB pre-update: 0 <- which tlb entry it will update, here it indicates it will update 'TLB 0', chooses the oldest entry in tlb if none free. which is the one next to the newest entry, ie: if previous insert is tlb 0, next is tlb 1
...TLB 0, vpn, phy & valid: 0 0 0 
...TLB 1, vpn, phy & valid: 0 0 0 
...TLB 2, vpn, phy & valid: 0 0 0 

5. and prints out post update that in the TLB:

The corresponding TLBentry for Page 0 in TLB is 0 <-phy value

6.  post page in will look like this:
...IPT: 0, pid: 0 vpn: 0 last_used: 12  validtity: 1  <- indicates IPT 0 was last used at tick 12
...TLB 0, vpn, phy & valid: 0 0 1 <- 'TLB 0' was updated


<Type: Matching page entry found in IPT:>

	Translate 0x30, read: *** no valid TLB entry found for this virtual page 0!
Exception: page fault/no TLB entry
...IPT: 0, pid: 0 vpn: 0 last_used: 12  validtity: 1 
...IPT: 1, pid: 0 vpn: 9 last_used: 25  validtity: 1 
...IPT: 2, pid: 0 vpn: 26 last_used: 17  validtity: 1 
...IPT: 3, pid: 0 vpn: 1 last_used: 22  validtity: 1 
.....Caught IPT 0,  pid: 0 vpn: 0 last_used: 12  validtity: 1  <- virutal page 0 found in IPT 0 or phy frame 0

// Continue as per normal but without any paging ins.

// Per normal being that a TLB entry will be updated with the caught entry in IPT

<Type: No valid entry in tlb or ipt, but IPT is full!>

// NOTE: LruAlgorithm() will prioritize validity over oldest frame, see further down below:

	Translate 0x528, read: *** no valid TLB entry found for this virtual page 10!
...IPT: 0, pid: 0 vpn: 0 last_used: 28  validtity: 1 
...IPT: 1, pid: 0 vpn: 9 last_used: 25  validtity: 1 
...IPT: 2, pid: 0 vpn: 26 last_used: 17  validtity: 1 
...IPT: 3, pid: 0 vpn: 1 last_used: 22  validtity: 1  <- as we can see here, IPT is full
.....IPT w matching pid and vpn missing!
paging out: pid 0, phyPage 2, vpn 26 <-- NOTE: paging out is not always done, pageout only occurs due to dirty bit being set, which means it has been modified and must be written back to disk
paging in: pid 0, phyPage 2, vpn 10 <-- Using LruAlgorithm() and DoPageoutPagein(), 'IPT 2' / frame 2 / phyPage 2 with last_used:17, which indicates it was last used at tick 17, is the oldest frame.

// This oldest frame due to it's dirty bit is thus selected for page out in order to take in 'VPN 10'.
// The TLB entry for vpn 26 is also immediately made invalid before update shown here:

TLB pre-update: 2 
...TLB 0, vpn, phy & valid: 1 3 1 
...TLB 1, vpn, phy & valid: 0 0 1 
...TLB 2, vpn, phy & valid: 26 2 0 <-- immediately invalid

// Thus after the new VPN is paged in and IPT n TLB are updated:
...IPT: 2, pid: 0 vpn: 10 last_used: 28  validtity: 1 
...TLB 2, vpn, phy & valid: 10 2 1 

// Note, without dirty bit set:

	Translate 0xc0, read: *** no valid TLB entry found for this virtual page 1!
Exception: page fault/no TLB entry
...IPT: 0, pid: 0 vpn: 0 last_used: 49  validtity: 1 
...IPT: 1, pid: 1 vpn: 26 last_used: 66  validtity: 1 
...IPT: 2, pid: 1 vpn: 0 last_used: 61  validtity: 1 
...IPT: 3, pid: 1 vpn: 9 last_used: 68  validtity: 1 
.....IPT w matching pid and vpn missing!
paging in: pid 1, phyPage 0, vpn 1 <-- only page in.
TLB pre-update: 0 
...TLB 0, vpn, phy & valid: 0 2 1 <-- thus no immediate change to invalid.
...TLB 1, vpn, phy & valid: 9 3 1 
...TLB 2, vpn, phy & valid: 26 1 1 

// Thus after IPT n TLB are updated:
...IPT: 0, pid: 1 vpn: 1 last_used: 71  validtity: 1
...TLB 0, vpn, phy & valid: 1 0 1 

// In a scenario where there are invalid entries in IPT:

...IPT: 0, pid: 1 vpn: 1 last_used: 71  validtity: 0 
...IPT: 1, pid: 1 vpn: 26 last_used: 66  validtity: 0 
...IPT: 2, pid: 1 vpn: 0 last_used: 76  validtity: 0 
...IPT: 3, pid: 1 vpn: 9 last_used: 73  validtity: 0 

// The first invalid entry is automatically chosen to be replaced by LRU, regardless of how old it is, ie:

1.
...IPT: 0, pid: 0 vpn: 0 last_used: 109  validtity: 1 
...IPT: 1, pid: 1 vpn: 26 last_used: 66  validtity: 0 
...IPT: 2, pid: 1 vpn: 0 last_used: 76  validtity: 0 
...IPT: 3, pid: 1 vpn: 9 last_used: 73  validtity: 0 

2.
...IPT: 0, pid: 0 vpn: 0 last_used: 109  validtity: 1 
...IPT: 1, pid: 0 vpn: 9 last_used: 111  validtity: 1 
...IPT: 2, pid: 1 vpn: 0 last_used: 76  validtity: 0 
...IPT: 3, pid: 1 vpn: 9 last_used: 73  validtity: 0 

3.
...IPT: 0, pid: 0 vpn: 0 last_used: 109  validtity: 1 
...IPT: 1, pid: 0 vpn: 9 last_used: 111  validtity: 1 
...IPT: 2, pid: 0 vpn: 10 last_used: 113  validtity: 1 
...IPT: 3, pid: 1 vpn: 9 last_used: 73  validtity: 0

4.
...IPT: 0, pid: 0 vpn: 0 last_used: 109  validtity: 1 
...IPT: 1, pid: 0 vpn: 9 last_used: 111  validtity: 1 
...IPT: 2, pid: 0 vpn: 10 last_used: 114  validtity: 1 
...IPT: 3, pid: 0 vpn: 26 last_used: 114  validtity: 1 



<Where paging in PID suddenly changes:> <im not so sure about this part i think>

//CASE 1: If IPT's pids were 0 and the paging in PID suddenly changes to a new PID (ie 1), all TLB entries immediately are made invalid.:

	Translate 0x0, read: *** no valid TLB entry found for this virtual page 0!
Exception: page fault/no TLB entry
...IPT: 0, pid: 0 vpn: 0 last_used: 49  validtity: 1 
...IPT: 1, pid: 0 vpn: 9 last_used: 46  validtity: 1 
...IPT: 2, pid: 0 vpn: 10 last_used: 28  validtity: 1 
...IPT: 3, pid: 0 vpn: 26 last_used: 44  validtity: 1 
.....IPT w matching pid and vpn missing!
paging in: pid 1, phyPage 2, vpn 0 <-- paging in PID now 1, a new value
TLB pre-update: 0 
...TLB 0, vpn, phy & valid: 9 1 0  <-- All TLB entries set to invalid
...TLB 1, vpn, phy & valid: 26 3 0 
...TLB 2, vpn, phy & valid: 0 0 0 
The corresponding TLBentry for Page 0 in TLB is 0 

//CASE 2: If IPT's pids are now 1 and the paging in PID suddenly reverts back to the old PID (ie: 0), all TLB and IPT entries are immediately made invalid.

	Translate 0x48, read: *** no valid TLB entry found for this virtual page 0!
Exception: page fault/no TLB entry
...IPT: 0, pid: 1 vpn: 1 last_used: 71  validtity: 0 <-- ALL IPT entries set to invalid
...IPT: 1, pid: 1 vpn: 26 last_used: 66  validtity: 0 
...IPT: 2, pid: 1 vpn: 0 last_used: 76  validtity: 0 
...IPT: 3, pid: 1 vpn: 9 last_used: 73  validtity: 0 
.....IPT w matching pid and vpn missing!
paging in: pid 0, phyPage 0, vpn 0 <-- paging in PID now 0, the old value!
TLB pre-update: 0 
...TLB 0, vpn, phy & valid: 1 0 0  <-- ALL TLB entries set to invalid
...TLB 1, vpn, phy & valid: 0 2 0 
...TLB 2, vpn, phy & valid: 26 1 0 
The corresponding TLBentry for Page 0 in TLB is 0 

<No code cus too many differing implementations>


<Edit: 9/11/2022 6:40PM SGT>

<Rough explaination how dirty pages are identified in output:>

//under the file translate.cc, there is a function called Machine::Translate(int virtAddr, int* physAddr, int size, bool writing)
// the 'writing' parameter essentially decides wether or not a page is dirty and that a pageout will occur.

 DEBUG('a', "\tTranslate 0x%x, %s: ", virtAddr, writing ? "write" : "read"); <-- section of the function that prints out wether a translation vpn is to be considered 
 
 // in the output text, it is reflected like this:
 
 Translate 0xd6c, write: *** no valid TLB entry found for this virtual page 26! <-- this tells us vpn 26 is being written to and is likely to be paged out at some point
 
 // versus the normal text when it is not dirty:
 
 Translate 0xc0, read: *** no valid TLB entry found for this virtual page 1! <-- signifies this page will not be paged out.


<END>

　　　　　　　 　_____.,.へ.
　　　 _,.-‐''''"´　／ ＠ ＼_.
　　,.'"　　r___,.rﾆ'ｰ'´￣｀ヽ!ヽ､
　　!_,.rソ´　i ____ｲ　 　｀i　,｀'ｰﾄ､_
　r' i,ｲ /　　ﾊ__/_､ﾊ 　/__ﾊ　 ',7」
　 Ｙ´ /!/ﾚｲ　　　　 レ'!,ｨ､ !/|ﾊソ
　 ,'　　,'　　| "￣｀　 　 -､!ﾊ　Y´　　AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA。
. / 　 /　i　ﾊ　　　____｀　"i ハ!
く　　ｲ ./　| |>､ 　｀ ´　　人 ﾊ
ﾉ＾ｰ'!/,ｲ-ハﾄ､.!｀=ｰｒ＜´!.ﾊヽ!
＾Y ,イ＾ヽ､ゝ 　＼「７/｀ヽ!Vヽ!
　ﾚ'ｿ　　　 Lヽ､　 〈ﾊ〉　 〉､　　 /7￣￣￣￣￣￣￣7
　「｀ｰ'＾ｰ'＾７　｀ヽ/§ヽ!／ヽ　.// ____________________ ./
.ｲ｀ｰ--‐イ､_,.－r!､.§　〉 ／｀// /　 PC.冥界 　 //
7ヽr､__r'"´ /　// '"´｀ヽ､　 .//　￣￣￣￣￣￣ /
i　　　ｲ　 　| 　| |､_ ,､ ! .ﾊ　//　　　　　　　　　　/
ヽ､　　　　　i 　| ﾄ=='===='i二二二二二二二二i