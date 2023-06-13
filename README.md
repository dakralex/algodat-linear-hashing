# Linear Hashing set

This is an implementation of the data structure **Linear Hashing** in C++ for 
the course *Algorithms and Data structures 1* at the University of Vienna. The 
requirements are that I implement the given template class without the usage 
of any standard library helping data structures or algorithms, except 
*std::hash* and *std::equal_to*. The goal is to first get a working codebase, 
which should be optimized for space-time in the subsequent builds.

## Linear Hashing

Linear Hashing, in general, is a hash table algorithm that consists of buckets 
labelled *1* through *M*. Each bucket can hold *N* values and when trying to 
insert another value to it will create an overflow bucket. Initially, the pointer 
*next_to_split* will point to the first bucket in the table. Every time an 
overflow bucket is created, the bucket at *next_to_split* will be split. In 
that way, the hash table grows linearly in size, therefore the name *Linear 
Hashing*.

When adding items to the hash table, we use a hash function *h_d(c)* where *c* 
is the value to be stored and *d* the current split round (initially, *d* is 
*0*). *h_d(c)* is defined as *h_d(c) = c mod 2^d * M_0*, where *M_0* is the 
initial amount of buckets (usually, it is *0*). So to begin with, we use *h_0* 
for all buckets that are part of the current split round (here, *0*) until 
the first overflow, where we expand the hash table by one more bucket and 
rehash the values of the bucket at *next_to_split* with *h_{d+1}*. This works, 
because the new hash function considers one more least-significant bit (LSD) 
from the values, so it is ensured that the values will be split between the 
split bucket and new bucket only.

## Example

Here's an example of an already filled hash table and what happens when adding 
another value overflows a bucket.

```
Hash table values
===

Bucket size         N               = 3
Table size          M               = 2
Split round         d               = 1
Split pointer      *next_to_split   = 0b00
Hash function 1     h_1(c)          = c mod 2 (= c mod 2^1)
Hash function 2     h_2(c)          = c mod 4 (= c mod 2^2)

Involved decimals in binary
===

/-------------------------------------\
| Decimal | Binary | Decimal | Binary |
|---------|--------|---------|--------|
| 0       | 0b0000 | 5       | 0b0101 |
| 1       | 0b0001 | 6       | 0b0110 |
| 2       | 0b0010 | 7       | 0b0111 |
| 3       | 0b0011 | 8       | 0b1000 |
| 4       | 0b0100 |         |        |
\-------------------------------------/

Example adding a value before overflow
===

#1: Hash table after adding 0, 1, 2, 3, 4 and 5. Nothing special happened 
    since we only hashed the values and put them in their buckets.

         /––––––-----\
-> 0b00  | 0 | 2 | 4 |
         |-----------|
   0b01  | 1 | 3 | 5 |
         \-----------/

#2: Now we want to insert 6 to the hash table. Here is a step-by-step explain-
    ation of the process.

    #2a: Hash the value 6 with h_1(c)

    h_1(6) = 6 % 2 = 0b0110 % 2 = 0b0
                          ^
                          we care about this bit

            /––––––-----\   /-----------\
    -> 0b0  | 0 | 2 | 4 |-->| 6 |   |   |
            |-----------|   \-----------/
       0b1  | 1 | 3 | 5 |
            \-----------/

    #2b: Since an overflow bucket was created, expand the table by one bucket 
         and rehash the values from 0b0 with h_2(c). Also increment the split 
         pointer.

    h_2(0) = 0b0000 % 2 = 0b00
    h_2(2) = 0b0010 % 2 = 0b10
    h_2(4) = 0b0100 % 2 = 0b00
    h_2(6) = 0b0110 % 2 = 0b10
                 ^^
                 now we care about these bits

             /––––––-----\
       0b00  | 0 | 4 |   |
             |-----------|
    -> 0b01  | 1 | 3 | 5 |
             |-----------|
       0b10  | 2 | 6 |   |
             \-----------/

#3: Now let's insert the value 8 and 7.

    #3a: Hash the value 8 with h_2(c), since h_1(c) < next_to_split

    h_1(8) = 0b1000 % 2 = 0b0
    h_2(8) = 0b1000 % 4 = 0b00

    Okay, it's still the same bucket, so let's insert it there.

             /––––––-----\
       0b00  | 0 | 4 | 8 |
             |-----------|
    -> 0b01  | 1 | 3 | 5 |
             |-----------|
       0b10  | 2 | 6 |   |
             \-----------/

    #3b: Add the value 7 by hashing it with h_1(c), and because it caused an 
         overflow, expand the table by one bucket and rehash the bucket at 
         0b01. Because we split all buckets that we initially had, we reset 
         the split pointer to the first bucket and increment the split round.

    h_1(7) = 0b0111 % 2 = 0b1
    h_2(1) = 0b0001 % 4 = 0b01
    h_2(3) = 0b0011 % 4 = 0b11
    h_2(5) = 0b0101 % 4 = 0b01
    h_2(7) = 0b0111 % 4 = 0b11

             /––––––-----\
    -> 0b00  | 0 | 4 |   |
             |-----------|
       0b01  | 1 | 5 |   |
             |-----------|
       0b10  | 2 | 6 |   |
             |-----------|
       0b11  | 3 | 7 |   |
             \-----------/

Note
===

In this example, all buckets were split where the overflow ocurred. This was a 
coincidence and does not have to be like that. For example, we could have 
added the value 7 in #2, then bucket 0b1 would have overflowed, but bucket 0b0 
would split.
```

## Additional reading

For more information about the algorithm, I advise you to read the 
[original paper (1980)](https://cs-web.bu.edu/faculty/gkollios/ada17/LectNotes/linear-hashing.PDF)
from the creator, Witold Litwin, himself and/or 
[a summary paper](http://delab.csd.auth.gr/papers/LinearHashing2017.pdf).

