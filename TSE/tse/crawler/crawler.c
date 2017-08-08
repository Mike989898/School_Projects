#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "../lib/cs50ds.h"
#include "../common/common.h"

/*************Globals*************/
char *seedURL;
char *pageDirectory;
int maxDepth;
bag_t *bag;
hashtable_t *hashtable;

/*************Internal Function Prototypes*************/
static bool getURLs(WebPage *page);
static bool cleanUp();
static void crawlWeb();
static void deleteWebPage(void *page);
static bool createHtmlPageFile(int count, WebPage *page);

/*************main()*************/
int main(const int argc, char *argv[]){
	//check the number of arguments
	if(argc != 4){
		printf("Inappropriate number of arguments. Please enter: seedURL, pageDirectory maxDepth\n");
	return 1; //invalid number of arguments
	}

	//assign arguments to internal variables
	seedURL = malloc(strlen(argv[1]) + 1);
	strcpy(seedURL, argv[1]);

	pageDirectory = argv[2];
	//scan in maxdepth as an integer
	if ((sscanf (argv[3], "%d", &maxDepth)) == 0){
		printf("Invalid maxdepth, please input a number in range: 0-10\n");
		return 4; //invalid maxdept
	}

	//check that the max depth is valid and isn't too large
	if(maxDepth < 0 || maxDepth > 10){
		printf("maxDepth not in valid range. Please enter a maxdepth in range: 0-10\n");
	return 4; //invalid maxdepth
	}

	//validate URL
	if(!IsInternalURL(seedURL)){
		printf("Please enter a valid URL that is internal to: http://old-www.cs.dartmouth.edu/\n");
		return 2; //invalid URL
	}

	/*Attempt to create a file in the given directory.
	*If it can be create, the passed pagedirectory is valid.
	*/

	//build file path
	char *temp_file_name = malloc(sizeof(char *) * (strlen(pageDirectory) 
		+ strlen("/.crawler") + 1));
	strcpy(temp_file_name, pageDirectory);//copy the string so as to not change contents of pageDirectory
	strcat(temp_file_name, "/.crawler");

	//check if the creation of the file works 
	FILE *temp_file;
	if((temp_file=(fopen(temp_file_name,"w"))) == NULL){
		printf("Please enter a valid page directory\n");
		free(temp_file_name);
		return 3;
	}
	free(temp_file_name);
	//All arguments are ok, call the crawlWeb function to do the processing
	crawlWeb();

	// Delete all of our files
	cleanUp();
	return 0;
}

/*************crawlWeb()*************/
/* This function crawls web starting from the seed page.
 * It uses two helper functions: createHtmlPageFile()
 * to create a document containing all the relevant data
 * of the page found and getURLs() to get all of the
 * urls contained in the page that it is currently processing. 
 */
static void crawlWeb(){
	int page_count = 1; //number of pages visited

	//create the bag and the hashtable
	bag = bag_new(deleteWebPage);
	hashtable = hashtable_new(100, deleteWebPage); 	//Note: Hash table can be resized for optimization

	//create a WebPage object for the seed page
	WebPage *seed = calloc(1, sizeof(WebPage));
	seed->url = seedURL;
	seed->depth = 0;

	//insert seed page into bag and hashtable
	bag_insert(bag, seed);
	hashtable_insert(hashtable, seedURL, seed);
	printf("Added (%d): %s\n", 0, seedURL);
	//extract from the bag until it is empty(no more pages)
	WebPage *next_page;
	while((next_page = bag_extract(bag)) != NULL){
		//wait 1 second before next fetch
		sleep(INTERVAL_PER_FETCH);
		//perform a query for the page and get the html
		if(GetWebPage(next_page)){
			//attempt to write the html,url,and depth out to a numbered file
			if(createHtmlPageFile(page_count, next_page)){
				page_count++;
			}
			getURLs(next_page);
		}
	}
}

/*************deleteWebPage()*************/
/* Function for getting rid of a webpage object
 * This function is used by the hashtable and bag 
 * to clean up. It is also used by the getURLs fuction.
 * Takes: a Webpage.
 */
static void deleteWebPage(void *page){
	WebPage *page_to_delete = (WebPage *) page;

	free(page_to_delete->html);
	free(page_to_delete->url);
	free(page_to_delete);
	page_to_delete = NULL;
}

/*************createHtmlPageFile()*************/
/* Fuction for creating a document containing a
 * pages url, depth found at, and the html content
 * of the page. The count parameter is used as the
 * document's ID number.
 * Takes: An integer ID, and a Webpage.
 */
static bool createHtmlPageFile(int count, WebPage *page){

	//build the file path to write t
	char html_page_num[10]; //this could potentially need to be bigger depending on how many pages we add
	sprintf(html_page_num, "%d", count);
	char *full_file_path = malloc(sizeof(char *)* (strlen(pageDirectory) 
		+ 17));
	strcpy(full_file_path, pageDirectory);//copy pageDirectory so as to not change contents
	//append /
	strcat(full_file_path, "/");
	//append page number
	strcat(full_file_path, html_page_num);
	//attempt to write file with 
	FILE *write_file;
	if((write_file=(fopen(full_file_path, "w"))) == NULL){
		free(full_file_path);
		return false;
	}
	else{
		free(full_file_path);
		//write out url, page depth, and html to file
		fprintf(write_file, "%s\n%d\n", page->url, page->depth);
		fprintf(write_file, "%s", page->html);
		fclose(write_file);
		return true;
	}
}
/*************getURLs()*************/
/* Function for getting all the internal URLS to
 * a page up to a maximum specified depth.
 * Takes: a Webapge
 */
static bool getURLs(WebPage *page) {
	char *result;
	int pos = 0;
	while ((pos = GetNextURL(page->html, pos, page->url, &result)) > 0) {
		printf("Found: %s\n", result);
		if (!IsInternalURL(result)) {
			printf("IgnExtrn: %s\n", result);
			free(result);
			continue;
		}

    	if (!hashtable_find(hashtable, result) &&
    		page->depth + 1 <= maxDepth) {

    		WebPage *next_page = calloc(1, sizeof(WebPage));
			if (next_page == NULL) {
				free(next_page);
				// Remove before submit
				printf("Error: Could not allocate memory for next_page\n");
				free(result);
				continue;
			}
			next_page->url = result; 
			next_page->depth = page->depth + 1;

			hashtable_insert(hashtable, result, next_page);
			bag_insert(bag, next_page);
			printf("Added (%d): %s\n", next_page->depth, result);
    	} 
    	else {
    		if (page->depth + 1 > maxDepth) {
				printf("IgnTooDeep (%d): %s\n",  page->depth + 1, result);
				free(result);
    		}
    		else {
				printf("IgnSaved: %s\n", result);
				free(result);
    		}
    	}
	}	
	return true;

}

/*************cleanUp()*************/
/* Function for cleaning up the data
 * structures used by the crawler.
 */
static bool cleanUp() {
	bag_delete(bag);
	hashtable_delete(hashtable);
	return true;
}
