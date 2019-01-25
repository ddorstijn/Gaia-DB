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

#include <assert.h>
#include <stdio.h>
int
main()
{
    DB_CTX* db_ctx;
    int ret;
    int loop_count = 4;
    // Setup databases - one for the stars and one secondary
    // The secondary indexes the morton codes for quick access
    db_ctx = gaia_setup_database("databases");

    // Test: insert
    for (u_int64_t i = 0; i < loop_count; i++) {
        ret = gaia_new_star(db_ctx->dbp, i, 1.2, 2.3, 4.5, 128, 10.2, 0);
        assert(ret == 0);

        if (i % 50000 == 0) {
            printf("%d\n", i);
        }
    }

    // Test: retrieve
    SStar* star = gaia_get_star(db_ctx->dbp, 2);
    assert(star != NULL);
    assert(star->id == 2);
    assert(star->morton_index == 2);

    // Test: update morton
    gaia_update_star_morton(db_ctx->dbp, 0, 101);
    star = gaia_get_star(db_ctx->dbp, 0);
    assert(star->morton_index == 101);

    // Test: iterate
    DBC* cursor = gaia_open_cursor(db_ctx->dbp);
    int index = 0;

    while (gaia_cursor_has_next(cursor)) {
        star = gaia_cursor_get_star(cursor);
        assert(star != NULL);
        assert(star->colour == 128);

        if (index == 2) {
            gaia_cursor_update_star_morton(cursor, 102);
            star = gaia_get_star_by_morton(db_ctx->sdbp, 102);
            assert(star != NULL);
        }

        if (index % 50000 == 0) {
            printf("%d\n", index);
        }
        index++;
    }
    assert(index == loop_count);

    int status = gaia_close_cursor(cursor);
    assert(status == 0);

    // Close the database and write to disk
    status = gaia_close_database(db_ctx);
    assert(status == 0);
}
