#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	printf("arg number = %d\n", argc);
	char fblock[1024];
	int fin, fout, nread;
	if(argc < 3)
	{
		printf("argc number wrong!\n");
		exit(0);
	}
	printf("%s\n", argv[0]);
	printf("%s\n", argv[1]);
	printf("%s\n", argv[2]);
	fin = open(argv[1], O_RDONLY);
	if(fin < 0)
	{
		printf("file doesn't exist!");
		return 0;
	}
	fout = open(argv[2], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	while((nread = read(fin, fblock, sizeof(fblock))) > 0)
	{
		write(fout, fblock, nread);
	}
}
