#include <configurator/configurator.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// On définit notre structure de configuration
typedef struct {
    char ipv4[64];
    int port;
    char server_name[256];
} ConfApp_t;

// On définit notre fonction gérant les paires clé=valeur
int my_config_handler(void *conf_app, const char *key, const char *value) {
    // On vérifie bien les paramètres
    if (!conf_app || !key || !value)
        return -1;

    // On cast le pointeur void *
    ConfApp_t *myConfApp = (ConfApp_t *)conf_app;

    // On vérifie la clé pour attribuer la valeur
    if (strcmp(key, "ipv4") == 0) {
        strncpy(myConfApp->ipv4, value, 63);
    }
    else if (strcmp(key, "port") == 0) {
        myConfApp->port = atoi(value);
    }
    else if (strcmp(key, "server_name") == 0) {
        strncpy(myConfApp->server_name, value, 255);
    }
    else {
        return -1;  // Clé=valeur inconnue
    }

    return 0;
}

static void read_config(void) {
    // Je ne recommande pas de stocker le pointeur renvoyé
    // Mais si vous le faites, assigner le à NULL pour éviter des conneries plus tard
    ConfApp_t *myConfig = (ConfApp_t *)configurator_acquire_read();

    printf(
        "CONFIGURATION :\n\tipv4 : %s\n\tport : %d\n\tserver_name : %s\n",
        myConfig->ipv4, myConfig->port, myConfig->server_name
    );

    // Important, il faut rendre la lecture
    myConfig = NULL; // Comme ça on est sur de toucher à rien
    configurator_release_read();
} 

int main(void) {

    // On initialise le configurateur
    configurator_init(
        "config.cfg",
        sizeof(ConfApp_t),
        my_config_handler,
        NULL
    );

    // On lit la configuration
    read_config();  // fonction à moi, pas de la lib

    // On met à jour un champ pour tester
    configurator_set_value("port", "15");
    read_config();

    // On le remet à son ancienne valeur
    configurator_set_value("port", "10");
    read_config();

    // Fin du programme
    configurator_clean();
    return 0;
}