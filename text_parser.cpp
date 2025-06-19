#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef unsigned int  uint ;

#define  HTAB     9

//**********************************************************************
void strip_newlines(char *rstr)
{
   int slen = (int) strlen(rstr) ;
   while (1) {
      if (slen == 0)
         break;
      if (*(rstr+slen-1) == '\n'  ||  *(rstr+slen-1) == '\r') {
         slen-- ;
         *(rstr+slen) = 0 ;
      } else {
         break;
      }
   }
}

//**********************************************************************
char *next_field(char *q)
{
   while (*q != ' '  &&  *q != HTAB  &&  *q != 0)
      q++ ; //  walk past non-spaces
   while (*q == ' '  ||  *q == HTAB)
      q++ ; //  walk past all spaces
   return q;
}

//**********************************************************************
int main(int argc, char **argv)
{
    char fname[MAX_PATH_LEN+1] = "" ;

    int idx ;
    for (idx=1; idx<argc; idx++)
    {
        char *p = argv[idx];
        strncpy(fname, p, MAX_PATH_LEN);
        fname[MAX_PATH_LEN] = 0 ;   //  ensure NULL-term
    }
    if (fname[0] == 0)
    {
        puts("Usage: text_parser data_filename");
        return 1;
    }
    
    FILE *fd = fopen(fname, "rt");
    if (fd == NULL)
    {
        printf("%s: %s\n", fname, strerror(errno));
        return 1;
    }
#define  MAX_READ_LEN   128
    char inpstr[MAX_READ_LEN+1];
    uint lcount = 0 ;
    while (fgets(inpstr, MAX_READ_LEN, fd) != NULL)
    {
        strip_newlines(inpstr);
        if (strlen(inpstr) == 0)
        {
            continue;
        }
        //  a text string is ready now...
        lcount++ ;

    }
    fclose(fd);
    printf("%s: %u lines read\n", fname, lcount);
    return 0;
}   //lint !e818

