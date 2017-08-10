USE_DEBUG = NO

ifeq ($(USE_DEBUG),YES)
CFLAGS=-Wall -O -g
else
CFLAGS=-Wall -O3 -s
endif

all: ascii.exe beer_cals.exe dms2dd.exe mortgage.exe prime32.exe printf2.exe ulocate.exe serial_enum.exe

clean:
	rm -f *.exe

ascii.exe: ascii.cpp
	g++ $(CFLAGS) -Weffc++ $< -o $@

beer_cals.exe: beer_cals.cpp
	g++ $(CFLAGS) -Weffc++ $< -o $@

dms2dd.exe: dms2dd.cpp
	g++ $(CFLAGS) -Weffc++ $< -o $@

mortgage.exe: mortgage.cpp
	g++ $(CFLAGS) -Weffc++ $< -o $@

prime32.exe: prime32.cpp
	g++ $(CFLAGS) -Weffc++ $< -o $@

printf2.exe: printf2.c
	gcc $(CFLAGS) -DTEST_PRINTF $< -o $@

ulocate.exe: ulocate.cpp
	g++ $(CFLAGS) -Weffc++ $< -o $@

serial_enum.exe: serial_enum.cpp
	g++ $(CFLAGS) -DUNICODE -D_UNICODE $< -o $@ -lsetupapi

