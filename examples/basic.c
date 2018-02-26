#include <stdio.h>
#include "catalog.h"

void callback(void *arg) {
    printf("file '%c.txt' was modified!\n", (char) (long int) arg);
}

int main(int argc, char *argv[]) {

    Catalog catalog;

    if (!catalog_init(&catalog)) {
        perror("catalog_init");
        return 1;
    }

    if (!catalog_add(&catalog, "a.txt", callback, (void *) 'a') ||
        !catalog_add(&catalog, "b.txt", callback, (void *) 'b') ||
        !catalog_add(&catalog, "c.txt", callback, (void *) 'c') ||
        !catalog_add(&catalog, "d.txt", callback, (void *) 'd')) {

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
