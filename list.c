#include <string.h>
#include <libspotify/api.h>
#include "listify.h"
#include "cmd.h"
#include "link.h"
#include "list.h"

/* --- Data --- */
sp_playlistcontainer *g_pc;



/* --------------------------  PLAYLIST CALLBACKS  ------------------------- */
/**
 * Callback from libspotify, saying that a track has been added to a playlist.
 *
 * @param  pl          The playlist handle
 * @param  tracks      An array of track handles
 * @param  num_tracks  The number of tracks in the \c tracks array
 * @param  position    Where the tracks were inserted
 * @param  userdata    The opaque pointer
 */
static void tracks_added(sp_playlist *pl, sp_track * const *tracks,
                         int num_tracks, int position, void *userdata)
{
	printf("listify: %d tracks were added\n", num_tracks);
	fflush(stdout);
	cmd_done();
}

/**
 * Callback from libspotify, saying that a track has been added to a playlist.
 *
 * @param  pl          The playlist handle
 * @param  tracks      An array of track indices
 * @param  num_tracks  The number of tracks in the \c tracks array
 * @param  userdata    The opaque pointer
 */
static void tracks_removed(sp_playlist *pl, const int *tracks,
                           int num_tracks, void *userdata)
{
	printf("jukebox: %d tracks were removed\n", num_tracks);
	fflush(stdout);
	cmd_done();
}

/**
 * Callback from libspotify, telling when tracks have been moved around in a playlist.
 *
 * @param  pl            The playlist handle
 * @param  tracks        An array of track indices
 * @param  num_tracks    The number of tracks in the \c tracks array
 * @param  new_position  To where the tracks were moved
 * @param  userdata      The opaque pointer
 */
static void tracks_moved(sp_playlist *pl, const int *tracks,
                         int num_tracks, int new_position, void *userdata)
{
	const char *name = sp_playlist_name(pl);
	printf("jukebox: %d tracks were moved around, in playlist %s\n", num_tracks, name);
	fflush(stdout);
	cmd_done();
}

/**
 * Callback from libspotify. Something renamed the playlist.
 *
 * @param  pl            The playlist handle
 * @param  userdata      The opaque pointer
 */
static void playlist_renamed(sp_playlist *pl, void *userdata)
{
	const char *name = sp_playlist_name(pl);
	printf("jukebox: some playlist renamed to \"%s\".\n", name);
	fflush(stdout);
	cmd_done();
	
}

/**
 * The callbacks we are interested in for individual playlists.
 */
static sp_playlist_callbacks pl_callbacks = {
	.tracks_added = &tracks_added,
	.tracks_removed = &tracks_removed,
	.tracks_moved = &tracks_moved,
	.playlist_renamed = &playlist_renamed,
};


/* --------------------  PLAYLIST CONTAINER CALLBACKS  --------------------- */
/**
 * Callback from libspotify, telling us a playlist was added to the playlist container.
 *
 * We add our playlist callbacks to the newly added playlist.
 *
 * @param  pc            The playlist container handle
 * @param  pl            The playlist handle
 * @param  position      Index of the added playlist
 * @param  userdata      The opaque pointer
 */
static void playlist_added(sp_playlistcontainer *pc, sp_playlist *pl,
                           int position, void *userdata)
{
	const char *name = sp_playlist_name(pl);	
	printf("playlist with name %s was added\n", name);
	fflush(stdout);
	sp_playlist_add_callbacks(pl, &pl_callbacks, NULL);
	cmd_done();
}

/**
 * Callback from libspotify, telling us a playlist was removed from the playlist container.
 *
 * This is the place to remove our playlist callbacks.
 *
 * @param  pc            The playlist container handle
 * @param  pl            The playlist handle
 * @param  position      Index of the removed playlist
 * @param  userdata      The opaque pointer
 */
static void playlist_removed(sp_playlistcontainer *pc, sp_playlist *pl,
                             int position, void *userdata)
{
	
	printf("playlist_removed() was called\n");
	fflush(stdout);
	cmd_done();
	/*sp_playlist_remove_callbacks(pl, &pl_callbacks, NULL);
	 * */
}


/**
 * Callback from libspotify, telling us the rootlist is fully synchronized
 * We just print an informational message
 *
 * @param  pc            The playlist container handle
 * @param  userdata      The opaque pointer
 */
static void container_loaded(sp_playlistcontainer *pc, void *userdata)
{
	g_pc = pc;
	printf("container_loaded() was called\n");
	fflush(stdout);
	cmd_done();
	/*
	fprintf(stderr, "jukebox: Rootlist synchronized\n");
	* */
}


/**
 * The playlist container callbacks
 */
sp_playlistcontainer_callbacks pc_callbacks = {
	.playlist_added = &playlist_added,
	.playlist_removed = &playlist_removed,
	.container_loaded = &container_loaded,
};


/* -------------------------  END  PLAYLIST CONTAINER CALLBACKS ------------ */

/* ---------------------------  SESSION CALLBACKS  ------------------------- */

/*static const char* get_link_type_label(sp_link *link)
{
    static const char *LINK_TYPES[] = {
        "invalid",
        "track",
        "album",
        "artist",
        "search",
        "playlist"
    };

    return LINK_TYPES[sp_link_type(link)];
}

 * 
 * The session callbacks aren't the real callbacks. But are invoked
 * by the respective real callback-functions. So for example, at each
 * login, the real callback-function logged_in() calls logged_in_playlist().
 * 
 * Another property is that the error parameter is removed, since the
 * error should already have been handled by the real call-back functions.
 * 
 * 
 * */
  
void logged_in_playlist(sp_session* session){
	sp_playlistcontainer *pc = sp_session_playlistcontainer(g_session);
	int i;

	printf("jukebox: Looking at %d playlists\n", sp_playlistcontainer_num_playlists(pc));

	for (i = 0; i < sp_playlistcontainer_num_playlists(pc); ++i) {
		sp_playlist *pl = sp_playlistcontainer_playlist(pc, i);
		
		sp_playlist_add_callbacks(pl, &pl_callbacks, NULL);

		printf("Following playlist was added: %s\n", sp_playlist_name(pl));

	}

	cmd_done();
}

/* -------------------------  END SESSION CALLBACKS  ----------------------- */


/* -------------------------  IMPLEMENTED COMMANDS  ------------------------ */

/*
 * 
 * The Implemented commands are those non-callback functions that the
 * user can call from the Programs menu.
 * 
 * */


/**
 * Create a new playlist.
 * 
 * @param string containing the name of the playlist.
 * @return -1.
 * 
 */
int cmd_new_playlist(int argc, char **argv){	
	if(argc != 2){		
		fprintf(stderr, "Usage: %s <name>\n", argv[0]);
		return -1;
	}
	char *URI = new_playlist(argv[1]);
	if(!URI){
		fprintf(stderr, "Failed in creating a new playlist");
	}
	
	printf("The new playlist has the URI\n%s\n", URI);
	free(URI);
	return -1;
}



int cmd_new_hide(int argc, char **argv){
	if(argc != 2){		
		fprintf(stderr, "Usage: %s <name>\n", argv[0]);
		return -1;
	}
	char *URI = new_playlist(argv[1]);
	if(!URI){
		fprintf(stderr, "Failed in creating a new playlist");
	}
	
	printf("The new playlist has the URI\n%s\n", URI);
	hide_playlist(URI); // In this verison we also hide the playlist
	free(URI);
	return -1;
	
}


/**
 * Add an existing playlist to our container. 
 * 
 * @param string containing the name of the playlist.
 * @return -1.
 * 
 */
int cmd_add_playlist(int argc, char **argv){	
	if(argc != 2){
		fprintf(stderr, "Usage: %s <URI>\n", argv[0]);
		return -1;
	}	
	sp_link *link = URI_to_link(argv[1]);
	if(!link){
		fprintf(stderr, "URI couldn't be translated, can't add it to the playlist.\n");
		return -1;		
	}
	sp_playlist *pl = sp_playlistcontainer_add_playlist(g_pc, link);
	if(!pl){
		fprintf(stderr, "Couldn't add the link to the container, is it already in the container?\n");
		return -1;
	}
	return -1;
}


/**
 * Clear a playlist
 * 
 * @param 1
 * The first token should be the full URI of the playlist, like:
 * spotify:user:JohnSmith:playlist:68sMl8CBblj6uBcqbJsnoj
 * 
 * @return -1.
 */
int cmd_clear_playlist(int argc, char **argv){
	if(argc != 2){
		fprintf(stderr, "Usage: %s <URI>\n", argv[0]);
		return -1;
	}	
	sp_playlist *pl = URI_to_playlist(argv[1]);
	if(!pl){		
		fprintf(stderr, "The given URI couldn't be converted to a playlist\n");
        return -1; // URI -> playlist failed	
	}
	int n = sp_playlist_num_tracks(pl);
	if(n > 0){
		// for some reason it seems like something crashes when n = 0
		int array[n];
		int i;
		for(i = 0; i < n; i++){
			array[i] = i;
		}
		sp_error err = sp_playlist_remove_tracks(pl, array, n); // remove all tracks in it.
		if(err != SP_ERROR_OK){
			fprintf(stderr, "Error '%s' when trying to delete tracks of the playlist.\n", sp_error_message(err));
			return -1;
		}
	}
	return -1;	
}


/**
 * Add tracks to the end of a given playlist.
 * 
 * @param 1
 * The first token should be the full URI of the playlist, like:
 * spotify:user:JohnSmith:playlist:68sMl8CBblj6uBcqbJsnoj
 * @param 2
 * The second token should be the full URI of the track, like:
 * spotify:track:3GhpgjhCNZZa6Lb7Wtrp3S
 * 
 * @return -1.
 */
int cmd_add_tracks(int argc, char **argv){
	if(argc < 3){
		fprintf(stderr, "Usage: %s <URI-playlist> <URI-track 1> <URI-track 2> ...\n", argv[0]);
		return -1;
	}
	// let's retrieve the playlist	
	sp_playlist *pl = URI_to_playlist(argv[1]);
	if(!pl){		
		fprintf(stderr, "The given URI couldn't be converted to a playlist\n");
        return -1; // URI -> playlist failed	
	}
	
	// let's retrieve the tracks
	int n = argc-2;
	sp_track *tracks[n];
	int i;
	for(i = 0; i < n; i++){		
		const char * track_URI = argv[2+i];
		sp_link *track_link = sp_link_create_from_string(track_URI);
		if(!track_link) {
			fprintf(stderr, "failed to get link from a Spotify URI\n");
			return -1;
		}
		sp_linktype lt = sp_link_type(track_link);
		if(lt != SP_LINKTYPE_TRACK){
			const char * link_type_label = get_link_type_label(lt);		
			fprintf(stderr, "The URI was of type '%s', not as the exptected '%s'\n", link_type_label, get_link_type_label(SP_LINKTYPE_TRACK));
			return -1;
		}

		sp_track *track = sp_link_as_track(track_link);
		
		if(!track){		
			fprintf(stderr, "Failed to retrieve the track from the link %s\n", track_URI);
			return -1;
		}
		tracks[i] = track;
	}
	//Now we hopefully got non-corrupt pointers to both the
	//playlist and the tracks.
	
	int end = sp_playlist_num_tracks(pl);
	sp_error err = sp_playlist_add_tracks(pl, (const sp_track**)tracks, n, end, g_session);
	if(err != SP_ERROR_OK){
		fprintf(stderr, "Error '%s' when trying to insert one track to the playlist.\n", sp_error_message(err));
		return -1;
	}
	return -1;
	
	// For some reason, for version 0.0.4, as I've understood it they
	// want sp_playlist_add_tracks take the session as an additional
	// last arguement. Which at least compiles for me.
}


/**
 * Count the amount of tracks in a playlist.
 * I mainly created this to see how "safe" the counting function is.
 * 
 * @param the URI
 * 
 * @return -1.
 * */
int cmd_count_tracks(int argc, char **argv){
	printf("Hi!\n");
	if(argc != 2){
		fprintf(stderr, "Usage: %s <URI>\n", argv[0]);
		return -1;
	}	
	printf("Hi!\n");
	sp_playlist *pl = URI_to_playlist(argv[1]);
	printf("Hi!\n");
	if(!pl){		
		fprintf(stderr, "The given URI couldn't be converted to a playlist\n");
        return -1; // URI -> playlist failed	
	}
	printf("Hi!\n");
	int n = sp_playlist_num_tracks(pl);
	printf("Hi!\n");
	
	printf("%i tracks.\n", n);
	return -1;
}




/**
 * Remove a playlist from our container. Where 'our container' refers
 * to the container that is g_pc, which should be the users container
 * if the user has successfuly logged in.
 * 
 * Indeed letting the playlist exist yet be removed from our container
 * gives the illusion of "hiding" the playlist.
 * 
 * @param 1
 * The first token should be the full URI of the playlist, like:
 * spotify:user:JohnSmith:playlist:68sMl8CBblj6uBcqbJsnoj
 * 
 * @return -1.
 */
int cmd_hide_playlist(int argc, char **argv){
	if(argc != 2){
		fprintf(stderr, "Usage: %s <URI>\n", argv[0]);
		return -1;
	}	
	return hide_playlist(argv[1]);
}

/* ---------------------- END  IMPLEMENTED COMMANDS  ------------------------ */






/* ---------------------- HELP FUNCTIONS ------------------------------------ */


/**
 * Create a new playlist with the given name.
 * 
 * @param The desired name of the 
 *        Note that if you have spaces, it should be dashes (_)
 *        in the argument.
 *        WARNING: name will be modified by the function.
 *                 (It will convert dashes to spaces)
 * 
 * @return The URI of the created playlist. NULL if failed.
 * */
char * new_playlist(char* name){	
	static const int buffSize = 200;
	char * buff = (char*) malloc (buffSize);
	if (buff==NULL){
		fprintf(stderr, "Out of memory. Couldn't use malloc.\n"); 
	}
	
	char* cp = name;
	while(*cp != '\0'){
		if(*cp == '_') (*cp) = ' ';
		cp++;
	}
	
	sp_playlist *pl = sp_playlistcontainer_add_new_playlist(g_pc, name);
	if(!pl){
		fprintf(stderr, "new_playlist: creating playlist with name %s failed\n", name);
		return NULL;		
	}
	// Get the sp_link-handle for the playlist
	sp_link *spl = sp_link_create_from_playlist(pl);
	
	// Get the URI of the link.
	sp_link_as_string(spl, buff, buffSize);
	return buff;
} 


/* ---------------------- END HELP FUNCTIONS -------------------------------- */
