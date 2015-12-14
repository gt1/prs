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
#if defined(__APPLE__)

#if defined(__BYTE_ORDER__) 

#if BYTE_ORDER == __ORDER_LITTLE_ENDIAN__
#define PARALLEL_INTERLEAVED_RADIX_SORT_LITTLE_ENDIAN
#elif BYTE_ORDER == __ORDER_BIG_ENDIAN__
#else
#error "Unsupported system (unable to determine endianess)"
#endif

#else

#error "Unsupported system (unable to determine endianess)"
   
#endif

#elif defined(__linux)
#include <endian.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define PARALLEL_INTERLEAVED_RADIX_SORT_LITTLE_ENDIAN
#elif __BYTE_ORDER == __BIG_ENDIAN
#define PARALLEL_INTERLEAVED_RADIX_SORT_BIG_ENDIAN
#else
#error "Unsupported byte order (not little or big endian)"
#endif

#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <sys/endian.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define PARALLEL_INTERLEAVED_RADIX_SORT_LITTLE_ENDIAN
#elif __BYTE_ORDER == __BIG_ENDIAN
#define PARALLEL_INTERLEAVED_RADIX_SORT_BIG_ENDIAN
#else
#error "Unsupported byte order (not little or big endian)"
#endif

#else

#error "Unsupported system (unable to determine endianess)"
#endif
