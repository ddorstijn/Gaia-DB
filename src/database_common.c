/**
 * @file database_common.c
 * @author Danny Dorstijn
 * @brief Helper functions for database interaction
 * @version 0.9
 * @date 2019-01-23
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "database_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <inttypes.h>

/**
 * @brief Form a path from the string to the directory and the name of the db.
 * This needs to be freed. Done automatically by berkeley if used to create db.
 *
 * @param str1 - First string
 * @param str2 - Second string
 * @return char* - Both strings combined
 */
char*
make_path(const char* str1, const char* str2)
{
    if (strlen(str1) == 0) {
        return (char*)str2;
    } else if (strlen(str2) == 0) {
        return (char*)str1;
    }

    size_t size;

#ifdef _WIN32
    char delimiter = '\\';
#else
    char delimiter = '/';
#endif

    // Added 2 extra charachters. One for the "/" and one spare for the null
    // termination
    size = strlen(str1) + strlen(str2) + 2;
    char* ret = (char*)malloc(size);
    snprintf(ret, size, "%s%c%s", str1, delimiter, str2);

    return ret;
}

/**
 * @brief Small helper function for logging errors in the logfile.
 *
 * @param dbp - Handle to the database
 * @param ret - The error code to be printed
 */
void
log_error(DB* dbp, int ret)
{
    FILE* log_file;
    dbp->get_errfile(dbp, &log_file);
    fprintf(log_file, "%s\n", db_strerror(ret));
}

/**
 * @brief Initialize a database.
 *
 * @param dbpp - A pointer to a handle for the new db
 * @param db_directory - The directory where to place the db
 * @param db_name - Name of the database
 * @param log_file - Log file to print all errors in
 * @param db_flags - Flags for creating a db
 * @param db_type - Type of the db (BTree, Heap, Queue, etc.)
 * @return int - Error code or 0 if all is fine
 */
int
db_init(DB** dbpp, const char* db_directory, const char* db_name,
        FILE* log_file, u_int32_t db_flags, DBTYPE db_type)
{
    // Return value for db operations
    int ret;
    DB* dbp;

    // Initialize db handle
    ret = db_create(&dbp, NULL, 0);
    if (ret != 0) {
        printf("Creation of data failed with the following error: \n%s",
               db_strerror(ret));
        return ret;
    }

    // let the dbp point to the newly opened db
    *dbpp = dbp;

    /* Set up error handling for this database */
    dbp->set_errfile(dbp, log_file);
    dbp->set_errpfx(dbp, db_name);

    dbp->set_cachesize(dbp, 0, 4096000, 0);

    // Open or create the database
    char* db_path = make_path(db_directory, db_name);
    ret = dbp->open(dbp, NULL, db_path, NULL, db_type, db_flags, 0);
    if (ret != 0) {
        printf("Creation of data failed with the following error: \n%s",
               db_strerror(ret));
        return ret;
    }

    printf("database creation successful\n");
    return 0;
}

/**
 * @brief Close the database.
 *
 * @param dbp - Handle to the db to be closed
 * @return int - Error code or 0 if all is fine
 */
int
db_close(DB* dbp)
{
    int ret = dbp->close(dbp, 0);
    if (ret != 0) {
        printf("Error closing database");
    }

    return ret;
}

/**
 * @brief Insert a value in the database.
 *
 * @param dbp - Handle to the db
 * @param d_key - Pointer to the data of the key
 * @param s_key - Size of the key (using sizeof)
 * @param d_data - Pointer to the data
 * @param s_data - Sizeof the corresponding data (using sizeof)
 * @return int - Error code or 0 if all is fine
 */
int
db_insert(DB* dbp, void* d_key, size_t s_key, void* d_data, size_t s_data)
{
    int ret;

    DBT key, data;
    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    key.data = d_key;
    key.size = s_key;

    data.data = d_data;
    data.size = s_data;

    ret = dbp->put(dbp, NULL, &key, &data, DB_NOOVERWRITE);
    if (ret == DB_KEYEXIST) {
        printf("Key already exsists");
    }
    if (ret != 0) {
        // printf("Insert failed: \n%s", db_strerror(ret));
        log_error(dbp, ret);
        return ret;
    }

    return ret;
}

/**
 * @brief Get an item from the database.
 *
 * @param dbp - Handle to the database
 * @param d_key - Pointer to the key
 * @param s_key - Size of the key
 * @return void* - Data of the record
 */
void*
db_get(DB* dbp, void* d_key, int s_key)
{
    int ret;
    DBT key, data;

    // Make sure DBT's are empty
    memset(&key, 0, sizeof key);
    memset(&data, 0, sizeof data);

    key.data = d_key;
    key.size = s_key;

    ret = dbp->get(dbp, NULL, &key, &data, 0);
    if (ret != 0) {
        log_error(dbp, ret);
    }

    return data.data;
}