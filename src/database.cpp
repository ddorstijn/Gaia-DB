#include "database.h"

#include <stdio.h>

int
gaia_db_init(DB_CTX* context, const char* database_directory = "")
{
    // Return value for db operations
    int ret;

    // Initialize db handle
    ret = db_create(context->database, NULL, 0);
    if (ret != 0) {
        printf("Creation of data failed with the following error: \n%s",
               db_strerror(ret));
        return ret;
    }

    ret = context->database->open(context->database, NULL, "database.db", NULL,
                                  DB_BTREE, DB_CREATE, 0);
    if (ret != 0) {
        printf("Creation of data failed with the following error: \n%s",
               db_strerror(ret));
        return ret;
    }

    printf("database creation successful\n");
    return 0;
}

int
gaia_db_close(DB_CTX* context);