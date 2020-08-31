#define MINIMUM_HT_SIZE 128 // it is the minimum hash table size
#define SIZE_INCREMENT 32 // it is the default resize increment
//key-value are null-terminated string with predefined maximum size to avoid memory fragmentation
#define MAX_KEYS_SIZE 32 // max size for key strings
#define MAX_VALS_SIZE 64 // max size for value strings

typedef struct{
    char  		key[MAX_KEYS_SIZE];
	  char   		val[MAX_VALS_SIZE];
	  uint32_t	hash;
    uint32_t	psl; //probe sequence length for Robin Hood hashing algorithm
}bucket;

//variables to manage the hash table
size_t ht_size; // size of the hash table
size_t ht_count; // number of items in the hash table
bucket *ht_buckets; // items in the hash table
/////////////////////////////////////////////

//return the hash value computed on the key string
uint32_t compute_hash(const char *key)
{
    uint32_t sum = 0;
    //character position is used as weight to avoid the same hash value with keys
    //containing the same characters but in different position
    for(size_t i=0; key[i]; i++)
        sum += (i+1)*key[i];
    return sum;
}

//return the pointer to the bucket that has been putted in
bucket *ht_put(const char *key, const char *val)
{
    uint32_t idx;
    bucket *bck;
    bucket newbucket;

    if(strlen(key)>MAX_KEYS_SIZE) return NULL;
    strcpy(newbucket.key, key);
    if(strlen(val)>MAX_VALS_SIZE) return NULL;
    strcpy(newbucket.val, val);

	  newbucket.hash = compute_hash(key);
    newbucket.psl = 0; //initialize probe sequence length

	  idx = newbucket.hash%ht_size;
    bck = &ht_buckets[idx];
    while(bck->key){ //buckect is not free
        if (bck->hash == newbucket.hash && strcmp(bck->key, newbucket.key) == 0){ // Duplicate key
            return bck; // return the existing value
        }

        // if PSL of the element being inserted is greater than PSL
        // of the element in the bucket, then swap them and continue.
        if (newbucket.psl > bck->psl){
             bucket tmpbck;
             tmpbck = newbucket;
             newbucket = *bck;
             *bck = tmpbck;
        }
        newbucket.psl++;

		    idx = (idx+1)%ht_size; // continue to the next bucket
		    bck = &ht_buckets[idx];
    }

    //Found a free bucket
	  *bck = newbucket; // copy the newbucket
	  ht_count++;
	  return bck; // return the new value
}

//return the pointer to the new hash table in case of success
//return NULL pointer in case of error
bucket *ht_resize(size_t size)
{
    bucket *oldbuckets = ht_buckets;
    size_t oldsize = ht_size;
    bucket *newbuckets;

    if(size<1) return NULL;
    if(size<ht_count) return NULL; //new size must contains old buckets at least

    newbuckets = calloc(size, sizeof(bucket));
    if(newbuckets)==NULL) return NULL;

    ht_buckets = newbuckets;
    ht_size = size;
    ht_count = 0;

    //copy of the existing buckets
    for (size_t i = 0; i < oldsize; i++){
        bucket *bucket = &oldbuckets[i];

        if (!bucket->key){ // Skip the empty buckets
            continue;
        }

        ht_put(bucket->key, bucket->val);
    }

    if (oldbuckets) {
        free(oldbuckets);
    }

    ht_buckets = newbuckets;
    return newbuckets;
}

//return the pointer to the new hash table in case of success
//return NULL pointer in case of error
bucket *ht_create(size_t size)
{
    ht_size=0;
    ht_count=0;
    ht_buckets=NULL;
		return ht_resize(MAX(size, MINIMUM_HT_SIZE));
}

void ht_destroy()
{
    if (ht_buckets){
		    free(ht_buckets);
        ht_buckets=NULL;
	  }
    ht_size=0;
    ht_count=0;
}

//return the pointer to the bucket that has been inserted
bucket *ht_insert(const char *key, const char *val)
{
    bucket *res;
    res=ht_put(key, val);
    if(ht_count==ht_size){ //resize hash table when item count == hash table size
        if(ht_resize(ht_size+SIZE_INCREMENT)==NULL)
            return NULL;
    }
    return res;
}


//return the value associated to the key in case of success
//return NULL in case of error
char *ht_retrieve(const char *key)
{
    uint32_t idx;
    uint32_t psl=0;
    bucket *bck;

    if(strlen(key)>MAX_KEYS_SIZE) return NULL;
    uint32_t hash = compute_hash(key);

    idx = hash%ht_size;
    bck = &ht_buckets[idx];
    while(bck->key){ //stop linear probing if we hit an empty bucket
        if (bck->hash == hash && strcmp(bck->key, key) == 0) { //found!!!
		        return bck->val;
	      }

        //stop linear probing if we hit a "rich" bucket which should have been captured
        if (psl > bck->psl){
      	    return NULL;
      	}
      	psl++;

        idx = (idx+1)%ht_size; // continue to the next bucket
		    bck = &ht_buckets[idx];
    }
    //return NULL if we hit an empty bucket
    return NULL;
}

//return the value associated to the key in case of success
//return NULL in case of error
char *ht_delete(const char *key)
{
    uint32_t idx;
    uint32_t psl=0;
    bucket *bck;
    char *val;

    if(strlen(key)>MAX_KEYS_SIZE) return NULL;
    uint32_t hash = compute_hash(key);

    idx = hash%ht_size;
    bck = &ht_buckets[idx];
    if(!bck->key) return NULL; //return NULL if we hit an empty bucket
    while(bck->hash != hash || strcmp(bck->key, key) != 0){ //stop linear probing when bucket is found
        //stop linear probing if we hit a "rich" bucket which should have been captured
        if (psl > bck->psl){
		        return NULL;
	      }
        psl++;

        idx = (idx+1)%ht_size; // continue to the next bucket
        bck = &ht_buckets[idx];
        if(!bck->key) return NULL; //return NULL if we hit an empty bucket
    }

    // Free the bucket
		val = bck->val;
	  ht_count--;

    //the following code preserve the probe sequence in case of deletion
	  while(1){
		    bucket *nextbck;

				idx = (idx+1)%ht_size; // continue to the next bucket
		    nextbck = &ht_buckets[idx];

        //stop sequence preservation in case of empty bucket or key in its base location
        if (!nextbck->key || nextbck->psl == 0){
			       break;
		    }
        nextbck->psl--;
		    *bck = *nextbck;
		    bck = nextbck;
	}

	return val;

}
