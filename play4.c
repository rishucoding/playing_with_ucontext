// i do getcontext() to initialize. 
// i set values of uctx - sp, size, link
// i do makecontext to link function with context
// i do swapcontext to go to that func, and remember present context in oucp

// My oucp and ucp are same. 
// Output: the context points to next line first (written), and context switches to ucp which is same, so the next line executes. Thus, swapcontext() doesn't do anything here. But, it does help in putting a context of present lines of code

#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>

static ucontext_t uctx_main, uctx_func1, uctx_func2;
//These are the user context for main function, and two other functions.

// Interesting: one line function for handling the error
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while(0)

static void func1(void){
    //dummy function
    printf("func1: started\n");
    printf("func1: am going to do swapcontext(&uctx_func1, &uctx_func2)\n");
    // I think we specify the current context and the new context which we want to be scheduled(program order)

    printf("func1: returning\n");
}



int main (int argc, char *argv[]){
    char func1_stack[16384];
    char func2_stack[16384];

    if(getcontext(&uctx_func1) == -1){handle_error("getcontext for func1 failed! ");}

    //create the context for func1
    uctx_func1.uc_stack.ss_sp = func1_stack;
    uctx_func1.uc_stack.ss_size = sizeof(func1_stack);
    uctx_func1.uc_link = &uctx_main;
    //link context to func1
    makecontext(&uctx_func1, func1, 0);

    
    if(getcontext(&uctx_func1) == -1){handle_error("getcontext for func1 failed! ");}

   printf("main: swapcontext(&uctx_main, &uctx_func1)\n");
    //this is like a try catch block
    if(swapcontext(&uctx_func1, &uctx_func1) == -1){handle_error("swapcontext for main --> func2 failed");}

    printf("main: exiting\n");
    exit(EXIT_SUCCESS);


}



