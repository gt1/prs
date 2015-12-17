all: benchmark benchmark_q benchmark_q_64_p

benchmark: Makefile benchmark.c prs.ic prs.ih prs_uint64_t.c prs_uint64_t.h prs_uint.c prs_uint.h
	$(CC) -pedantic -ansi -W -Wall -O3 benchmark.c prs_uint64_t.c prs_uint.c -pthread -o $@

benchmark_q: Makefile benchmark_q.c prs.ic prs.ih prs_uint64_t.c prs_uint64_t.h prs_uint.c prs_uint.h
	$(CC) -pedantic -ansi -W -Wall -O3 benchmark_q.c prs_uint64_t.c prs_uint.c -pthread -o $@
benchmark_q_128: Makefile benchmark_q_128.c prs.ic prs.ih prs_uint128_t.c prs_uint128_t.h prs_uint64_t.c prs_uint64_t.h prs_uint.c prs_uint.h
	$(CC) -msse2 -pedantic -ansi -W -Wall -O3 benchmark_q_128.c prs_uint128_t.c prs_uint64_t.c prs_uint.c -pthread -o $@
benchmark_q_64_p: Makefile benchmark_q_64_p.c prs.ic prs.ih prs_uint64_t_pair.h prs_uint64_t_pair.c prs_uint64_t.c prs_uint64_t.h prs_uint.c prs_uint.h
	$(CC) -pedantic -ansi -W -Wall -O3 benchmark_q_64_p.c prs_uint64_t_pair.c prs_uint64_t.c prs_uint.c -pthread -o $@

clean:
	rm -f benchmark benchmark_q benchmark_q_128 benchmark_q_64_p
distclean: clean
	rm -f *~

