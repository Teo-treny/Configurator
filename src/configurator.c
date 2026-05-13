#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "configurator.h"

static void *global_config = NULL;              // Pointeur générique vers la configuration
static config_handler_t global_handler = NULL;  // fonction de traduction
static size_t config_size = 0;                  // Taille de la configuration

int configurator_init(
    FILE *f, 
    size_t struct_size, 
    config_handler_t handler, 
    const void *default_values
) {

    // on copie la taille de la structure
    config_size = struct_size;
    global_handler = handler;

    // allocation de mémoire pour la configuration
    global_config = malloc(config_size);
    if (global_config == NULL)
        return -1;

    // On applique la config par défaut, s'il y en a une
    if (default_values) {
        memcpy(global_config, default_values, struct_size);
    }

    // on appelle le parser
    return parse_config_file(f, handler, global_config);
}

void configurator_clean(void) {
    if (global_config) {
        free(global_config);
        global_config = NULL;
    }
}

const void *configurator_get_config(void) {
    return global_config;
}

void configurator_update_all_config(const void *new_config) {
    if (!new_config || !global_config) 
        return;

    memcpy(global_config, new_config, config_size);
}

int configurator_set_value(const char *key, const char *value) {
    if (!global_config || !global_handler || !key || !value)
        return -1;

    // on appelle tout simplement la fonction user
    // pour mettre à jour la valeur
    int result = global_handler(global_config, key, value);

    return result;
}
