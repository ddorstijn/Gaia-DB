#include "database.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int
db_close(DB* dbp)
{
    int ret = dbp->close(dbp, 0);
    if (ret != 0) {
        printf("Error closing database");
    }

    return ret;
}

int
db_insert(DB* dbp, void* d_key, int s_key, void* d_data, int s_data)
{
    DBT key, data;
    int ret;

    memset(&key, 0, sizeof key);
    memset(&data, 0, sizeof data);

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
