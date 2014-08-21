#ifndef NUMBERLIKEARRAY_H
#define NUMBERLIKEARRAY_H
#include<memory.h>
// Make sure we have NULL.
#ifndef NULL
#define NULL 0
#endif

/* A NumberlikeArray<Blk> object holds a heap-allocated array of Blk with a
 * length and a capacity and provides basic memory management features.
 * BigUnsigned and BigUnsignedInABase both subclass it.
 *
 * NumberlikeArray provides no information hiding.  Subclasses should use
 * nonpublic inheritance and manually expose members as desired using
 * declarations like this:
 *
 * public:
 *     NumberlikeArray< the-type-argument >::getLength;
 */

template <typename XXXXX> class NumberlikeArray {
protected:
	unsigned int capacity; // The current allocated capacity of this NumberlikeArray (in blocks)
	unsigned int curr_len; // The actual length of the value stored in this NumberlikeArray (in blocks)
	XXXXX *buffer;
public:
	// The number of bits in a block, defined below.
	static const unsigned int N = 8 * sizeof(XXXXX);
//-------------------------------------------------------------------------
//  Constructor
//-------------------------------------------------------------------------
	// Heap-allocated array of the blocks (can be NULL if curr_len == 0)
	// Constructs a ``zero'' NumberlikeArray with the given capacity.
	NumberlikeArray(unsigned int c) : capacity(c), curr_len(0){
		buffer = (capacity > 0) ? (new XXXXX[capacity]) : NULL;
	}
	/* Constructs a zero NumberlikeArray without allocating a backing array.
	 * A subclass that doesn't know the needed capacity at initialization
	 * time can use this constructor and then overwrite buffer without first
	 * deleting it. */
	NumberlikeArray() : capacity(0), curr_len(0) {
		buffer = NULL;
	}
	// Copy constructor
	NumberlikeArray(const NumberlikeArray<XXXXX> &x) : capacity(x.curr_len), curr_len(x.curr_len){
        buffer = new XXXXX[capacity];
        memcpy(buffer, x.buffer, sizeof(XXXXX) * curr_len);
	}
	// Constructor that copies from a given array of blocks
	NumberlikeArray(const XXXXX *b, unsigned int blen) : capacity(blen), curr_len(blen){
        buffer = new XXXXX[capacity]; // Create array
        memcpy(buffer, b, sizeof(XXXXX) * curr_len);
    }
	~NumberlikeArray(){
		delete [] buffer;
	}
//-------------------------------------------------------------------------
//  Functions
//-------------------------------------------------------------------------
	/* Ensures that the array has at least the requested capacity; may destroy the contents. */
    inline void allocate(unsigned int c){
        if (c <= capacity) return;     // If the requested capacity is not more than the current capacity...
        delete [] buffer;             // Delete the old number array
        capacity = c;
        buffer = new XXXXX[capacity]; // Allocate the new array
    }

	/* Ensures that the array has at least the requested capacity; does not destroy the contents. */
    inline void allocateAndCopy(unsigned int c){
        if (c <= capacity) return;    // If the requested capacity is not more than the current capacity...
        XXXXX *oldBlk = buffer;
        capacity = c;
        buffer = new XXXXX[capacity]; // Allocate the new array
        memcpy(buffer, oldBlk, sizeof(XXXXX) * curr_len);
        delete [] oldBlk;             // Delete the old number array
    }
//-------------------------------------------------------------------------
//  Operator
//-------------------------------------------------------------------------
	// Assignment operator
	inline void operator=(const NumberlikeArray<XXXXX> &x){
        /* Calls like a = a have no effect; catch them before the aliasing
         * causes a problem */
        if (this == &x) return;
        curr_len = x.curr_len;  // Copy length
        allocate(curr_len);     // Expand array if necessary
        memcpy(buffer, x.buffer, sizeof(XXXXX) * curr_len);
	}
	/* Equality comparison: checks if both objects have the same length and
	 * equal (==) array elements to that length.  Subclasses may wish to
	 * override. */
	inline bool operator ==(const NumberlikeArray<XXXXX> &x) const{
        if (curr_len != x.curr_len) return false; // Definitely unequal.
        unsigned int i;
        for (i = 0; i < curr_len; i++) if (buffer[i] != x.buffer[i]) return false; // Compare corresponding blocks one by one.
		return true; // No blocks differed, so the objects are equal.
	}

	inline bool operator !=(const NumberlikeArray<XXXXX> &x) const {
		return !(operator ==(x));
	}
//-------------------------------------------------------------------------
//  ACCESS
//-------------------------------------------------------------------------
	inline unsigned int getCapacity()     const { return capacity;      }
	inline unsigned int getLength()       const { return curr_len;      }
	inline XXXXX getBlock(unsigned int i) const { return buffer[i];        }
	inline bool  isEmpty()                const { return curr_len == 0; }
};


#endif
