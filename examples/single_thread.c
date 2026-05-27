#include <configurator/configurator.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    printf("Debut du test\n");

    // On ouvre le fichier
    FILE *f = fopen("config.cfg", "r");
    if (!f) {
        printf("Erreur ouverture fichier\n");
        return -1;
    }

    // Initialisation du configurator
    if (configurator_init(f) == -1) {
        printf("configurator_init error\n");
        fclose(f);
        return -1;
    }

    // A partir de là, on peut fermer le fichier car la configuration est chargée en mémoire
    fclose(f);

    // Lecture des param
    int cfg_port = configurator_get_int("port", 5000);
    const char *cfg_ipv4 = configurator_get_string("ipv4", "127.0.0.1");
    const char *cfg_server_name = configurator_get_string("server_name", "default_string");

    printf(
        "Configuration chargée :\n\t- ipv4 : %s\n\t- port : %d\n\t- server_name : %s\n\n",
        cfg_ipv4, cfg_port, cfg_server_name
    );

    configurator_set_int("port", 2000);
    configurator_set_string("ipv4", "192.168.5.3");


    // On ouvre un fichier en mode écriture
    f = fopen("config_clone.cfg", "w");
    if (!f) {
        printf("Erreur ouverture fichier\n");
        return -1;
    }

    if (configurator_save_config(f) != 0) {
        printf("configurator_save_config error\n");
        fclose(f);
        return -1;
    }

    fclose(f);

    configurator_clean();

    return 0;
}