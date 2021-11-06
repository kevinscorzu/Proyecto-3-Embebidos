lib_LTLIBRARIES = libparking.la
include_HEADERS = $(top_srcdir)/include/gpio.h $(top_srcdir)/include/server.h $(top_srcdir)/include/uart.h
AM_CPPFLAGS = -I$(top_srcdir)/include
libparking_la_SOURCES = gpio.c server.c uart.c
AM_LDFLAGS = -lpthread -lulfius -ljansson