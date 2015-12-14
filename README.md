prs
===

prs contains code for parallel byte access based radix sorting. The files
prs.ic and prs.ih contain generic code which can be configured by defining
the preprocessor definitions PARALLEL_INTERLEAVED_RADIX_SORT_VALUE_TYPE
and PARALLEL_INTERLEAVED_RADIX_SORT_FUNCTIONNAME. The headers prs_uint.h and
prs_uint64_t.h contain two examples for doing so. The file prs_uint.h for
instance sets 

	#define PARALLEL_INTERLEAVED_RADIX_SORT_VALUE_TYPE unsigned int
	#define PARALLEL_INTERLEAVED_RADIX_SORT_FUNCTIONNAME radixsort_unsigned_int

This generates a function named radixsort_unsigned_int with the signature

	int radixsort_unsigned_int(
		unsigned int * ita,
		unsigned int * tita,
		size_t const n,
		unsigned int const tnumthreads,
		unsigned int const rounds,
		unsigned int const * keybytes,
		int const interleave
	);

The function implements radix sort using byte key access. The input sequence of length n is expected in [ita,ita+n).
The range [tita,tita+n) is used as temporary space. The sorting will use up to tnumthreads
threads (tnumthreads whenever n is at least tnumthreads). The routine performs rounds iterations
of bucket sorting where the k'th round extracts byte keybytes[k] from the objects to be sorted.
The sorted sequence is stored in [ita,ite) if rounds is even and [tita,tite) if rounds is odd.
The function will compute the histogram for the next round inside the current round when
interleave is set to one, otherwise the byte histogram for a round is computed in a separate scan.
As setting interleave to 1 uses one division operation per element this is often slower
than performing another linear scan, although it reduces the amount of memory accesses.
The function returns 0 on success and a non-zero value on failure (failed to
allocate memory or to create mutexes/conditions variables/threads).

The benchmark program measures the speed up obtained by using a number of
threads for sorting the sequence n-1,n-2,...,1,0 stored in 64 bit (type
uint64_t) numbers.

The benchmark_q program compares the speed of the radix sort with the
qsort function on a random sequence of unsigned int type numbers.
