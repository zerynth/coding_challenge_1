# Nasal demons

Each of the following snippets of C code is wrong in some way. Fix it and explain why:

## One
```
    #define MIN(a,b) (a < b) ? (a) : (b)
	
	the macro has two potential problems:
	a) it is not type safe and can accept any kind of data, which
		makes the code prone to future errors without any warnings.
	b) It might suffer from double evaluation.
		e.g.  MIN(a, b++) expands in "(a < b++) ? (a) : (b++)" 
		incrementing b twice. This can be not obvious to see at first
		glance and might create "hard to find" bugs in the future.

	To solve the problem one can use a function instead of a macro
	(but it will require to make a different function for each type):

		int min(int a, int b)
		{
			return ((a) < (b) ? (a) : (b));
		}


	or making the macro make a copy of the parameters of desired type:

		#define MIN(a,b) \
		({\
			__typeof__ (a) A = (a);\
			__typeof__ (b) B = (b);\
			A < B ? A : B;\
		})

		note that the second approach still accept any kind of data.
```

## Two
```
    int xto3(volatile int *x){
        return *x * *x * *x;
    }
	
	using a volatile variable and passing it as a pointer to a 
	function is dangerous and could bring in many problems.
	First of all, the volatile variable will cause the compiler
	to not optimize it, wasting some resources.
	In addition, being volatile, the variable might be changed by
	another process while we are in our xto3 function, which will return
	to us a result different from what we expected when we called
	
	Being passed as a pointer exposes the function to the
	possibility to being passed a uninitialized poiter, without warning,
	causing it to return a random result.

	Since we are just returning a value I would semplify the
	function as follows:

		int xto3(int x)
		{
			return x*x*x;
		}
```

## Three

```
    int *align_to_bits(int *addr, int bits){
        return ((uintptr_t)addr) & (uintptr_t)((1<<bits)-1);
    }
	
	the return type differs from what we declared since uintptr_t
	is not int. To solve this, since we are aligning an int variable,
	we can cast (int*) on the return:

		int *align_to_bits(int *addr, int bits)
		{
			return (int*)((uintptr_t)addr) & (uintptr_t)((1<<bits)-1));
		}

	In addition it seems to me that the function do not allign the adderess
	to the selected bits as its name suggest.
	To fix this we can change the right member of the bitwiseAND operation
	in (uintptr_r)(~(bits-1)). In this way the function will return the first
	downwards address (referring to the "addr" argument) that is a multiple
	of the "bits" argument.

	In this way the final function would be:

		int *align_to_bits(int *addr, int bits)
		{
			return (int*)((uintptr_t)addr) & (uintptr_r)(~(bits-1)));
		}
```

## Four
```
    int return0(int *a, int *b) {
        *b=0;
        *a=42;
        return *b;
    }
	
	We cannot do "*a = 42" since "a" is not passed as a pointer.
	Assuming that there is a reason behind assigning 42 to the
	variable "a", that cannot be comprehended just by looking at the
	function (the answer to the ultimate question of life, the universe
	and everything can be hard to grasp!) one solution can be to pass
	"a" as a pointer (int *a). So:
	
		int return0(int *a, int *b) {
			*b=0;
			*a=42;
			return *b;
		}

	if, instead, this is not pun intended and we just want to return
	0 as the function name suggest, we can just remove "a" if we still
	want to assign 0 to "b":

		int return0(int *b)
		{
			*b = 0;
			return *b;
		}

	or even removing both the arguments if we just want to return 0:

		int return0(void)
		{
			return 0;
		}
```
