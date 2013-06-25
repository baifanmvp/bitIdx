AUTOMAKE_OPTIONS = foreign

lib_LTLIBRARIES = libbitjson.la

libbitjson_la_SOURCES = cjson.c\
                        cJSON.c

libbitjson_la_CPPFLAGS =   -shared

includedir=${prefix}/include/${bitidx_version}/lib/

nobase_include_HEADERS = cJSON.h\
                         cjson.h