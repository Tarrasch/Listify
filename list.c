#include "listify.h"
#include "cmd.h"



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

/*
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



/**
 * Add a new playlist
 * 
 * @param string containing the name of the playlist.
 * @return -1 if fails. 0 otherwise.
 * 
 */
int cmd_new_playlist(int argc, char **argv){
	if(argc != 2){		
		fprintf(stderr, "Usage: %s <name>\n", argv[0]);
		return -1;
	}
	if(!sp_playlistcontainer_add_new_playlist(g_pc, argv[1])){
		fprintf(stderr, "new_playlist: adding playlist with name %s failed\n", argv[0]);
		return -1;		
	}
	return 0;
}

/**
 * Clear a playlist
 * 
 * @param string containing the name of the playlist.
 * @return -1 if fails. 0 otherwise.
 */
int cmd_clear_playlist(int argc, char **argv){
	return 0;
	
}


/**
 * Add a track to a given playlist
 * 
 * @return -1 if fails. 0 otherwise.
 */
int cmd_add_track(int argc, char **argv){
	
	return 0;	
}


/* ---------------------- END  IMPLEMENTED COMMANDS  ------------------------ */



