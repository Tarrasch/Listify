#ifndef LINK_H__
#define LINK_H__

#include <libspotify/api.h>

const char* get_link_type_label(sp_linktype lt);
sp_link* URI_to_link(const char *URI);
sp_playlist *sp_link_as_playlist(sp_link *link);
sp_playlist *URI_to_playlist(const char *URI);
int hide_playlist(const char *URI);
int hide_playlist(const char *URI);

#endif

