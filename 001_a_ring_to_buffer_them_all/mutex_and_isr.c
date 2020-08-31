#include "mutex_and_isr.h"

void mutex_init(some_mutex_t *mutex, size_t mutex_n)
{
    mutex->mutex_id = mutex_n;
    mutex->is_taken = false;
}

int mutex_take(some_mutex_t *mutex)
{
	// if the mutex is not free we return an err
    if (mutex->is_taken)
        return -1;

    mutex->is_taken = true;
    return 0;
}

void mutex_give(some_mutex_t *mutex)
{
    mutex->is_taken = false;
}
