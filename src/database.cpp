/**
 * @file database.cpp
 * @author Danny Dorstijn
 * @brief Implementation for the Gaia DB wrapper
 * @version 0.8
 * @date 2019-01-23
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "database.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <inttypes.h>

/**
 * @brief Add to strings together. This needs to be freed.
 *
 * @param str1 - First string
 * @param str2 - Second string
 * @return char* - Both strings combined
 */
char*
concatenate_strings(const char* str1, const char* str2)
{
    if (strlen(str1) == 0) {
        return (char*)str2;
    } else if (strlen(str2) == 0) {
        return (char*)str1;
    }

    size_t size;

    size = strlen(str1) + strlen(str2);
    char* ret = (char*)malloc(size);
    snprintf(ret, size, "%s%s", str1, str2);

    return ret;
}

/**
 * @brief Initialize a database.
 *
 * @param dbpp - A pointer to a handle for the new db
 * @param db_directory - The directory where to place the db
 * @param db_name - Name of the database
 * @param db_flags - Flags for creating a db
 * @param db_type - Type of the db (BTree, Heap, Queue, etc.)
 * @return int - Error code or 0 if all is fine
 */
int
db_init(DB** dbpp, const char* db_directory, const char* db_name,
        u_int32_t db_flags, DBTYPE db_type)
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
    dbp->set_errfile(dbp, stderr);
    dbp->set_errpfx(dbp, db_name);

    // Open or create the database
    char* db_path = concatenate_strings(db_directory, db_name);
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
        printf("Insert failed: \n%s", db_strerror(ret));
        return ret;
    }

    return ret;
}

/**
 * @brief Get a value from the database.
 *
 * @param dbp - Handle to the database
 * @param d_key - Pointer to the value of the key
 * @param s_key - Size of the value (using sizeof)
 * @return void* - The data object retrieved from the db
 */
void*
db_get(DB* dbp, void* d_key, int s_key)
{
    DBT key, data;

    // Make sure DBT's are empty
    memset(&key, 0, sizeof key);
    memset(&data, 0, sizeof data);

    key.data = d_key;
    key.size = s_key;

    dbp->get(dbp, NULL, &key, &data, 0);

    return data.data;
}

/**
 * @brief Delete a record from the database.
 *
 * @param dbp - Handle for the database
 * @param d_key - Pointer to the value of the key
 * @param s_key - Size of the value (using sizeof)
 * @return int - Error code or 0 if all is fine
 */
int
db_delete(DB* dbp, void* d_key, int s_key)
{
    DBT key;
    int ret;

    memset(&key, 0, sizeof key);

    key.data = d_key;
    key.size = s_key;

    ret = dbp->del(dbp, NULL, &key, 0);
    if (ret == DB_NOTFOUND) {
        printf("Key not found");
    }

    return ret;
}
