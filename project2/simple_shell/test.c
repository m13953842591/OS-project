#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 
#include <fcntl.h> 
#include <sys/types.h>
#include <sys/stat.h> 
#include <string.h>
#include <strings.h> 
  

char * strip(char * str){
    int left = 0, right = strlen(str) - 1, len;
    while(left <= right && str[left] == ' ') left++;
    while(left <= right && str[right] == ' ') right--;
    if(left > right) return NULL;
    len = right - left + 1;
    char * newstr = (char * )malloc((len + 1) * sizeof(char));
    strncpy(newstr, str + left, len);
    newstr[len] = '\0';
    return newstr;
}

int main() { 
    printf((1 > 0)? "hello, world" : "what the hell");
    return 0;
}
