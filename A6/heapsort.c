#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "person.h"

int pagenum, recordnum;
char **tmparray;

void readPage(FILE *fp, char *pagebuf, int pagenum)
{
	fseek(fp, pagenum*PAGE_SIZE, SEEK_SET);
	fread(pagebuf, PAGE_SIZE, 1, fp);
}

void writePage(FILE *fp, const char *pagebuf, int pagenum)
{
	fseek(fp, pagenum*PAGE_SIZE, SEEK_SET);
	fwrite(pagebuf, PAGE_SIZE, 1, fp);
}

void buildHeap(FILE *inputfp, char **heaparray)
{
	int i;
	int cur_page = 1, cur_record = 1;
	int heapnum, cur, prt;
	char tmp[RECORD_SIZE];
	char header_page[PAGE_SIZE], page[PAGE_SIZE];

	while(cur_page < pagenum)
	{
		readPage(inputfp, page, cur_page);

		for(i = 0; i < PAGE_SIZE / RECORD_SIZE; i++){

			if(cur_record > recordnum)
				break;

			strncpy(tmparray[cur_record++], page+(i*RECORD_SIZE), sizeof(Person));
		}
		cur_page++;
	}

	heapnum = cur_record-1;
	cur = 1;

	while(heapnum > 0){
		strcpy(heaparray[cur], tmparray[heapnum]);

		if(cur == 1){
			cur++;
			heapnum--;
			continue;
		}

		prt = cur / 2;

		if(atoll(heaparray[cur]) < atoll(heaparray[prt])){
			strcpy(tmp, heaparray[cur]);
			strcpy(heaparray[cur], heaparray[prt]);
			strcpy(heaparray[prt], tmp);
		}

		cur++;
		heapnum--;
	}
}

void makeSortedFile(FILE *outputfp, char **heaparray)
{
	int i, cur_page = 0, cur_record = 1;
	int last = recordnum, chd1, chd2, cur;
	char tmp[RECORD_SIZE];
	char pagebuf[PAGE_SIZE];

	memset(pagebuf, (char)0xff, PAGE_SIZE);
	memcpy(pagebuf, &pagenum, sizeof(int));
	memcpy(pagebuf+4, &recordnum, sizeof(int));
	writePage(outputfp, pagebuf, cur_page++);

	while(pagenum > cur_page){
		memset(pagebuf, (char)0xff, PAGE_SIZE);

		for(i = 0; i < PAGE_SIZE/RECORD_SIZE; i++){
			if(cur_record > recordnum)
				break;

			memcpy(pagebuf+(i*RECORD_SIZE), heaparray[1], RECORD_SIZE);

			cur = 1;
			strcpy(heaparray[cur], heaparray[last--]);

			while(1){
				chd1 = 2 * cur;
				chd2 = 2 * cur + 1;

				if(heaparray[chd1] == NULL && heaparray[chd2] == NULL)
					break;

				if(atoll(heaparray[chd1]) > atoll(heaparray[chd2]))
					chd1 = chd2;

				if(atoll(heaparray[chd1]) > atoll(heaparray[cur]))
					break;
				else{
					if(chd1 > last)
						break;
					strcpy(tmp, heaparray[cur]);
					strcpy(heaparray[cur], heaparray[chd1]);
					strcpy(heaparray[chd1], tmp);
				}
				cur = chd1;
			}
			cur_record++;
		}
		writePage(outputfp, pagebuf, cur_page);

		cur_page++;
	}
}

int main(int argc, char *argv[])
{
	int i;
	char header_page[PAGE_SIZE], page[PAGE_SIZE];
	char **heaparray;
	FILE *inputfp;	// 입력 레코드 파일의 파일 포인터
	FILE *outputfp;	// 정렬된 레코드 파일의 파일 포인터

	if(argc != 4){ // 입력 개수 오류 처리 
		fprintf(stderr, "args error\n");
		exit(1);
	}

	if(strcmp(argv[1], "s")){ // 없는 옵션 오류 처리
		fprintf(stderr, "only s option\n");
		exit(1);
	}

	inputfp = fopen(argv[2], "r");
	outputfp = fopen(argv[3], "w+");

	if(inputfp == NULL || outputfp == NULL){ // fopen error
		fprintf(stderr, "fopen error\n");
		exit(1);
	}

	readPage(inputfp, header_page, 0);
	memcpy(&pagenum, header_page, sizeof(int));
	memcpy(&recordnum, header_page+4, sizeof(int));

	heaparray = malloc(sizeof(char*) * recordnum+1);
	tmparray = malloc(sizeof(char*) * recordnum+1);
	for(i = 0; i <= recordnum; i++){
		heaparray[i] = malloc(sizeof(char) * RECORD_SIZE);
		tmparray[i] = malloc(sizeof(char) * RECORD_SIZE);
	}

	buildHeap(inputfp, heaparray);

	makeSortedFile(outputfp, heaparray);

	return 1;
}
