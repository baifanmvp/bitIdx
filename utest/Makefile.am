AUTOMAKE_OPTIONS = foreign
bin_PROGRAMS = bidxtest optest

bidxtest_SOURCES = bIdxTest.c
bidxtest_LDADD = -lbitidx -lbitjson
bidxtest_CPPFLAGS = /usr/local/include/bitidx/ `pkg-config glib-2.0 --cflags --libs` -I/usr/local/include/eql/

optest_SOURCES = bIdxOpTest.c
optest_LDADD = -lbitidx -lbitjson
optest_CPPFLAGS = /usr/local/include/bitidx/ `pkg-config glib-2.0 --cflags --libs`
