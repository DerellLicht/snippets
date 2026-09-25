USE_DEBUG = NO
USE_64BIT = NO
USE_CLANG = YES
USE_LEGACY = NO

include .\tool_select.mak

ifeq ($(USE_DEBUG),YES)
CFLAGS=-Wall -O -g
else
CFLAGS=-Wall -O3 -s 
endif
# use -static for clang and cygwin/mingw
# CFLAGS += -static

all: hex_dump.exe heron.exe ascii.exe beer_cals.exe dms2dd.exe mortgage.exe prime64.exe \
printf2.exe ulocate.exe serial_enum.exe textfont.exe apptest.exe \
cline.exe proc_time.exe read_files.exe ulocate.exe llu_check.exe

clean:
	rm -f *.exe

#  standard build rule
#  Files which need variations on this, can be specified below
%.exe: %.cpp
	$(TOOLS)\$(GNAME) $(CFLAGS) -Weffc++ $< -o $@

# specific build instructions are used for programs which require build toolchain
# other than the default d:\tdm32, or those who require custom command line
apptest.exe: apptest.cpp
	$(TOOLS)\$(GNAME) $(CFLAGS) -DUNICODE -D_UNICODE -Wno-write-strings -Weffc++ $< -o $@

wcmdline.exe: wcmdline.cpp
	$(TOOLS)\$(GNAME) $(CFLAGS) -DUNICODE -D_UNICODE -Weffc++ $< -o $@

prime64.exe: prime64.cpp
#	C:/cygwin64/bin/x86_64-w64-mingw32-g++ $(CFLAGS) -static -Weffc++ $< -o $@
	D:\llvm/bin/x86_64-w64-mingw32-clang++.exe $(CFLAGS) -static -Weffc++ $< -o $@
   
#  I use cygwin vs tdm here, because I want %llu to work
ulocate.exe: ulocate.cpp
	C:\cygwin64/bin/x86_64-w64-mingw32-g++ -Wno-stringop-truncation $(CFLAGS) -static -Weffc++ $< -o $@
#	D:\llvm/bin/x86_64-w64-mingw32-clang++.exe $(CFLAGS) -static -Weffc++ $< -o $@

llu_check.exe: llu_check.cpp
#	C:\cygwin64/bin/i686-w64-mingw32-g++ -Wno-stringop-truncation $(CFLAGS) -static -Weffc++ $< -o $@
	D:\llvm/bin/i686-w64-mingw32-clang++.exe $(CFLAGS) -static -Weffc++ $< -o $@

printf2.exe: printf2.c
	d:\tdm32\bin\gcc $(CFLAGS) -DTEST_PRINTF -Wno-int-to-pointer-cast $< -o $@

serial_enum.exe: serial_enum.cpp
	$(TOOLS)\$(GNAME) $(CFLAGS) -Wno-unused-function -DUNICODE -D_UNICODE $< -o $@ -lsetupapi
