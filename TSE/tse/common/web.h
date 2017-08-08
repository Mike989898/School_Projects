/* =========================================================================
 * File: web.h
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Crawler
 *
 * Author: Ira Ray Jenkins - jenkins@cs.dartmouth.edu
 * Date: Fri Apr 25, 2014
 * 
 * Updated by: David Kotz
 * Date: April 2016
 *
 * This file contains the web/html processing functionality.
 *
 * ========================================================================= */
#ifndef __WEB_H
#define __WEB_H

#include <stdbool.h> // bool type
#include <stddef.h> // size_t

static const int INTERVAL_PER_FETCH = 1;   // seconds between fetches

// Structure to represent a web page, and its contents
typedef struct WebPage {
  char *url;                               // url of the page
  char *html;                              // html code of the page
  size_t html_len;                         // length of html code
  int depth;                               // depth of crawl
} WebPage;

/***********************************************************************
 * GetWebPage - curl page->url, store into page->html
 * @page: the webpage struct containing the url to curl
 *
 * Returns true if the curl was successful; otherwise, false. 
 * If the curl succeeded, page->html will contain the content retrieved. 
 * The WebPage struct should have been allocated by the caller, 
 * but the page->html pointer is expected to be NULL. 
 * If this function is successful, a new, null-terminated character
 * buffer will be allocated. It is the caller's responsibility to free this
 * memory.
 *
 * Usage example:
 * WebPage* page = calloc(1, sizeof(WebPage));
 * page->url = "http://www.example.com";
 *
 * if(GetWebPage(page)) {
 *     printf("Found html: %s\n", page->html);
 *     free(page->html);
 *     free(page);
 * }
 */
bool GetWebPage(WebPage *page);

/***********************************************************************
 * GetNextURL - returns the next url from html[pos] into result
 * @html: pointer to the html buffer
 * @pos: current position in html buffer
 * @base_url: the base url of the webpage, used to fixup relative links
 * @result: a pointer to character pointer, used to pass the url back out
 *
 * Returns the current position search so far in html; otherwise, returns < 0.
 * The html and base_url should both be valid character strings.
 * The pos argument should be 0 on the initial call.
 * The result argument should be a NULL pointer.
 * On successful parse of html, result will contain a newly allocated character
 * buffer; may be NULL on failed return. The caller is responsible for free'ing
 * this memory.
 *
 * Usage example: (retrieve all urls in a page)
 * int pos = 0;
 * char* result;
 * char* base_url = "http://www.example.com";
 *
 * while ((pos = GetNextURL(html, pos, base_url, &result)) > 0) {
 *     printf("Found url: %s\n", result);
 *     free(result);
 * }
 *
 */
int GetNextURL(char *html, int pos, char *base_url, char **result);

/***********************************************************************
 * NormalizeURL - attempts to normalize the url
 * @url: absolute url to normalize
 *
 * Returns true on success; 
 * returns false if the url can't be parsed or normalized.
 * returns false if the url refers to a file unlikely to contain html.
 * 
 * Usage example:
 * char* url = calloc(100, sizeof(char));
 * strcpy(url, "HTTP://UsEr:PaSs@www.EXAMPLE.com/path/.././file.html?name=val#top");
 *
 * if (NormalizeURL(url)) {
 *     // url should be: http://UsEr:PaSs@www.example.com/file.html?name=val#top
 * }
 */
bool NormalizeURL(char *url);


/***********************************************************************
 * IsInternalURL - 
 * @url: absolute url to normalize and verify
 *
 * Normalizes URL as a side effect.
 * Returns true if the URL is valid and 'internal' to our domain, 
 * returns false otherwise.
 * 
 * "valid" means that NormalizeURL() returns true;
 * "internal" means that the url begins with INTERNAL_URL_PREFIX.
 */
bool IsInternalURL(char *url);

// All URLs beginning with this prefix are considered "internal"
static const
  char INTERNAL_URL_PREFIX[] = "http://old-www.cs.dartmouth.edu/";

#endif // __WEB_H
