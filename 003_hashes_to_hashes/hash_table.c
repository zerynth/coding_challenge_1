#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** hashtab entry */
typedef struct table_entry 
{
    struct table_entry *next;
    char *key;
    char *value;
} table_entry_t;

typedef struct hash_table
{
    size_t size;
    struct table_entry **tab;
} hash_table_t;

/**
 * @brief creates hash_table
 * @param[in] size : table size
 * @note dynamically allocated, remember to delete()
 * @return NULL on failure an structure of type hash_table_t on success
 * */
hash_table_t *create(size_t size)
{
    hash_table_t *h_Table = NULL;
    if ((h_Table = malloc(sizeof(hash_table_t))) == NULL)
        return NULL;
    /* allocate the h_Table table */
    if ((h_Table->tab = malloc(sizeof(table_entry_t) * size)) == NULL)
        return NULL;
    /* Null-initialize table */
    int i;
    for (i = 0; i < size; i++)
        h_Table->tab[i] = NULL;
    h_Table->size = size;
    return h_Table;
}

/**
 * @brief Hash function
 * @param[in] str :  a null terminated string
 * @return the index hash_value
 * */
static unsigned hash_function(char *str)
{
    unsigned hash_value;
    int i =0;
    for (hash_value = 0; *str != '\0'; str++)
    {
		hash_value = *str + 31 * hash_value;
		i++;
	}
    return hash_value / i;
}

/**
 * @brief Delete element from the hash table
 * @param[in] table :  a hash table
 * @return void
 * */
static void delete(table_entry_t *table)
{
    table_entry_t *next;
    while (table != NULL)
    {
        next = table->next;
        free(table->key);
        free(table->value);
        free(table);
        table = next;
    }
}

/**
 * @brief creates a key-value pair
 * @param[in] table :  a hash table
 * @return a NULL on failure, a table entry on success
 * */
static table_entry_t *new(char *key, char *value)
{
    table_entry_t *table = NULL;

    if ((table = calloc(1, sizeof(*table))) == NULL
        || (table->key = strdup(key)) == NULL
        || (table->value = strdup(value)) == NULL)
    {
        delete(table);
        return NULL;
    }
    table->next = NULL;
    return table;
}

/**
 * @brief retrieve element
 * @param[in] htable :  a hash table
 * @param[in] k : the key
 * @return a NULL on failure, a table entry on success
 * */
static table_entry_t *retrieve(hash_table_t *htable, char *k)
{
    table_entry_t *table;
    /* step through linked list */
    for (table = htable->tab[hash_function(htable, k) % htable->size]; table != NULL; table = table->next)
        if (strcmp(table->key, k) == 0)
            return table; /* found */
    return NULL; /* not found */
}

/**
 * @brief inserts the key-value pair
 * @param[in] htable :  a hash table
 * @param[in] key : the key
 * @param[in] value: the value
 * @return a NULL on failure, a table entry on success
 * */
/*  */
hash_table_t *insert(hash_table_t *htable, char *key, char *value)
{
    table_entry_t *table;
    /* unique entry */
    if ((table = retrieve(htable, key)) == NULL)
    {
        table = new(key, value);
        unsigned hashvalue = hash_function(htable, key) % htable->size;
        /* insert at beginning of linked list */
        table->next = htable->tab[hashvalue]; 
        htable->tab[hashvalue] = table;
    }
    /* replace value of previous entry */
    else
    {
        free(table->value);
        if ((table->value = strdup(value)) == NULL)
            return NULL;
    }
    return htable
}
