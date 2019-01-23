#include "gaia_read.hpp"

#include <stdlib.h>
#include <string.h>

int
get_id_callback(DB* dbp, const DBT* pkey, const DBT* pdata, DBT* skey)
{
    u_int offset;

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

DB_CTX*
gaia_setup_database(const char* directory)
{
    DB_CTX* context = (DB_CTX*)malloc(sizeof *context);
    db_init(&context->dbp, directory, "stars.db", DB_CREATE, DB_BTREE);
    db_init(&context->sdbp, directory, "morton_index.sdb", DB_CREATE, DB_BTREE);

    context->dbp->associate(context->dbp, NULL, context->sdbp, get_id_callback,
                            0);

    return context;
}

int
gaia_close_database(DB_CTX* context)
{
    int ret;

    ret = db_close(context->sdbp);
    if (ret != 0) {
        printf("Error close secondary db");
    }

    ret = db_close(context->dbp);
    if (ret != 0) {
        printf("Error close primary db");
    }

    free(context);

    return ret;
}

int
gaia_new_star(DB* dbp, u_int64_t id, double x, double y, double z,
              u_int32_t colour, u_int64_t morton_index)
{
    if (morton_index == 0) {
        morton_index = id;
    }

    STAR star = { id, x, y, z, colour, morton_index };
    int ret;

    printf("%d\n", sizeof star);
    printf("%d\n", sizeof(STAR));

    ret = db_insert(dbp, &star, sizeof star, &star.id, sizeof star.id);

    return ret;
}

STAR*
gaia_get_star(DB* dbp, u_int64_t id)
{
    void* data = db_get(dbp, &id, sizeof id);
    STAR* star = (STAR*)data;

    return star;
}

STAR*
gaia_get_star_by_morton(DB* sdbp, u_int64_t id)
{

    STAR* star = (STAR*)db_get(sdbp, &id, sizeof id);
    if (star == NULL) {
        printf("ERROR: star is null");
    }
    return star;
}

int
gaia_delete_star(DB* dbp, u_int64_t id)
{
    int ret;
    ret = db_delete(dbp, &id, sizeof id);

    return ret;
}

int
gaia_update_star_morton(DB* dbp, u_int64_t id, u_int64_t morton_index)
{

    return 0;
}
