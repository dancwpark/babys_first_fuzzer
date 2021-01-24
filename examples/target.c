#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  //printf("I'm doing some pretty cool things here\n");
  if (argc < 2) {
    printf("Wrong number of arguments\n");
    return 0;
  }
  //printf("Hello!\n");
  char *key = "helloworld";
  char *input = argv[1];
  printf("%s\n", input);
  printf("%ld\n", strlen(input));
  if (strlen(input) == 10) {
    printf("Found me\n");
    *(char *)1 = 2;
    return 1;
  }
  return 0;
}
