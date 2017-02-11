/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
 * Copyright (C) 2017 Kevin Short <kevin@neptune>
 * 
 * short-ps3 is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * short-ps3 is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>		// for err function
#include <string.h>		// for memset....

#include <utmp.h>

// standard file locations
// #define UTMP_FILE       "/var/run/utmp"  -> already in utmp.h


// Return codes: starting higher than 9 so we can use return values from system calls directly...
#define	 ERR_MEM_FAILURE		10
#define	 ERR_FILEOPEN_FAILURE	11
#define  ERR_ZEROLENGTH_FILE	12


int main() {

	struct utmp *loginData;  // note - this is a pointer so we need to allocate memory for the pointer
	FILE * loginFile;		// this is a file pointer but the call to fopen will assign the memory to this pointer....
	size_t  elementsRead;
	
	printf("Start\n");

	// Allocate memory for the structure
	loginData = malloc( sizeof(struct utmp) );
	if( loginData == NULL )  // even though this is a small amount of memory, you AWAYS check the return from malloc
		err(EXIT_FAILURE, "cannot allocate %d bytes", (int)sizeof(struct utmp) );

	//  this could also be done with a single line of code which I hate but get used to seeing it as others code this way:
	//	if( loginData=malloc( sizeof(struct utmp) == NULL ) return (1);

	// clear out the memory.  not necessary but makes debugging easier
	memset( loginData, 0, sizeof(struct utmp) );
	
	// Open the file check the return code
	loginFile = fopen( UTMP_FILENAME, "rb" );	   // the b is not necessary in modern Linux
	if( loginFile == NULL )
		err(EXIT_FAILURE, "cannot open %s", UTMP_FILENAME);

	// Check for a zero length file - could also use and "assert" here but best to always check for this case
	if( feof( loginFile ) != 0 ) {
		fclose( loginFile );  // Not necessarily required in modern linux...
		err(EXIT_FAILURE, "possible zero length file %s\n", UTMP_FILENAME);
	}


	// Start reading from the file 1 element (utmp structure) at a time
	elementsRead = fread( loginData, sizeof(struct utmp), 1, loginFile );

	// verify the number of elements read
	if( elementsRead != 1 ) {
		fclose( loginFile );
		err(EXIT_FAILURE, "cannot read from %s", UTMP_FILENAME);
	}

	// print out the first record read
	printf( "First Record: %s : %s \n", 
	       loginData->ut_user, loginData->ut_host );

	// don't forget to cleanup your memory and file handles!
	free( loginData );
	fclose( loginFile );
	
	printf("Done\n");
	return (0);
}

