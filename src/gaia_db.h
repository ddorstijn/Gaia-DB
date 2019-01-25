/**
 * @file gaia_db.h
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

#include "db.h"

#ifdef BUILDING_GAIADB_DLL
#define GAIADB_DLL __declspec(dllexport)
#else
#define GAIADB_DLL
#endif // BUILDING_GAIADB_DLL

/**
 * @brief Star struct which holds basic data of a star.
 */
typedef struct _star
{
    u_int64_t morton_index; /** @brief Morton-code of the star in a 3d-grid. */
    u_int64_t id;           /** @brief ID extracted from dataset. */

    double x; /** @brief X position star. */
    double y; /** @brief Y position star. */
    double z; /** @brief Z position star. */

    u_int32_t colour; /** @brief Colour of the star in hex converted to int. */
    float brightness; /** @brief Absolute magnitude of the star. */
} SStar;

/**
 * @brief A small struct which holds pointers to databases and the directory
 *        they are in.
 */
typedef struct _db_ctx
{
    DB* dbp;  /** @brief Handle to the primary database. */
    DB* sdbp; /** @brief Handle to the secondary database that holds indices for
               * the morton codes. */
    const char*
        db_dir; /** @brief Home directory the databases are located in. */
} DB_CTX;

DB_CTX* GAIADB_DLL
gaia_setup_database(const char* directory);

int GAIADB_DLL
gaia_close_database(DB_CTX* context);

int GAIADB_DLL
gaia_new_star(DB* dbp, u_int64_t id, double x, double y, double z,
              u_int32_t colour, float brightness, u_int64_t morton_index);

SStar* GAIADB_DLL
gaia_get_star(DB* dbp, u_int64_t id);

SStar* GAIADB_DLL
gaia_get_star_by_morton(DB* sdbp, u_int64_t index);

int GAIADB_DLL
gaia_delete_star(DB* dbp, u_int64_t id);

int GAIADB_DLL
gaia_update_star_morton(DB* dbp, u_int64_t id, u_int64_t morton_index);

// Cursor
DBC* GAIADB_DLL
gaia_open_cursor(DB* dbp);

char GAIADB_DLL
gaia_cursor_has_next(DBC* dbcp);

SStar* GAIADB_DLL
gaia_cursor_get_star(DBC* dbcp);

int GAIADB_DLL
gaia_cursor_goto_star(DBC* dbcp, u_int64_t id);

int GAIADB_DLL
gaia_cursor_update_star_morton(DBC* dbcp, u_int64_t morton_index);

int GAIADB_DLL
gaia_close_cursor(DBC* dbcp);

#endif // !GAIA_DB_H
