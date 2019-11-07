#include "sqlite3.h"
#include <string.h>
#include <strings.h>
#include <stdio.h>
int main(int argc, const char *argv[])
{
	sqlite3 *db;
	int len =  sqlite3_open("~/www/db/app.db",&db);
	if (len){
		fprintf(stderr,"can't open database:%s\n",sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}
	printf("create_db sucessful\n");
	return 0;
}
