#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "libpq-fe.h"
#include <limits.h>
#include "keyin.h"
#include <stdarg.h>


/* Defines */
#define DBNAME			"music_memory"


#define LIMIT 			450	/* buf max size */
#define ENTITY_COUNT 	3	/* Number of entities (artist, album, song) to be used in queries */
#define USER_EXIT		6


/* Relation Constants */
/* Supporting Elementary Relations */
#define ARTIST_ROLES         "artist_roles"
#define GENDERS              "genders"
#define COUNTRIES            "countries"
#define MUISC_GROUP_TYPES    "music_group_types"
#define INSTRUMENTS          "instruments"
#define GENRES               "genres"
#define SONG_FORMATS         "song_formats"

/* Primary Relations */
#define ARTISTS              "artists"
#define ALBUMS               "albums"
#define SONGS                "songs"

/* Musician Involvement Relations */
#define MUSICIANS            "musicians"
#define SONG_PRODUCERS       "song_producers"
#define SONG_COMPOSERS       "song_composers"
#define SONG_LYRICISTS       "song_lyricists"
#define SONG_ARRANGERS       "song_arrangers"

/* Users Information Relations */
#define USERS                "users"
#define USER_SONG_LISTENS    "user_song_listens"
#define USER_PLAYLISTS       "user_playlists"
#define USER_PLAYLIST_SONGS  "user_playlist_songs"

/* Tag Relations */
#define TAGS                 "tags"
#define SONG_TAGS            "song_tags"



/* How general can we get? */
typedef enum commands_t {
	ADD_ENTITY,
	SEARCH_ENTITY,
	EDIT_ENTITY,
	VIEW_ENTITY
	
} commands;


	
/*
 * How do I organize many similar but different options like
 * add song, add artist, show songs, show albums, edit song,
 * edit album, etc. as the queries behind these commands will
 * more or less be very similar
 *
 *
 * When user enters more characters than expected there are 
 * still left in the buffer so subsequent calls will consume
 * the remaining data. 
 * 	-> How can I detect over limit and discard the rest?
 * 
 * 
 * 
 */
 
 /* Utility functions */
int 	string_compare(char *str1, const char *str2);
char 	*insert_string(char *dest, const char *src, size_t dest_size, int index);

/* Core functions */
void 	query_show(PGconn *connection, const char *query, PQprintOpt *print_options);
int 	add_song(PGconn *connection);
int		search(PGconn *connection, PQprintOpt *print_options);
int		edit_entity(PGconn *connection);

void 	main_menu(void);

int main(void) {
	/* History array */
	static 
	
	/* PostgreSQL database connection variables */
	const char 	*conninfo;
	PGconn 		*connection;

	/* User input variables */
	char 		user_input[LIMIT];
	long int 	user_option;
	
	/* Number validation */
	char		*end_ptr;
	
	/* Should this be created only when the user wants to 
	 * query#FFFFFF? */
	PQprintOpt 	print_options;
	
	/* Setup print options structure */
	print_options.header 	= 1;
	print_options.align 	= 1;
	print_options.fieldSep 	= "|";

	
	/* Connect to database */
	conninfo = "dbname = " DBNAME;
	connection = PQconnectdb(conninfo);

	/* Check for successful connection */
	if (PQstatus(connection) != CONNECTION_OK) {
		printf("Failed to connect to database [%s]: %s\n", DBNAME, PQerrorMessage(connection));
		return 1;
	} else {
		printf("Successfully connected to database [%s]\n", DBNAME);
	}

	do {
		main_menu();
		
		printf("Select Option: ");
		if (getstr(user_input, sizeof(user_input)) == NULL)
			break;
		
		user_option = strtol(user_input, &end_ptr, 10);
		if (user_option == LONG_MAX || 
			user_option == LONG_MIN) {
				perror("Error: ");
				continue;
		}
		
		/* Check if user entered number */
		if (*end_ptr == '\0' && user_input[0] != '\0') {
			switch (user_option) {
				case 1:
					add_song(connection);
					break;
					
				case 2:
					query_show(connection, "SELECT artist_name FROM artists;", &print_options);
					break;
					
				case 3:
					query_show(connection, "SELECT album_name FROM albums;", &print_options);
					break;
					
				case 4:
					query_show(connection, "SELECT song_name FROM songs;", &print_options);
					break;
					
				case 5:
					search(connection, &print_options);
					break;
					
				case USER_EXIT:
					printf("program will close\n");
					break;
					
				default:
					printf("invalid input\n");
					
			}	
		} else {
			printf("Please enter a number\n");
		}
	} while (user_option != USER_EXIT);


	PQfinish(connection);
	return 0;
}


void main_menu(void) {
	printf(
		"Options:\n"
		"1. Add a Song\n"
		"2. Show all Artists\n"
		"3. Show all Albums\n"
		"4. Show all Songs\n"
		"5. Search\n"
		"6. Quit\n"
	);
	
}


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


int	edit_entity(PGconn *connection) {
	
	return 0;
}


/* perhaps separate entity display functionality from this 
 * function */
void query_show(PGconn *connection, const char *query, PQprintOpt *print_options) {
	PGresult *query_result;
	
	if (connection == NULL) {
		fprintf(stderr, "Connection is null\n");
		return;
	}
	
	query_result = PQexec(connection, query);
	PQprint(stdout, query_result, print_options);
	PQclear(query_result);
}



int search(PGconn *connection, PQprintOpt *print_options) {
	/* Hold main entities in string format like 'artist', 'album', 'song' */
	const char *entities[ENTITY_COUNT];
	/* User string to search for */
	char search_str[LIMIT];
	char *escaped_str;
	char *end_ptr;
	
	/* Query string to send to dbms */
	char entity_query[LIMIT];
	
	PGresult *query_results[ENTITY_COUNT];
	int result_count;
	
	long int user_option;
	
	size_t i, j, k;
	
	int return_code;
	
	return_code = 0;
	result_count = 0;
	
	entities[0] = "artist";
	entities[1] = "album";
	entities[2] = "song";
	
	printf("Enter entity name: ");
	if (getstr(search_str, sizeof(search_str)) == NULL)
		return -1;

	if ((escaped_str = PQescapeLiteral(connection, (char *) search_str, LIMIT)) == NULL) {
		perror("error: ");
		return -2;
	}
	
	j = 1;
	for (i = 0; i < ENTITY_COUNT; i++) {
		strncpy(entity_query, 	"SELECT _id, _name "
								"FROM s "
								"WHERE _name ILIKE ;", sizeof(entity_query));								
		insert_string(entity_query, entities[i], sizeof(entity_query), 7);
		insert_string(entity_query, entities[i], sizeof(entity_query), strlen(entity_query) - 32);
		insert_string(entity_query, entities[i], sizeof(entity_query), strlen(entity_query) - 21);
		insert_string(entity_query, entities[i], sizeof(entity_query), strlen(entity_query) - 13);
		insert_string(entity_query, escaped_str, sizeof(entity_query), strlen(entity_query) - 1);

		
		query_results[i] = PQexec(connection, entity_query);
		result_count += PQntuples(query_results[i]);
		
		if (PQntuples(query_results[i]) != 0) {
			printf("%s:\n", entities[i]);
			for (k = 0; j <= (size_t) result_count; j++, k++) {
				printf("%4lu: %s\n", j, PQgetvalue(query_results[i], k, 1));
			}
			
			if (i < ENTITY_COUNT - 1) 
				printf("---------------------------\n");
		}
	}
	
	PQfreemem(escaped_str);
	
	if (result_count <= 0) {
		printf("No results for search term: %s\n", search_str);
		return_code = 1;
		goto Clear_Resources;
	} else {
		printf("Select item: ");
		if (getstr(search_str, sizeof(search_str)) == NULL) {
			return_code = -1;
			goto Clear_Resources;
		}
		
		if ((user_option = strtol(search_str, &end_ptr, 10)) == LONG_MAX ||
			user_option == LONG_MIN) {
			perror("Error: ");
			return_code = 2;
			goto Clear_Resources;
		}
		
		if (*end_ptr != '\0' || search_str[0] == '\0') {
			printf("Input is not a proper number\n");
			return_code = 2;
			goto Clear_Resources;
		}
		
		result_count = 0;
		for (i = 0; i < ENTITY_COUNT; i++) {
			if (user_option > PQntuples(query_results[i])) {
				user_option -= PQntuples(query_results[i]);
			} else {
				strncpy(entity_query, 	"SELECT * "
										"FROM s "
										"WHERE _id=;", sizeof(entity_query));
				insert_string(entity_query, entities[i], sizeof(entity_query), 14);
				insert_string(entity_query, entities[i], sizeof(entity_query), strlen(entity_query) - 5);
				insert_string(entity_query, PQgetvalue(query_results[i], user_option - 1, 0), sizeof(entity_query), strlen(entity_query) - 1);
				query_show(connection, entity_query, print_options);
				break;
			}
		}
	}
	
	Clear_Resources:
		for (i = 0; i < ENTITY_COUNT; i++) {
			PQclear(query_results[i]);
		}
		
	return return_code;
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
 * 
 * 
 */
int add_song(PGconn *connection) {	
	/* Artist -> Album -> Song */
	const char	*entities[ENTITY_COUNT];
	short int	insert_indexes[ENTITY_COUNT];
	
	char		entity_values[ENTITY_COUNT][LIMIT];
	char		entity_ID[ENTITY_COUNT][LIMIT];
	
	/* Master query */
	char 		query[LIMIT];

	/* Query result variables */
	PGresult 	*query_result;

	int tuples;
	
	/* May be used for numeric input validation */
	/*char *endptr;*/
	
	/* Artist count */
	/*char artist_count;*/
	
	/* loop control variable */
	size_t i;
	
	/* PQ escaped string */
	char *escaped_str;
	
	/* Initialize entity array */
	entities[0] = "artist";
	entities[1] = "album";
	entities[2] = "song";
	
	
	/* Initialize main query buf and str */
	strncpy(query, 	"SELECT a.artist_id, al.album_id, s.song_id "
					"FROM songs AS s "
					"JOIN song_contributing_artists AS ca ON ca.song_id=s.song_id "
					"JOIN artists AS a ON ca.artist_id=a.artist_id "
					"JOIN albums AS al ON s.album_id=al.album_id "
					"WHERE a.artist_name= AND al.album_name= AND s.song_name=;", sizeof(query));
	
	insert_indexes[0] = 37;
	insert_indexes[1] = 18;
	insert_indexes[2] = 1;
	
	/* Get artist, album, and song from user and insert into
	 * item existence check query */
	for (i = 0; i < ENTITY_COUNT; i++) {
		printf("Enter %s name: ", entities[i]);
		/* How to tell if there is still data in the input
		 * buffer? i.e. did the user try to enter more data
		 * than LIMIT */
		if (getstr((char *) entity_values[i], LIMIT) == NULL) {
			return -1;
		}

			
		/* 
		 * Ensure string is safe to include in sql query to psql
		 * Need to free memory with PQfreemem();
		 */
		if ((escaped_str = PQescapeLiteral(connection, (char *) entity_values[i], LIMIT)) == NULL) {
			perror("error: ");
			return -2;
		}
		
		strncpy((char *) entity_values[i], escaped_str, LIMIT);
		insert_string(query, escaped_str, sizeof(query), strlen(query) - insert_indexes[i]);
		
		PQfreemem(escaped_str);
	}
	
	
	/*printf("Final Query: [%s]\n", query);*/
	
	query_result = PQexec(connection, query);
	tuples = PQntuples(query_result);
	PQclear(query_result);
	
	/* Insert item into database if it doesn't already exist */
	if (tuples == 0) {
		query_result = PQexec(connection, "BEGIN;");
		PQclear(query_result);
		/* Form basic insert queries */
		for (i = 0; i < ENTITY_COUNT; i++) {
			strncpy(query, 	"INSERT INTO s (_name) "
							"VALUES () "
							"RETURNING _id;", sizeof(query));
			/* Insert relation name */						
			insert_string(query, entities[i], sizeof(query), 12);
			/* Insert column name */
			insert_string(query, entities[i], sizeof(query), strlen(query) - 31);
			/* If song insert album_id */
			if (i == 2) {
				insert_string(query, ", album_id", sizeof(query), strlen(query) - 26);
			}			
			/* Insert value */
			insert_string(query, entity_values[i], sizeof(query), strlen(query) - 16);
			/* If song insert album_id value */
			if (i == 2) {
				insert_string(query, ",", sizeof(query), strlen(query) - 16);
				insert_string(query, entity_ID[1], sizeof(query), strlen(query) - 16);
			}
			/* Insert column name */
			insert_string(query, entities[i], sizeof(query), strlen(query) - 4);
			/*printf("Insert Query: [%s]\n", query);*/
			
			
			
			query_result = PQexec(connection, query);
			strncpy((char *)entity_ID[i], PQgetvalue(query_result, 0, 0), sizeof(query));
			/*printf("Returned ID: [%s]\n", entity_ID[i]);*/
			PQclear(query_result);
		}
		
		/* Form insert queries on the associative relations */
		strncpy(query, 	"INSERT INTO song_contributing_artists (artist_id, song_id) "
						"VALUES (,);", sizeof(query));
		insert_string(query, entity_ID[0], sizeof(query), strlen(query) - 3);
		insert_string(query, entity_ID[2], sizeof(query), strlen(query) - 2);			
		/*printf("Associative Insert Query: [%s]\n", query);*/
		
		query_result = PQexec(connection, query);
		PQclear(query_result);
		
		query_result = PQexec(connection, "END;");
		PQclear(query_result);
	} else {
		printf("Item already exists\n");
	}
	return 0;
}


#define FORMAT_SPECIFIER_START '%'

/*
 * %s
 * %d
 * %ld
 * %f
 * %c
 * %u
 * %hi
 * %hu
 * %Lf
 * 
 * for now let's just start with simple single specifiers:
 * %s, %d, %f, %c, 
 * 
 */
char *string_insert(char *dest, size_t dest_size, const char *format, ...) {
	va_list args;
	char *p_char;
	char *format_str_bound;
	size_t format_len;
	
	
	
	p_char = NULL;
	format_len = strlen(format);
	
	/* Sanity check */
	if (format_len > dest_size) {
		return NULL;
	}
	
	va_start(args, format);
	
	do {
		p_char = index(format, FORMAT_SPECIFIER_START);
		
		if (p_char == NULL) {
			break;
		}
		
		strncpy(dest, format, (p_char - format));
		p_char++;
		
		switch (*p_char) {
			case 's':
				
				break;
			
			case 'd':
			
				break;
				
			case 'f':
			
				break;
				
			case 'c':
			
				break;
				
			case '%':
			
				break;
				
			default:
			
				break;
			
			
		}
	} while (p_char != NULL);
	
	/* Need to record the size of the argument:
	 * char*:
	 * 	use strlen() with sizeof char
	 * int, float, double, long:
	 * 	Don't use sizeof - ultimately the number will be 
	 * 	represented as a string in the buffer. So it's 
	 * 	better to use sizeof on char and count the number of
	 * 	digits in the number.
	 * char:
	 * 	use sizeof - single char variable should resolve 
	 * 	to a single character unlike an integer where each
	 * 	digit in the number will have to be represented by
	 * 	a character. */
	vsprintf(dest, format, args);
		
	
	return NULL;
}

/* TODO: follow formatted string pattern:
 * 			introduce format specifiers that can indicate
 * 			where the string should be inserted. */
char *insert_string(char *dest, const char *src, size_t dest_size, int index) {
	char *insert_addr;
	char *shift_addr;
	char *dest_str_bound;
	char *dest_buf_bound;
	size_t src_len;
	
	
	src_len = strlen(src);
	/* Inclusive upper bounds for string and buffer */
	dest_str_bound = dest + strlen(dest) - 1;
	dest_buf_bound = dest + dest_size - 1;
	
	if (dest_str_bound >= dest_buf_bound) {
		printf("Initial overflow\n");
		return NULL;
	}
	
	insert_addr = dest + index;
	
	/* The source string will be inserted within the 
	 * destination string and element shifting will be 
	 * required. */
	if (insert_addr >= dest && insert_addr <= dest_str_bound) {
		if ((dest_str_bound + src_len + 1) > dest_buf_bound) {
			printf("Insert overflow\n");
			return NULL;
		}
		shift_addr = insert_addr + src_len;
		memmove(shift_addr, insert_addr, strlen(insert_addr) + 1);
	} 
	/* The source string will be inserted after the 
	 * destination string and whitespace padding will 
	 * be required between source and destination string. */
	else if (insert_addr > dest_str_bound && insert_addr <= dest_buf_bound) {
		if (insert_addr + src_len > dest_buf_bound) {
			printf("Insert overflow\n");
			return NULL;
		}
		
		memset(dest_str_bound + 1, ' ', insert_addr - (dest_str_bound + 1));
		/* Ensure there is null char at end of entire string */
		*(insert_addr + src_len) = '\0';
	} else {
		printf("Index out of bounds\n");
		return NULL;
	}
	
	/* Finally insert src into the destination string 
	 * offsetted by index */
	memmove(insert_addr, src, src_len);
	return dest;
}
