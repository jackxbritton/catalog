#ifndef CATALOG_H
#define CATALOG_H

#include <sys/inotify.h>
#include <unistd.h>

struct CatalogEntry {
    int wd;
    void (*callback)();
    void *callback_arg;
};
typedef struct CatalogEntry CatalogEntry;

struct Catalog {

    int fd;

    CatalogEntry *table;
    size_t table_allocated;
    size_t table_count;

};
typedef struct Catalog Catalog;

int catalog_init(Catalog *catalog);
void catalog_destroy(Catalog *catalog);

int catalog_add(Catalog *catalog, const char *filename, void (*callback)(), void *callback_arg);
int catalog_service(Catalog *catalog);

#endif
