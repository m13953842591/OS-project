#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char const *argv[])
{
    char a[20];
    scanf("%s", a);
    printf("first input %s\n", a);
    scanf("%s", a);
    printf("second input %s\n", a);
    return 0;
}
