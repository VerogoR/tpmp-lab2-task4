# Makefile для сборки программы "Абитуриенты" (модель КИС)
# Исходники в src/, заголовки в include/

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pedantic -Iinclude
LDFLAGS =

SRCDIR = src
INCDIR = include
OBJDIR = bin
TARGET = applicants

SOURCES = $(SRCDIR)/main.c $(SRCDIR)/applicant_server.c
OBJECTS = $(OBJDIR)/main.o $(OBJDIR)/applicant_server.o

.PHONY: all clean run

all: $(OBJDIR) $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS)

$(OBJDIR)/main.o: $(SRCDIR)/main.c $(INCDIR)/applicant.h
	$(CC) $(CFLAGS) -c -o $@ $(SRCDIR)/main.c

$(OBJDIR)/applicant_server.o: $(SRCDIR)/applicant_server.c $(INCDIR)/applicant.h
	$(CC) $(CFLAGS) -c -o $@ $(SRCDIR)/applicant_server.c

run: $(TARGET)
	./$(TARGET) applicants.txt result.txt

clean:
	rm -rf $(OBJDIR) $(TARGET) result.txt applicants.txt
