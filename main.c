#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <yescrypt.h>
#include "sysendian.h"
#define MAX_AVAILABLE 68
#define MAX_BUFF 1024
#define DELIMITER ":"
#define MAX_USERS 10
#define MAX_SETTING_LEN 41
#define MAX_HASH_LEN 45
#define MAX_PASSWD_LEN 7
#define MAX_USERNAME_LEN 16
#define MAX_LINE_LENGTH 1024
char available[MAX_AVAILABLE];

typedef struct shadow{
    char username[MAX_USERNAME_LEN];
    char setting[MAX_SETTING_LEN];
    char hash[MAX_HASH_LEN];
} shadow;

shadow hashArr[MAX_USERS];

int sizeHashArr = 0;


void remakeWordList(FILE* wordlist, char* fileName){
    if (wordlist == NULL) {
        printf("Failed to open file\n");
        return;
    }

    FILE *temp_fp = fopen("temp.txt", "w");
    if (temp_fp == NULL) {
        printf("Failed to open temporary file\n");
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), wordlist)) {
        // Remove the newline character
        line[strcspn(line, "\n")] = 0;

        if (strlen(line) <= 6) {
            fprintf(temp_fp, "%s\n", line);
        }
    }

    fclose(temp_fp);

    // Replace the original file with the temporary file
    if (remove(fileName) == 0 && rename("temp.txt", fileName) == 0) {
        printf("Successfully removed words longer than 6 characters\n");
    } else {
        printf("Failed to replace the original file\n");
    }

    return;
}

void parse_shadow_file(char* line, int i) {
    
    char* hash = strrchr(line, '$');
    char* setting;
    if(strncpy(setting, line, 30) == NULL) {
    	printf("strncpy failed : setting\n");
    	return;
    }
    hash = strrchr(line, '$');
    hash = strrchr(line, '$');
    hash = strrchr(line, '$');
    hash++;

    strncpy(hashArr[i].setting, setting, MAX_SETTING_LEN - 1);
    strncpy(hashArr[i].hash, hash, MAX_HASH_LEN - 1);

    hashArr[i].setting[MAX_SETTING_LEN - 1] = '\0';
    hashArr[i].hash[MAX_HASH_LEN - 1] = '\0';

}


void getHashFromFile(FILE* fp){
	shadow* s = hashArr;
	char* username;
	char* hash;
	char* saveptr;
	char line[MAX_BUFF];
	printf("start get hash of users\n");
	rewind(fp);

	while(fgets(line, MAX_BUFF, fp) != NULL){
		saveptr = NULL;
		username = strtok_r(line, DELIMITER, &saveptr);
		hash = strtok_r(NULL, DELIMITER, &saveptr);
		if(hash == NULL || strcmp(hash,"*") == 0|| strcmp(hash,"!") == 0) continue;
		strncpy(hashArr[sizeHashArr].username, username, MAX_USERNAME_LEN - 1);
		hashArr[sizeHashArr].username[MAX_USERNAME_LEN - 1] = '\0';
		
		parse_shadow_file(hash, sizeHashArr++);

	}
}
void crackFromWordlist(FILE* wordlist){
	int i;
	char word[MAX_LINE_LENGTH];
	char hash[MAX_HASH_LEN];
	char* output;
	clock_t end, start;
	printf("start crack From wordList\n");
	for(i = 1; i < sizeHashArr; i++){
		rewind(wordlist);
		start = clock();
		while(fgets(word, sizeof(word), wordlist)){
			word[strcspn(word, "\n")] = '\0';
			output = yescrypt(word, hashArr[i].setting);
			output = strrchr(output, '$');
			output = strrchr(output, '$');
			output = strrchr(output, '$');
			output = strrchr(output, '$');
			output++;
			
			if(strcmp(hashArr[i].hash, output) == 0){
				printf("found passwd...\n");
				printf("username : %s, passwd : %s\n", hashArr[i].username, word);
				break;
				
			}
			
			
		}
		end = clock();
		printf("time spent : %.2f(s)\n", (double)(end-start)/CLOCKS_PER_SEC);
	}
}


void crackRandom(int num){ // num : index of hashArr
	int char0, char1, char2, char3, char4, char5;
	char word[MAX_PASSWD_LEN];
	char* output;
	printf("Cracking single-digit passwd...\n");
	clock_t start, end;
	start = clock();
	for(char0 = 0; char0 < MAX_AVAILABLE; char0++){ // single-digit passwd
		word[0] = available[char0];
		word[1] = '\0';
		output = yescrypt(word, hashArr[num].setting);
		output = strrchr(output, '$');
		output = strrchr(output, '$');
		output = strrchr(output, '$');
		output = strrchr(output, '$');
		output++;
			
		if(strcmp(hashArr[num].hash, output) == 0){
			printf("found passwd...\n");
			printf("username : %s, passwd : %s\n", hashArr[num].username, word);
			end = clock();
			printf("time spent : %.2f(s)\n", (double)(end-start)/CLOCKS_PER_SEC);
			return;
		}
	}
	end = clock();
	printf("time spent : %.2f(s)\n", (double)(end-start)/CLOCKS_PER_SEC);
	printf("Cracking double-digit password...\n");
	start = clock();

	for(char0 = 0; char0 < MAX_AVAILABLE; char0++){
		for(char1 = 0; char1 < MAX_AVAILABLE; char1++){
			word[0] = available[char0];
			word[1] = available[char1];
			word[2] = '\0';
			output = yescrypt(word, hashArr[num].setting);
			output = strrchr(output, '$');
			output = strrchr(output, '$');
			output = strrchr(output, '$');
			output = strrchr(output, '$');
			output++;
			
			if(strcmp(hashArr[num].hash, output) == 0){
				printf("found passwd...\n");
				printf("username : %s, passwd : %s\n", hashArr[num].username, word);
				end = clock();
				printf("time spent : %.2f(s)\n", (double)(end-start)/CLOCKS_PER_SEC);
				return;
			}
		}
	}
	end = clock();
	printf("time spent : %.2f(s)\n", (double)(end-start)/CLOCKS_PER_SEC);
	printf("Cracking three-digit password...\n");
	start = clock();
	for(char0 = 0; char0 < MAX_AVAILABLE; char0++){
		for(char1 = 0; char1 < MAX_AVAILABLE; char1++){
			for(char2 = 0; char2 < MAX_AVAILABLE; char2++){
				word[0] = available[char0];
				word[1] = available[char1];
				word[2] = available[char2];
				word[3] = '\0';
				output = yescrypt(word, hashArr[num].setting);
				output = strrchr(output, '$');
				output = strrchr(output, '$');
				output = strrchr(output, '$');
				output = strrchr(output, '$');
				output++;
			
				if(strcmp(hashArr[num].hash, output) == 0){
					printf("found passwd...\n");
					printf("username : %s, passwd : %s\n", hashArr[num].username, word);
					end = clock();
					printf("time spent : %.2f(s)\n", (double)(end-start)/CLOCKS_PER_SEC);	
					return;
				}
			}
		}
		
	}
	end = clock();
	printf("time spent : %.2f(s)\n", (double)(end-start)/CLOCKS_PER_SEC);
	printf("Cracking four-digit password...\n");
	start = clock();
	for(char0 = 0; char0 < MAX_AVAILABLE; char0++){
		for(char1 = 0; char1 < MAX_AVAILABLE; char1++){
			for(char2 = 0; char2 < MAX_AVAILABLE; char2++){
				for(char3 = 0; char3 < MAX_AVAILABLE; char3++){	
					word[0] = available[char0];
					word[1] = available[char1];
					word[2] = available[char2];
					word[3] = available[char3];
					word[4] = '\0';
					output = yescrypt(word, hashArr[num].setting);
					output = strrchr(output, '$');
					output = strrchr(output, '$');
					output = strrchr(output, '$');
					output = strrchr(output, '$');
					output++;
			
					if(strcmp(hashArr[num].hash, output) == 0){
						printf("found passwd...\n");
						printf("username : %s, passwd : %s\n", hashArr[num].username, word);
						end = clock();
						printf("time spent : %.2f(s)\n", (double)(end-start)/CLOCKS_PER_SEC);
						return;
					}
				}
			}
		}
	}
	end = clock();
	printf("time spent : %.2f(s)\n", (double)(end-start)/CLOCKS_PER_SEC);
	printf("Cracking five-digit password...\n");
	start = clock();
	for(char0 = 0; char0 < MAX_AVAILABLE; char0++){
		for(char1 = 0; char1 < MAX_AVAILABLE; char1++){
			for(char2 = 0; char2 < MAX_AVAILABLE; char2++){
				for(char3 = 0; char3 < MAX_AVAILABLE; char3++){	
					for(char4 = 0; char4 < MAX_AVAILABLE; char4++){	
						word[0] = available[char0];
						word[1] = available[char1];
						word[2] = available[char2];
						word[3] = available[char3];
						word[4] = available[char4];
						word[5] = '\0'; 
						output = yescrypt(word, hashArr[num].setting);
						output = strrchr(output, '$');
						output = strrchr(output, '$');
						output = strrchr(output, '$');
						output = strrchr(output, '$');
						output++;
			
						if(strcmp(hashArr[num].hash, output) == 0){
							printf("found passwd...\n");
							printf("username : %s, passwd : %s\n", hashArr[num].username, word);
							end = clock();
							printf("time spent : %.2f(s)\n", (double)(end-start)/CLOCKS_PER_SEC);
							return;
						}
					}
				}
			}
		}
	}
	end = clock();
	printf("time spent : %.2f(s)\n", (double)(end-start)/CLOCKS_PER_SEC);
	printf("Cracking six-digit password...\n");
	start = clock();
	for(char0 = 0; char0 < MAX_AVAILABLE; char0++){
		for(char1 = 0; char1 < MAX_AVAILABLE; char1++){
			for(char2 = 0; char2 < MAX_AVAILABLE; char2++){
				for(char3 = 0; char3 < MAX_AVAILABLE; char3++){	
					for(char4 = 0; char4 < MAX_AVAILABLE; char4++){	
						for(char5 = 0; char5 < MAX_AVAILABLE; char5++){
							word[0] = available[char0];
							word[1] = available[char1];
							word[2] = available[char2];
							word[3] = available[char3];
							word[4] = available[char4];
							word[5] = available[char5];
							word[6] = '\0'; 
							output = yescrypt(word, hashArr[num].setting);
							output = strrchr(output, '$');
							output = strrchr(output, '$');
							output = strrchr(output, '$');
							output = strrchr(output, '$');
							output++;
			
						if(strcmp(hashArr[num].hash, output) == 0){
							printf("found passwd...\n");
							printf("username : %s, passwd : %s\n", hashArr[num].username, word);
							end = clock();
							printf("time spent : %.2f(s)\n", (double)(end-start)/CLOCKS_PER_SEC);
							return;
						}
					}
				}
			}
		}
	}
	printf("random passwd cracking failed.\n");
	end = clock();
	printf("time spent : %.2f(s)\n", (double)(end-start)/CLOCKS_PER_SEC);
	}
}	
void setArr(){
	int i = 0;
	for(int j = 65; j <= 90; i++, j++) 
		available[i] = j; // 'A' ~ 'Z'
	for(int j = 97; j <= 122; i++, j++)
		available[i] = j; // 'a' ~ 'z'
	for(int j = 48; j <= 57; i++, j++)
		available[i] = j;
	available[i++] = '!';
	available[i++] = '@';
	available[i++] = '#';
	available[i++] = '$';
	available[i++] = '^';
	available[i++] = '*';
}

int main(int argc, char* argv[]) {
	setArr();
	
	FILE* wordlist = NULL, *target = NULL;
	char* flag; // flag of argv[1]
	char* wordlistFileName; // filename of argv[1]
	char* username; // username from shadow
	char* hash; // hash from shadow
	char* salt; // salt generated by random
	char* passwd; // passwd generated by random


	if(argc == 3) { // open wordlist
		flag = strtok(argv[1], DELIMITER);
		wordlistFileName = strtok(NULL, DELIMITER); // how to use : --wordlist:fileName
		if(strcmp(flag, "--wordlist") != 0) {
			fprintf(stderr, "how to use : --wordlist:FILENAME\n");
			exit(1);
		}
		if(!(wordlist = fopen(wordlistFileName, "r"))){
			fprintf(stderr,"%s open failed\n", wordlistFileName);
			exit(2);
		}
		
		if(!(target = fopen(argv[2], "r"))) { // open shadow
			fprintf(stderr, "%s open failed\n", argv[3]);
			exit(3);
		}
	}				
	else if(argc == 2) {
		if(!(target = fopen(argv[1], "r"))) { // open shadow
			fprintf(stderr, "%s open failed\n", argv[2]);
			exit(3);
		}
	} 
	else {
		fprintf(stderr,"how to use : %s [--wordlist:FILENAME] target\n", argv[0]);
		exit(4);
	}
	
	
	
	// get hash of users
	getHashFromFile(target);
	for(int i = 0; i < sizeHashArr; i++){
		printf("%d : %s, hash : %s, setting : %s\n", i+1,hashArr[i].username , hashArr[i].hash, hashArr[i].setting);
	}

	if(wordlist){
	// start crack from wordlist
		crackFromWordlist(wordlist);
	}	
	printf("start crack randomly.\n");
	// start crack randomly
	for(int i = 1; i < sizeHashArr; i++){
		crackRandom(i);
	}
	
	fclose(target);
	if(wordlist) fclose(wordlist);
	return 0;
}
