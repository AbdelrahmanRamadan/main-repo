#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <ctime>
using namespace std;
int to_int(char* str) {
	int ret = 0;
	for(int i = 0; str[i]; ++i)
		ret = ret * 10 + str[i] - '0';
	return ret;
}
int main(int argc, char* argv[]) {
	freopen("input.txt", "w", stdout);
	
	srand(time(NULL));
	int pop_size = 51, ch_size = 64;
	if(argc > 1)
		pop_size = to_int(argv[1]);
	if(argc > 2)
		ch_size = to_int(argv[2]);
	printf("%d %d\n", pop_size, ch_size);
	for(int i = 0; i < pop_size; ++i) {
		for(int j = 0; j < ch_size; ++j)
			putchar((rand() & 1) + '0');
		putchar('\n');
	}
	
	return 0;
}
