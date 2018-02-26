#include <stdio.h>
#include "catalog.h"

void callback(void *arg) {
    printf("file changed! %d\n", *((int *) arg));
}

int main(int argc, char *argv[]) {

    Catalog catalog;
    int a = 0,
        b = 1,
        c = 2,
        d = 3;

    if (!catalog_init(&catalog)) {
        perror("catalog_init");
        return 1;
    }

    if (!catalog_add(&catalog, "a.txt", callback, &a) ||
        !catalog_add(&catalog, "b.txt", callback, &b) ||
        !catalog_add(&catalog, "c.txt", callback, &c) ||
        !catalog_add(&catalog, "d.txt", callback, &d)) {

        perror("catalog_add");
        catalog_destroy(&catalog);
        return 1;
    }

    while (1) {

        if (!catalog_service(&catalog)) {
            perror("catalog_service");
            catalog_destroy(&catalog);
            return 1;
        }

        sleep(1);
    }

    catalog_destroy(&catalog);

    return 0;

}
