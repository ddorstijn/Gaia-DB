/**
 * @file gaia_db.cpp
 * @author Danny Dorstijn
 * @brief Implementation of the BerkeleyDB wrapper
 * @version 0.8
 * @date 2019-01-23
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "gaia_db.h"

#include "database_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Callback used by the db for creating indices for the morton codes.
 *
 * @param dbp - Handle for dbp (unused)
 * @param pkey - Handle for the key of the main db (unused)
 * @param pdata - Handle for the data of the main db. Used to extract morton idx
 * @param skey - Handle for the secondary key. This is what we put in the db
 * @return int - Returns 0 to signal all is fine
 */
int
get_id_callback(DB* dbp, const DBT* pkey, const DBT* pdata, DBT* skey)
{
    // We don't need the dbp or pkey
    dbp = NULL;
    pkey = NULL;

    memset(skey, 0, sizeof *pkey);
    skey->data = (u_int64_t*)pdata->data;
    skey->size = sizeof(u_int64_t);

    return 0;
}

/**
 * @brief Setup the databases for GAIA.
 *
 * @param directory - The base directory to put the databases in
 * @return DB_CTX* - A helper to manage the databases
 */
DB_CTX*
gaia_setup_database(const char* directory)
{
    DB_CTX* ctx = (DB_CTX*)malloc(sizeof *ctx);
    ctx->db_dir = directory;

    FILE* stars_log = fopen("stars.log", "a");

    db_init(&ctx->dbp, directory, "stars.db", stars_log, DB_CREATE, DB_HASH);
    db_init(&ctx->sdbp, directory, "morton_index.sdb", stars_log, DB_CREATE,
            DB_HASH);

    ctx->dbp->associate(ctx->dbp, NULL, ctx->sdbp, get_id_callback, 0);

    return ctx;
}

/**
 * @brief Close the databases. Writes the db's to a file.
 *
 * @param ctx - The context with both the database handles
 * @return int - Returns err code or 0 if all was fine
 */
int
gaia_close_database(DB_CTX* ctx)
{
    int ret;

    ret = db_close(ctx->sdbp);
    if (ret != 0) {
        printf("Error close secondary db");
    }

    ret = db_close(ctx->dbp);
    if (ret != 0) {
        printf("Error close primary db");
    }

    free(ctx);

    return ret;
}

/**
 * @brief Insert new star in the database.
 *
 * @param dbp - Handle for the primary db
 * @param id - ID of the star extracted from the uuid in the gaia dataset
 * @param x - X position of the star
 * @param y - Y position of the star
 * @param z - Z position of the star
 * @param colour - The colour of the star
 * @param morton_index - The morton index of the star. Leave 0 if unsure
 * @return int - Error code or 0 if fine
 */
int
gaia_new_star(DB* dbp, u_int64_t id, double x, double y, double z,
              u_int32_t colour, float brightness, u_int64_t morton_index)
{
    int ret;

    if (morton_index == 0) {
        morton_index = id;
    }

    SStar star = { morton_index, id, x, y, z, colour, brightness };
    ret = db_insert(dbp, &(star.id), sizeof star.id, &star, sizeof star);

    return ret;
}

/**
 * @brief Get a star from the db based on the ID of the star.
 *
 * @param dbp - Handle for the primary database
 * @param id - ID of the star you are looking for
 * @return SStar* - Star object with all the data corresponding to the id given
 */
SStar*
gaia_get_star(DB* dbp, u_int64_t id)
{
    SStar* star = (SStar*)db_get(dbp, &id, sizeof id);
    if (star == NULL) {
        log_error(dbp, DB_NOTFOUND);
    }

    return star;
}

/**
 * @brief Get the star based on it's morton code.
 *
 * @param sdbp - Handle for the secondary database with morton code indices
 * @param index - The morton code index
 * @return SStar* - The star data corresponding to the index given
 */
SStar*
gaia_get_star_by_morton(DB* sdbp, u_int64_t index)
{
    SStar* star = (SStar*)db_get(sdbp, &index, sizeof index);
    if (star == NULL) {
        log_error(sdbp, DB_NOTFOUND);
    }

    return star;
}

/**
 * @brief Get a new cursor to iterate over the database.
 *
 * @param dbp - Handle to the database
 * @return DBC* - The cursor
 */
DBC*
gaia_open_cursor(DB* dbp)
{
    int ret;

    DBC* dbcp;
    ret = dbp->cursor(dbp, NULL, &dbcp, 0);
    if (ret != 0) {
        log_error(dbp, ret);
    }

    return dbcp;
}

/**
 * @brief Close the cursor after you are done using it.
 *
 * @param dbcp - Handle to the database
 * @return int - Error code or 0 if all is fine
 */
int
gaia_close_cursor(DBC* dbcp)
{
    int ret = -1;

    if (dbcp != NULL) {
        ret = dbcp->close(dbcp);
    }

    return ret;
}

/**
 * @brief Get the star the cursor is pointing to.
 *
 * @param dbcp - Handle to the database
 * @return SStar* - The star the cursor is pointing to
 */
SStar*
gaia_cursor_get_star(DBC* dbcp)
{
    DBT key, data;
    int ret;

    /* Initialize our DBTs. */
    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    ret = dbcp->get(dbcp, &key, &data, DB_CURRENT);
    if (ret != 0) {
        if (ret == DB_NOTFOUND) {
            printf("At the end of the database");
        } else {
            printf("Error getting next record");
        }
    }

    SStar* star = ((SStar*)data.data);
    return star;
}

/**
 * @brief Check if the cursor is on the last record and jump to that record.
 *
 * @param dbcp - Handle to the database
 * @return char - 1 if jump is succesful. Else 0
 */
char
gaia_cursor_has_next(DBC* dbcp)
{
    DBT key, data;
    int ret;

    /* Initialize our DBTs. */
    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    ret = dbcp->get(dbcp, &key, &data, DB_NEXT);
    if (ret != 0) {
        if (ret == DB_NOTFOUND) {
            printf("At the end of the database");
        } else {
            printf("Error getting next record");
        }
    }

    return ret == 0;
}

/**
 * @brief Set the cursor to a star with the id given.
 *
 * @param dbcp - Handle to the database
 * @param id - ID to jump to
 * @return int - Error code or 0 if all is fine
 */
int
gaia_cursor_goto_star(DBC* dbcp, u_int64_t id)
{
    DBT key, data;
    int ret;

    /* Initialize our DBTs. */
    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    ret = dbcp->get(dbcp, &key, &data, DB_SET);
    if (ret != 0) {
        if (ret == DB_NOTFOUND) {
            printf("At the end of the database");
        } else {
            printf("Error getting next record");
        }
    }

    return ret;
}

/**
 * @brief Search and remove a star from the db.
 *
 * @param dbp - Handle for the primary database
 * @param id - The id of the star
 * @return int - Error code or 0 if all is fine
 */
int
gaia_delete_star(DB* dbp, u_int64_t id)
{
    DBT key;
    int ret;

    memset(&key, 0, sizeof key);

    key.data = &id;
    key.size = sizeof id;

    ret = dbp->del(dbp, NULL, &key, 0);
    if (ret == DB_NOTFOUND) {
        printf("Key not found");
    }

    return ret;
}

/**
 * @brief Update the morton code of the star.
 *
 * @param dbp - Handle of the primary database
 * @param id - The id of the star
 * @param morton_index - The new morton index
 * @return int - Error code or 0 if all is fine
 */
int
gaia_update_star_morton(DB* dbp, u_int64_t id, u_int64_t morton_index)
{

    return 0;
}
