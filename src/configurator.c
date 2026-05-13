#define _GNU_SOURCE
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "configurator.h"

static void *global_config = NULL;              // Pointeur générique vers la configuration
static config_handler_t global_handler = NULL;  // fonction de traduction
static size_t config_size = 0;                  // Taille de la configuration
static pthread_rwlock_t config_rwlock = PTHREAD_RWLOCK_INITIALIZER;

int configurator_init(
    FILE *f, 
    size_t struct_size, 
    config_handler_t handler, 
    const void *default_values
) {

    // Protection fichier null
    if (!f) return -1;

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
    pthread_rwlock_destroy(&config_rwlock);
    if (global_config) {
        free(global_config);
        global_config = NULL;
    }
}

const void *configurator_acquire_read(void) {
    if (global_config == NULL)
        return NULL;

    // On prend la ressource en mode LECTURE
    pthread_rwlock_rdlock(&config_rwlock);

    // on retourne direct le pointeur
    // attention c'est dangereux, il faut bien appeler la fonction
    // release_read après utilisation sinon ça fait le bazar !!
    return global_config;
}

void configurator_release_read(void) {
    pthread_rwlock_unlock(&config_rwlock);
}

void configurator_update_all_config(const void *new_config) {
    if (!new_config || !global_config) 
        return;

    // on verrouille la ressource en mode écriture
    pthread_rwlock_wrlock(&config_rwlock);
    memcpy(global_config, new_config, config_size);
    pthread_rwlock_unlock(&config_rwlock);
}

int configurator_set_value(const char *key, const char *value) {
    if (!global_config || !global_handler || !key || !value)
        return -1;

    // On prend la ressource en mode écriture
    pthread_rwlock_wrlock(&config_rwlock);

    // on appelle tout simplement la fonction user
    // pour mettre à jour la valeur
    int result = global_handler(global_config, key, value);

    // on déverouille
    pthread_rwlock_unlock(&config_rwlock);

    return result;
}

int configurator_save_all(void) {
    if (!global_config) 
        return -1;

    // Verouillage en lecture
    pthread_rwlock_rdlock(&config_rwlock);

    // On va réécrire un fichier de configuration mais en mode temporaire
    // comme ça, en cas d'erreur, on n'a rien fait à l'original et l'utilisateur
    // n'a aucun problème.
    // Par contre, on a besoin de parcourir le fichier originel pour savoir
    // les clés de la structure de configuration. Ca reste agnostique comme ça
    // mais c'est pas super efficace.
}