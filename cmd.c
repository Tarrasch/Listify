/**
 * Copyright (c) 2006-2010 Spotify Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <string.h>

#include "listify.h"
#include "cmd.h"

static int cmd_help(int argc, char **argv);

/**
 *
 */
struct {
	const char *name;
	int (*fn)(int argc, char **argv);
	const char *help;
} commands[] = {
	{ "logout",       cmd_logout,         "Logout and exit app" },
	{ "exit",         cmd_logout,         "Logout and exit app" },
	{ "new_list",     cmd_new_playlist,   "Add a new playlist with a given name." },
	{ "new_hide",     cmd_new_hide,       "Add a new playlist. Then hide it"},
	{ "add_list",     cmd_add_playlist,   "Given a URI add the playlist to our container." },
	{ "clear_list",   cmd_clear_playlist, "Clear a playlist, given it's URI" },
	{ "add_tracks",   cmd_add_tracks,     "Add tracks to a list." },
	{ "count_tracks", cmd_count_tracks,   "Counts the amount of tracks in a playlist." },
	{ "hide_list",    cmd_hide_playlist,  "Hide the given playlist. (Inverse of add)"},
	{ "help",         cmd_help,           "This help" },
};


/**
 *
 */
static int tokenize(char *buf, char **vec, int vsize)
{
	int n = 0;
	while(1) {
		while(*buf > 0 && *buf < 33)
			buf++;
		if(!*buf)
			break;
		vec[n++] = buf;
		if(n == vsize)
			break;
		while(*buf > 32)
			buf++;
		if(*buf == 0)
			break;
		*buf = 0;
		buf++;
	}
	return n;
}


/**
 *
 */
void cmd_exec_unparsed(char *l)
{
	char *vec[32];
	int c = tokenize(l, vec, 32);
	cmd_dispatch(c, vec);
}


/**
 *
 */
void cmd_dispatch(int argc, char **argv)
{
	int i;

	if(argc < 1) {
		cmd_done();
		return;
	}

	for(i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {
		if(!strcmp(commands[i].name, argv[0])) {
			if(commands[i].fn(argc, argv))
				cmd_done();
			return;
		}
	}
	printf("No such command\n");
	cmd_done();
}

/**
 *
 */
static int cmd_help(int argc, char **argv)
{
	int i;
	for(i = 0; i < sizeof(commands) / sizeof(commands[0]); i++)
		printf("  %-20s %s\n", commands[i].name, commands[i].help);
	return -1;
}
