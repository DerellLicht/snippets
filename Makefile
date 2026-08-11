USE_DEBUG = NO
USE_64BIT = NO
USE_LEGACY = YES

include ..\tool_select.mak 

ifeq ($(USE_DEBUG),YES)
CFLAGS=-Wall -O -g
else
CFLAGS=-Wall -O3 -s
endif
# use -static for clang and cygwin/mingw
# CFLAGS += -static

#  standard build rule
#  Files which need variations on this, can be specified below
%.exe: %.cpp
	$(TOOLS)\$(GNAME) $(CFLAGS) -Weffc++ $< -o $@

all: hex_dump.exe heron.exe ascii.exe beer_cals.exe dms2dd.exe mortgage.exe prime64.exe \
printf2.exe ulocate.exe serial_enum.exe textfont.exe apptest.exe \
cline.exe proc_time.exe read_files.exe ulocate.exe copy_icloud.exe

clean:
	rm -f *.exe

# specific build instructions are used for programs which require build toolchain
# other than the default d:\tdm32, or those who require custom command line
apptest.exe: apptest.cpp
	$(TOOLS)\$(GNAME) $(CFLAGS) -DUNICODE -D_UNICODE -Wno-write-strings -Weffc++ $< -o $@

wcmdline.exe: wcmdline.cpp
	$(TOOLS)\$(GNAME) $(CFLAGS) -DUNICODE -D_UNICODE -Weffc++ $< -o $@

prime64.exe: prime64.cpp
#	d:\tdm64\bin\g++ $(CFLAGS) -Weffc++ $< -o $@
#	C:/cygwin64/bin/x86_64-w64-mingw32-g++ $(CFLAGS) -static -Weffc++ $< -o $@
	D:\llvm/bin/x86_64-w64-mingw32-clang++.exe $(CFLAGS) -static -Weffc++ $< -o $@
   
#  I use cygwin vs tdm here, because I want %llu to work
ulocate.exe: ulocate.cpp
#	d:\tdm64\bin\g++ -Wno-stringop-truncation $(CFLAGS) -Weffc++ $< -o $@
	C:\cygwin64/bin/x86_64-w64-mingw32-g++ -Wno-stringop-truncation $(CFLAGS) -static -Weffc++ $< -o $@
#	D:\llvm/bin/x86_64-w64-mingw32-clang++.exe $(CFLAGS) -static -Weffc++ $< -o $@

copy_icloud.exe: copy_icloud.cpp
	D:\llvm/bin/x86_64-w64-mingw32-clang++.exe -std=c++17 -O2 -o copy_icloud.exe copy_icloud.cpp

printf2.exe: printf2.c
	d:\tdm32\bin\gcc $(CFLAGS) -DTEST_PRINTF -Wno-int-to-pointer-cast $< -o $@

serial_enum.exe: serial_enum.cpp
	$(TOOLS)\$(GNAME) $(CFLAGS) -Wno-unused-function -DUNICODE -D_UNICODE $< -o $@ -lsetupapi

