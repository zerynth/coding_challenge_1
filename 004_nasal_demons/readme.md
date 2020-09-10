# Nasal demons

Each of the following snippets of C code is wrong in some way. Fix it and explain why:

## One
```
    #define MIN(a,b) (a < b) ? (a) : (b)
```

Missing parentheses in macro expansion (check which is smaller). You can never know what can be provided as argument. Also usual way of declaring the macro is putting it to the do-while(0) loop - this prevents some other compilation issues.

```
    #define MIN(a,b)    do { ((a) < (b)) ? (a) : (b) } while(0)
```

But if possible (C99) inline should be first option - leave compiler to decide.

```
    static inline int min( int a, int b ) { if ( a < b ) return a; return b; }
```

## Two
```
    int xto3(volatile int *x){
        return *x * *x * *x;
    }
```

I see nothing wrong here - this should works but just for the sake of readability dereferencing should be isolated with parentheses.

```
    int xto3(volatile int *x){
        return (*x) * (*x) * (*x);
    }
```

## Three

```
    int *align_to_bits(int *addr, int bits)
    {
        return ((uintptr_t)addr) & (uintptr_t)((1<<bits)-1);
    }
```

In general I am avoiding `uintptr_t` because it is relatively newer feature (of the c++ as I know). I usually use unsigned long in embedded for the particular purpose as usually biggest unsigned type. Also think tha tilde is missing - in this way we will get lower bits of the address not cut of them. Also bits should be unsigned variables - it can be negative from the logical POW.


```
    int *align_to_bits(int *addr, unsigned int bits)
    {
        unsigned long res = ( ( unsigned long )addr ) &
                            (~( unsigned long )( ( 1 << bits ) - 1 );

        return (int *)res;
    }
```

## Four
```
    int return0(int a, int *b) {
        *b=0;
        *a=42;
        return *b;
    }
```

Not sure what the function should do here - and why argument `a` is provided when it have no affect on the result of the function, also it is dereferenced but passed by value. So I suppose that something like this is OK.

```
    int return0(int a, int *b) {
        *b=0;
        (void)a;    //  just to avoid warning and keep original prototype.
        return *b;
    }
```