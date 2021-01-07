#include <stdio.h>
#include <execinfo.h>
#include <stdlib.h>

#define MAX_STACK_SIZE 16

void level1();
void level2();
void level3();

void do_backtrace() {
	// int backtrace(void **buffer, int size);
	void *buffer[MAX_STACK_SIZE] = {0};
	int nrets = backtrace(buffer, MAX_STACK_SIZE);
	
	// char **backtrace_symbols(void *const *buffer, int size);
	char **symbol_string = backtrace_symbols(buffer, nrets);
	for (int i = 0; i < nrets; ++i) {
		printf("%s\n", symbol_string[i]);
	}
	printf("\n");

	free(symbol_string);
}

void level1() {
	printf("level1\n");
	level2();
}

void level2() {
	printf("level2\n");
	level3();
}

void level3() {
	printf("level3\n");
	do_backtrace();
}

int main(void) {
	level1();

	return 0;
}
