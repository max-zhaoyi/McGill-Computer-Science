#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//Student ID: 260913518
//Student Name: PI, Zhao Yi

int zeroTrimmer(char str[100], char strcopy[99]) { //after separation, trim the zeros for visual consistency of entries

	int index = 0;


	for (int i = 0; i<strlen(str); i++) {
		if (str[i] !=32) {
			strcopy[index] = str[i];
			index++;
		}
	}
	return 1;
}

int nDigits(int i) //useless (was to return the digit length of number)
{
  if (i <0) i = -i;
  if (i <10) return 1;
  if (i <100) return 2;
  if (i <1000) return 3;
  if (i <10000) return 4;
  if (i <100000) return 5;
  if (i <1000000) return 6;      
  if (i <10000000) return 7;
  if (i <100000000) return 8;
  if (i <1000000000) return 9;
  return 10;
}

char* separationFunction(char* line, int a) { //implements strtok to delimit rows by their commas

	char* commaToken;

	for (commaToken = strtok(line, ","); commaToken && *commaToken; commaToken=strtok(NULL, ",\n")) {

		if (!--a) return commaToken;
	}

	return NULL;
}

int main(int argc, char *argv[]) {

	char* cmd = argv[1];

	if(argc < 2) { //check for inputs

		printf("You did not provide any arguments. Please enter: ./database CMD OPT1 OPT2 OPT3 OPT4 \n");
		return 1;
	}

	if((strcmp(cmd, "SHOW") && strcmp(cmd, "DELETE") && strcmp(cmd, "ADD"))==1) {//if CMD is not the right command
	 
		printf("The command you requested in invalid. Please select from one of these: SHOW, DELETE, ADD \n");
		return 1;
	}

	if (strcmp(cmd, "SHOW")==0) { //SHOW 

		FILE *database = fopen("database.csv", "rt");

		char line[1024];
		int i=0;
		while (fgets(line, 1024, database)) {

			char nullArray[512] = {'\n'};

			if (strcmp(line, nullArray) !=0) {
				char lineTrimmed[512];
				i++;

				zeroTrimmer(line, lineTrimmed);
				char *first = strdup(lineTrimmed);

				char *second = strdup(lineTrimmed);
				char *third = strdup(lineTrimmed);
				char *fourth = strdup(lineTrimmed);

				printf("Record %d: ID=%3s   NAME=%5s   AGE=%3s   GPA=%3s\n", i, separationFunction(first, 1), separationFunction(second, 2), separationFunction(third,3), separationFunction(fourth, 4));
				
				
			}

		}
		fclose(database);
		return 0;
		
	}

	if (strcmp(cmd, "DELETE")==0) { //DELETE GOES HERE

		if (argc < 3) { //graceful execution

			printf("Name of record to delete is missing \n");
		
			return 1;
		}
		char line[900];
		FILE *database = fopen("database.csv", "rt");
		FILE *tempData = fopen("database.tmp", "wt");
		char *ID;
		char lineTrimmed[100];
		int howManyDeletions = 0; //counts whether any deletion were performed
		while (fgets(line, 2048, database)) {
			zeroTrimmer(line, lineTrimmed);
			char *tmp = strdup(lineTrimmed);
			ID = separationFunction(tmp, 1);

			if (howManyDeletions == 0) { //threshold for first deletion
				char *argv2 = argv[2];
				if (strcmp(ID, argv2) !=0) {
					fprintf(tempData, "%s", line);
				}else {
				
					howManyDeletions++;

				}
			}else {
			
				fprintf(tempData, "%s", line);
			}
			
		}

		if (howManyDeletions==0) printf("Sorry, that user was not found. Nothing was deleted.\n");

		system("rm database.csv");				//system commands to remove and rename
		system("mv database.tmp database.csv");
		return 0;
	

		/* //BAD CODE
		
		
		FILE *databasetmp = fopen("database.txt", "wt");
		int bruh =1;
		while (!feof(database)) {
			if(bruh=2) {
			fgets(singleLine, 899, database);
			fputs(singleLine, databasetmp);
			
			bruh++;
			}
		}
		//fflush(database);
		//fflush(databasetmp);
		*/
		//fclose(database);
		//fclose(tempData);
		
	}

	if (strcmp(cmd, "ADD")==0) {//graceful execution

		if (argc < 6) { 
			printf("Missing ID, Name, AGE, and GPA arguments \n");
			return 1;
		}
		FILE *database = fopen("database.csv", "at");

		int id, age;
		float gpa;
		char *name;

		id = atoi(argv[2]); //Converts each argv into their respective types
		age = atoi(argv[4]);
		gpa = atof(argv[5]);
		name = argv[3];

		fprintf(database, "%d, %s, %d, %g\n", id, name, age, gpa);

		fclose(database);
		return 0;
	}

	
}
	

