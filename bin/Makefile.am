bin_PROGRAMS=bIdxd
bIdxd_SOURCES=bIdxServer.c
bIdxd_LDADD=-lbitidx $(glib_LIBS) -lbitjson
bIdxd_CPPFLAGS=-I//usr/local/include/bitidx/ -L/usr/local/lib/ -D_GNU_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 $(glib_CFLAGS) -I/usr/local/include/eql/