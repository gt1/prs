/*

Copyright (c) 2015, German Tischler (tischler@mpi-cbg.de)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. The name of German Tischler may not be used to endorse or promote products
   derived from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include "prs_uint64_t_pair.h"

#include <sys/time.h>
#include <float.h>
#include <limits.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "prs_endian.h"

static void subtim(struct timeval * aft_tv, struct timeval * bef_tv, struct timeval * sub_tv)
{
	sub_tv->tv_sec = aft_tv->tv_sec - bef_tv->tv_sec;
	if ( aft_tv->tv_usec >= bef_tv->tv_usec )
	{
		sub_tv->tv_usec = aft_tv->tv_usec - bef_tv->tv_usec;
	}
	else
	{
		long aft_u = aft_tv->tv_usec;
		aft_u += 1000000l;
		aft_u -= (long)bef_tv->tv_usec;
		sub_tv->tv_usec = aft_u;
		sub_tv->tv_sec -= 1;
	}
}

int compare_uint128_t(void const * va, void const * vb)
{
	uint64_t * pa = (uint64_t*)va;
	uint64_t * pb = (uint64_t*)vb;
	unsigned int i;

	#if defined(PARALLEL_INTERLEAVED_RADIX_SORT_LITTLE_ENDIAN)
	pa += 2;
	pb += 2;

	for ( i = 0; i < 2; ++i )
	{
		uint64_t const a = *(--pa);
		uint64_t const b = *(--pb);

		if ( a < b )
			return -1;
		else if ( b < a )
			return 1;
	}
	#elif defined(PARALLEL_INTERLEAVED_RADIX_SORT_BIG_ENDIAN)
	for ( i = 0; i < 2; ++i )
	{
		uint64_t const a = *(--pa);
		uint64_t const b = *(--pb);

		if ( a < b )
			return -1;
		else if ( b < a )
			return 1;
	}
	#endif

	return 0;
}

int main()
{
	struct timeval bef_tv;
	struct timeval aft_tv;
	struct timeval sub_tv;

	unsigned int const nbits = 27;
	size_t const n = (((size_t)(1))<<nbits);
	int interleave = 0;

	typedef uint64_t_pair value_type;
	/* unsigned int const rounds = sizeof(value_type); */
	unsigned int const rounds = 8;

	value_type * A = 0;
	value_type * B = 0;
	value_type * C = 0;
	unsigned int keybytes[sizeof(value_type)];
	size_t i;
	unsigned int p;
	double T[33];
	double qsorttime = -1;

	A = (value_type *)malloc(n*sizeof(value_type));
	B = (value_type *)malloc(n*sizeof(value_type));
	C = (value_type *)malloc(n*sizeof(value_type));

	if ( ! A || ! B || ! C )
	{
		free(A);
		free(B);
		free(C);
		return EXIT_FAILURE;
	}

	srand(time(0));

	for ( i = 0; i < n; ++i )
	{
		uint64_t v[2];
		unsigned int j, k;

		for ( j = 0; j < 2; ++j )
		{
			v[j] = 0;
			for ( k = 0; k < sizeof(uint64_t); ++k )
			{
				v[j] <<= 8;
				v[j] |= (uint64_t)(rand()&0xff);
			}
		}
	}

	/* copy C to A */
	memcpy(A,C,n*sizeof(value_type));

	gettimeofday(&bef_tv,0);
	/* sort A */
	qsort(A,n,sizeof(value_type),compare_uint128_t);
	gettimeofday(&aft_tv,0);
	subtim(&aft_tv, &bef_tv, &sub_tv);
	qsorttime = ( sub_tv.tv_sec + 1e-6 * sub_tv.tv_usec );

	fprintf(stderr,"qsort num/sec=%f time=%f\n", n / qsorttime, qsorttime);

	#if defined(PARALLEL_INTERLEAVED_RADIX_SORT_LITTLE_ENDIAN)
	for ( i = 0; i < sizeof(value_type); ++i )
		keybytes[i] = i;
	#elif defined(PARALLEL_INTERLEAVED_RADIX_SORT_BIG_ENDIAN)
	for ( i = 0; i < sizeof(value_type); ++i )
		keybytes[i] = sizeof(value_type)-i-1;
	#else
	#error "Unknown byte order"
	#endif

	for ( interleave = 0; interleave < 2; ++interleave )
	{
		for ( i = 0; i < 33; ++i )
			T[i] = DBL_MAX;

		for ( p = 1; p <= 4; ++p )
		{
			unsigned int run;

			for ( run = 0; run < 10u; ++run )
			{
				/* #define RADIXSORT_CHECK */

				double t;
				#if defined(RADIXSORT_CHECK)
				value_type * R;
				#endif

				memcpy(A,C,n*sizeof(value_type));

				gettimeofday(&bef_tv,0);
				radixsort_uint64_t_pair(A,B,n,p,rounds,&keybytes[0],interleave);
				gettimeofday(&aft_tv,0);

				subtim(&aft_tv, &bef_tv, &sub_tv);

				t = ( sub_tv.tv_sec + 1e-6 * sub_tv.tv_usec );
				T[p] = t < T[p] ? t : T[p];

				#if defined(RADIXSORT_CHECK)
				fprintf(stderr,"checking\n");
				R = (rounds & 1) ? &B[0] : &A[0];
				for ( i = 1; i < n; ++i )
					assert ( compare_uint128_t(R+(i-1),R+i) <= 0 );

				memcpy(B,C,n*sizeof(value_type));
				qsort(B,n,sizeof(value_type),compare_uint128_t);
				for ( i = 0; i < n; ++i )
					assert ( compare_uint128_t(R+i,B+i) == 0 );
				#endif

				fprintf(stderr,"threads=%d interleave=%d num/sec=%f time=%f speedup/q=%f speedup/1=%f\n", (int)p, interleave, n / T[p], T[p], qsorttime/T[p], T[1]/T[p]);
			}
		}
	}

	free(A);
	free(B);
	free(C);

	return 0;
}
