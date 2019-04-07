#include <stdio.h>

int main(int argc, char *argv[])
{
  printf("Je suis le client\n");
  for(int i=0; i<=argc; i++)
    printf("%s ", argv[i]);
  return 0;
}
