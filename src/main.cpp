#include "gaia_db.hpp"

int
main()
{
    DB_CTX* db_ctx;

    // Setup databases - one for the stars and one secondary
    // The secondary indexes the morton codes for quick access
    db_ctx = gaia_setup_database("");

    // Test: insert
    gaia_new_star(db_ctx->dbp, 21312, 1.2, 2.3, 4.5, 128, 0);

    // Test: retrieve
    SStar* star = gaia_get_star(db_ctx->dbp, 21312);

    // Close the database and write to disk
    gaia_close_database(db_ctx);
}