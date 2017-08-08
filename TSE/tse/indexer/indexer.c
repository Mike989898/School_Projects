/*
 * indexer.c     Akira Murphy & Mike Kilgore    May 1st, 2016
 *
 * This file creates an indexer, which reads through a series of files created
 * by a crawler as defined in the previous lab, and makes a table from word
 * to all documents which contain that word, and the count of words in that
 * document.
 *
 * Akira Murphy & Mike Kilgore, May 2016
 * Comment template is taken from CS50 Lecture Example tree.6 by David Kotz.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "../lib/cs50ds.h"
#include "../common/common.h"

/*************Internal Function Prototypes*************/
static bool parseArguments(const int argc, char *argv[]);
static bool index_build();
void deleteCounters(void *data);
void printDocAndWordCount();
static char *convertFileContentsToString(FILE *fp);

/*Globals*/
char *photo_page_dir; 
FILE *index_file;

/*************Main()*************/
int main(const int argc, char *argv[]){
	if(parseArguments(argc, argv)){
		if (index_build()){
			fclose(index_file);
			return 0;
		}
		else{
			//index couldn't be built
			return 2;
		}
	}
	else{
		//invalid arguments
		return 1;
	}
}

/*************parseArguments()*************/
/* Goes through the arguments and checks that a valid directory was given, along
 * with the path to a writable file.
 */
static bool parseArguments(const int argc, char *argv[]){
	if(argc != 3){
		printf("Inproper number of arguments. Please input: pageDirectory indexFilename\n");
		return false;
	}

	//check that the photopage directory given is a valid one
	//start by building a file path to check if .crawler exists
	//and is readable
	photo_page_dir = argv[1];
	char *full_file_path = malloc(sizeof(char *)*(strlen(photo_page_dir)));
	strcpy(full_file_path, photo_page_dir);//copy pageDirectory so as to not change contents
	strcat(full_file_path, "/.crawler");

	//check the status of the .crawler file
	FILE *read_file;
	if((read_file=(fopen(full_file_path, "r"))) == NULL){
		printf("Could not find %s", full_file_path);
		free(full_file_path);
		printf(". Invalid pageDirectory!\n");
		return false;
	}
	else{
		free(full_file_path);
		fclose(read_file);
	}

	//finally, check that we can create the for the index specified by the user
	if((index_file=(fopen(argv[2], "w"))) == NULL){
		printf("Cannot create file %s\n", argv[2]);
		return false;
	}
	//all of the conditions were met for valid input
	return true;
}

/*************index_build()*************/
/* Scans throgh all the files present in the directory, and adds all words
 * found to the hashtable.
 */
static bool index_build(){
	hashtable_t *hashtable = hashtable_new(2000, deleteCounters); 

	//read all the files in the directory and get out the words inside
	int file_count = 1; //document ID
	FILE *next_file;
	do{
		//build the file path of the next numbered document file(ie pageDirectory/0, pageDirectory/1, ...)
		char counter_string_buffer[6]; //this could potentially need to be bigger depending on how many pages we add
		sprintf(counter_string_buffer, "%d", file_count);
		char *temp_file_name;
		if ((temp_file_name = malloc(sizeof(char *)*(strlen(photo_page_dir)))) == NULL){
			return false; // not enough spece for the temp_file_name
		}
		strcpy(temp_file_name, photo_page_dir);
		strcat(temp_file_name, "/");
		strcat(temp_file_name, counter_string_buffer);

		//attempt to open the next file
		if(!((next_file=(fopen(temp_file_name, "r"))) == NULL)){
			//load the contents of the file into a string
			char *file_contents; 
			if ((file_contents = convertFileContentsToString(next_file)) == NULL){
				return false; //couldn't read and store contents of file
			}
			else{
				int position = 0;
				char *next_word;

				//loop through all the words in the html doc and add
				//them to the hashtable if they aren' there already
				//also insert a new counter for each word key
				while((position = GetNextWord(file_contents, position, &next_word)) != -1){
					if(next_word == NULL){
						return false; // couldn't read a word in the file
					}
					// Skip words that are too short.
					if (strlen(next_word) < 3) {
						free(next_word);
						continue;
					}
					counters_t *word_counter;
					if((word_counter = hashtable_find(hashtable, NormalizeWord(next_word))) != NULL){
						//hashtable already has the word in there, increment the word's count
						//for the current document ID
						counters_add(word_counter, file_count);
					}
					else{
						//create a new counter, start up a new count for the word
						//and insert it into the hash
						word_counter = counters_new();
						counters_add(word_counter, file_count);
						hashtable_insert(hashtable, next_word, word_counter);
					}
					//free the word malloced by GetNextWord
					free(next_word);
				}
				free(file_contents);
			}
			fclose(next_file);
		}

		//increment the doc ID
		file_count++;
		free(temp_file_name);

	} while(next_file !=NULL);

	index_save(hashtable, index_file);
	hashtable_delete(hashtable);
	//successfully built the index
	return true;
}

static char *convertFileContentsToString(FILE *fp){
	long file_size;
	char *buffer;

	//go to the end of the file
	//to find out how large the file
	//is in bytes
	fseek(fp , 0L , SEEK_END);
	file_size = ftell(fp);
	//bring file pointer back to start of file
	rewind(fp);

    //allocate memory for entire contents of file 
	buffer = calloc(1, file_size+1);
	if(buffer == NULL){
		//couldn't allocate enough memory for the buffer
		return NULL;
	}

    // copy the file into the buffer 
	if((fread(buffer , file_size, 1 , fp)) != 1){
		//couldn't read the contents of the file into the buffer
		return NULL;
	}

	//we want to remove the first two lines of the file
	//so that we can start reading words directly from the
	//html
	int i;
	int new_line_counter = 0;
	char *new_buffer = calloc(1, file_size+1);
	for(i=0; i<strlen(buffer); i++){
		if(buffer[i] == '\n'){
			new_line_counter++;
			if(new_line_counter == 2){
				strncpy(new_buffer, &buffer[i+1], strlen(buffer)-(i+1));
				break;
			}
		}
	}
	free(buffer);
	//Remember to free the buffer!!!
	return new_buffer;
}

/*************deleteCounters()*************/
void deleteCounters(void *data){
	counters_t *counter_to_delete = (counters_t *) data;
	counters_delete(counter_to_delete);
}