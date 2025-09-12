#include <stdlib.h>

int ourAlloc(void *start, int howMuch) {

}
void ourFree(void *start, int offset) {

}

int main(void) {
	// int *ptr = malloc(100 * sizeof(int));
	// free(ptr);
	void *ptr = malloc(1e9);
	int offset = ourAlloc(ptr, 100);
	*(ptr + offset) = ;
}
