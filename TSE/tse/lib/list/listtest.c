/*
 *listtest.c- A Program for testing the lis data structure
 *usage: 
 *		Testing all the opperations of the list data structure to see that 
 *		they all function properly.
 *
 *Michael Kilgore, April 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "list.h"

/*Prototypes*/
void delete_struct(void *c);
void printList(void *arg, char *key, void *data);

//a test structure for putting into the bag
struct test_structure{
	char *test_string;
	int test_number;
};

int main(){
	//create a new list and insert some key-value pairs into the list
	list_t *myList = list_new(delete_struct);
	
	//try inserting some test structures into the list and retrieving their data
	struct test_structure *trial1 = malloc(sizeof(struct test_structure));
	trial1->test_string = "Test1";
	trial1->test_number = 1;

	struct test_structure *trial2 = malloc(sizeof(struct test_structure));
	trial2->test_string = "Test2";
	trial2->test_number = 2;

	struct test_structure *trial3 = malloc(sizeof(struct test_structure));
	trial3->test_string = "Test3";
	trial3->test_number = 3;

	//insert the structs into the list
	list_insert(myList, "Structure_Key_1", trial1);
	list_insert(myList, "Structure_Key_2", trial2);
	list_insert(myList, "Structure_Key_3", trial3);
	
		//iterate over the over the list and print the contents
	list_iterate(myList, printList, NULL);
	list_delete(myList);
}

//a function for deleting the structs
void delete_struct(void *c){
	free(c);
}

//a function for printing out a list node and its data
void printList(void *arg, char *key, void *data){
	struct test_structure *item = (struct test_structure *) data;
	printf("The Key is: %s and the data is-- String:%s Number:%d\n", key, item->test_string, item->test_number);
}

