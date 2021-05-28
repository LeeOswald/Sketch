/* Generate sizeof(uint32_t) bytes of as random data as possible to seed
   the hash function.
*/

#include "../jansson_port.h"

#include "jansson.h"

volatile uint32_t hashtable_seed = 0;

void json_object_seed(size_t seed) {
    uint32_t new_seed = (uint32_t)seed;

    if (hashtable_seed == 0) {
        if (new_seed == 0) {
          sketch_c_rand(&new_seed, sizeof(new_seed));
        }

        hashtable_seed = new_seed;
    }
}

