

#include "link.h"
#include "list.h"
#include "listify.h"
#include <stdio.h>
#include <string.h>


/*
 * I define a help-functions here. They all in one way or another
 * related to the spotifys link/URI-system.
 * 
 * Most functions here are code written by the Listify author, rather
 * than being imported from various example-codes. 
 * 
 * */



/**
 * Give a textual repsresentation for a given linktype.
 * 
 * This function was taken from the Spotify documentation site.
 * 
 * */
const char* get_link_type_label(sp_linktype lt)
{
    static const char *LINK_TYPES[] = {
        "invalid",
        "track",
        "album",
        "artist",
        "search",
        "playlist"
    };

    return LINK_TYPES[lt];
}


/**
 * Convert try to convert a playlist URI to a sp_playlist*.
 * 
 * @param URI of the playlist.
 * 
 * @return the link for the playlist if succeded. NULL if fails.
 */
sp_link* URI_to_link(const char *URI){
	sp_link *link = sp_link_create_from_string(URI);
	if(!link) {
        fprintf(stderr, "failed to get link from a Spotify URI\n");
        return NULL;
    }
	sp_linktype lt = sp_link_type(link);
	if(lt != SP_LINKTYPE_PLAYLIST){
		const char * link_type_label = get_link_type_label(lt);		
		fprintf(stderr, "The URI was of type '%s', not as the exptected '%s'\n", link_type_label, get_link_type_label(SP_LINKTYPE_PLAYLIST));
		return NULL;	
	}
	return link;
}


/**
 * Convert a link to a playlist. For some reason
 * libspotify doesn't provide this feature by default, and it can
 * only be obtained by adding the link to the container and then
 * removing it and then remember what playlist just got added
 * to the container.
 * 
 * @param link to the playlist.
 * 
 * @return the playlist if succeded, NULL if failed.
 * */

sp_playlist *sp_link_as_playlist(sp_link *link){
	return sp_playlist_create(g_session, link);
}


/**
 * Turn a URI to a playlist. As for the exact same reasons as written
 * above for sp_link_as_playlist(), this function becomes way more
 * complicated than it could be if the api provided us with more
 * key functions.
 * 
 * @param the URI of the playlist.
 * 
 * @return the playlist if succeded, NULL if failed.
 * 
 * */
sp_playlist *URI_to_playlist(const char *URI){
	sp_link *link = URI_to_link(URI);
	if(!link) {
		fprintf(stderr, "URI --> link failed!\n");
		return NULL;
	}
	return sp_link_as_playlist(link);
}


/**
 * Given a playlists URI, remove it from the container
 * and giving it a +1 in reference so the spotify system
 * keeps it alive.
 * 
 * I don't know what bad effects it has for "us" if we lose track
 * of the link and the playlist floats around without anyone pointing
 * to it.
 * 
 * @param the URI of the playlist.
 * 
 * @return -1 if fails. 0 if succeeds.
 * 
 * */  
int hide_playlist(const char *URI){
	sp_link *link = URI_to_link(URI);
	if(!link){
		fprintf(stderr, "URI --> link failed!\n");
        return -1; // URI -> playlist failed
	}
	//So by now we know that the given argument is a correct URI
	//of type playlist. So now let's now go through the elements of our
	//container, the container will maybe contain a playlist who has 
	//"the same sp_link", which means they are equal.
	int i  =  0;
	int n = sp_playlistcontainer_num_playlists(g_pc);
	int lengthURI = strlen(URI);
	while(i < n){
		static char buff[100];
		sp_playlist * pl = sp_playlistcontainer_playlist(g_pc, i);
		sp_link *link2 = sp_link_create_from_playlist(pl);
		
		if(sp_link_as_string(link2, buff, lengthURI+3) == lengthURI &&
		   strcmp(URI, buff) == 0 ){
			//we found a match
			break;
		}
		i++;
	}
	if(i == n){
		printf("There was no link with the given URI inside the container.\n");
		return -1;
	}
	
	//now let's try to remove it
	sp_link_add_ref(link); //ok, I doubt it will ever be released now ...
	sp_error err = sp_playlistcontainer_remove_playlist(g_pc, i);
	if(err != SP_ERROR_OK){
		fprintf(stderr, "Error '%s' when trying to delete the playlist.\n", sp_error_message(err));
		return -1;
	}
	return -1;
}

