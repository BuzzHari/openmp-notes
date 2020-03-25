# Introduction to OpenMP 

Notes on Video lectures by Tim Mattson on YouTube.


## Module 1 - Part 1

* Moore's Law: Number of transistors incorporated in a chip would approximately double every 24 months.
* Power = CV^2F
* Same amount of work done, using multiple cores at lower frequency.
* No magic compiler to convert serial code into proper parallel code. 
* Time to roll up our sleeves and get it done. 
  
## Module 1 - Part 2

* Concurrent Applications: An application for which computations logically execute simultaneously due to the semantics of teh application.
* Parallel Applications: An application for which the computations actually execute simultaneously in order to complete a problem in less time.
* OpenMP:
    * at the low level we shared address space hardware 
    * OS/system support for shared memory and threading
    * OpenMP Runtime Library.
    * Prog layer: Directive compilers, openMP library, env variables
    * End user application.
* OpenMP core syntax in C: `#pragma omp construct [clause[clause]...]`, like `#pragma omp parallel num_threads(4)`
* Function prototypes and types in the file `#include <omp.h>`.
* Most OpenMP constructs apply to a "Structured Block".
* Structured Block : A block of one more statements with one point of entry at the top and one pointof exit at the bottom. You can't jump in the middle, you can't jump out of the middle.

## Module 2

* Some Windows bs.
* Some VS bs.
* Linux be superior.
* Just do `gcc -fopenmp foo.c`.
* To set num of threads `export OMP_NUM_THREADS=4`
* Check `ex1B.c` for the `hello world` program.    

## Discussion 1

* Shared Memory Computers, any computer composed of multiple processing elements that share an address space. There are two classes:
    * Symmetric Multiprocessor (SMP): A shared address space with "equal-time" access for each processor, and the OS treats every processor the same way. It assums there are no special blocks in memory. All memory is equally accessible at a roughly equal cost.
    * Non Uniform Address Space Multiprocessor(NUMA): Different memory regions have different access costs. Think of memory segmented into "Near" and "Far" memory.
* Cray-2: The last large scale SMP computer. Release in 1985. 1.9 GFLOPS. Fastest supercomputer till 1990.
* Mordern CPUs might look like SMP, as they share last layer of cache. But it's not. There is a cache hierrachy, if it's close to the processor, it has much faster access time. 
* Process: Instance of a program execution.
* Threads: "Light weight process". Process are decomposed to threads. Threads have their own stack but share the Text, Data and the Heap region.
* Uninteded sharing of data causes race conditions.
* Race Condition: when the program's outcome changes as thre threads are shceduled differently.
* To control race conditions, use synchronization to protect data conflicts.
* Synchroize as little as possible to get the right output, as it's costly.

## Moudule 3

* Fork-Join Parallelism. Main process thread, forks of into multiple threads, called team of threads, when required by the code, and after the team of threads is done it's job, they all join back to the main process thread, also called the master thread.
* This is done using `pragma omp` construct, like we did it in the hello world(ex1B.c) program.
* Behind the scene omp bascially uses pthreads to do the job.
  
 ## Discussion 2

* My solution `ex2.c`, while it works, it's horseshit, should have done better.
* Tim Mattson's solution `ex2b.c`, way better than mine, refer to this code.
* **Important**: We request for threads, we might end up getting lesser. This has to be kept in mind while coding, and should be handled accordingly.
* False sharing: If independant data elements happen to sit on the same cache line, each update will cause the cache lines to "slosh back and forth" between threads.
* If you promote scalars to an array to support creation of an SPMD program, the array elements are contiguous in memory and hence share cache line. This results in poor scalability.
* Refer to code `ex2b_pad.c`. It's much faster, as now each different element is sitting in a different cache line.
* To check for cache line size:
    * Go to /sys/devices/system/cpu/cpu0/cache/
    * There are subdirectory for each level of cache.
    * cat coherency_line_size
    * On my pc, it's 64 bits, for all L1,L2,L3 cache. 

## Module 4

* At a high level, there 2 types of scnychronisation:
    * Barrier: Each thread wait at the barrier until all threads arrive.
    * Mutual Exclusion: Define a block of code that only one thread at a time can execute.
* Constructs involed: Critical, Atomic, Barrier Ordered. This all at a high level.
* Low level synchronization construcst: Flush, locks.
* Add this `#pragma omp barrier` to add barrier in the code.
* Mutual exclusion is done using a critcial construct, `#pragma omp critical`.
* There are hardware constructs which provide really efficent mutual exclusion, so what `#pragma omp atomic`says, if those hardware are available, use them. Otherwise it's just the same as critical section. It applies to simple binary operations like updating a value, for example a counter.

## Discussion 3

* My solution `ex3.c`. My solution has `sum` is still global and there's updating it is the critical section. I am dumb af. It's basically serializing the code!
* Refer to Tim's solution `ex3b.c`, where he makes `sum` local and keeps `pi` global, and updates that in the critical section. This is obviously much better, I failed to get the right code again.
* The idea of this exercise was to remove the possiblity of false sharing. 
* The critical solution and the padded solution, have pretty much the same performance, but the critical code now is portalable to other machines, as it no longer depends on the cache line size.  
         
## Module 5 - Part 1

* Worksharing: Taking a single construct and sharing it across the threads.
* Worksharing constructs: Loop construct, Sections/Seciton, Task.
* Loop construct: `#pragma omp for`, this basically tells the compiler to spilt the immediately following `for loop` into multiple threads. We did this by hand in previous exersices.
* `#pragma omp for schedule`, tells the compiler how to split the `for loop` in threads
    * `schedule(static [,chunk])`: breaks them into blocks of size 'chunk' to each thread. By default, it divides it into equal blocks per thread. This is happens at the compile time.
    * `schedule(dynamic [,chunk])`: Each thread grabs 'chuck' iterations off a queue until all iterations have been handled. Compiler is deciding at run-time how it's gonna break up the threads.
    * `schedule(guided [,chunk])`: Threads dynamically grab blocks of iterations. The size of the block starts large and shrinks down to size 'chunk' as the calculation proceeds. Not much used.
    * `schedule(runtime)`: Schedule and chunk size taken from the OMP_SCHEDULE env vairable(or the runtime library). Used to figure out which schedule is best with recompiling the program.
    * `schedule(auto)`: Schedule is left up to the runtime to choose.
* Uses cases of The Scheule Clause:
    * Static : Pre-determinded and predictable work by the programmer. Least work at runtime: Scheduling done at compile time.
    * Dynamic : Unpredictable, hight vairable work per iteration. Most work at runtime: Complex scheduling logic used at runtime.
* The loop construct can be combined with the paralle construct as this `#pragma omp parallel for.

## Module 5 - Part 2

* Find the compute-intensive loops.
* Remove loop-carried dependencies so they can be safely execute in any order.
* Place the appropriate OpenMP directive and test.
* Reduction: Operatins where we are accumlating accros a data sturture, for ex a += b[i].
* OpenMP reduction clause: `reduction(op:list)`, can be added to the paralled/for construct.
* What it does is, the compiler creates a local copy of each list variable, and intializes it depending on the 'op', e.g. 0 for '+'.
* Updates occur on the local copy. Local copies are reduced into a single value and combined with the original global value.

## Discussion 4

* Refer to `ex4.c` for the parallel pi program using `for reduction` construct.
* Refer to `ex4b.c` for Tim's solution. It's pretty much the same. Finally got something right.
* The speed up is pretty much the same.

## Module 6 - Part 1

* There's an implicit barrier after every parallel for loop. This is done to be safe, and decrease the chances of programing mistakes.
* If we don't need the implicit barrier, we can use the `nowait` clause to do so.
* There's an implicit barrier at the end of the parallel region, and it can't be turned off for obvious region. 
* `#pragma omp master`,just the master thread will do the work inside this region. There's no synchronization. We have explicitly add `#pragma omp barrier`. 
* `single` clause, says that only one thread will do the work in the structured block that's attached to it. The first thread that gets to it will do the work. As this is an worksharing construct, there's a implied barrier at the end of it. `nowait` can be used here to diable this barrier.
* `sections/section` construct, is another worksharing construct. Used to create different sections to certain work. e.g.

    ```c
    #pragma omp parallel
    {
        #pragma omp sections
        {
            #pragma omp section
                x_calc();
            #pragma omp section
                y_calc();
            #pragma omp section
                x_calc();
        }
    }
    ```
    
## Module 6 - Part 2

* Lock Routines
    * `omp_init_lock()`: To initialize lock variable.
    * `omp_set_lock()`: To get the lock.
    * `omp_unset_lock()`: To release the lock.
    * `omp_destroy_lock()`: Destroys the lock.
    * `omp_test_lock():` Check's if the lock is available or not.

## Module 6 - Part 3

* Runtime Library Routines
    * `omp_set_num_threads()` : Sets the number of threads..More like asks for number of threads.
    * `omp_get_num_threads()` : Gets the number of threads.
    * `omp_get_thread_num()` : Gets thread ID.
    * `omp_get_max_threads()`: Returns the maximum number of threads you can get.
    * `omp_in_parallel()`: Tells you wether if it's running in parallel or not.
    * `omp_set_dynamic()`: Set dynamic mode. Allocates different number of threads from one parallel region to another smartly.
    * `omp_get_dynamic()`: Tells you if you're in dynamic mode or not.
    * `omp_num_procs()`: Returns the number of processor a system has.

## Module 6 - Part 4

* Environment Variables:
    * `OMP_NUM_THREADS int_literal`: The default number of threads.
    * `OMP_STACKSIZE`: The amount of stack size when creating the threads.
    * `OMP_WAIT_POLICY ACTIVE|PASSIVE`: Sets the wait policy.
        * `ACTIVE` : Sets a spinlock. Keeps on activly waiting for something to be available.
        * `PASSIVE`: The thread that is waiting is put to sleep until whatever it's waiting for is done.
        * Side note: To suspend a thread and to rewake is costly as well as activly waiting is costly too, as it consumes unnecessary resources. Use them properly depending on the senario.
    * `OMP_PROC_BIND TRUE|FALSE`: Bind a thread to particular processor and leave it there.

## Module 7

* Global variables are shared among threads.
* If something is on the stack it's private to the threads.
* Stack private, Heap shared.
* Changing Storage Attributes:
    * SHARED: Makes them shared variables.
    * PRIVATE: Makes them private. PRIVATE doesn't initalizes of the temproray created. They disappear outside openmp parrallel scope.
    * FIRSTPRIVATE: Creates a private copy, but initalizes it to the global value. 
    * LASTPRIVATE: Saves the last value of the variable to the global variable.  
    * DEFAULT(PRIVATE|SHARED|NONE)

## Discussion 5

* Refer to `mandel_mysol.c` for my solution. The original `mandel.c` is at `./Mattson_OMP_exercises`.
* The loop index for the parallelized loop will be made private for us, but not the nested loop.

## Module 8

* Exercise to prallalize linked.c

## Discussion 6

* Refer to `linked_mysol.c` for my solution. My solution has major flaw. If the number of elements inthe linked list is more than the available threads, then it wouldn't work.
* Tim's solution at `./Mattson_OMP_exercises/solutions/`  
* He passed through the linked list and stored the pointer to the nodes into an array.
* The right way to do this is to use tasks.
      
## Module 9

* Tasks are independent units of work.
* Tasks are composed of:
    * code to execute
    * data environment
    * internal control variables(ICV)
* The runtime system decides when tasks are executed.
* `task` construct which creats a task.
  ```c
  #pragma omp parallel
  {
    #pragma omp task
    foo();
    #pragma omp barrier
    #pragma omp single
    {
        #pragma omp task
        bar();
    }
  }
  ```
* `pragma omp taskwait`, waits for multiple tasks which have been created to be completed.

## Discussion 7

* Refer to `linked_task_mysol.c` for my solution.
* Tim's solution at `./Mattson_OMP_exercises/solutions/`  
* One thread is creating the set of tasks, the other threads sit on the barrier, and work on the tasks.
* Watch the video. Has really good explanation with diagrams and stuff. 
  
## Module 10

* Sequential Consistency: In a multi-processor, ops(R(read), W(write)), S(synchronizations)) are consistentif:
    * They remain in program order for each processor.
    * They are seen to be in the same overall order by each of trhe other processors.
* If Program order = code order = commit order. This is bad, we have put so much constraints on how memory moves through the subsystem that we aren't taking advantage of the compiler optimizations.
* Sequential Consistency for shared memory systems is bad.
* For these machines we, Relaxed Consistency, basically removing some of the ordering constraints formemory ops(R, W, S).
* OpenMP supports Relaxed Consistency model.
* Can not reorder S ops with R or W ops on the same thread. 
* It guarantees: S>>W, S>>R, R>>S, W>>S, S>>S
* The sychronization operation getting talked about, is flush.
* Flush: Defines a sequence point at which a thread is guaranteed to see a consistent view of memory  with respect to the "flush set". 
* The flush set is a list of variables when the `flush(list)` construct is used. "All thread visible variables" for a flush construct without an argument list.
* The action of `flush` is to guarantee that:
    * All R,W ops that overlap the flush set and occur prior to the flush complete before the flush executes.
    * All R,W ops that overlap the flush set and occur after the flush don't execute until after the flush.
    * Flushes with overlapping flush sets can not be reordered.
* A flush operation is implied by OpenMP synchronizations:
    * At entry/exit of parallel regions.
    * At implicit and explicit barriers.
    * At entry/exit of critical regions.
    * Whenever a lock is set/unset.

## Discussion 8

* Refer to `prod_cons_mysol.c`. Didn't attempt, directly went to the solution.
* Requires pair-wise synchronization.
* OpenMP lacks synchronization constructs that work between pairs of threads.
* Pair Wise synchronization:
    * Use a shared flag variable.
    * Reader spins waiting for the new flag value. Infinite loop checking the value of flag.
    * Use `flush` to force updates to and from memory.
* In OpenMP 3.1, Atomic was expanded to cover the full range of common scenarios where we need to protect a memory operation so it occurs atomically
* `#pragma omp atomic [read|write|update|capture]`
    * `atomic read`: It's either complete read, or no read at all.
    * `atomic write`: Either we complete write or write at all.

## Module 11

* Threadprivate: 
    * Makes global data private to a thread. If we want file scope and static variables visible to all the functions but be private to a particular thread.
    * `threadprivate(list)`: Intializes the variables to the static value. And it'll have the value of those variables per thread. It'll keep it local per thread but global to all the functions that have visibility to those variables.
    * `copyin(list)`:  Copy the value of variable into each thread.

## Discussion 9

* Linear Congruential Generator (LCG), algorithm for randon number generator.
* Refer to `pi_mc.c` and it's parallel version in `solutions` folder.
* Intel math library mkl. 
* Leap Frog Method.

## Recapitulation

* Just a recap video.
