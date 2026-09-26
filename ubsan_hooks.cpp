// ubsan_hooks.cpp : route UBSan runtime output to the debugger (DbgView)
// Compile into the UBSan build only (add to CPPSRC in ndir_ubsan.mak).
// Tags: "P:" = came through __sanitizer_on_print, "S:" = came through the summary hook

#include <windows.h>
#include <strsafe.h>

// Called by the sanitizer runtime with each chunk of text it prints.
// Chunks are not necessarily whole lines.
// Side effect: sends a tagged copy to OutputDebugStringA(); the runtime
// still writes the original text to stderr.
// Over-long chunks are truncated to fit the local buffer.
extern "C" void __sanitizer_on_print(const char *str)
{
   char buf[2048];
   StringCbPrintfA(buf, sizeof(buf), "P: %s", str);
   OutputDebugStringA(buf);
}

// Called with the one-line error summary just before the abort.
// Side effect: sends a tagged copy to OutputDebugStringA().
extern "C" void __sanitizer_report_error_summary(const char *summary)
{
   char buf[2048];
   StringCbPrintfA(buf, sizeof(buf), "S: %s\n", summary);
   OutputDebugStringA(buf);
}
