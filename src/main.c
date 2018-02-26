#include <stdio.h>
#include "catalog.h"

void callback(void *arg) {
    printf("file changed!\n");
}

int main(int argc, char *argv[]) {

    Catalog catalog;

    if (!catalog_init(&catalog)) {
        perror("catalog_init");
        return 1;
    }

    if (!catalog_add(&catalog, "test.txt", callback, NULL)) {
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
