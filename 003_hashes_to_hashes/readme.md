# Hashes to hashes

Write a hash table implementation in C where keys and values are both null terminated strings, with
functions to retrieve, insert and delete. Try to optimize for space.

# my approach

The hash table created is an array of linked lists. These linked lists will starts to fill when
collisions need to be handeled.
I choosed to use the "jenkins one at a time hash" algorithm to create the hash index. The reason
why I choose this algorithm is that it has a low rate of collision while not becoming too slow.
Having a low number of collison should allow us to fill as much as possible of the first elements
of the linked lists before starting to create new nodes. In this way we should waste as little as
possible of the pre-allcated memory before starting allocating new one.
For this scope the best solution would have been to create a perfect hash that compared all the
letter of the string using a tree, but that would have been long to implement, so I compromised
with a fast and efficient implementation given the ammount of time given.

The hash table has insert, retrive and delete_node API implemented. To save additional space,
if an added key-value pair is already present, the code just update its value instead of adding
the pair again.

everything is on the hash_table.c file, that also contains some tests on the main(). 