# Nasal demons

Each of the following snippets of C code is wrong in some way. Fix it and explain why:

## One
```
    #define MIN(a,b) (a < b) ? (a) : (b)
```

## Two
```
    int xto3(volatile int *x){
        return *x * *x * *x;
    }
```

## Three

```
    int *align_to_bits(int *addr, int bits){
        return ((uintptr_t)addr) & (uintptr_t)((1<<bits)-1);
    }
```

## Four
```
    int return0(int *a, int *b) {
        *b=0;
        *a=42;
        return *b;
    }
```
