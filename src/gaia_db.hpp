/**
 * @file gaia_db.hpp
 * @author Danny Dorstijn
 * @brief Gaia DB wrapper
 * @version 0.8
 * @date 2019-01-23
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef GAIA_DB_H
#define GAIA_DB_H

#include "database.hpp"

typedef struct _star
{
    u_int64_t id;           // ID extracted from dataset
    double x;               // X position star
    double y;               // Y position star
    double z;               // Z position star
    u_int32_t colour;       // Colour of the star in hex converted to int
    u_int64_t morton_index; // Morton-code of the star in a 3d-grid
} SStar;

typedef struct _db_ctx
{
    DB* dbp;
    DB* sdbp;
    char* db_dir;
} DB_CTX;

DB_CTX*
gaia_setup_database(const char* directory);

int
gaia_close_database(DB_CTX* context);

int
gaia_new_star(DB* dbp, u_int64_t id, double x, double y, double z,
              u_int32_t colour, u_int64_t morton_index);

SStar*
gaia_get_star(DB* dbp, u_int64_t id);

SStar*
gaia_get_star_by_morton(DB* sdbp, u_int64_t index);

int
gaia_delete_star(DB* dbp, u_int64_t id);

int
gaia_update_star_morton(DB* dbp, u_int64_t id, u_int64_t morton_index);

#endif // !GAIA_DB_H