#ifndef MAIN_H
#define MAIN_H

#include <stddef.h>

int get_users_from_db(char *);

static int answer_to_request(void *, struct MHD_Connection *,
		  const char *, const char *, const char *,
		  const char *, size_t *, void **);

int start_server();

#endif
