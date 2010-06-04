#include "listify.h"


/*
 * I define a help-function here.
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

