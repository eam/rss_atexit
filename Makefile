all:
	gcc -Wall -c -o rss_atexit_shim.o -fpic rss_atexit_shim.c
	gcc -ldl -shared -o rss_atexit_shim.so rss_atexit_shim.o

clean:
	rm -f rss_atexit_shim.so rss_atexit_shim.o /tmp/rss_log

test:
	LD_PRELOAD=$(CURDIR)/rss_atexit_shim.so /bin/true
	cat /tmp/rss_log
