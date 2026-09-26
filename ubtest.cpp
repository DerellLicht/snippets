// d:\llvm\bin\x86_64-w64-mingw32-clang++ -g -fsanitize=undefined -fno-sanitize-recover=undefined -static ubtest.cpp ubsan_hooks.cpp -o ubtest.exe

int main(int argc, char **argv)
{
    int x = 2147483647;
    x += argc;   // argc is 1 here, so this overflows at runtime
    return x < 0;
}
