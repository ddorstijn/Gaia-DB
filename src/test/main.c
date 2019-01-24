/**
 * @file main.c
 * @author Danny Dorstijn
 * @brief Test function for the database wrapper
 * @version 0.6
 * @date 2019-01-24
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "../gaia_db.h"

#include "assert.h"

int
main()
{
    DB_CTX* db_ctx;
    int ret;

    // Setup databases - one for the stars and one secondary
    // The secondary indexes the morton codes for quick access
    db_ctx = gaia_setup_database("databases");

    // Test: insert
    for (u_int64_t i = 0; i < 3; i++) {
        ret = gaia_new_star(db_ctx->dbp, i, 1.2, 2.3, 4.5, 128, 10.2, 0);
        assert(ret == 0);
    }

    // Test: retrieve
    SStar* star = gaia_get_star(db_ctx->dbp, 2);
    assert(star != NULL);
    assert(star->id == 2);
    assert(star->morton_index == 2);

    // Test: iterate
    DBC* cursor = gaia_open_cursor(db_ctx->dbp);
    int index = 0;

    while (gaia_cursor_has_next(cursor)) {
        SStar* star = gaia_cursor_get_star(cursor);
        assert(star != NULL);
        assert(star->colour == 128);
        index++;
    }
    assert(index == 3);

    int status = gaia_close_cursor(cursor);
    assert(status == 0);

    // Close the database and write to disk
    status = gaia_close_database(db_ctx);
    assert(status == 0);
}
