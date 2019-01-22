#include "gaia_read.h"

int
main()
{
    DB_CTX* db_ctx;
    db_ctx = gaia_setup_database("");
    gaia_new_star(db_ctx->dbp, 21312, 1.2, 2.3, 4.5, 128, 0);
    STAR* star = gaia_get_star(db_ctx->dbp, 21312);
    printf("%f, %f", star->x, star->y);
}
