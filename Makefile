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

#  standard build rule
#  Files which need variations on this, can be specified below
%.exe: %.cpp
	$(TOOLS)\g++ $(CFLAGS) -Weffc++ $< -o $@

all: hex_dump.exe ascii.exe beer_cals.exe dms2dd.exe mortgage.exe prime32.exe printf2.exe ulocate.exe serial_enum.exe textfont.exe apptest.exe readall.exe

clean:
	rm -f *.exe

apptest.exe: apptest.cpp
	$(TOOLS)\g++ $(CFLAGS) -DUNICODE -D_UNICODE -Wno-write-strings -Weffc++ $< -o $@

printf2.exe: printf2.c
	$(TOOLS)\gcc $(CFLAGS) -DTEST_PRINTF $< -o $@

serial_enum.exe: serial_enum.cpp
	$(TOOLS)\g++ $(CFLAGS) -DUNICODE -D_UNICODE $< -o $@ -lsetupapi

