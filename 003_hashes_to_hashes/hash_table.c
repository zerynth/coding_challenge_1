#define _CRT_SECURE_NO_WARNINGS // to avoid some C11 warnings on my machine. Might be removed on other machines.
/**
 * @file custom hash table example
 * @author Elia Guglielmin
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/**
 * @brief size of the hash table
 *
 * prime number to avoid pattern that can cause collision
 * when we use % operator to rescale the original hash 
 * calculated.
 * Aside from that, it is hard to estimate the right size of
 * our hash table, since we do not know how many pairs of 
 * kyes-values we might have to store. 
 */
#define HASH_SIZE 997

/** @brief node struct for the linked list */
typedef struct node {
    /** @brief pointer to node key */
    char* key;
    /** @brief pointer to the data of the node */
    char *node_data;
    /** @brief pointer to the next node */
    struct node* next;
} node;

/** @brief main hash table */
node* hash_table[HASH_SIZE] = { NULL };

/** @brief initialize hash tabe to NULL */
static void init_hash_table(void)
{
    for (int i = 0; i < HASH_SIZE; i++) {
        hash_table[i] = (node*)malloc(sizeof(node));
        hash_table[i]->key = NULL;
        hash_table[i]->node_data = NULL;
        hash_table[i]->next = NULL;
    }
}

/**
 * @brief jenkins one at a time hash
 *
 * this hash was choosen after some researches trying to
 * find a good hash for string that had a low rate of conflicts,
 * while also avoinding to create a perferct hash that checks every
 * letter of the key string one by one.
 * My idea was that, not knowing how many entries the code should
 * handle in advance, one should try to have as little as possible
 * conflicts to avoid wasting allocation space creating long linked
 * list, while having memory initially allocated from "node* hash_table[]"
 * still free to use.
 *
 * @param key pointer to key
 * @param len lenght of the key
 *
 * @retval hash value for the key
 */
static uint32_t hash(const char *key, size_t len)
{
    size_t i = 0;
    uint32_t hash = 0;
    while (i != len) {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;

    return hash;
}

/**
 * @brief return index scaled to the size of the hash table
 *
 * @param key pointer to key
 * @param len lenght of the key
 *
 * @retval hash index for the key
 */
static uint32_t get_hash_rescaled(const char* key, size_t len)
{
    uint32_t index = 0;
    index = hash(key, len);
	// rescale the hash with the size of our table
    return (index % HASH_SIZE);
}

/**
 * @brief delete a node
 *
 * can be used for both temporary node and hash table node
 *
 * @param node node to delete pointer
 */
void delete_node(node* node)
{
    uint32_t index = get_hash_rescaled(node->key, strlen(node->key));
    if (node != NULL) {
        // if we have a next on the node we need to make this node the first of the linked list
        if (node->next != NULL) {
            hash_table[index] = node->next;
        }
        // memory is freed
        free(node->key);
        free(node->node_data);
        node->next = NULL;
        free(node);
    }
}

/**
 * @brief allocate the required memory to get data to a node
 *
 * @param node pointer to node to allcoate
 * @param key pinter to key
 * @param key_len key string length
 * @param value pointer to value of node
 * @param value_len value string length
 */
static void allocate_node_memory(node* ptr_node, const char* key, size_t key_len,
    const char* value, size_t value_len)
{
    // allocate memory
    ptr_node->key = (char*)calloc(key_len, sizeof(char)+1);
    ptr_node->node_data = (char*)calloc(value_len, sizeof(char)+1);
    ptr_node->next = NULL;
    // copy values
    strcpy(ptr_node->key, key);
    strcpy(ptr_node->node_data, value);
}

/**
 * @brief insert a new pair into the hash table
 *
 * @param key key of the value
 * @param value value to store in the node
 */
static void insert(const char* key, const char* value)
{
    size_t key_len = strlen(key);
    size_t value_len = strlen(value);
    //get the index for the key passed
    uint32_t index = get_hash_rescaled(key, key_len);

    node* ptr_node = (node*)malloc(sizeof(node));
    if (ptr_node == NULL)
        return;

    // allocate memory for the node data and store the value string
    allocate_node_memory(ptr_node, key, key_len, value, value_len);

    // if the node is empty we store the data
    if (hash_table[index]->key == NULL)
        hash_table[index] = ptr_node;
    else {
        // if the key is already present on the hash table, we change its value.
        // in this way we can save memory from duplicates.
        if (strncmp(hash_table[index]->key, key, key_len) == 0) {
            // we the new string might be longer the the previous one so we need to reallocate memory
            free(hash_table[index]->node_data);
            hash_table[index]->node_data = (char*)malloc(value_len+1);
            strcpy(hash_table[index]->node_data, value);
            // we free the memory used by the node since datas are already allocated in the table
            delete_node(ptr_node);
            return;
        }

        // if the node is not empty we iter the linked list looking for an empty one
        node* ptr_iter = hash_table[index];
        while (1) {
            // the next node in the linked list is free.
            // we can set it directly with our new node.
            if (ptr_iter->next == NULL) {
                ptr_iter->next = ptr_node;
                break;
            }
            // move to the next node
            ptr_iter = ptr_iter->next;
        }
    }
}

/**
 * @brief retrive the desired node if key is present
 *
 * search the desired key on the hash table and in all the linked list
 * checking the hash index first and comparing the string on the 
 * linked list next.
 *
 * @param key pointer to the key to search
 *
 * @retval node pointer to the node
 * @retval NULL if the node cannot be found
 */
static node *retrive(const char *key)
{   
    size_t key_len = strlen(key);
    uint32_t index = get_hash_rescaled(key, key_len);

    node* ptr_node = hash_table[index];
    // if the node is empty we return NULL
    if (ptr_node == NULL)
        return NULL;

    // the node is not empty, we compare teh strings itering on the linked list
    while (strncmp(ptr_node->key, key, key_len) != 0) { // see allcoate_node_memory func for why strncmp is used
        // if next is NULL and we didn't find out match we return NULL
        if (ptr_node->next == NULL)
            return NULL;
        // move to the next node
        ptr_node = ptr_node->next;
    }

    // we fount the match, so we return the pointer to the node.
    return ptr_node;
}

/** some test of functionalities*/
int main()
{   
    // initialize hash table
    init_hash_table();
    // add a node
    insert("Mario Rossi", "126.016.254.001");
    insert("Paolo Bianchi", "126.016.254.006");
    // read the values of the two stored node
    node* mr = retrive("Mario Rossi");
    node* pb = retrive("Paolo Bianchi");

    // print the values 
    printf(mr->node_data);
    printf("\n");
    printf(pb->node_data);

    // test to see if the substitution work
    insert("Mario Rossi", "126.016.254.003");
    printf("\n");
    printf(mr->node_data);

    // clear the nodes
    delete_node(mr);
    delete_node(pb);
    // print here causes exception error since it cannot handle NULLs
}

