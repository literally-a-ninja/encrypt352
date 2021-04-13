#ifndef H_COROUTINES

/**
 * Coordinator thread, responsible for managing lifecycle of all worker threads.
 **/
void *co_coordinator (void *a);

/**
 * The reader thread is responsible for reading from the input file one
 * character at a time, and placing the characters in the input buffer. It must
 * do so by calling the provided function read_input(). Each buffer item
 * corresponds to a character. Note that the reader thread may need to block
 * until other threads have consumed data from the input buffer. Specifically, a
 * character in the input buffer cannot be overwritten until the encryptor
 * thread and the input counter thread have processed the character. The reader
 * continues until the end of the input file is reached.
 **/
void *co_reader (void *a);

/**
 * The input counter thread simply counts occurrences of each letter in the
 * input file by looking at each character in the input buffer. It must call the
 * provided function count_input(). Of course, the input counter thread will
 * need to block if no characters are available in the input buffer.
 **/
void *co_rcount (void *a);

/**
 * The encryption thread consumes one character at a time from the input buffer,
 * encrypts it, and places it in the output buffer. It must do so by calling the
 * provided function caesar_encrypt(). Of course, the encryption thread may need
 * to wait for an item tobecome available in the input buffer, and for a slot to
 * become available in the output buffer. Note that a character in the output
 * buffer cannot be overwritten until the writer thread and the output counter
 * thread have processed the character. The encryption thread continues until
 * all characters of the input file have been encrypted.
 **/
void *co_rcipher (void *a);

/**
 * The writer thread is responsible for writing the encrypted characters in the
 * output buffer to the output file. It must do so by calling the provided
 * function write_output(). Note that the writer may need to block until an
 * encrypted character is available in the buffer. The writer continues until it
 * has written the last encrypted character.
 **/
void *co_writer (void *a);

/**
 * The output counter thread simply counts occurrences of each letter in the
 * output file by looking at each character in the output buffer. It must call
 * the provided function count_output(). Of course, the output counter thread
 * will need to block if no characters are available in the output buffer.
 **/
void *co_wcount (void *a);
#endif
