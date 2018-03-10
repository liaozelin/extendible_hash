#include"hash.h"
#include"setting.h"

int get_hash_value(int key, int depth) {
    if (LOW_HASH)
        return get_hash_value_low(key, depth);
    else
        return get_hash_value_high(key, depth);
}

int get_hash_value_low(int key, int depth) {
    return (key & ((1 << depth) - 1));
}

int get_hash_value_high(int key, int depth) {
    int invert = 0;
    // in key, from high to low, find index of first bit that is 1
    int i;
    for (i = MAX_DEPTH - 1; i >= 0; --i) {
        if ((key & (1 << i)) > 0)
            break;
    }
    int sig = i;

    // aligning the first 1 bit in key from high with depth
    // sig is the first 1 bit in key from high
    // eg: depth is 4, key is ..10110 ==> key become 1011
    // eg: deoth is 4, key is ..00110 ==> key become 1100
    if (sig >= depth)
        key >>= (sig - depth);
    else
        key <<= (depth - sig);

    // set bits whose index is higher than depth to 0
    key &= ((1 << depth) - 1);
    //return key;

    // invert key by depth
    // eg: depth is 4, key is 1010 ==> invert become 0101
    for (i = 1; i <= depth; ++i)
        invert += ((key & (1 << (i - 1))) > 0) << (depth - i);
 
    /** int low = get_hash_value_low(key, depth);
      * for (i = 1; i <= depth; ++i)
      *     invert += ((key & (1 << (i - 1))) > 0) << (depth - i); */

    return invert;
}
