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

#include "gaia_db.hpp"

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
    u_int offset;

    // We don't need the dbp or pkey
    dbp = NULL;
    pkey = NULL;

    // ID + x + y + z + colour
    offset = sizeof(u_int64_t) + sizeof(double) * 3 + sizeof(u_int32_t);

    if (pdata->size < offset) {
        return -1;
    }

    memset(skey, 0, sizeof *pkey);
    skey->data = (u_int64_t*)pdata->data + offset;
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
    db_init(&ctx->dbp, directory, "stars.db", DB_CREATE, DB_BTREE);
    db_init(&ctx->sdbp, directory, "morton_index.sdb", DB_CREATE, DB_BTREE);

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
              u_int32_t colour, u_int64_t morton_index)
{
    if (morton_index == 0) {
        morton_index = id;
    }

    SStar star = { id, x, y, z, colour, morton_index };

    int ret;
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
    void* data = db_get(dbp, &id, sizeof id);
    SStar* star = (SStar*)data;

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
        printf("ERROR: star is null");
    }
    return star;
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
    int ret;
    ret = db_delete(dbp, &id, sizeof id);

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