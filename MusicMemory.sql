CREATE TABLE countries (
	country_id smallserial NOT NULL,
	country_name varchar(100) NOT NULL,
	CONSTRAINT countries_pk PRIMARY KEY (country_id)
);


CREATE TABLE genders (
	gender_id smallserial NOT NULL,
	gender_name varchar(75) NOT NULL,
	CONSTRAINT genders_pk PRIMARY KEY (gender_id)
);


CREATE TABLE instruments (
	instrument_id smallserial NOT NULL,
	instrument_name varchar(50) NOT NULL,
	CONSTRAINT instruments_pk PRIMARY KEY (instrument_id)
);


CREATE TABLE genres (
	genre_id smallserial NOT NULL,
	genre_name varchar(50) NOT NULL,
	CONSTRAINT genres_pk PRIMARY KEY (genre_id)
);


CREATE TABLE artist_roles (
	artist_role_id smallserial NOT NULL,
	artist_role_name varchar(50) NOT NULL,
	CONSTRAINT artist_roles_pk PRIMARY KEY (artist_role_id)
);


CREATE TABLE tags (
	tag_id bigserial NOT NULL,
	tag_name varchar(100) NOT NULL,
	CONSTRAINT tags_pk PRIMARY KEY (tag_id)
);
COMMENT ON TABLE tags IS 'These could be used to indicate how the music made someone feel or think. Basically associations that would come to mind with the song.';


CREATE TABLE music_group_types(
	group_type_id smallserial NOT NULL,
	group_type_name varchar(50) NOT NULL,
	CONSTRAINT music_group_types_pk PRIMARY KEY (group_type_id)
);


CREATE TABLE musicians(
	musician_id bigserial NOT NULL,
	musician_name varchar(75) NOT NULL,
	musician_nickname varchar(75),
	musician_birthdate date,
	country_id smallint,
	gender_id smallint,
	CONSTRAINT musicians_pk PRIMARY KEY (musician_id),
	CONSTRAINT countries_fk FOREIGN KEY (country_id) REFERENCES countries (country_id) MATCH FULL ON DELETE SET NULL ON UPDATE CASCADE,
	CONSTRAINT genders_fk FOREIGN KEY (gender_id) REFERENCES genders (gender_id) MATCH FULL ON DELETE SET NULL ON UPDATE CASCADE
);


CREATE TABLE music_agencies (
	agency_id serial NOT NULL,
	agency_name varchar(100) NOT NULL,
	country_id smallint NOT NULL,
	CONSTRAINT agencies_pk PRIMARY KEY (agency_id),
	CONSTRAINT countries_fk FOREIGN KEY (country_id) REFERENCES countries (country_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE
);
COMMENT ON TABLE music_agencies IS 'founder?';


CREATE TABLE agency_roles (
	role_id SERIAL NOT NULL,
	role_name VARCHAR(100),
	CONSTRAINT agency_roles_pk PRIMARY KEY (role_id)
);


CREATE TABLE artists(
	artist_id serial NOT NULL,
	artist_name varchar(75) NOT NULL,
	artist_debut_year smallint,
	country_id smallint,
	CONSTRAINT artists_pk PRIMARY KEY (artist_id),
	CONSTRAINT countries_fk FOREIGN KEY (country_id) REFERENCES countries (country_id) MATCH FULL ON DELETE SET NULL ON UPDATE CASCADE
);
COMMENT ON TABLE artists IS 'artist names may not be unique in real world, so surrogate key is used. plus artist names may change.';


CREATE TABLE artist_group_types(
	artist_id integer NOT NULL,
	group_type_id smallint NOT NULL,
	CONSTRAINT artist_group_types_pk PRIMARY KEY (artist_id, group_type_id),
	CONSTRAINT artists_fk FOREIGN KEY (artist_id) REFERENCES artists (artist_id) MATCH FULL ON DELETE SET NULL ON UPDATE CASCADE,
	CONSTRAINT music_group_types_fk FOREIGN KEY (group_type_id) REFERENCES music_group_types (group_type_id) MATCH FULL ON DELETE SET NULL ON UPDATE CASCADE
);


CREATE TABLE artist_members(
	artist_id integer NOT NULL,
	musician_id bigint NOT NULL,
	join_date date,
	depart_date date,
	CONSTRAINT artist_members_pk PRIMARY KEY (artist_id,musician_id),
	CONSTRAINT artists_fk FOREIGN KEY (artist_id) REFERENCES artists (artist_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE,
	CONSTRAINT musicians_fk FOREIGN KEY (musician_id) REFERENCES musicians (musician_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE
);


CREATE TABLE instrumentalists_instruments(
	instrument_id smallint NOT NULL,
	artist_id integer NOT NULL,
	musician_id bigint NOT NULL,
	CONSTRAINT instrumentalists_instruments_pk PRIMARY KEY (instrument_id,artist_id,musician_id),
	CONSTRAINT instruments_fk FOREIGN KEY (instrument_id) REFERENCES instruments (instrument_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE,
	CONSTRAINT artist_members_fk FOREIGN KEY (artist_id,musician_id) REFERENCES artist_members (artist_id,musician_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE
);


CREATE TABLE artist_principal_genres(
	genre_id smallint NOT NULL,
	artist_id integer NOT NULL,
	CONSTRAINT artist_principal_genres_pk PRIMARY KEY (genre_id,artist_id),
	CONSTRAINT genres_fk FOREIGN KEY (genre_id) REFERENCES genres (genre_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE,
	CONSTRAINT artists_fk FOREIGN KEY (artist_id) REFERENCES artists (artist_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE
);


CREATE TABLE album_types (
	album_type_id SERIAL NOT NULL,
	album_type_name VARCHAR(100) NOT NULL,
	CONSTRAINT album_types_pk PRIMARY KEY (album_type_id)
);


CREATE TABLE albums(
	album_id bigserial NOT NULL,
	album_name varchar(75) NOT NULL,
	album_type integer,
	album_release_date date,
	album_artwork_path varchar(255),
	CONSTRAINT albums_pk PRIMARY KEY (album_id),
	CONSTRAINT album_types_fk FOREIGN KEY (album_type) REFERENCES album_types (album_type_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE
);
COMMENT ON TABLE albums IS 'Collection of songs. Special kinds of albums such as singles, EPs, etc. will be indicated by album_type.
Surrogate key will be used as pk because unsure of uniqueness of album names.';
COMMENT ON COLUMN albums.album_type IS 'EP, Single, etc. THIS SHOULD BE A REFERENCE INSTEAD';

-- Consider maintaining naming consistency --
CREATE TABLE album_music_agencies(
	album_id bigint NOT NULL,
	music_agency_id bigint NOT NULL,
	agency_role_id bigint,
	CONSTRAINT album_music_agencies_pk PRIMARY KEY (album_id, music_agency_id),
	CONSTRAINT albums_fk FOREIGN KEY (album_id) REFERENCES albums (album_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE,
	CONSTRAINT music_agencies_fk FOREIGN KEY (music_agency_id) REFERENCES music_agencies (agency_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE,
	CONSTRAINT agency_roles_fk FOREIGN KEY (agency_role_id) REFERENCES agency_roles (role_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE
);


CREATE TABLE song_formats (
	song_format_id SERIAL NOT NULL,
	song_format_name VARCHAR(100) NOT NULL,
	CONSTRAINT song_formats_pk PRIMARY KEY (song_format_id)
);


CREATE TABLE songs(
	song_id bigserial NOT NULL,
	song_name varchar(75) NOT NULL,
	song_track_no smallint,
	song_duration interval HOUR TO SECOND,
	song_recording_date date,
	song_format_id integer,
	album_id bigint,
	CONSTRAINT songs_pk PRIMARY KEY (song_id),
	CONSTRAINT albums_fk FOREIGN KEY (album_id) REFERENCES albums (album_id) MATCH FULL ON DELETE SET NULL ON UPDATE CASCADE,
	CONSTRAINT song_formats_fk FOREIGN KEY (song_format_id) REFERENCES song_formats (song_format_id) MATCH FULL ON DELETE SET NULL ON UPDATE CASCADE
);
COMMENT ON TABLE songs IS 'Can a song belong to multiple albums?';


CREATE TABLE song_arrangers(
	musician_id bigint NOT NULL,
	song_id bigint NOT NULL,
	CONSTRAINT song_arrangers_pk PRIMARY KEY (song_id,musician_id),
	CONSTRAINT songs_fk FOREIGN KEY (song_id) REFERENCES songs (song_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE,
	CONSTRAINT musicians_fk FOREIGN KEY (musician_id) REFERENCES musicians (musician_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE
);


CREATE TABLE song_composers(
	musician_id bigint NOT NULL,
	song_id bigint NOT NULL,
	CONSTRAINT song_composers_pk PRIMARY KEY (song_id,musician_id),
	CONSTRAINT songs_fk FOREIGN KEY (song_id) REFERENCES songs (song_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE,
	CONSTRAINT musicians_fk FOREIGN KEY (musician_id) REFERENCES musicians (musician_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE
);


CREATE TABLE song_lyricists(
	musician_id bigint NOT NULL,
	song_id bigint NOT NULL,
	CONSTRAINT song_lyricists_pk PRIMARY KEY (song_id,musician_id),
	CONSTRAINT songs_fk FOREIGN KEY (song_id) REFERENCES songs (song_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE,
	CONSTRAINT musicians_fk FOREIGN KEY (musician_id) REFERENCES musicians (musician_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE
);


CREATE TABLE song_producers(
	musician_id bigint NOT NULL,
	song_id bigint NOT NULL,
	CONSTRAINT song_producers_pk PRIMARY KEY (song_id,musician_id),
	CONSTRAINT songs_fk FOREIGN KEY (song_id) REFERENCES songs (song_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE,
	CONSTRAINT musicians_fk FOREIGN KEY (musician_id) REFERENCES musicians (musician_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE
);


CREATE TABLE song_contributing_artists(
	artist_id integer NOT NULL,
	song_id bigint NOT NULL,
	CONSTRAINT song_contributing_artists_pk PRIMARY KEY (song_id,artist_id),
	CONSTRAINT artists_fk FOREIGN KEY (artist_id) REFERENCES artists (artist_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE,
	CONSTRAINT songs_fk FOREIGN KEY (song_id) REFERENCES songs (song_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE
);


CREATE TABLE album_genres(
	genre_id smallint NOT NULL,
	album_id bigint NOT NULL,
	CONSTRAINT album_genres_pk PRIMARY KEY (genre_id,album_id),
	CONSTRAINT genres_fk FOREIGN KEY (genre_id) REFERENCES genres (genre_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE,
	CONSTRAINT albums_fk FOREIGN KEY (album_id) REFERENCES albums (album_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE
);


CREATE TABLE song_tags(
	tag_id bigint NOT NULL,
	song_id bigint NOT NULL,
	CONSTRAINT song_tags_pk PRIMARY KEY (tag_id,song_id),
	CONSTRAINT tags_fk FOREIGN KEY (tag_id) REFERENCES tags (tag_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE,
	CONSTRAINT songs_fk FOREIGN KEY (song_id) REFERENCES songs (song_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE
);


CREATE TABLE song_genres(
	genre_id smallint NOT NULL,
	song_id bigint NOT NULL,
	CONSTRAINT song_genres_pk PRIMARY KEY (song_id,genre_id),
	CONSTRAINT genres_fk FOREIGN KEY (genre_id) REFERENCES genres (genre_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE,
	CONSTRAINT songs_fk FOREIGN KEY (song_id) REFERENCES songs (song_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE
);


CREATE TABLE users(
	user_id bigserial NOT NULL,
	username varchar(100) NOT NULL,
	user_email varchar(150),
	user_join_date timestamp with time zone,
	user_password varchar(100),
	CONSTRAINT users_pk PRIMARY KEY (user_id)
);
COMMENT ON COLUMN users.user_email IS 'should this be encrypted?';
COMMENT ON COLUMN users.user_password IS 'encryption needed';


CREATE TABLE user_song_listens(
	song_id bigint NOT NULL,
	user_id bigint NOT NULL,
	user_listen_count smallint,
	user_first_listen_date date,
	user_song_rating numeric(1),
	user_like_song boolean,
	CONSTRAINT user_song_listens_pk PRIMARY KEY (user_id,song_id),
	CONSTRAINT songs_fk FOREIGN KEY (song_id) REFERENCES songs (song_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE,
	CONSTRAINT users_fk FOREIGN KEY (user_id) REFERENCES users (user_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE
);


CREATE TABLE artist_member_roles(
	artist_role_id smallint NOT NULL,
	artist_id integer NOT NULL,
	musician_id bigint NOT NULL,
	CONSTRAINT artist_member_roles_pk PRIMARY KEY (artist_role_id,artist_id,musician_id),
	CONSTRAINT artist_roles_fk FOREIGN KEY (artist_role_id) REFERENCES artist_roles (artist_role_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE,
	CONSTRAINT artist_members_fk FOREIGN KEY (artist_id,musician_id) REFERENCES artist_members (artist_id,musician_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE
);


CREATE TABLE user_playlists(
	playlist_id bigserial NOT NULL,
	playlist_name varchar(100) NOT NULL,
	playlist_creation_date timestamp,
	playlist_description varchar(500),
	user_id bigint NOT NULL,
	CONSTRAINT user_playlists_pk PRIMARY KEY (playlist_id),
	CONSTRAINT users_fk FOREIGN KEY (user_id) REFERENCES users (user_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE
);

COMMENT ON TABLE user_playlists IS 'playlist name subject to change so surragate key used';


CREATE TABLE user_playlist_songs(
	user_playlist_id bigint NOT NULL,
	song_id bigint NOT NULL,
	CONSTRAINT user_playlist_songs_pk PRIMARY KEY (user_playlist_id,song_id),
	CONSTRAINT user_playlists_fk FOREIGN KEY (user_playlist_id) REFERENCES user_playlists (playlist_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE,
	CONSTRAINT songs_fk FOREIGN KEY (song_id) REFERENCES songs (song_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE
);


CREATE TABLE artist_aliases(
	primary_artist_id integer NOT NULL,
	alias_artist_id integer NOT NULL,
	CONSTRAINT artist_aliases_pk PRIMARY KEY (primary_artist_id,alias_artist_id),
	CONSTRAINT artist_fk FOREIGN KEY (primary_artist_id) REFERENCES artists (artist_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE,
	CONSTRAINT alias_fk FOREIGN KEY (alias_artist_id) REFERENCES artists (artist_id) MATCH FULL ON DELETE RESTRICT ON UPDATE CASCADE,
	CONSTRAINT unique_aliases_constraint UNIQUE (alias_artist_id)
);
COMMENT ON TABLE artist_aliases IS 'an artist can have many aliases (who are also stored as separate artists), but an alias may not be shared by many artists. ';
