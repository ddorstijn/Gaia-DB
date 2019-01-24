/**
 * @file database.hpp
 * @author Danny Dorstijn
 * @brief Wrapper for the BerkeleyDB
 * @version 0.8
 * @date 2019-01-23
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef DATABASE_H
#define DATABASE_H

#include "db.h"

int
db_init(DB** dbpp, const char* db_directory, const char* db_name,
        FILE* log_file, u_int32_t db_flags, DBTYPE db_type);

int
db_close(DB* dbp);

int
db_insert(DB* dbp, void* d_key, size_t s_key, void* d_data, size_t s_data);

void*
db_get(DB* dbp, void* d_key, int s_key);

DBC*
db_open_cursor(DB* dbp);

int
db_close_cursor(DBC* dbcp);

void*
db_cursor_read_next(DBC* dbcp);

void*
db_cursor_jump_to(DBC* dbcp, u_int64_t id);

int
db_delete(DB* dbp, void* d_key, int s_key);

#endif // !DATABASE_H
