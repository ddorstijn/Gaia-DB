#pragma once

#include "db.h"

int
db_init(DB** dbpp, const char* db_directory, const char* db_name,
        u_int32_t db_flags, DBTYPE db_type);

int
db_close(DB* dbp);

int
db_insert(DB* dbp, void* d_key, int s_key, void* d_data, int s_data);

void*
db_get(DB* dbp, void* d_key, int s_key);

int
db_delete(DB* dbp, void* d_key, int s_key);

