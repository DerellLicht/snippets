USE_DEBUG = NO
USE_64BIT = NO

ifeq ($(USE_DEBUG),YES)
CFLAGS=-Wall -O -g
else
CFLAGS=-Wall -O3 -s
endif

#  this flag does *not* enable %llu in printf...
#CFLAGS += -std=c++98
#LFLAGS += -std=c++98

#  clang++ vs tdm g++
#  Basically, clang gives *much* clearer compiler error messages...
#  Note: programs built with clang++ will require libc++.dll 
#  in order to be used elsewhere.
#  That is why the executable files are smaller than TDM ...

ifeq ($(USE_64BIT),YES)
TOOLS=d:\tdm64\bin
#TOOLS=c:\tdm-gcc-64\bin
else
TOOLS=c:\tdm32\bin
#TOOLS=D:\clang\bin
endif

GPP_NAME=g++
#GPP_NAME=clang++

#  standard build rule
#  Files which need variations on this, can be specified below
%.exe: %.cpp
	$(TOOLS)\$(GPP_NAME) $(CFLAGS) -Weffc++ $< -o $@

all: hex_dump.exe heron.exe ascii.exe beer_cals.exe dms2dd.exe mortgage.exe prime64.exe \
printf2.exe ulocate.exe serial_enum.exe textfont.exe apptest.exe readall.exe \
cline.exe wcmdline.exe proc_time.exe

clean:
	rm -f *.exe

apptest.exe: apptest.cpp
	$(TOOLS)\$(GPP_NAME) $(CFLAGS) -DUNICODE -D_UNICODE -Wno-write-strings -Weffc++ $< -o $@

wcmdline.exe: wcmdline.cpp
	$(TOOLS)\$(GPP_NAME) $(CFLAGS) -DUNICODE -D_UNICODE -Weffc++ $< -o $@

prime64.exe: prime64.cpp
	d:\tdm64\bin\g++ $(CFLAGS) -Weffc++ $< -o $@

ulocate.exe: ulocate.cpp
	d:\tdm64\bin\g++ $(CFLAGS) -Weffc++ $< -o $@

printf2.exe: printf2.c
	$(TOOLS)\gcc $(CFLAGS) -DTEST_PRINTF -Wno-int-to-pointer-cast $< -o $@

serial_enum.exe: serial_enum.cpp
	$(TOOLS)\$(GPP_NAME) $(CFLAGS) -Wno-unused-function -DUNICODE -D_UNICODE $< -o $@ -lsetupapi

