/**
 * @file mutex and isr utilities
 * @author Elia Guglielmin
 */

#ifndef __MUTEX_AND_ISR_H__
#define __MUTEX_AND_ISR_H__

#include <stdbool.h>
#include <stdio.h>

/** @brief disable and enable uart ISR sample macros */
#define ENABLE_UART_ISR() (void)0
#define DISABLE_UART_ISR() (void)0

/** @brief sample of macro to set the desired priority to uart ISR */
#define SET_ISR_PRIORITY(pri) (void)0

/** @brief fake mutex handle struct to use as example */
typedef struct some_mutex_t {
    /** @brief mutex id */
    size_t mutex_id;
    /** @brief mutex state */
    bool is_taken;
} some_mutex_t;

/**
 * @brief simulate mutex initialization
 *
 * @param mutex pointer to mutex to init
 * @param mutex_n mutex id
 */
void mutex_init(some_mutex_t *mutex, size_t mutex_n);

/**
 * @brief simulate mutex take
 *
 * @retval 0 if mutex is avaiable
 * @retval -1 if mutex is already taken
 */
int mutex_take(some_mutex_t *mutex);

/** @brief simulate mutex give */
void mutex_give(some_mutex_t *mutex);

#endif