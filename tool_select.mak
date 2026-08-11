#  This makefile depends upon the following macros, 
#  which should be defined in the outer makefile
# USE_64BIT = NO
# USE_UNICODE = NO
# USE_CLANG = NO
# sadly, cygwin mingw does not support gdiplus...
# USE_CYGWIN = NO

# use -static for clang and cygwin/mingw
#  clang vs tdm
#  clang gives *much* clearer compiler error messages...
#  However, programs built with clang++ will require libc++.dll and libunwind.dll
#  in order to be used elsewhere 
#  (unless built with -static, which significantly boosts file size)

#**********************************************************
#  64-bit build options
#**********************************************************
#  NOTE: TDM64 is deprecated, and especially should not be used for UNICODE projects.
#  _stprintf(), aka wsprintf(), are not working properly at all,
#  in TDM64 V10.3.0 with UNICODE enabled
#**********************************************************
ifeq ($(USE_64BIT),YES)
ifeq ($(USE_CLANG),YES)
#TOOLS=d:\llvm\bin
TOOLS=d:/llvm/bin
GNAME=x86_64-w64-mingw32-clang++
WRNAME:=x86_64-w64-mingw32-windres.exe
USE_STATIC = YES
else
ifeq ($(USE_CYGWIN),YES)
TOOLS:=C:/cygwin64/bin
GNAME:=x86_64-w64-mingw32-g++
WRNAME:=x86_64-w64-mingw32-windres.exe
USE_STATIC = YES
else
$(error "echo Either CLANG or CYGWIN must be specified for 64-bit build")
endif	# if USE_CYGWIN
endif # if USE_CLANG

#**********************************************************
#  32-bit build options
#**********************************************************
else	# if NOT USE_64BIT

ifeq ($(USE_CLANG),YES)
TOOLS=d:/llvm/bin
GNAME=i686-w64-mingw32-clang++
WRNAME:=i686-w64-mingw32-windres.exe
USE_STATIC = YES
else
ifeq ($(USE_CYGWIN),YES)
TOOLS:=C:/cygwin64/bin
GNAME:=i686-w64-mingw32-g++
WRNAME:=i686-w64-mingw32-windres.exe
USE_STATIC = YES

else
TOOLS=d:\tdm32\bin
GNAME=g++
WRNAME:=windres.exe
USE_STATIC = NO
endif	# if USE_CYGWIN
endif # if USE_CLANG
endif	# if !USE_64BIT

