#pragma once

#include "db.h"

typedef struct _db_ctx
{
    DB* database;
    char* db_directory;
} DB_CTX;

int
gaia_db_init(DB_CTX* context, const char* database_directory = "");

int
gaia_db_close(DB_CTX* context);