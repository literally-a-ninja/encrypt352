#include "vendor/encrypt-module.c"
#include "vendor/encrypt-module.h"

void reset_finished() {}

void reset_requested() {}

int main() {
    init("in.txt", "out.txt");
    char c;

    while ((c = read_input()) != EOF) {
        count_input(c);
        c = caesar_encrypt(c);
        count_output(c);
        write_output(c);
    }

    printf("End of file reached.\n");
    return 0;
}
