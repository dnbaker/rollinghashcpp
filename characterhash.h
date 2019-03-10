#ifndef CHARACTERHASH
#define CHARACTERHASH

typedef unsigned long long uint64;
typedef unsigned int uint32;
typedef unsigned int uint;

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <random>
using std::cerr;
using std::runtime_error;
using std::endl;

template <typename hashvaluetype>
#if __cplusplus >= 201402L
constexpr
#endif
hashvaluetype maskfnc(int bits) {
    assert(bits>0);
    assert(unsigned(bits)<=sizeof(hashvaluetype)*8);
    hashvaluetype x = static_cast<hashvaluetype>(1) << (bits - 1);
    return x ^ (x - 1);
}

template <typename hashvaluetype = uint32, typename chartype =  unsigned char>
class CharacterHash {
public:

    CharacterHash(hashvaluetype maxval, uint32 seed1=0x1337, uint32 seed2=0x137) {
        seed(maxval, seed1, seed2);
    }
    void seed(hashvaluetype maxval, uint32_t seed1, uint64_t seed2) {
        if(sizeof(hashvaluetype) <=4) {
            std::mt19937_64 randomgenerator(seed1);
            randomgenerator.seed(seed1);
            hashvaluetype tmaxval = maxval;
            tmaxval |= tmaxval >> 1;
            tmaxval |= tmaxval >> 2;
            tmaxval |= tmaxval >> 4;
            tmaxval |= tmaxval >> 8;
            tmaxval |= tmaxval >> 16;
            --tmaxval;
            for(size_t k =0; k<nbrofchars; ++k) {
                hashvaluetype next;
                do { next = randomgenerator() & tmaxval ;} while(next > maxval);
                hashvalues[k] = next;
            }
        } else if (sizeof(hashvaluetype) == 8) {
            std::mt19937_64 randomgenerator(seed1);
            hashvaluetype tmaxval = maxval;
            tmaxval |= tmaxval >> 1;
            tmaxval |= tmaxval >> 2;
            tmaxval |= tmaxval >> 4;
            tmaxval |= tmaxval >> 8;
            tmaxval |= tmaxval >> 16;
            tmaxval |= tmaxval >> 32;
            --tmaxval;
            for(size_t k =0; k<nbrofchars; ++k) {
                hashvaluetype next;
                do { next = randomgenerator() & tmaxval;} while(next > maxval);
                hashvalues[k] = next;
            }
        } else if (sizeof(hashvaluetype) == 16) {
            std::mt19937_64 randomgenerator(seed1);
            std::mt19937_64 randomgeneratorbase(seed2);
            hashvaluetype tmaxval = maxval;
            tmaxval |= tmaxval >> 1;
            tmaxval |= tmaxval >> 2;
            tmaxval |= tmaxval >> 4;
            tmaxval |= tmaxval >> 8;
            tmaxval |= tmaxval >> 16;
            tmaxval |= tmaxval >> 32;
            tmaxval |= tmaxval >> 64;
            --tmaxval;
            for(size_t k =0; k<nbrofchars; ++k) {
                hashvaluetype val;
                do {
                    val = static_cast<hashvaluetype>(randomgeneratorbase()) |
                         (static_cast<hashvaluetype>(randomgenerator()) << 64);
                    val &= tmaxval;
                } while(val > maxval);
                hashvalues[k] = val;
            }
        } else throw runtime_error("unsupported hash value type");
    }

    enum {nbrofchars = 1 << ( sizeof(chartype)*8 )};

    hashvaluetype hashvalues[1 << ( sizeof(chartype)*8 )];
};

#endif
