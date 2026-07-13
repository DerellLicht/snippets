USE_DEBUG = NO
USE_64BIT = NO
USE_LEGACY = YES

ifeq ($(USE_DEBUG),YES)
CFLAGS=-Wall -O -g
else
CFLAGS=-Wall -O3 -s
endif
# use -static for clang and cygwin/mingw
# CFLAGS += -static

#  clang++ vs tdm g++
#  Basically, clang gives *much* clearer compiler error messages...
#  Note: programs built with clang++ will require libc++.dll 
#  in order to be used elsewhere, unless compiled with -static
#  That is why the executable files are smaller than TDM ...

#  note that you don't need two separate toolchain installations to build for 32 and 64 bit; 
#  it's enough with one of them, and you can call e.g. 
#  x86_64-w64-mingw32-clang++ to build for 64 bit and 
#  i686-w64-mingw32-clang++ to build for 32 bit. 
#  The prefixless clang++ builds for the architecture that is the default 
#     for the toolchain you're using.

# cygwin mingw paths
# C:\cygwin64/bin/i686-w64-mingw32-g++.exe
# C:\cygwin64/bin/x86_64-w64-mingw32-g++.exe

ifeq ($(USE_64BIT),YES)
TOOLS=d:\tdm64\bin
#TOOLS=d:\llvm64\bin
#TOOLS=d:\tdm-gcc-64\bin
#  64-bit executables
#GPP_NAME=x86_64-w64-mingw32-g++ 
#GPP_NAME=g++
#GPP_NAME=clang++
else
ifeq ($(USE_LEGACY),YES)
TOOLS=d:\tdm32\bin
GPP_NAME=g++.exe
else
#TOOLS=d:\tdm32\bin
#TOOLS=d:\llvm\bin
TOOLS=C:\cygwin64\bin
#  32-bit executables
GPP_NAME=i686-w64-mingw32-g++.exe
endif
endif

#  standard build rule
#  Files which need variations on this, can be specified below
%.exe: %.cpp
	$(TOOLS)\$(GPP_NAME) $(CFLAGS) -Weffc++ $< -o $@

all: hex_dump.exe heron.exe ascii.exe beer_cals.exe dms2dd.exe mortgage.exe prime64.exe \
printf2.exe ulocate.exe serial_enum.exe textfont.exe apptest.exe \
cline.exe proc_time.exe read_files.exe ulocate.exe copy_icloud.exe

clean:
	rm -f *.exe

# specific build instructions are used for programs which require build toolchain
# other than the default d:\tdm32, or those who require custom command line
apptest.exe: apptest.cpp
	$(TOOLS)\$(GPP_NAME) $(CFLAGS) -DUNICODE -D_UNICODE -Wno-write-strings -Weffc++ $< -o $@

wcmdline.exe: wcmdline.cpp
	$(TOOLS)\$(GPP_NAME) $(CFLAGS) -DUNICODE -D_UNICODE -Weffc++ $< -o $@

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
	d:\tdm64\bin\\g++ -std=c++17 -O2 -o copy_icloud.exe copy_icloud.cpp

printf2.exe: printf2.c
	d:\tdm32\bin\gcc $(CFLAGS) -DTEST_PRINTF -Wno-int-to-pointer-cast $< -o $@

serial_enum.exe: serial_enum.cpp
	$(TOOLS)\$(GPP_NAME) $(CFLAGS) -Wno-unused-function -DUNICODE -D_UNICODE $< -o $@ -lsetupapi

