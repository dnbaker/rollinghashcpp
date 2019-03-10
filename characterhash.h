#ifndef CHARACTERHASH
#define CHARACTERHASH

typedef unsigned long long uint64;
typedef unsigned int uint32;
typedef unsigned int uint;

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <random>
#include "mersennetwister.h"

using namespace std;




class mersenneRNG {
public:
    mersenneRNG(uint32 maxval) : mtr(),n(maxval) {};
    uint32 operator()() {
        return mtr.randInt(n);
    }
    void seed(uint32 seedval) {
        mtr.seed(seedval);
    }
    void seed() {
        mtr.seed();
    }
    uint32 rand_max() {
        return n;
    }
private:
    MTRand mtr;
    int n;
};

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
            mersenneRNG randomgenerator(maxval);
            randomgenerator.seed(seed1);
            for(size_t k =0; k<nbrofchars; ++k)
                hashvalues[k] = static_cast<hashvaluetype>(randomgenerator());
        } else if (sizeof(hashvaluetype) == 8) {
            mersenneRNG randomgenerator(maxval>>32);
            mersenneRNG randomgeneratorbase((maxval>>32) ==0 ? maxval : 0xFFFFFFFFU);
            randomgenerator.seed(seed1);
            randomgeneratorbase.seed(seed2);
            for(size_t k =0; k<nbrofchars; ++k) {
                hashvalues[k] = static_cast<hashvaluetype>(randomgeneratorbase())
                                | (static_cast<hashvaluetype>(randomgenerator()) << 32);
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
