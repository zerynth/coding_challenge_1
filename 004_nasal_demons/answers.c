// One

//The following macro introduces a side effect if a or b include assignment
#define MIN(a,b) (a < b) ? (a) : (b)
//For example, the expression z=MIN(x,y++); is transformed by the preprocessor in
//z=(x < y++) ? (x) : (y++); with two undesired effects:
//if x=6 and y=4, the desired output should be x=6, y=5, z=4 but the actual result is
// x=6, y=6, z=5 because y has a double increment and the output of the condition has a single increment

//The problem should be fixed using the following code
#define MIN(a,b) ({ __typeof__(a) _a=a; __typeof__(b) _b=b; _a < _b ? _a : _b; })




// Two

//The following code peforms the cube of the integer value pointed by pointer named x.
//Such pointer is declared as volatile and the compiler does not cache the value of the pointer
//but generate a code that takes the value of the pointer every time the x name is encountered
//Consequently, if the value of x changes during the execution of the xto3 function due to some
//event (interrupts, other threads, etc.), this code return an undesired result.
//For example if x is a pointer to an integer value equals to 2 before the calling of
//the xto3(x), and x changes during the execution of the xto3 function, the result will not be 8
//but another value
int xto3(volatile int *x){
    return *x * *x * *x;
}

//The problem should be fixed using the following code
int xto3(volatile int *x){
    int *localx;
    localx=x;
    return *localx * *localx * *localx;
}





// Three

//The following code uses the uintptr_t type in order to perform integer operations
//on a pointer. The correct use of the uintptr_t involves the cast from the data pointer
//to the uintptr_t and then cast back to the data pointer
int *align_to_bits(int *addr, int bits){
    return ((uintptr_t)addr) & (uintptr_t)((1<<bits)-1);
}

//The code should be fixed using the following code
int *align_to_bits(int *addr, int bits){
    return (int*)(((uintptr_t)addr) & (uintptr_t)((1<<bits)-1));
}






// Four

//The following code does not compile because a is an integer variable and the
//dereference operator is not allowed
int return0(int a, int *b){
    *b=0;
    *a=42;
    return *b;
}

//The problem should be fixed using the following code
int return0(int a, int *b){
    *b=0;
    a=42;
    return *b;
}
