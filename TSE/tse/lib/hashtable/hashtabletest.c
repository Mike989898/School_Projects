#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "list.h"
#include "hashtable.h"

//a test structure for putting into the hashtable
struct test_structure{
	char *test_string;
	int test_number;
};

/*Prototypes*/
void delete_struct(void *c);
void printHashtable(void *arg, char *key, void *data);

int main(){
	//create a hash table and insert a few itmems
	hashtable_t *myHash = hashtable_new(4, delete_struct);

	struct test_structure *trial1 = malloc(sizeof(struct test_structure));
	trial1->test_string = "Test1";
	trial1->test_number = 1;

	struct test_structure *trial2 = malloc(sizeof(struct test_structure));;
	trial2->test_string = "Test2";
	trial2->test_number = 2;

	struct test_structure *trial3 = malloc(sizeof(struct test_structure));
	trial3->test_string = "Test3";
	trial3->test_number = 3;

	struct test_structure *trial4 = malloc(sizeof(struct test_structure));
	trial4->test_string = "Test4";
	trial4->test_number = 4;

	struct test_structure *trial5 = malloc(sizeof(struct test_structure));
	trial5->test_string = "Test5";
	trial5->test_number = 5;

	//insert test structures
	hashtable_insert(myHash, "First_struct Key", trial1);
	hashtable_insert(myHash, "Second_struct Key", trial2);
	hashtable_insert(myHash, "Third_struct Key", trial3);
	hashtable_insert(myHash, "Fourth_struct Key", trial4);
	hashtable_insert(myHash, "Fith_struct Key", trial5);

	//iterate through the hashtable and print out the contents
	hashtable_iterate(myHash, printHashtable, NULL);

	hashtable_delete(myHash);
}

//a function for deleting the strings
void delete_struct(void *c){
	free(c);
}

//method for printing out hashtable
void printHashtable(void *arg, char *key, void *data){
	struct test_structure *item = (struct test_structure *) data;
	printf("The Key is: %s and the data is-- String:%s Number:%d\n", key, item->test_string, item->test_number);
}