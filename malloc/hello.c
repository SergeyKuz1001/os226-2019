
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	fprintf(stderr,"%p\n", malloc(16));
	return 0;
}
