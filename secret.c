#include <stdio.h>
#include <string.h>

void secret() {
    printf("You have exploited the buffer overflow!\n");
}

void vulnerable() {
    char buffer[64];
    printf("Input your data:\n");
    gets(buffer);
}

int main() {
    vulnerable();
    return 0;
}
