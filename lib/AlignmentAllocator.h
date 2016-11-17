
#pragma once

#include <stdlib.h>
#include <malloc.h>


//multiplatform malloc/free versions
inline void* aligned_malloc(size_t size, size_t align) {
    void *result;
    #ifdef _MSC_VER
    result = _aligned_malloc(size, align);
    #else
     if(posix_memalign(&result, align, size)) result = 0;
    #endif
    return result;
}

inline void aligned_free(void *ptr) {
    #ifdef _MSC_VER
        _aligned_free(ptr);
    #else
      free(ptr);
    #endif

}



const size_t AVX2Alignment = 64;

template <typename T, size_t N = AVX2Alignment>
class AlignmentAllocator {
		//usage e.g., std::vector<CDiffusion3SolverSegment*, AlignmentAllocator<CDiffusion3SolverSegment*, AVX2Alignment>> mSegments;
public:
	typedef T value_type;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;

	typedef T * pointer;
	typedef const T * const_pointer;

	typedef T & reference;
	typedef const T & const_reference;

public:
	inline AlignmentAllocator() throw () { }

	template <typename T2>
	inline AlignmentAllocator(const AlignmentAllocator<T2, N> &) throw () { }

	inline ~AlignmentAllocator() throw () { }

	inline pointer adress(reference r) {
		return &r;
	}

	inline const_pointer adress(const_reference r) const {
		return &r;
	}

	inline pointer allocate(size_type n) {
		return (pointer)aligned_malloc(n*sizeof(value_type), N);
	}

	inline void deallocate(pointer p, size_type) {
		aligned_free(p);
	}

	inline void construct(pointer p, const value_type & wert) {
		new (p)value_type(wert);
	}

	inline void destroy(pointer p) {
		p->~value_type();
	}

	inline size_type max_size() const throw () {
		return size_type(-1) / sizeof(value_type);
	}

	template <typename T2>
	struct rebind {
		typedef AlignmentAllocator<T2, N> other;
	};

	bool operator!=(const AlignmentAllocator<T, N>& other) const  {
		return !(*this == other);
	}

	// Returns true if and only if storage allocated from *this
	// can be deallocated from other, and vice versa.
	// Always returns true for stateless allocators.
	bool operator==(const AlignmentAllocator<T, N>& other) const {
		return true;
	}
};

template <size_t alignment>
class alignas(alignment) CAligned {

		//alignas alings on the stack, overloaded new aligns on the heap
public:
	virtual ~CAligned() {};

	void* operator new(size_t i){
		return _aligned_malloc(i, alignment);
	}

	void operator delete (void *p) {
		_aligned_free(p);
	}
};
