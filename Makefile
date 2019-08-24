USE_DEBUG = NO
USE_64BIT = NO

ifeq ($(USE_DEBUG),YES)
CFLAGS=-Wall -O -g
else
CFLAGS=-Wall -O3 -s
endif

ifeq ($(USE_64BIT),YES)
TOOLS=c:\tdm64\bin
else
TOOLS=c:\mingw\bin
endif

all: hex_dump.exe ascii.exe beer_cals.exe dms2dd.exe mortgage.exe prime32.exe printf2.exe ulocate.exe serial_enum.exe textfont.exe apptest.exe

clean:
	rm -f *.exe

ascii.exe: ascii.cpp
	$(TOOLS)\g++ $(CFLAGS) -Weffc++ $< -o $@

apptest.exe: apptest.cpp
	$(TOOLS)\g++ $(CFLAGS) -DUNICODE -D_UNICODE -Wno-write-strings -Weffc++ $< -o $@

hex_dump.exe: hex_dump.cpp
	$(TOOLS)\g++ $(CFLAGS) -Weffc++ $< -o $@

beer_cals.exe: beer_cals.cpp
	$(TOOLS)\g++ $(CFLAGS) -Weffc++ $< -o $@

dms2dd.exe: dms2dd.cpp
	$(TOOLS)\g++ $(CFLAGS) -Weffc++ $< -o $@

mortgage.exe: mortgage.cpp
	$(TOOLS)\g++ $(CFLAGS) -Weffc++ $< -o $@

prime32.exe: prime32.cpp
	$(TOOLS)\g++ $(CFLAGS) -Weffc++ $< -o $@

printf2.exe: printf2.c
	$(TOOLS)\gcc $(CFLAGS) -DTEST_PRINTF $< -o $@

ulocate.exe: ulocate.cpp
	$(TOOLS)\g++ $(CFLAGS) -Weffc++ $< -o $@

textfont.exe: textfont.cpp
	$(TOOLS)\g++ $(CFLAGS) -Weffc++ $< -o $@

serial_enum.exe: serial_enum.cpp
	$(TOOLS)\g++ $(CFLAGS) -DUNICODE -D_UNICODE $< -o $@ -lsetupapi

