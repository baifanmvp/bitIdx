SUBDIRS = lib  

AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libbitidx.la

libbitidx_la_SOURCES = bIdxBit.c\
                       bIdxLock.c\
                       bIdxThrPool.c\
                       bIdxFile.c\
                       bIdxArray.c\
                       bIdxHash.c\
                       bIdxBasicOp.c\
                       bIdxer.c


libbitidx_la_LIBADD = $(glib_LIBS) -leql -lbitjson
libbitidx_la_CPPFLAGS = $(AM_CPPFLAGS) $(glib_CFLAGS)  -D_GNU_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64  -shared -L/usr/local/lib -I/usr/local/include/ -I/usr/local/include/eql/

includedir=${prefix}/include/${bitidx_version}/

nobase_include_HEADERS = bIdxBit.h\
                         bIdxDef.h\
                         bIdxLock.h\
                         bIdxThrPool.h\
                         bIdxFile.h\
                         bIdxArray.h\
                         bIdxHash.h\
                         bIdxBasicOp.h\
                         bIdxer.h
