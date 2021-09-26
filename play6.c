//checking the context value from a contet 
//doing setcontext

#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>

static ucontext_t uctx_main, uctx_func1, uctx_func2;
//These are the user context for main function, and two other functions.

// Interesting: one line function for handling the error
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while(0)

void foo(void){
    setcontext(&uctx_func1);
    printf("This should not get printed\n");
}

static void func1(void){
    //dummy function
    printf("func1: started\n");

    for(int i = 0; i < 23; i++){
    
    
    printf("reg[%d] is: %lld \n", i, uctx_func1.uc_mcontext.gregs[i]);
    }
    
    printf("rip is: %lld \n", uctx_func1.uc_mcontext.gregs[16]);
    printf("func1: am going to do swapcontext(&uctx_func1, &uctx_func2)\n");
    // I think we specify the current context and the new context which we want to be scheduled(program order)
    int a = 10;
    int c = 'a';
    int d = 'b';
    printf("rip is: %lld \n", uctx_func1.uc_mcontext.gregs[16]);
    int e = 'c';
    //if(swapcontext(&uctx_main, &uctx_func1) == -1){handle_error("swapcontext for main --> func2 failed");}
    printf("rip is: %lld \n", uctx_func1.uc_mcontext.gregs[16]);
    printf("NGREG is: %d", NGREG); 
    //setcontext(&uctx_func1);
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

    //printf("rip is: %lld \n", uctx_func1.uc_mcontext.gregs.);
    
    //if(getcontext(&uctx_func1) == -1){handle_error("getcontext for func1 failed! ");}

   printf("main: swapcontext(&uctx_main, &uctx_func1)\n");
    //this is like a try catch block
    if(swapcontext(&uctx_main, &uctx_func1) == -1){handle_error("swapcontext for main --> func2 failed");}

    printf("main: exiting\n");
    exit(EXIT_SUCCESS);


}



