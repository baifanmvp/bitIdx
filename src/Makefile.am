AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libbitidx.la

libbitidx_la_SOURCES = bIdxBit.c\
                       bIdxFile.c\
                       bIdxArray.c\
                       bIdxHash.c\
                       bIdxBasicOp.c


libbitidx_la_LIBADD = $(glib_LIBS)
libbitidx_la_CPPFLAGS = $(AM_CPPFLAGS) $(glib_CFLAGS)  -D_GNU_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64  -shared

includedir=${prefix}/include/${bitidx_version}/

nobase_include_HEADERS = bIdxBit.h\
                         bIdxFile.h\
                         bIdxArray.h\
                         bIdxHash.h\
                         bIdxBasicOp.h
