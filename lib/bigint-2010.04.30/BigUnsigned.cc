
#ifndef ___BigUnsigned_cc__
#define ___BigUnsigned_cc__
#include "BigUnsigned.hh"

// Memory management definitions have moved to the bottom of NumberlikeArray.hh.

// The templates used by these constructors and converters are at the bottom of
// BigUnsigned.hh.

BigUnsigned::BigUnsigned(unsigned long  x) { initFromPrimitive      (x); }
BigUnsigned::BigUnsigned(unsigned int   x) { initFromPrimitive      (x); }
BigUnsigned::BigUnsigned(unsigned short x) { initFromPrimitive      (x); }
BigUnsigned::BigUnsigned(         long  x) { initFromSignedPrimitive(x); }
BigUnsigned::BigUnsigned(         int   x) { initFromSignedPrimitive(x); }
BigUnsigned::BigUnsigned(         short x) { initFromSignedPrimitive(x); }

unsigned long  BigUnsigned::toUnsignedLong () const { return convertToPrimitive      <unsigned long >(); }
unsigned int   BigUnsigned::toUnsignedInt  () const { return convertToPrimitive      <unsigned int  >(); }
unsigned short BigUnsigned::toUnsignedShort() const { return convertToPrimitive      <unsigned short>(); }
long           BigUnsigned::toLong         () const { return convertToSignedPrimitive<         long >(); }
int            BigUnsigned::toInt          () const { return convertToSignedPrimitive<         int  >(); }
short          BigUnsigned::toShort        () const { return convertToSignedPrimitive<         short>(); }

// BIT/BLOCK ACCESSORS

void BigUnsigned::setBlock(unsigned int i, Blk newBlock) {
	if (newBlock == 0) {
		if (i < curr_len) {
			buffer[i] = 0;
			zapLeadingZeros();
		}
		// If i >= curr_len, no effect.
	} else {
		if (i >= curr_len) {
			// The nonzero block extends the number.
			allocateAndCopy(i+1);
			// Zero any added blocks that we aren't setting.
			for (unsigned int j = curr_len; j < i; j++) buffer[j] = 0;
			curr_len = i+1;
		}
		buffer[i] = newBlock;
	}
}

/* Evidently the compiler wants BigUnsigned:: on the return type because, at
 * that point, it hasn't yet parsed the BigUnsigned:: on the name to get the
 * proper scope. */
unsigned int BigUnsigned::bitLength() const {
	if (isZero())
		return 0;
	else {
		Blk leftmostBlock = getBlock(curr_len - 1);
		unsigned int leftmostBlockLen = 0;
		while (leftmostBlock != 0) {
			leftmostBlock >>= 1;
			leftmostBlockLen++;
		}
		return leftmostBlockLen + (curr_len - 1) * N;
	}
}

void BigUnsigned::setBit(unsigned int bi, bool newBit) {
	unsigned int blockI = bi / N;
	Blk block = getBlock(blockI), mask = Blk(1) << (bi % N);
	block = newBit ? (block | mask) : (block & ~mask);
	setBlock(blockI, block);
}

// COMPARISON
BigUnsigned::CmpRes BigUnsigned::compareTo(const BigUnsigned &x) const {
	// A bigger length implies a bigger number.
	if (curr_len < x.curr_len) return less;
	else if (curr_len > x.curr_len)
		return greater;
	else {
		// Compare blocks one by one from left to right.
		unsigned int i = curr_len;
		while (i > 0) {
			i--;
			if (buffer[i] == x.buffer[i])
				continue;
			else if (buffer[i] > x.buffer[i])
				return greater;
			else
				return less;
		}
		// If no blocks differed, the numbers are equal.
		return equal;
	}
}

// COPY-LESS OPERATIONS

/*
 * On most calls to copy-less operations, it's safe to read the inputs little by
 * little and write the outputs little by little.  However, if one of the
 * inputs is coming from the same variable into which the output is to be
 * stored (an "aliased" call), we risk overwriting the input before we read it.
 * In this case, we first compute the result into a temporary BigUnsigned
 * variable and then copy it into the requested output variable *this.
 * Each put-here operation uses the DTRT_ALIASED macro (Do The Right Thing on
 * aliased calls) to generate code for this check.
 *
 * I adopted this approach on 2007.02.13 (see Assignment Operators in
 * BigUnsigned.hh).  Before then, put-here operations rejected aliased calls
 * with an exception.  I think doing the right thing is better.
 *
 * Some of the put-here operations can probably handle aliased calls safely
 * without the extra copy because (for example) they process blocks strictly
 * right-to-left.  At some point I might determine which ones don't need the
 * copy, but my reasoning would need to be verified very carefully.  For now
 * I'll leave in the copy.
 */
#define DTRT_ALIASED(cond, op) \
	if (cond) { \
		BigUnsigned tmpThis; \
		tmpThis.op; \
		*this = tmpThis; \
		return; \
	}



void BigUnsigned::add(const BigUnsigned &a, const BigUnsigned &b) {
	DTRT_ALIASED(this == &a || this == &b, add(a, b));
	// If one argument is zero, copy the other.
	if (a.curr_len == 0) {
		operator =(b);
		return;
	} else if (b.curr_len == 0) {
		operator =(a);
		return;
	}
	// Some variables...
	// Carries in and out of an addition stage
	bool carryIn, carryOut;
	Blk temp;
	unsigned int i;
	// a2 points to the longer input, b2 points to the shorter
	const BigUnsigned *a2, *b2;
	if (a.curr_len >= b.curr_len) {
		a2 = &a;
		b2 = &b;
	} else {
		a2 = &b;
		b2 = &a;
	}
	// Set prelimiary length and make room in this BigUnsigned
	curr_len = a2->curr_len + 1;
	allocate(curr_len);
	// For each block index that is present in both inputs...
	for (i = 0, carryIn = false; i < b2->curr_len; i++) {
		// Add input blocks
		temp = a2->buffer[i] + b2->buffer[i];
		// If a rollover occurred, the result is less than either input.
		// This test is used many times in the BigUnsigned code.
		carryOut = (temp < a2->buffer[i]);
		// If a carry was input, handle it
		if (carryIn) {
			temp++;
			carryOut |= (temp == 0);
		}
		buffer[i] = temp; // Save the addition result
		carryIn = carryOut; // Pass the carry along
	}
	// If there is a carry left over, increase blocks until
	// one does not roll over.
	for (; i < a2->curr_len && carryIn; i++) {
		temp = a2->buffer[i] + 1;
		carryIn = (temp == 0);
		buffer[i] = temp;
	}
	// If the carry was resolved but the larger number
	// still has blocks, copy them over.
	for (; i < a2->curr_len; i++)
		buffer[i] = a2->buffer[i];
	// Set the extra block if there's still a carry, decrease length otherwise
	if (carryIn)
		buffer[i] = 1;
	else
		curr_len -= 1;
}

void BigUnsigned::subtract(const BigUnsigned &a, const BigUnsigned &b) {
	DTRT_ALIASED(this == &a || this == &b, subtract(a, b));
	if (b.curr_len == 0) {
		// If b is zero, copy a.
		operator =(a);
		return;
	} else if (a.curr_len < b.curr_len)
		// If a is shorter than b, the result is negative.
		throw "BigUnsigned::subtract: "
			"Negative result in unsigned calculation";
	// Some variables...
	bool borrowIn, borrowOut;
	Blk temp;
	unsigned int i;
	// Set preliminary length and make room
	curr_len = a.curr_len;
	allocate(curr_len);
	// For each block index that is present in both inputs...
	for (i = 0, borrowIn = false; i < b.curr_len; i++) {
		temp = a.buffer[i] - b.buffer[i];
		// If a reverse rollover occurred,
		// the result is greater than the block from a.
		borrowOut = (temp > a.buffer[i]);
		// Handle an incoming borrow
		if (borrowIn) {
			borrowOut |= (temp == 0);
			temp--;
		}
		buffer[i] = temp; // Save the subtraction result
		borrowIn = borrowOut; // Pass the borrow along
	}
	// If there is a borrow left over, decrease blocks until
	// one does not reverse rollover.
	for (; i < a.curr_len && borrowIn; i++) {
		borrowIn = (a.buffer[i] == 0);
		buffer[i] = a.buffer[i] - 1;
	}
	/* If there's still a borrow, the result is negative.
	 * Throw an exception, but zero out this object so as to leave it in a
	 * predictable state. */
	if (borrowIn) {
		curr_len = 0;
		throw "BigUnsigned::subtract: Negative result in unsigned calculation";
	} else
		// Copy over the rest of the blocks
		for (; i < a.curr_len; i++)
			buffer[i] = a.buffer[i];
	// Zap leading zeros
	zapLeadingZeros();
}

/*
 * About the multiplication and division algorithms:
 *
 * I searched unsucessfully for fast C++ built-in operations like the `b_0'
 * and `c_0' Knuth describes in Section 4.3.1 of ``The Art of Computer
 * Programming'' (replace `place' by `Blk'):
 *
 *    ``b_0[:] multiplication of a one-place integer by another one-place
 *      integer, giving a two-place answer;
 *
 *    ``c_0[:] division of a two-place integer by a one-place integer,
 *      provided that the quotient is a one-place integer, and yielding
 *      also a one-place remainder.''
 *
 * I also missed his note that ``[b]y adjusting the word size, if
 * necessary, nearly all computers will have these three operations
 * available'', so I gave up on trying to use algorithms similar to his.
 * A future version of the library might include such algorithms; I
 * would welcome contributions from others for this.
 *
 * I eventually decided to use bit-shifting algorithms.  To multiply `a'
 * and `b', we zero out the result.  Then, for each `1' bit in `a', we
 * shift `b' left the appropriate amount and add it to the result.
 * Similarly, to divide `a' by `b', we shift `b' left varying amounts,
 * repeatedly trying to subtract it from `a'.  When we succeed, we note
 * the fact by setting a bit in the quotient.  While these algorithms
 * have the same O(n^2) time complexity as Knuth's, the ``constant factor''
 * is likely to be larger.
 *
 * Because I used these algorithms, which require single-block addition
 * and subtraction rather than single-block multiplication and division,
 * the innermost loops of all four routines are very similar.  Study one
 * of them and all will become clear.
 */

/*
 * This is a little inline function used by both the multiplication
 * routine and the division routine.
 *
 * `getShiftedBlock' returns the `x'th block of `num << y'.
 * `y' may be anything from 0 to N - 1, and `x' may be anything from
 * 0 to `num.len'.
 *
 * Two things contribute to this block:
 *
 * (1) The `N - y' low bits of `num.buffer[x]', shifted `y' bits left.
 *
 * (2) The `y' high bits of `num.buffer[x-1]', shifted `N - y' bits right.
 *
 * But we must be careful if `x == 0' or `x == num.len', in
 * which case we should use 0 instead of (2) or (1), respectively.
 *
 * If `y == 0', then (2) contributes 0, as it should.  However,
 * in some computer environments, for a reason I cannot understand,
 * `a >> b' means `a >> (b % N)'.  This means `num.buffer[x-1] >> (N - y)'
 * will return `num.buffer[x-1]' instead of the desired 0 when `y == 0';
 * the test `y == 0' handles this case specially.
 */
inline BigUnsigned::Blk getShiftedBlock(const BigUnsigned &num, unsigned int x, unsigned int y) {
	BigUnsigned::Blk part1 = (x == 0 || y == 0) ? 0 : (num.buffer[x - 1] >> (BigUnsigned::N - y));
	BigUnsigned::Blk part2 = (x == num.curr_len) ? 0 : (num.buffer[x] << y);
	return part1 | part2;
}

void BigUnsigned::multiply(const BigUnsigned &a, const BigUnsigned &b) {
	DTRT_ALIASED(this == &a || this == &b, multiply(a, b));
	// If either a or b is zero, set to zero.
	if (a.curr_len == 0 || b.curr_len == 0) {
		curr_len = 0;
		return;
	}
	/*
	 * Overall method:
	 *
	 * Set this = 0.
	 * For each 1-bit of `a' (say the `i2'th bit of block `i'):
	 *    Add `b << (i blocks and i2 bits)' to *this.
	 */
	// Variables for the calculation
	unsigned int i, j, k;
	unsigned int i2;
	Blk temp;
	bool carryIn, carryOut;
	// Set preliminary length and make room
	curr_len = a.curr_len + b.curr_len;
	allocate(curr_len);
	// Zero out this object
	for (i = 0; i < curr_len; i++) buffer[i] = 0;
	// For each block of the first number...
	for (i = 0; i < a.curr_len; i++) {
		// For each 1-bit of that block...
		for (i2 = 0; i2 < N; i2++) {
			if ((a.buffer[i] & (Blk(1) << i2)) == 0)
				continue;
			/*
			 * Add b to this, shifted left i blocks and i2 bits.
			 * j is the index in b, and k = i + j is the index in this.
			 *
			 * `getShiftedBlock', a short inline function defined above,
			 * is now used for the bit handling.  It replaces the more
			 * complex `bHigh' code, in which each run of the loop dealt
			 * immediately with the low bits and saved the high bits to
			 * be picked up next time.  The last run of the loop used to
			 * leave leftover high bits, which were handled separately.
			 * Instead, this loop runs an additional time with j == b.curr_len.
			 * These changes were made on 2005.01.11.
			 */
			for (j = 0, k = i, carryIn = false; j <= b.curr_len; j++, k++) {
				/*
				 * The body of this loop is very similar to the body of the first loop
				 * in `add', except that this loop does a `+=' instead of a `+'.
				 */
				temp = buffer[k] + getShiftedBlock(b, j, i2);
				carryOut = (temp < buffer[k]);
				if (carryIn) {
					temp++;
					carryOut |= (temp == 0);
				}
				buffer[k] = temp;
				carryIn = carryOut;
			}
			// No more extra iteration to deal with `bHigh'.
			// Roll-over a carry as necessary.
			for (; carryIn; k++) {
				buffer[k]++;
				carryIn = (buffer[k] == 0);
			}
		}
	}
	// Zap possible leading zero
	if (buffer[curr_len - 1] == 0) curr_len -= 1;
}

/*
 * DIVISION WITH REMAINDER
 * This monstrous function mods *this by the given divisor b while storing the
 * quotient in the given object q; at the end, *this contains the remainder.
 * The seemingly bizarre pattern of inputs and outputs was chosen so that the
 * function copies as little as possible (since it is implemented by repeated
 * subtraction of multiples of b from *this).
 *
 * "modWithQuotient" might be a better name for this function, but I would
 * rather not change the name now.
 */
void BigUnsigned::divideWithRemainder(const BigUnsigned &b, BigUnsigned &q) {
	/* Defending against aliased calls is more complex than usual because we
	 * are writing to both *this and q.
	 *
	 * It would be silly to try to write quotient and remainder to the
	 * same variable.  Rule that out right away. */
	if (this == &q)
		throw "BigUnsigned::divideWithRemainder: Cannot write quotient and remainder into the same variable";
	/* Now *this and q are separate, so the only concern is that b might be
	 * aliased to one of them.  If so, use a temporary copy of b. */
	if (this == &b || &q == &b) {
		BigUnsigned tmpB(b);
		divideWithRemainder(tmpB, q);
		return;
	}

	/*
	 * Knuth's definition of mod (which this function uses) is somewhat
	 * different from the C++ definition of % in case of division by 0.
	 *
	 * We let a / 0 == 0 (it doesn't matter much) and a % 0 == a, no
	 * exceptions thrown.  This allows us to preserve both Knuth's demand
	 * that a mod 0 == a and the useful property that
	 * (a / b) * b + (a % b) == a.
	 */
	if (b.curr_len == 0) {
		q.curr_len = 0;
		return;
	}

	/*
	 * If *this.curr_len < b.curr_len, then *this < b, and we can be sure that b doesn't go into
	 * *this at all.  The quotient is 0 and *this is already the remainder (so leave it alone).
	 */
	if (curr_len < b.curr_len) {
		q.curr_len = 0;
		return;
	}

	// At this point we know (*this).curr_len >= b.curr_len > 0.  (Whew!)

	/*
	 * Overall method:
	 *
	 * For each appropriate i and i2, decreasing:
	 *    Subtract (b << (i blocks and i2 bits)) from *this, storing the
	 *      result in subtractBuf.
	 *    If the subtraction succeeds with a nonnegative result:
	 *        Turn on bit i2 of block i of the quotient q.
	 *        Copy subtractBuf back into *this.
	 *    Otherwise bit i2 of block i remains off, and *this is unchanged.
	 *
	 * Eventually q will contain the entire quotient, and *this will
	 * be left with the remainder.
	 *
	 * subtractBuf[x] corresponds to blk[x], not blk[x+i], since 2005.01.11.
	 * But on a single iteration, we don't touch the i lowest blocks of blk
	 * (and don't use those of subtractBuf) because these blocks are
	 * unaffected by the subtraction: we are subtracting
	 * (b << (i blocks and i2 bits)), which ends in at least `i' zero
	 * blocks. */
	// Variables for the calculation
	unsigned int i, j, k;
	unsigned int i2;
	Blk temp;
	bool borrowIn, borrowOut;

	/*
	 * Make sure we have an extra zero block just past the value.
	 *
	 * When we attempt a subtraction, we might shift `b' so
	 * its first block begins a few bits left of the dividend,
	 * and then we'll try to compare these extra bits with
	 * a nonexistent block to the left of the dividend.  The
	 * extra zero block ensures sensible behavior; we need
	 * an extra block in `subtractBuf' for exactly the same reason.
	 */
	unsigned int origLen = curr_len; // Save real length.
	/* To avoid an out-of-bounds access in case of reallocation, allocate
	 * first and then increment the logical length. */
	allocateAndCopy(curr_len + 1);
	curr_len++;
	buffer[origLen] = 0; // Zero the added block.

	// subtractBuf holds part of the result of a subtraction; see above.
	Blk *subtractBuf = new Blk[curr_len];

	// Set preliminary length for quotient and make room
	q.curr_len = origLen - b.curr_len + 1;
	q.allocate(q.curr_len);
	// Zero out the quotient
	for (i = 0; i < q.curr_len; i++)
		q.buffer[i] = 0;

	// For each possible left-shift of b in blocks...
	i = q.curr_len;
	while (i > 0) {
		i--;
		// For each possible left-shift of b in bits...
		// (Remember, N is the number of bits in a Blk.)
		q.buffer[i] = 0;
		i2 = N;
		while (i2 > 0) {
			i2--;
			/*
			 * Subtract b, shifted left i blocks and i2 bits, from *this,
			 * and store the answer in subtractBuf.  In the for loop, `k == i + j'.
			 *
			 * Compare this to the middle section of `multiply'.  They
			 * are in many ways analogous.  See especially the discussion
			 * of `getShiftedBlock'.
			 */
			for (j = 0, k = i, borrowIn = false; j <= b.curr_len; j++, k++) {
				temp = buffer[k] - getShiftedBlock(b, j, i2);
				borrowOut = (temp > buffer[k]);
				if (borrowIn) {
					borrowOut |= (temp == 0);
					temp--;
				}
				// Since 2005.01.11, indices of `subtractBuf' directly match those of `buffer', so use `k'.
				subtractBuf[k] = temp;
				borrowIn = borrowOut;
			}
			// No more extra iteration to deal with `bHigh'.
			// Roll-over a borrow as necessary.
			for (; k < origLen && borrowIn; k++) {
				borrowIn = (buffer[k] == 0);
				subtractBuf[k] = buffer[k] - 1;
			}
			/*
			 * If the subtraction was performed successfully (!borrowIn),
			 * set bit i2 in block i of the quotient.
			 *
			 * Then, copy the portion of subtractBuf filled by the subtraction
			 * back to *this.  This portion starts with block i and ends--
			 * where?  Not necessarily at block `i + b.curr_len'!  Well, we
			 * increased k every time we saved a block into subtractBuf, so
			 * the region of subtractBuf we copy is just [i, k).
			 */
			if (!borrowIn) {
				q.buffer[i] |= (Blk(1) << i2);
				while (k > i) {
					k--;
					buffer[k] = subtractBuf[k];
				}
			}
		}
	}
	// Zap possible leading zero in quotient
	if (q.buffer[q.curr_len - 1] == 0)
		q.curr_len--;
	// Zap any/all leading zeros in remainder
	zapLeadingZeros();
	// Deallocate subtractBuf.
	// (Thanks to Brad Spencer for noticing my accidental omission of this!)
	delete [] subtractBuf;
}

/* BITWISE OPERATORS
 * These are straightforward blockwise operations except that they differ in
 * the output length and the necessity of zapLeadingZeros. */

void BigUnsigned::bitAnd(const BigUnsigned &a, const BigUnsigned &b) {
	DTRT_ALIASED(this == &a || this == &b, bitAnd(a, b));
	// The bitwise & can't be longer than either operand.
	curr_len = (a.curr_len >= b.curr_len) ? b.curr_len : a.curr_len;
	allocate(curr_len);
	unsigned int i;
	for (i = 0; i < curr_len; i++) buffer[i] = a.buffer[i] & b.buffer[i];
	zapLeadingZeros();
}

void BigUnsigned::bitOr(const BigUnsigned &a, const BigUnsigned &b) {
	DTRT_ALIASED(this == &a || this == &b, bitOr(a, b));
	unsigned int i;
	const BigUnsigned *a2, *b2;
	if (a.curr_len >= b.curr_len) {
		a2 = &a;
		b2 = &b;
	} else {
		a2 = &b;
		b2 = &a;
	}
	allocate(a2->curr_len);
	for (i = 0; i < b2->curr_len; i++) buffer[i] = a2->buffer[i] | b2->buffer[i];
	for (; i < a2->curr_len; i++) buffer[i] = a2->buffer[i];
	curr_len = a2->curr_len;
	// Doesn't need zapLeadingZeros.
}

void BigUnsigned::bitXor(const BigUnsigned &a, const BigUnsigned &b) {
	DTRT_ALIASED(this == &a || this == &b, bitXor(a, b));
	unsigned int i;
	const BigUnsigned *a2, *b2;
	if (a.curr_len >= b.curr_len) {
		a2 = &a;
		b2 = &b;
	} else {
		a2 = &b;
		b2 = &a;
	}
	allocate(a2->curr_len);
	for (i = 0; i < b2->curr_len; i++) buffer[i] = a2->buffer[i] ^ b2->buffer[i];
	for (; i < a2->curr_len; i++) buffer[i] = a2->buffer[i];
	curr_len = a2->curr_len;
	zapLeadingZeros();
}

void BigUnsigned::bitShiftLeft(const BigUnsigned &a, int b) {
	DTRT_ALIASED(this == &a, bitShiftLeft(a, b));
	if (b < 0) {
		if (b << 1 == 0)
			throw "BigUnsigned::bitShiftLeft: "
				"Pathological shift amount not implemented";
		else {
			bitShiftRight(a, -b);
			return;
		}
	}
	unsigned int shiftBlocks = b / N;
	unsigned int shiftBits = b % N;
	// + 1: room for high bits nudged left into another block
	curr_len = a.curr_len + shiftBlocks + 1;
	allocate(curr_len);
	unsigned int i, j;
	for (i = 0; i < shiftBlocks; i++) buffer[i] = 0;
	for (j = 0, i = shiftBlocks; j <= a.curr_len; j++, i++) buffer[i] = getShiftedBlock(a, j, shiftBits);
	// Zap possible leading zero
	if (buffer[curr_len - 1] == 0) curr_len -= 1;
}

void BigUnsigned::bitShiftRight(const BigUnsigned &a, int b) {
	DTRT_ALIASED(this == &a, bitShiftRight(a, b));
	if (b < 0) {
		if (b << 1 == 0)
			throw "BigUnsigned::bitShiftRight: "
				"Pathological shift amount not implemented";
		else {
			bitShiftLeft(a, -b);
			return;
		}
	}
	// This calculation is wacky, but expressing the shift as a left bit shift
	// within each block lets us use getShiftedBlock.
	unsigned int rightShiftBlocks = (b + N - 1) / N;
	unsigned int leftShiftBits = N * rightShiftBlocks - b;
	// Now (N * rightShiftBlocks - leftShiftBits) == b
	// and 0 <= leftShiftBits < N.
	if (rightShiftBlocks >= a.curr_len + 1) {
		// All of a is guaranteed to be shifted off, even considering the left
		// bit shift.
		curr_len = 0;
		return;
	}
	// Now we're allocating a positive amount.
	// + 1: room for high bits nudged left into another block
	curr_len = a.curr_len + 1 - rightShiftBlocks;
	allocate(curr_len);
	unsigned int i, j;
	for (j = rightShiftBlocks, i = 0; j <= a.curr_len; j++, i++) buffer[i] = getShiftedBlock(a, j, leftShiftBits);
	// Zap possible leading zero
	if (buffer[curr_len - 1] == 0) curr_len -= 1;
}

// INCREMENT/DECREMENT OPERATORS

// Prefix increment
void BigUnsigned::operator ++() {
	unsigned int i;
	bool carry = true;
	for (i = 0; i < curr_len && carry; i++) {
		buffer[i]++;
		carry = (buffer[i] == 0);
	}
	if (carry) {
		// Allocate and then increase curr_length, as in divideWithRemainder
		allocateAndCopy(curr_len + 1);
		curr_len++;
		buffer[i] = 1;
	}
}

// Postfix increment: same as prefix
void BigUnsigned::operator ++(int) {
	operator ++();
}

// Prefix decrement
void BigUnsigned::operator --() {
	if (curr_len == 0)
		throw "BigUnsigned::operator --(): Cannot decrement an unsigned zero";
	unsigned int i;
	bool borrow = true;
	for (i = 0; borrow; i++) {
		borrow = (buffer[i] == 0);
		buffer[i]--;
	}
	// Zap possible leading zero (there can only be one)
	if (buffer[curr_len - 1] == 0) curr_len -= 1;
}

// Postfix decrement: same as prefix
void BigUnsigned::operator --(int) {
	operator --();
}
#undef DTRT_ALIASED
#endif
