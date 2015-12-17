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
#include "prs_uint64_t.h"

#include <sys/time.h>
#include <float.h>
#include <limits.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "prs_endian.h"

int main()
{
	struct timeval bef_tv;
	struct timeval aft_tv;
	struct timeval sub_tv;

	unsigned int const nbits = 28;
	size_t const n = (((size_t)(1))<<nbits);
	unsigned int const rounds = (nbits + 7)/8;
	double T[33];

	typedef uint64_t value_type;

	value_type * A = (value_type *)malloc(n*sizeof(value_type));
	value_type * B = (value_type *)malloc(n*sizeof(value_type));
	unsigned int keybytes[sizeof(value_type)];
	size_t i;
	unsigned int p;
	value_type * R;

	#if defined(PARALLEL_INTERLEAVED_RADIX_SORT_LITTLE_ENDIAN)
	for ( i = 0; i < sizeof(value_type); ++i )
		keybytes[i] = i;
	#elif defined(PARALLEL_INTERLEAVED_RADIX_SORT_BIG_ENDIAN)
	for ( i = 0; i < sizeof(value_type); ++i )
		keybytes[i] = sizeof(value_type)-i-1;
	#else
	#error "Unknown byte order"
	#endif

	for ( i = 0; i < 33; ++i )
		T[i] = DBL_MAX;

	for ( p = 1; p <= 4; ++p )
	{
		unsigned int run;

		for ( run = 0; run < 10u; ++run )
		{
			size_t i;
			double t;
			for ( i = 0; i < n; ++i )
				A[i] = n-i-1;

			gettimeofday(&bef_tv,0);
			radixsort_uint64_t(&A[0],&B[0],n,p,rounds,&keybytes[0],1 /* interleave */);
			gettimeofday(&aft_tv,0);

			sub_tv.tv_sec = aft_tv.tv_sec - bef_tv.tv_sec;
			if ( aft_tv.tv_usec >= bef_tv.tv_usec )
			{
				sub_tv.tv_usec = aft_tv.tv_usec - bef_tv.tv_usec;
			}
			else
			{
				long aft_u = aft_tv.tv_usec;
				aft_u += 1000000l;
				aft_u -= (long)bef_tv.tv_usec;
				sub_tv.tv_usec = aft_u;
				sub_tv.tv_sec -= 1;
			}

			t = ( sub_tv.tv_sec + 1e-6 * sub_tv.tv_usec );
			T[p] = t < T[p] ? t : T[p];

			R = (rounds & 1) ? &B[0] : &A[0];
			for ( i = 1; i < n; ++i )
				assert ( R[i-1] < R[i] );
		}

		fprintf(stderr,"threads=%d num/sec=%f time=%f speedup=%f\n", (int)p, n / T[p], T[p], T[1]/T[p]);
	}


	#if 0
	for ( size_t i = 0; i < n; ++i )
		fprintf(stderr,"%d\n", (int)A[i]);
	#endif

	return 0;
}
