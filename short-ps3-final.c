#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>   // for gettimeofday()
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <utmp.h>
#include <err.h>
#include <stdbool.h>

#define UTMP_MAX 20
#define USER_MAX 32
#define FALSE 0				// changed to be more obvious
#define TRUE 1
#define ONE 1
#define ZERO 0

#define ERR_MEM_FAILURE 10
#define ERR_FILEOPEN_FAILURE 11
#define ERR_ZEROLENGTH_File 12

/********************************************************************
* File: ps3.c
* Author: Paul K Short
* 
* Created on February 9, 2017, 2:00 PM
* Last Modified: February 14, 2017 4:32 PM
*
* Purpose: This program asks the user what username they would like to look up
*          then it prints out the last login date if the user exists
*          
* Algorithm: 
*       1. Take in user input
*       2. Read in utmp/wtmp files
*       3. Search the records
*       4. If found, print out the last login date
*
*********************************************************************/

/*********************************************************************
* Description: The searchU function takes in a user input and a utmp
*              structure and searches through the structures and prints
*              out the last login date.
*   
* Input:  char* user
*	 
*                        
* Output: Returns true if the user was found and returns false if not found
*        
* Global Constants:
* 
*      ZERO = 0
*      ONE = 1
*                
* Variables used:
*      struct utmp* loginData - pointer to the utmp structure
*      File* loginFileU - pointer to the utmp login file
*      int logins - bool 
*               
* Last Modified: 2/14/17
*********************************************************************/ 
int searchU (char* user) {

	struct utmp *loginData;
    FILE * loginFileU;
	int logins = ZERO;


    // Allocate memory for the structure
    loginData = malloc( sizeof(struct utmp) );
    if ( loginData == NULL ) {
        err(EXIT_FAILURE, "cannot allocate memory for loginData" );
    }
    // clear out the memory (makes debugging easier)
    memset( loginData, FALSE, sizeof(struct utmp) );

    // open the file check the return code - note that brackets are not needed with the if statement....
    loginFileU = fopen( UTMP_FILENAME, "rb" );
	if( loginFileU == NULL )
        err(EXIT_FAILURE, "cannot open %s\n", UTMP_FILENAME);

	
	// looping through user logins to find
    while ( loginFileU != FALSE ) {

		// Break out if at the end of the file
		if ( feof( loginFileU ) == TRUE ) 
			break;
		
		if( fread( loginData, sizeof(struct utmp), ONE, loginFileU ) != ONE )
			break;
		
        //utmpBool = searchU(userIn, loginData+i);   // this needs to be fixed....

		if( strcmp(loginData->ut_user, user) == ZERO ) {
			logins++;
			printf("User Logged In: %d %s %s time:  %d.%06d  %s", 
			       loginData->ut_pid, loginData->ut_line, loginData->ut_user, loginData->ut_tv.tv_sec, loginData->ut_tv.tv_usec, ctime(&loginData->ut_tv.tv_sec) );
		}
    }
    fclose ( loginFileU );

	free( loginData );
	return logins;
}


/*********************************************************************
* Description: The searchW function takes in a user input and a utmp
*              structure and searches through the wtmp structure and prints
*              out the last login date if found
*   
* Input:  char* user - user input
*         
*                        
* Output: Returns true if the user was found and returns false if not found
*        
* Global Constants:
* 
*      ZERO = 0
*      ONE = 1
*                
* Variables used:
*      struct utmp* loginData - pointer to the wtmp structure
*      FILE * loginFileW - pointer to the wtmp login files
*      int loginhist - bool
*               
* Last Modified: 2/14/17
*********************************************************************/
int searchW( char * user ) {

	struct utmp *loginData;
    FILE * loginFileW;
	int loginhist = ZERO;

    // Allocate memory for the structure
    loginData = malloc( sizeof(struct utmp) );
    if ( loginData == NULL ) {
        err(EXIT_FAILURE, "cannot allocate memory for loginData" );
    }
    // clear out the memory (makes debugging easier)
    memset( loginData, FALSE, sizeof(struct utmp) );

	loginFileW = fopen("/var/log/wtmp", "rb");
	if ( loginFileW == NULL )
   		err(EXIT_FAILURE, "cannot open /var/log/wtmp\n" );

	while ( loginFileW != FALSE ) {

		// Break out if at the end of the file
		if ( feof( loginFileW ) == TRUE ) 
			break;
	
		if( fread( loginData, sizeof(struct utmp), ONE, loginFileW ) != ONE )
			break;
		
		if( strcmp(loginData->ut_user, user) == ZERO ) {
			loginhist++;
			printf("User Login history: %d %s %s time:  %d.%06d  %s", 
				   loginData->ut_pid, loginData->ut_line, loginData->ut_user, loginData->ut_tv.tv_sec, loginData->ut_tv.tv_usec, ctime( &loginData->ut_tv.tv_sec) );
		}
	}
	fclose ( loginFileW );

	free(loginData);
	return loginhist;
}


/*********************************************************************
* Description: This main function handles the user input, allocating 
*              memory for the structure, and also calling searchU and
*              searchW
*   
* Input: Prompts user for username to lookup
*                        
* Output: Prints out the last time a user was logged in if the user was found
*        if user is not found it prints out that the user was never logged in
*
* Global Constants:
*      ZERO = 0
*      ONE = 1
*      USER_MAX = 32
*      UTMP_FILENAME
*      UTMP_MAX = 20
*                
* Variables used:
*      char* userIn - pointer to the user input
*               
* Last Modified: 2/12/17
*********************************************************************/
int main(){

    //size_t elementsRead;
    char * userIn;
    //int utmpBool = FALSE;
	//struct tm *tmLocalTime;
	
	
    userIn = (char *)malloc(USER_MAX * sizeof(char));
    if ( userIn == NULL ) {
        err(EXIT_FAILURE, "cannot allocate memory for userIn" );
    }
    memset( userIn, FALSE, USER_MAX * sizeof(char) );
	
    printf("---------\n");
    printf("Start\n");
    printf("---------\n");
    
    printf("Enter user to look up: \n");
    scanf("%s", userIn);

   	
	if ( searchU( userIn ) == ZERO )
		if ( searchW( userIn ) == ZERO )
			printf( "User was never logged in.\n" );
	
    printf( "Done\n" );

	return TRUE;
    
}



