# Makefile for r36s-pwrkey-handler

CC      ?= clang
CFLAGS  ?= -Os -Wall -Wextra
LDFLAGS ?= -s

PREFIX   := /usr
BINDIR   := $(DESTDIR)$(PREFIX)/bin
DINITDIR := $(DESTDIR)$(PREFIX)/lib/dinit.d

TARGET := r36s-pwrkey-handler
SRC    := r36s-pwrkey-handler.c

all: $(TARGET)
 
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<
 
install: $(TARGET)
	install -Dm755 $(TARGET) $(BINDIR)/$(TARGET)
	install -Dm644 dinit.d/$(TARGET) $(DINITDIR)/$(TARGET)
 
clean:
	rm -f $(TARGET)
 
.PHONY: all install clean
