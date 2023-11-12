//************************************************************************************************
//  How to generate argc/argv data for programs which do not have conventional main() function.
//  These include Windows (WinMain) and embedded projects
//************************************************************************************************

#include <windows.h>
#include <stdio.h>
#include <string.h>

#define  CLINE_PARSE_FILES    0
#define  CLINE_PARSE_PATHS    1
unsigned cline_parse_mode = CLINE_PARSE_FILES ;

#define  MAX_CLINE_ELEMENTS   21
static char *argv[MAX_CLINE_ELEMENTS] ; 
static int argc ;

void form_Cmain_args(void)
{
   static char cmd_line_copy[260] ;
   char *hdptr, *tlptr ;
   int state, done ;

   strcpy(cmd_line_copy, GetCommandLine()) ;
   argc = 0 ;

   hdptr = cmd_line_copy ;
   tlptr = hdptr+1 ;
   state = (*hdptr == '"') ? 1 : 0 ;
   done = 0 ;
   while (!done) {
      switch (state) {
      case 0:  //  normal
         if (*tlptr == 0) {
            argv[argc++] = hdptr ;
            done = 1 ;
            break;
         } 
         else if (*tlptr == ' ') {
            if (cline_parse_mode == CLINE_PARSE_FILES) {
               *tlptr++ = 0 ; //  NULL-term the string
               if (*hdptr != 0)
                  argv[argc++] = hdptr ;
               hdptr = tlptr++ ;
               if (*hdptr == 0)
                  done = 1 ;
               if (*hdptr == '"') {
                  state = 1 ;
               }
            } 
            //  if parsing paths, try to treat spaces as if they
            //  might be parts of filenames.  This doesn't permit
            //  passing multiple filenames though.
            else {
               
            }
         } else {
            tlptr++ ;
         }
         break;

      case 1:  //  eating quoted text
         if (*tlptr == 0) {
            argv[argc++] = hdptr ;
            done = 1 ;
         } else if (*tlptr == '"') {
            tlptr++ ;
            //  if we're add end of command line, set DONE flag
            if (*tlptr == 0) {
               argv[argc++] = hdptr ;
               done = 1 ;
            } else if (*tlptr == ' ') {
               *tlptr++ = 0 ; //  NULL-term the string
               if (*hdptr != 0) {
                  argv[argc++] = hdptr ;
               }
               hdptr = tlptr++ ;
               state = 0 ;
            }
         } else {
            tlptr++ ;
         }
         
         break;
      }  //  switch state
   }  //  while not done
   argv[argc] = 0 ;  //  make last array element NULL 
}

//*****************************************************************
int main(void)
{
   int j ;
   form_Cmain_args() ;
   printf("argv[0] = %s\n", argv[0]) ;
   printf("argc=%d\n", argc) ;
   for (j=1; j<argc; j++) {
      printf("%d: %s\n", j, argv[j]) ;
   }
   return 0;
}

