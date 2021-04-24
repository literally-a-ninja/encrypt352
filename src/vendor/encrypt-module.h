#ifndef H_ENCRYPT
#define H_ENCRYPT

#include <stdio.h>
/**
 * You must use these functions to perform all I/O, encryption and counting
 * operations.
 **/
void init (char *inputFileName, char *outputFileName);
int read_input ();
void write_output (int c);
int caesar_encrypt (int c);
void count_input (int c);
void count_output (int c);
int get_input_count (int c);
int get_output_count (int c);
int get_input_total_count ();
int get_output_total_count ();
void clear_counts ();

#endif // H_ENCRYPT
