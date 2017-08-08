/*
 *bagtest.c- A Program for testing the Bag data structure
 *usage: 
 *		Testing all the opperations of the bag data structure to see that 
 *		they all function properly.
 *
 *Michael Kilgore, April 2016
 */

#include <stdio.h>
#include <stdlib.h>
#include "bag.h"

/*Prototypes*/
void printBag(void *arg, void *data);
void delete_struct(void *c);

//a test structure for putting into the bag
struct test_structure{
	char *test_string;
	int test_number;
};

int main(){
	bag_t *mybag = bag_new(delete_struct);
	//insert a few items into the bag
	bag_insert(mybag, "First item");
	bag_insert(mybag, "Second item");
	bag_insert(mybag, "Third item");
	bag_insert(mybag, "Fourth item");
	int i;
	char *item;
	//print out all four items inserted
	//also try and remove from the bag when it is empty
	for(i=0; i<6; i++){
		if((item = (char *) bag_extract(mybag)) == NULL){
			printf("The Bag is empty\n");
		}
		else{
			printf("Removed item: %s\n", item);
		}
	}
	//make sure the bag can be inserted into after it is emptied
	//also rempty the bag
	bag_insert(mybag, "First item");
	bag_insert(mybag, "Second item");
	for(i=0; i<6; i++){
		if((item = (char *) bag_extract(mybag)) == NULL){
			printf("The Bag is empty\n");
		}
		else{
			printf("Removed item: %s\n", item);
		}
	}
	//try to insert into a null bag(shouldn't segfault)
	bag_t *null_bag = NULL;
	bag_insert(null_bag, "shouldn't make it in the bag");

	//try extracting from a null bag(shouldn't segfault)
	bag_extract(null_bag);

	//try putting a few structures in the bag and removing them
	struct test_structure *trial1 = malloc(sizeof(struct test_structure));
	trial1->test_string = "Test1";
	trial1->test_number = 1;

	struct test_structure *trial2 = malloc(sizeof(struct test_structure));;
	trial2->test_string = "Test2";
	trial2->test_number = 2;

	struct test_structure *trial3 = malloc(sizeof(struct test_structure));
	trial3->test_string = "Test3";
	trial3->test_number = 3;

	bag_insert(mybag, trial1);
	bag_insert(mybag, trial2);
	bag_insert(mybag, trial3);

	//remove structures and print the data they contain
	struct test_structure *item_struct;
	for(i=0; i<3; i++){
		if((item_struct = (struct test_structure *) bag_extract(mybag)) == NULL){
			printf("The Bag is empty\n");
		}
		else{
			printf("The data removed is- string: %s number: %d\n", item_struct->test_string, item_struct->test_number);
			free(item_struct);
		}
	}

	bag_delete(mybag);

	//create another bag

	bag_t *myNewBag = bag_new(delete_struct);
	//try putting a few items in the bag and then iterating over the contents of the bag
	bag_insert(myNewBag, "First item");
	bag_insert(myNewBag, "Second item");
	bag_insert(myNewBag, "Third item");
	bag_insert(myNewBag, "Fourth item");
	bag_insert(myNewBag, "Fifth item");
	bag_insert(myNewBag, "Sixth item");

	bag_iterate(myNewBag, printBag, NULL);
}

//function for printing out the contents of a bag item
void printBag(void *arg, void *data){
	char *contents = (char *) data;
	printf("Got item:%s\n", contents);
}

//a function for deleting the strings
void delete_struct(void *c){
	free(c);
}