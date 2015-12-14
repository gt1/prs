all: benchmark benchmark_q

benchmark: Makefile benchmark.c prs.ic prs.ih prs_uint64_t.c prs_uint64_t.h prs_uint.c prs_uint.h
	$(CC) -pedantic -ansi -W -Wall -O3 benchmark.c prs_uint64_t.c prs_uint.c -pthread -o $@

benchmark_q: Makefile benchmark_q.c prs.ic prs.ih prs_uint64_t.c prs_uint64_t.h prs_uint.c prs_uint.h
	$(CC) -pedantic -ansi -W -Wall -O3 benchmark_q.c prs_uint64_t.c prs_uint.c -pthread -o $@

clean:
	rm -f benchmark benchmark_q
distclean: clean
	rm -f *~

