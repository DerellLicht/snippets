//****************************************************************************
//  Copyright (c) 2020  Daniel D Miller
//  
//  mRNA2protein - parse an mRNA nucleotide sequence, and 
//  output corresponding protein codes, in either 3 or 1 character format.
//                                                                 
//  Written by:   Daniel D. Miller  
//                                                                 
//  compile:  gcc -Wall -O2 -s mrna2protein.cpp -o mrna2protein.exe
//                                                                 
// > mrna2protein.exe AUGGUGCACCUGACUCCUGAGGAGAAGUCUGCC
// codons: [AUGGUGCACCUGACUCCUGAGGAGAAGUCUGCC]
// Met Val His Leu Thr Pro Glu Glu Lys Ser Ala
//****************************************************************************

#include <windows.h>
#include <stdio.h>

//lint -esym(818, argv)  Pointer parameter 'argv' could be declared as pointing to const
//lint -e10  Expecting '}'
typedef unsigned int          uint ;

static const uint MAX_CODONS = 300 ;
static const uint MAX_MRNA_LEN = (3 * MAX_CODONS) ;

static char input_codon_buf[MAX_MRNA_LEN + 1] = "" ;

static bool use_3byte_codon = true ;

//****************************************************************************
//  codon lookup structure

typedef struct {
   char codon[4] ;
   char protein3[4] ;
   char protein1 ;   
} codon_table_t ;

static const uint TABLE_ENTRIES = 64 ;
static const codon_table_t codon_table[TABLE_ENTRIES] = 
{
{ "UUU", "Phe", 'F' }, { "UCU", "Ser", 'S' }, { "UAU", "Tyr", 'Y' }, { "UGU", "Cys", 'C' },   
{ "UUC", "Phe", 'F' }, { "UCC", "Ser", 'S' }, { "UAC", "Tyr", 'Y' }, { "UGC", "Cys", 'C' },   
{ "UUA", "Leu", 'L' }, { "UCA", "Ser", 'S' }, { "UAA", "STP", '!' }, { "UGA", "STP", '!' },   
{ "UUG", "Leu", 'L' }, { "UCG", "Ser", 'S' }, { "UAG", "STP", '!' }, { "UGG", "Trp", 'W' },   

{ "CUU", "Leu", 'L' }, { "CCU", "Pro", 'P' }, { "CAU", "His", 'H' }, { "CGU", "Arg", 'R' },   
{ "CUC", "Leu", 'L' }, { "CCC", "Pro", 'P' }, { "CAC", "His", 'H' }, { "CGC", "Arg", 'R' },   
{ "CUA", "Leu", 'L' }, { "CCA", "Pro", 'P' }, { "CAA", "Gln", 'Q' }, { "CGA", "Arg", 'R' },   
{ "CUG", "Leu", 'L' }, { "CCG", "Pro", 'P' }, { "CAG", "Gln", 'Q' }, { "CGG", "Arg", 'R' },   

{ "AUU", "Ile", 'I' }, { "ACU", "Thr", 'T' }, { "AAU", "Asn", 'N' }, { "AGU", "Ser", 'S' },   
{ "AUC", "Ile", 'I' }, { "ACC", "Thr", 'T' }, { "AAC", "Asn", 'N' }, { "AGC", "Ser", 'S' },   
{ "AUA", "Ile", 'I' }, { "ACA", "Thr", 'T' }, { "AAA", "Lys", 'K' }, { "AGA", "Arg", 'R' },   
{ "AUG", "Met", 'M' }, { "ACG", "Thr", 'T' }, { "AAG", "Lys", 'K' }, { "AGG", "Arg", 'R' },   

{ "GUU", "Val", 'V' }, { "GCU", "Ala", 'A' }, { "GAU", "Asp", 'D' }, { "GGU", "Gly", 'G' },   
{ "GUC", "Val", 'V' }, { "GCC", "Ala", 'A' }, { "GAC", "Asp", 'D' }, { "GGC", "Gly", 'G' },   
{ "GUA", "Val", 'V' }, { "GCA", "Ala", 'A' }, { "GAA", "Glu", 'E' }, { "GGA", "Gly", 'G' },   
{ "GUG", "Val", 'V' }, { "GCG", "Ala", 'A' }, { "GAG", "Glu", 'E' }, { "GGG", "Gly", 'G' },   

//  missing proteins: asx/B, glx/Z
};

//****************************************************************************
static int parse_codon(char *hd)
{
   if (strlen(hd) < 3) {
      printf("ERROR: short codon [%s], aborting\n", hd);
      return -1;
   }
   int idx ;
   for (idx=0; idx<(int)TABLE_ENTRIES; idx++) {
      if (strncmp(hd, codon_table[idx].codon, 3) == 0) {
         return idx;
      }
   }
   printf("ERROR: invalid codon [%c%c%c], aborting\n", *(hd), *(hd+1), *(hd+2));
   return -1;
}

//****************************************************************************
static bool validate_codon_chars(char *codons)
{
   char *hd = codons;
   while (*hd != 0) {
      switch (*hd) {
      case 'A':
      case 'U':
      case 'G':
      case 'C':
         break;

      default:
         return false;
      }
      hd++ ;   //  point to next char
   }
   return true;
}

//****************************************************************************
void usage(void)
{
   puts("Usage: mrna2protein -options CODON_SEQUENCE");
   puts("Options:");
   puts("-3 means output 3-character protein code (default)");
   puts("-1 means output 1-character protein code");
   puts("");
   puts("Note regarding CODON_SEQUENCE :");
   puts("5' prefix and 3' suffix should *not* be included !!");
   puts("Only [A,U,G,C] characters are valid");
}

//****************************************************************************
int main(int argc, char **argv)
{
   int idx ;
   for (idx=1; idx<argc; idx++) {
      char *p = argv[idx] ;
      if (*p == '-') {
         p++ ;
         switch (*p) {
         case '1':
            use_3byte_codon = false ;
            break;

         case '3':
            use_3byte_codon = true ;
            break;

         default:
            usage() ;
            return 1;
         }
      }
      else {
         strncpy(input_codon_buf, p, MAX_MRNA_LEN) ;
         input_codon_buf[MAX_MRNA_LEN] = 0 ;
      }
   }

   //  verify input
   if (input_codon_buf[0] == 0) {
      usage() ;
      return 1;
   }
   printf("codons: [%s]\n", input_codon_buf);

   //  make sure only valid characters are included
   bool chars_valid = validate_codon_chars(input_codon_buf);
   if (!chars_valid) {
      puts("ERROR: invalid characters in codon sequence!!") ;      
      puts("Aborting...");
      return 1;
   }

   char *hd = &input_codon_buf[0] ;
   bool done = false ;
   while (!done) {
      int codon_idx = parse_codon(hd) ;
      if (codon_idx < 0) {
         return 1;
      }
      if (use_3byte_codon) {
         printf("%s ", codon_table[codon_idx].protein3) ;
      }
      else {
         printf("%c", codon_table[codon_idx].protein1) ;
      }
      hd += 3 ;
      //  check for normal end of codons
      if (*hd == 0) {
         done = true ;
      }
   }
   puts("");

   return 0;
}

