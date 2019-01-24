#include "../gaia_db.hpp"

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
    for (u_int64_t i = 0; i < 21312; i++) {
        ret = gaia_new_star(db_ctx->dbp, i, 1.2, 2.3, 4.5, 128, 10.2, 0);
        assert(ret == 0);
    }

    // Test: retrieve
    SStar* star = gaia_get_star(db_ctx->dbp, 50);
    assert(star != NULL);

    // Close the database and write to disk
    gaia_close_database(db_ctx);
}
