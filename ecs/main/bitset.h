#ifndef BITSET_H
#define BITSET_H

#include "vector.h"

struct BitSet {

	uint64_t fast_bits = 0;

	Vector<uint64_t> bits = Vector<uint64_t>();

	_FORCE_INLINE_ void set(uint32_t p_index, bool p_value) {
		uint32_t element_index = p_index / 64;

		bool in_first = element_index == 0;

		if (in_first) {
			if (p_value) {
				fast_bits |= 1 << p_index;
			} else {
				fast_bits &= ~(1 << p_index);
			}
		} else {
			int size_difference = (element_index - 1) - bits.size();

			if (size_difference < 0) {
				size_difference = 0;
			}

			for (int i = 0; i < size_difference; i++) {
				bits.push_back(0);
			}

			element_index -= 1;

			uint8_t bit_offset = p_index % 64;

			if (p_value) {
				bits[element_index] |= 1 << bit_offset;
			} else {
				bits[element_index] &= ~(1 << bit_offset);
			}
		}
	}

	_FORCE_INLINE_ bool get(uint32_t p_index) const {
		uint32_t element_index = p_index / 64;

		bool in_first = element_index == 0;

		if (in_first) {
			return fast_bits & (1 << p_index);
		}

		if (bits.size() < element_index) {
			return false;
		}

		return bits[element_index - 1] & (1 << (p_index % 64));
	}

	_FORCE_INLINE_ bool operator==(const BitSet &p_other) const {
		bool is_same = fast_bits == p_other.fast_bits;

		int a_size = bits.size();
		int b_size = bits.size();

		int min_size = MIN(a_size, b_size);
		int max_size = MAX(a_size, b_size);

		for (int i = 0; i < min_size; i++) {
			is_same = is_same && (bits[i] == p_other.bits[i]);
		}

		const Vector<uint64_t> *other = (a_size > b_size) ? &bits : &p_other.bits;

		for (int i = 0; i < max_size - min_size; i++) {
			is_same = is_same && ((*other)[i + min_size] == 0);
		}

		return is_same;
	}

	_FORCE_INLINE_ bool operator!=(const BitSet &p_other) const {
		return !(*this == p_other);
	}

	_FORCE_INLINE_ BitSet operator&(const BitSet &p_other) const {
		BitSet bitset;
		bitset.fast_bits = fast_bits & p_other.fast_bits;

		int a_size = bits.size();
		int b_size = bits.size();

		int min_size = MIN(a_size, b_size);
		int max_size = MAX(a_size, b_size);

		bitset.bits.resize(max_size);

		for (int i = 0; i < min_size; i++) {
			bitset.bits[i] = bits[i] & p_other.bits[i];
		}

		for (int i = 0; i < max_size - min_size; i++) {
			bitset.bits[i + min_size] = 0;
		}

		return bitset;
	}

	_FORCE_INLINE_ BitSet operator|(const BitSet &p_other) const {
		BitSet bitset;
		bitset.fast_bits = fast_bits | p_other.fast_bits;

		int a_size = bits.size();
		int b_size = bits.size();

		int min_size = MIN(a_size, b_size);
		int max_size = MAX(a_size, b_size);

		bitset.bits.resize(max_size);

		for (int i = 0; i < min_size; i++) {
			bitset.bits[i] = bits[i] | p_other.bits[i];
		}

		const Vector<uint64_t> *other = (a_size > b_size) ? &bits : &p_other.bits;

		for (int i = 0; i < max_size - min_size; i++) {
			bitset.bits[i + min_size] = (*other)[i + min_size];
		}

		return bitset;
	}

	_FORCE_INLINE_ BitSet operator^(const BitSet &p_other) {
		BitSet bitset;
		bitset.fast_bits = fast_bits ^ p_other.fast_bits;

		int a_size = bits.size();
		int b_size = bits.size();

		int min_size = MIN(a_size, b_size);
		int max_size = MAX(a_size, b_size);

		bitset.bits.resize(max_size);

		for (int i = 0; i < min_size; i++) {
			bitset.bits[i] = bits[i] ^ p_other.bits[i];
		}

		const Vector<uint64_t> *other = (a_size > b_size) ? &bits : &p_other.bits;

		for (int i = 0; i < max_size - min_size; i++) {
			bitset.bits[i + min_size] = 0 ^ (*other)[i + min_size];
		}

		return bitset;
	}

	_FORCE_INLINE_ void operator=(const BitSet &p_other) {
		fast_bits = p_other.fast_bits;
		bits = p_other.bits;
	}
};

#endif // BITSET_H
