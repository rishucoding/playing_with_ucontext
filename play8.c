//checking the context value from a contet 
//doing setcontext

#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>

static ucontext_t uctx_main, uctx_func1, uctx_dummy;
//These are the user context for main function, and two other functions.
long long int func1_dummy_stack[16384];
long long int func1_stack[16384];
// Interesting: one line function for handling the error
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while(0)

void foo(void){
    if(getcontext(&uctx_dummy) == -1){handle_error("getcontext for func1 failed! ");}
    //printf("\nrip is: %lld \n", uctx_dummy.uc_mcontext.gregs[16]);
    //printf("\nrip is: %x \n", uctx_dummy.uc_mcontext.gregs[16]);
    //uctx_func1.uc_mcontext.gregs[16] = uctx_dummy.uc_mcontext.gregs[16] + 161; // brute force: using distance to go to return addr
    //uctx_func1.uc_mcontext.gregs[16] = 4196323; // directly defining the return address from gdb disas
    long long int *ptr = (uctx_dummy.uc_mcontext.gregs[10] + 8);
    //uctx_func1.uc_mcontext.gregs[16] = *ptr;
    

    uctx_dummy.uc_mcontext.gregs[16] = *ptr; // this is the return addr value being set as RIP
    
    //this line should change the return addr
    long long int *ptr2 = uctx_main.uc_mcontext.gregs[10] + 8;
    *(ptr)  = *ptr2;

    if(*((long long int *)(uctx_main.uc_mcontext.gregs[10] + 8)) == *((long long int *)(uctx_dummy.uc_mcontext.gregs[10] + 8))){
        printf("Return address should be changed! \n");
    }
    int j = 0;
    for(j = 0;j < 16384; j++){
        func1_dummy_stack[j] = func1_stack[j];
    }
    
    //let's search the idnex
    long long int *ptr3 = (uctx_func1.uc_mcontext.gregs[10]);//stores RBP of func1 stack
    int index = 0;
    for( j = 0; j < 16384; j++){ // opt by rev search
        if(*(ptr3) == func1_dummy_stack[j]){
            index = j;
            printf("Value of RBP from mcontext: %lld", *ptr3);
            printf("\tValue of RBP from stack: %lld\n", func1_dummy_stack[j]);
            printf("Index found!! , it is: %d\n", index);
            break;
        }
    }

    int offset = 16384 - index;

    //set the new RBP - 
    uctx_dummy.uc_stack.ss_sp = func1_dummy_stack;
    uctx_dummy.uc_stack.ss_size = sizeof(func1_dummy_stack);
    uctx_dummy.uc_link = &uctx_main;
    
    uctx_dummy.uc_mcontext.gregs[10] = func1_dummy_stack + index;
    setcontext(&uctx_dummy);
        printf("After set context: this should not get printed!\n");

}

static void func1(void){
    //dummy function
    
    int a = 10;
    int c = 'a';
    int d = 'b';
    //printf("rip is: %x \n", uctx_func1.uc_mcontext.gregs[16]);
    //printf("rbp is: %x \n", uctx_func1.uc_mcontext.gregs[10]);
    if(getcontext(&uctx_func1) == -1){handle_error("getcontext for func1 failed! ");}
    
    //printf("After getcontext(): rbp is: %llx \n", uctx_func1.uc_mcontext.gregs[10]);
    //long long int *ptr = (uctx_func1.uc_mcontext.gregs[10] + 8);
    //printf("After getcontext(): rip is: %llx \n", *ptr);
    int val1 = 100;
    int val2 = 101;
    long long int val3 = 1001;

    printf("BEFORE::\n");
    printf("value of val1 = %d\n", val1);
    printf("value of val2 = %d\n", val2);
    printf("value of val3 = %lld\n", val3);
    
    foo();
    printf("AFTER::\n");
    printf("value of val1 = %d\n", val1);
    printf("value of val2 = %d\n", val2);
    printf("value of val3 = %lld\n", val3);

    //if(getcontext(&uctx_func1) == -1){handle_error("getcontext for func1 failed! ");}
    
    //printf("After getcontext(): rbp is: %x \n", uctx_func1.uc_mcontext.gregs[10]);
    printf("AFter entering FOO: \n");
    printf("func1: returning\n");
}



int main (int argc, char *argv[]){
    //long long int func1_stack[16384];
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



