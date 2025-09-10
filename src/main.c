#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <microhttpd.h>
#include <unistd.h>

#include "main.h"

#define PORT 8080

/*
 * INFO: Database query handler function.
 */
int
get_users_from_db(char *response)
{
	sqlite3 *db;
	sqlite3_stmt *stmt;
	const char *sql = "SELECT id, name, email from users";
	int rc = sqlite3_open("database_name", &db);

	if (rc != SQLITE_OK) {
		sprintf(response, "ERROR: Opening database!\n");
		return 1;
	}

	rc = sqlite3_prepare(db, sql, -1, &stmt, 0);
	if (rc != SQLITE_OK) {
		sprintf(response, "ERROR: Preparing SQL statement!\n");
		sqlite3_close(db);
		return 1;
	}

	/*
	 * INFO: *retrieve* and *format* the data.
	 */
	char buffer[1024] = "";
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		int id = sqlite3_column_int(stmt, 0);
		const char *name = (const char *)sqlite3_column_text(stmt, 1);
		const char *email = (const char *)sqlite3_column_text(stmt, 2);

		/* INFO: *format* the result as JSON */
		sprintf(buffer + strlen(buffer),
			"{\"id\": %d \"Name\": \"%s\", \"Email\": \"%s\"}"
			, id, name, email);
	}

	/* INFO: Removes the last comma and close the database */
	if (strlen(buffer) > 0)
		buffer[strlen(buffer) - 1] = '\0';
	sprintf(response, "[%s]", buffer);

	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return 0;
}

/*
 * INFO: HTTP request handler function
 */
static int
answer_to_request(void *cls, struct MHD_Connection *connection,
		  const char *url, const char *method, const char *version,
		  const char *upload_data, size_t *upload_data_size, void **con_cls)
{
	char *response[2048] = "";

	if (strcmp(url, "/api/users") == 0) {
		if (get_users_from_db(response) == 0) {
			struct MHD_Response *mhd_response = MHD_create_response_from_buffer(strlen(response),
								       (void *)response, MHD_RESPMEM_PERSISTENT);
			MHD_add_response_header(mhd_response, "Content-type", "application/json");
			MHD_add_response_header(mhd_response, "Access-Control-Allow-Origin", "*");
			int ret = MHD_queue_response(connection, MHD_HTTP_OK, mhd_response);
			return ret;
		} else {
			struct MHD_Response *mhd_response = MHD_create_response_from_buffer(strlen(response), (void *)"Not Found", MHD_RESPMEM_PERSISTENT);
			int ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, mhd_response);
			MHD_destroy_response(mhd_response);
			return ret;
		}
	}
}

/*
 * INFO: Start HTTP server
 */
int start_server() {
	struct MHD_Daemon *daemon;
	daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, PORT, NULL, NULL, &answer_to_request, NULL, MHD_OPTION_END);
	if (!daemon)
		return 1;
	getchar();
	MHD_stop_daemon(daemon);
	return 0;
}

int main(int argc, char **argv)
{
	printf("Starting the server on port %d...\n", PORT);

	if (start_server() != 0) {
		fprintf(stderr, "ERROR: failed to start the server!\n");
		return -1;
	}
	return 0;
}
