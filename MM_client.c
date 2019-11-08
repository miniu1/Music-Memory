#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

#define LIMIT 450	/* buf max size */

/*
 * How do I organize many similar/different options like
 * add song, add artist, show songs, show albums, edit song,
 * edit album, etc. as the queries behind these commands will
 * more or less be very similar
 *
 *
 * TODO consider replacing all literal strings with an array
 * 		may not be all that good of an idea
 *
 *
 * TODO replace get_user_input() with implementation of fgets()
 * 	and call strlen() to get length of strings. The benefits 
 * 	of stored vs calculated values in this case may not 
 * 	be significant. This should remove the problem of returning
 * 	negative values from string manipulation functions as error 
 * 	codes that were intended to return the length of the string.
 *
 * 	OR 
 *
 * 	make sure that getchar() is implemented in a safe and 
 * 	efficient way. change the return types from all string 
 * 	manipulation functions to either void or char * to avoid
 * 	problem mentioned above.  
 * 	EDIT: return types int to indicate error codes
 * 
 * PROBLEM:
 * 	In the case of ensuring that dereferences doesn't occur
 *  with null pointers where there are more than 1 pointer,
 * 	should I only indicate that one of the many pointers 
 * 	contains a null value or should I be specific 
 * 
 * 
 * 
 */

int	get_user_input(const char *prompt, char *user_input, size_t user_input_limit);
int 	egetus(char *buf, size_t lim, const char *prompt);

int 	string_compare(char *str1, const char *str2);
int 	insert_string(char *str1, char *str2, size_t str1_limit, int insert_index);
void 	query_show(PGconn *connection, const char *query, PQprintOpt print_options);
int 	add_song(PGconn *connection);
int 	fill_string(char *d_str, size_t d_str_limit, const char *s_str);

void 	main_menu();

int main(void) {
	/*
	 *  PostgreSQL database connection variables
	 */
	const char 	*conninfo;
	PGconn 		*connection;

	/* 
	 * User input variables
	 */
	char 	user_input[LIMIT];
	char	*user_input_ptr;
	
	PQprintOpt print_options;
	
	user_input[0] = 0;	
	user_input_ptr = &(user_input[0]);

	
	/* Setup print options structure */
	print_options.header = 1;
	print_options.align = 1;
	print_options.fieldSep = "|";

	
	/* Connect to database */
	conninfo = "dbname = music_memory";
	connection = PQconnectdb(conninfo);

	/* Check for successful connection */
	if (PQstatus(connection) != CONNECTION_OK) {
		printf("Failed to connect to database: %s\n", PQerrorMessage(connection));
		return 1;
	} else {
		/* TODO indicate which database */
		printf("%s","Successfully connected to database\n");
	}

	/*
	 * TODO consider using switch-case structure
	 * User interaction
	 */
	do {
		main_menu();
		printf("Enter option: ");
		if (fgets(user_input_ptr, sizeof(user_input), stdin) == NULL) {
			printf("Error on user input\n");
			break;
		} else {
			if (*(user_input_ptr + strlen(user_input_ptr) - 1) == '\n')
				*(user_input_ptr + strlen(user_input_ptr) - 1) = '\0';
				
			if (string_compare(user_input_ptr, "show songs") == 0) {
				query_show(connection, "SELECT song_name FROM songs;", print_options);

			} else if (string_compare(user_input_ptr, "show artists") == 0) {
				query_show(connection, "SELECT artist_name FROM artists;", print_options);
				
			} else if (string_compare(user_input_ptr, "add song") == 0) {
				/* 
				 * Uniqueness doesn't appear to exist across artist names, 
				 * album names, and song names.
				 * Get ids for all entities then perform join query 
				 */
				add_song (connection);


			} else if (string_compare(user_input_ptr, "quit") == 0) {
				printf("%s", "program will close\n");

			} else {
				printf("%s\n", "invalid input");
			}
		}
	} while (string_compare(user_input_ptr, "quit") != 0);


	PQfinish(connection);
	return 0;
}

void main_menu() {
	printf(
		"Options:\n"
		"add song\n"
		"show artists\n"
		"show_songs\n"
		"quit\n"
	);
	
}

void exit_on_error() {
	
	
}


/*
 * Displays a prompt message to the user indicating the kind of
 * information they should input. Takes user input and stores it 
 * into a char array.
 *
 * TODO Remove extra whitespace (leading + trailing)
 *
 * 	What are all the errors that could occur with using 
 * 	getchar()?
 *
 * 	How do I absolutely ensure that the buf_size
 * 	is actually a safe value to work with?
 *
 * TODO edit return values to only indicate error codes:
 * 	0 being successfull
 * 	1 being improper buf_size value
 * 	2 null pointers passed in
 * 	
 *
 *
 *   ********** WORK IN PROGRESS ***********
 */ 
 /*
int get_user_input(const char *prompt, char *user_input, size_t user_input_limit) {
	if (user_input_limit <= 0)
		return 1;

	if (prompt == NULL || user_input == NULL)
		return 2;
		
	char 	user_char;
	size_t 	i;

	printf("%s", prompt);

	// How should I break up this long line?
	for (i = 0; i < (user_input_limit - 1) && (user_char = getchar()) != '\n' && (user_char != EOF && feof(stdin) == 0) && ferror(stdin) == 0; ++i) 
		*(user_input + i) = user_char;
	
	
	user_input[i] = '\0';

	return 0;
}
*/


/*
 * Compares two char array strings.
 * -1 is returned indicating null pointer passed in
 * 0 is returned indicating same strings.
 * 1 is returned if the two strings are not equal in length.
 * 2 is returned if the two strings are equal in length but
 * dont have the same characters.
 *
 */
int string_compare(char *str1, const char *str2) {
	size_t i, str1_len;
	
	if (str1 == NULL || str2 == NULL)
		return -1;
		

	str1_len = strlen(str1);

	/* Make sure same size */
	if (strlen(str2) != str1_len) 
		return 1;

	for (i = 0; i < str1_len; ++i) {
		/* Find not same character */
		if (*(str1 + i) != *(str2 + i))
			return 2;
	}

	return 0;
}


/*
 * questionable
 */
void query_show(PGconn *connection, const char *query, PQprintOpt print_options) {
	if (connection != NULL) {
		PGresult *query_result = NULL;
		query_result = PQexec(connection, query);
		PQprint(stdout, query_result, &print_options);
		PQclear(query_result);
	}
}


/*
 * Check existence of songs, albums, artists before storing in database
 * Want to create a string from combining the generic query command 
 * "SELECT [entity_col] FROM [entity_relation] WHERE 
 * [entity_col] = '[user_input]'" with user input.
 * 
 * PROBLEM:
 * 	User could enter partial data e.g. they may not know the 
 *  the album name so I want to keep track of unconnected
 * 	data. If a song doesn't have an album_id, or if its
 * 	album doesn't have any artist_ids then it should be
 * 	considered incomplete
 * 
 * 
 * PROBLEM:
 * 	How should songs with multiple artists be handled?
 * 		1. delimit artists by ',' symbol
 * 		2. Ask user for number of artists, take in line-by-line
 * 		it's settled, 2.
 */
int add_song(PGconn *connection) {	
	/* Master query */
	char 	main_query[LIMIT];
	char	*main_query_ptr;
	
	/* Query variables */
	char 	query[LIMIT];							
	char	*query_ptr;

	/* Query result variables */
	PGresult *query_result;

	/* User input variables */
	char	user_input[LIMIT];
	char	*user_input_ptr;
	
	PQprintOpt print_options;
	
	/* Insert user artist string into query string */
	char *query_insert_addr;
	char *query_shift_addr;
	
	size_t artist_nlength;
	
	char *comma_addr;
	char *temp_p;
	
	char *last_addr_in_query_str;
	char *last_addr_in_query_buf;
	
	int tuples;
	
	char a_id_buf[LIMIT];
	char *a_id_buf_ptr;
	
	char *endptr;
	
	/* long int variable holding user's selected artist id */
	long int artist_id;
	
	/* Artist count */
	char artist_count;
	
	/* Initialize main query buf and str */
	main_query_ptr = main_query;
	strncpy(main_query_ptr, "SELECT s.song_name, al.album_name, a.artist_name "
				"FROM songs AS s "
				"JOIN albums AS al ON s.album_id=al.album_id "
				"JOIN album_contributing_artists AS ca ON al.album_id=ca.album_id "
				"JOIN artists AS a ON ca.artist_id=a.artist_id "
				"WHERE ;", sizeof(main_query));
	
	/* Initialize query buf and str */
	query_ptr = query;
	strncpy(query_ptr, 	"SELECT * "
				"FROM artists "
				"WHERE artist_name=;", sizeof(query));
	
	/* Setup print options struct */
	print_options.header = 1;
	print_options.align = 1;
	print_options.fieldSep = "|";
	
	user_input_ptr = user_input;
	
	/* FEATURE: specify multiple artists will be put on hold for now */
	/* Get number of artists from user */
	/*
	printf("Enter number of artists: ");
	if (fgets(user_input_ptr, sizeof(user_input), stdin) == NULL) {
		return -1;
	}
	*/
	
    	/* Get artist name(s) from user */
	printf("Enter artist name: ");
	if (fgets(user_input_ptr, sizeof(user_input), stdin) == NULL) {
		printf("error: input");
		return -1;
	}
		
	/* Get rid of newline */
	if (*(user_input_ptr + (strlen(user_input_ptr) - 1)) == '\n')
		*(user_input_ptr + (strlen(user_input_ptr) - 1)) = '\0';
	
	/* 
	 * Ensure string is safe to include in sql query to psql
	 * Need to free memory with PQfreemem();
	 */
	if ((user_input_ptr = PQescapeLiteral(connection, user_input_ptr, sizeof(user_input))) == NULL) {
		printf("error: escape input");
		return -2;
	}
	
	/*
	 * Compute address to insert artist within query
	 * hardcoded at first for now
	 */
	query_insert_addr = query_ptr + strlen(query_ptr) - 1;
	query_shift_addr = NULL;
	
	artist_nlength = 0;
									
	/* comma_addr = NULL; */
	temp_p = user_input_ptr;
		
	last_addr_in_query_str = query_ptr + strlen(query_ptr);
	last_addr_in_query_buf = query_ptr + sizeof(query) - 1;
	
	/* number of rows returned from check query */
	tuples = 0;
	
	/* Shift elements in query */
	if (query_insert_addr + strlen(query_insert_addr) > last_addr_in_query_buf) {
		printf("error: unable to shift");
		return -3;
	} query_shift_addr = query_insert_addr + artist_nlength;
	
	memmove(query_shift_addr, query_insert_addr, strlen(query_insert_addr) + 1);
	/* Insert artist name into query */
	memmove(query_insert_addr, temp_p, artist_nlength);
	printf("[Artist Lookup Query]: %s\n", query_ptr);
	
	
	/* buffer used for artist id input from user as string */
	/*
	a_id_buf[0] = ' ';
	a_id_buf_ptr = &(a_id_buf[0]);
	

	/* Search for commas in user input */
	/*
	do {
		/* 
		 * compute artist name length. If no comma found,
		 * take len of user input as artist length.
		 * 
		 */
		/*
		if ((comma_addr = strchr(temp_p, ',')) != NULL)
			artist_nlength = comma_addr - temp_p;
		else
			artist_nlength = strlen(temp_p);
		
		/*
		 * Want to insert ' AND artist_name=' into query if more than 
		 * one artist has been found on each iteration
		 */
		/*
		 if (temp_p != user_input_ptr) {
			 printf("second artist\n");
			 /* Do shifting first */
			 /*
			 query_shift_addr = query_insert_addr + strlen(" AND artist_name=");
			 memmove(query_shift_addr, query_insert_addr, strlen(query_insert_addr) + 1);
			 
			 /* Now insert */
			/*
			 memmove(query_insert_addr, temp_p, artist_nlength);
			 query_insert_addr = query + strlen(query) - 1;
		 }
		
		/* 
		 * Determine if necessary to perform element shifting
		 * Case: artist name is inserted within query string
		 */
		/*
		if ((query_insert_addr >= query_ptr) && (query_insert_addr <= last_addr_in_query_str)) {
			/* Ensure element shifting doesn't cause overflow */
	/*
			if (query_insert_addr + strlen(query_insert_addr) > last_addr_in_query_buf) {
				printf("error: unable to shift");
				return -3;
			}
			/* 
			 * Element-Shifting
			 * 
			 * distance between comma_addr and temp_p
			 * will give the length of artist name
			 */
	/*
			query_shift_addr = query_insert_addr + artist_nlength;
			/* copy strlen(query_insert_addr) + 1 bytes from query_insert_addr to include '\0' */
	/*
			memmove(query_shift_addr, query_insert_addr, strlen(query_insert_addr) + 1);
			
		/* 
		 * Case: artist name is inserted after the query
		 * string. depending on how many elements after
		 * the query string will determine how many 
		 * whitespace characters will need to be written
		 * up until the insert address.
		 */
	
	/*
		} else if (query_insert_addr >=	(last_addr_in_query_str) && query_insert_addr <= last_addr_in_query_buf) {
			/*
			 * Ensure only one null char is present at the 
			 * end of the string. Pad every element leading
			 * up to insert addr with whitespace to make 
			 * sure unwanted null chars and data is not in 
			 * the buffer leading to the insert address.
			 */
	/*
			memset(last_addr_in_query_str, ' ', (query_insert_addr - last_addr_in_query_str));
			/* Ensure there is null char at end of entire string */
	/*
			*(query_insert_addr + artist_nlength) = '\0';
			
			
		/* Case: insert address falls outside the bounds of buffer. */
	/*
		} else {
			printf("error: out of bounds insert\n");
			return -4;
		}
		
		/* Insert ampersand */
	/*
		if (temp_p != user_input_ptr) {
			
		}
		
		/* Insert artist name into query */
	/*
		memmove(query_insert_addr, temp_p, artist_nlength);
		printf("[Artist Lookup Query]: %s\n", query_ptr);
		temp_p = comma_addr + 1;
	} while (comma_addr != NULL);
	
	
	/* PQclear(query_result); */
	return 0;
		
		
	/*
	 * Extract artist(s) from single line.
	 * Iterate each char in persuit for ',' char. If NULL 
	 * character is reached, break from iteration loop.
	 * Shift over characters in query string to insert 
	 * artist name in proper position. 
	 * Overflow must not happen in process of char shifting.
	 * Access to buffer is possible, perform sizeof 
	 * operation on buf to get limit.
	 * At any given position in buffer there will be some 
	 * number of characters available to write to before
	 * going out of bounds:
	 * sizeof(query) - |query_addr - insert_index| - 1
	 * Depending on whether a string is to be inserted
	 * inside or after the query, need to ensure NULL is 
	 * appended to query.
	 * 
	 * What if eui isn't terminated?
	 */
}
