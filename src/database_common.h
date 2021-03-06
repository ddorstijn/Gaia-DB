/**
 * @file database_common.h
 * @author Danny Dorstijn
 * @brief Helper functions for database interaction
 * @version 0.9
 * @date 2019-01-23
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef DATABASE_H
#define DATABASE_H

#include "db.h"

void
log_error(DB* dbp, int ret);

int
db_init(DB** dbpp, const char* db_directory, const char* db_name,
        FILE* log_file, u_int32_t db_flags, DBTYPE db_type);

int
db_close(DB* dbp);

int
db_insert(DB* dbp, void* d_key, size_t s_key, void* d_data, size_t s_data);

void*
db_get(DB* dbp, void* d_key, int s_key);

#endif // !DATABASE_H
