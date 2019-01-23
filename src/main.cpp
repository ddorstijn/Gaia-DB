// #include "gaia_read.hpp"
#include <stdio.h>

#include "db.h"

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

DB* dbp;   /* Handle of the database to be updated. */
DBC* dbcp; /* Cursor used for scanning over the database. */
DBT key;   /* The key to dbp->put()/from dbcp->get(). */
DBT data;  /* The data to dbp->put()/from dbcp->get(). */

int ret;                         /* Return code from call into Berkeley DB. */
const char* db_name = "test.db"; /* The name of the database to use. */
const char* progname = "ex_access"; /* Program name. */

typedef struct _star
{
    u_int64_t id;           // ID extracted from dataset
    double x;               // X position star
    double y;               // Y position star
    double z;               // Z position star
    u_int32_t colour;       // Colour of the star in hex converted to int
    u_int64_t morton_index; // Morton-code of the star in a 3d-grid
} SStar;

void
close_db()
{
    (void)dbp->close(dbp, 0);
}

int
main()
{
    //     DB_CTX* db_ctx;
    //     db_ctx = gaia_setup_database("");
    //     gaia_new_star(db_ctx->dbp, 21312, 1.2, 2.3, 4.5, 128, 0);
    //     STAR* star = gaia_get_star(db_ctx->dbp, 21312);
    //     printf("%f, %f", star->x, star->y);

    /* Create a database handle, to be configured, then opened. */
    if ((ret = db_create(&dbp, NULL, 0)) != 0) {
        fprintf(stderr, "%s: db_create: %s\n", progname, db_strerror(ret));
        return (EXIT_FAILURE);
    }

    /*
     * Prefix any error messages with the name of this program and a ':'.
     * Setting the errfile to stderr is not necessary, since that is the
     * default; it is provided here as a placeholder showing where one
     * could direct error messages to an application-specific log file.
     */
    dbp->set_errpfx(dbp, progname);
    dbp->set_errfile(dbp, stdout);

    /*
     * Configure the database to use:
     *	A database page size of 1024 bytes. It could be at most 64KB.
     *	A database cache size of 0 GB and 32KB, in a single region.
     */
    if ((ret = dbp->set_pagesize(dbp, 1024)) != 0) {
        dbp->err(dbp, ret, "set_pagesize");
        close_db();
        return EXIT_FAILURE;
    }
    if ((ret = dbp->set_cachesize(dbp, 0, 32 * 1024, 1)) != 0) {
        dbp->err(dbp, ret, "set_cachesize");
        close_db();
        return EXIT_FAILURE;
    }

    /*
     * Now open the configured handle. The DB_BTREE specifies that it
     * it will be a btree. You could experiment by changing that to DB_HASH
     * in order to see how the behavior changes.
     */
    if ((ret = dbp->open(dbp, NULL, db_name, NULL, DB_BTREE, DB_CREATE,
                         0664)) != 0) {
        dbp->err(dbp, ret, "%s: open", db_name);
        close_db();
        return EXIT_FAILURE;
    }

    /*
     * Insert records into the database, where the key is the user input
     * and the data is the user input in reverse order.
     * Zeroing the DBTs prepares them for the dbp->put() calls below.
     */
    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));

    SStar star_data = { 12341, 1.2, 3.4, 5.6, 32131, 0 };

    key.data = &star_data.id;
    key.size = sizeof(u_int64_t);

    data.data = &star_data;
    data.size = sizeof(SStar);

    /*
     * Add the record to the database. The DB_NOOVERWRITE flag
     * causes DB_KEYEXIST to be returned if the key had already
     * been put, either by a previous dbp->put() of this process,
     * or because a prior execution of this program had inserted
     * that key. Without DB_NOOVERWRITE, a dbp->put() of an already
     * existing key replaces the old record with the new one.
     */
    ret = dbp->put(dbp, NULL, &key, &data, DB_NOOVERWRITE);
    if (ret != 0) {
        /*
         * Some kind of error was detected during the attempt to
         * insert the record. The err() function is printf-like.
         */
        // dbp->err(dbp, ret, "DB(%s)->put(%s, %s)", db_name, key.data,
        // data.data);
        if (ret == DB_KEYEXIST) {
            printf("Key already exsists");
        } else {
            close_db();
            return EXIT_FAILURE;
        }
    }

    ret = dbp->get(dbp, NULL, &key, &data, 0);
    if (ret != 0) {
        /*
         * Some kind of error was detected during the attempt to
         * insert the record. The err() function is printf-like.
         */
        dbp->err(dbp, ret, "DB(%s)->put(%s, %s)", db_name, key.data, data.data);
        close_db();
        return EXIT_FAILURE;
    }

    SStar* dat = (SStar*)data.data;
    printf("%d\n", *dat);

    close_db();
}