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

/**
 * @brief Star struct which holds basic data of a star.
 *
 */
typedef struct _star
{
    /**
     * @brief Morton-code of the star in a 3d-grid.
     *
     */
    u_int64_t morton_index;

    /**
     * @brief ID extracted from dataset.
     *
     */
    u_int64_t id;

    /**
     * @brief X position star.
     *
     */
    double x;

    /**
     * @brief Y position star.
     *
     */
    double y;

    /**
     * @brief Z position star.
     *
     */
    double z;

    /**
     * @brief Colour of the star in hex converted to int.
     *
     */
    u_int32_t colour;

    /**
     * @brief Absolute magnitude of the star
     */
    float brightness;
} SStar;

/**
 * @brief A small struct which holds pointers to databases and the directory
 *        they are in.
 *
 */
typedef struct _db_ctx
{
    /**
     * @brief Handle to the primary database.
     *
     */
    DB* dbp;

    /**
     * @brief Handle to the secondary database that holds indices for the
     *        morton codes.
     *
     */
    DB* sdbp;

    /**
     * @brief Home directory the databases are located in.
     *
     */
    char* db_dir;
} DB_CTX;

DB_CTX*
gaia_setup_database(const char* directory);

int
gaia_close_database(DB_CTX* context);

int
gaia_new_star(DB* dbp, u_int64_t id, double x, double y, double z,
              u_int32_t colour, float brightness, u_int64_t morton_index);

SStar*
gaia_get_star(DB* dbp, u_int64_t id);

SStar*
gaia_get_star_by_morton(DB* sdbp, u_int64_t index);

int
gaia_delete_star(DB* dbp, u_int64_t id);

int
gaia_update_star_morton(DB* dbp, u_int64_t id, u_int64_t morton_index);

// Cursor
DBC*
gaia_get_cursor(DB* dbp);

SStar*
gaia_get_next_star(DBC* dbcp);

SStar*
gaia_goto_star(DBC* dbcp, u_int64_t id);

int
gaia_close_cursor(DBC* dbcp);

#endif // !GAIA_DB_H
