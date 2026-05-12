# Configurator
Une librairie C, thread-safe, pour lire, stocker et modifier des fichiers de configurations à la volée.

## Points clés
-**Agnostique** : La librairie s'adapte à n'importe quelle structure de projet.
-**Haute performance** : Très peu d'accès mémoire complexe, offrant un accès et une édition rapide des champs.
-**Thread-safe** : Utilisation d'un verrou *read-write* (``pthread_rwlock_t``). Autorise une infinité de lecture concourante mais bloque tout en cas d'écriture.
-**Tolérance** : Nettoyage automatique des espaces / tabulations, ignorent les lignes commentées ou vides.

## Guide d'utilisation (quick start)
L'utilisation se fait en trois étapes : définir sa structure de configuration, créer un traducteur (fonction ``handler``) et initialiser le configurateur.

### Exemple
```cfg
# Fichier de configuration
ipv4 = 192.168.1.10
port = 8000
server_name = Mon super serveur
```

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <configurator/configurator.h>

// 1. Création de la structure de configuration
typedef struct {
    char ipv4[64];
    int port;
    char server_name[256];
} AppConfig_t;

// 2. Définiton de la fonction de traduction
int my_config_handler(void *user_data, const char *key, const char *value) {
    AppConfig_t *cfg = (AppConfig_t *)user_data;

    if (strcmp(key, "ipv4") == 0) {
        strncpy(cfg->ipv4, value, 63);
        return 0;
    }
    else if (strcmp(key, "port") == 0) {
        cfg->port = atoi(value);
        return 0;
    }
    else if (strcmp(key, "server_name") == 0) {
        strncpy(cfg->server_name, value, 255);
        return 0;
    }
    return -1;
}

int main(void) {
    // Initialisation de la librairie
    configurator_init(
        "config.cfg",           // Fichier de configuration
        sizeof(AppConfig_t),    // Taille de la structure de données
        my_config_handler,      // Pointeur vers la fonction de traduction
        NULL                    // Valeurs par défaut de la configuration (OPTIONNEL)
    );

    // Exemple de lecture de paramètres
    const AppConfig_t *my_cfg = (const AppConfig_t *)configuration_acquire_read();

    // Traitement des paramètres
    // ...

    // OBLIGATOIRE : on rend le verrou
    configurator_release_read();

    // Exemple de modification à la volée
    // Modifie la valeur du port, mais il faut redémarrer votre serveur tout de même
    configurator_set_value("port", "9050");

    // Nettoyage en fin de programme
    configurator_clean();
    return 0;
}
```

## Compilation et installation
### Prérequis
Pour générer une librairie statique (``.a``) ou dynamique (``.so``), il faut :
- ``gcc``
- ``make``
- ``criterion`` (pour les tests unitaires uniquement)
  
### Les commandes compilation
```bash
# Compiler la librairie (.a et .so)
make all

# Lancer les TU
make test

# Nettoyer les fichiers générés
make clean
```

*Les fichiers compilés sont construits dans le dossier build/*

## Références

### ``configurator_init``
```c
int configurator_init(const char *filename, size_t struct_size, config_handler_t handler, const void *default_values);
```
Alloue la mémoire, applique d'éventuelles valeurs par défaut et parse le fichier de configuration donnée. A appeler une fois au démarrage.

### ``configurator_acquire_read``
```c
const void *configurator_acquire_read(void);
```
Pose un verrou de lecture et retourne un pointeur en lecture seule vers la configuration. Bloque les écritures mais permet des lectures simultanées. **Doit toujours être suivi d'un appel ``configurator_release_read``**.

### ``configurator_release_read``
```c
void configurator_release_read(void);
```
Lève le verrou de lecture posé par la fonction ``configurator_acquire_read``.

### ``configurator_set_value``
```c
int configurator_set_value(const char *key, const char *value);
```
Prend un verrou d'écriture exclusif et utilise le `handler` pour modifier une valeur spécifique en **uniquement en mémoire**. 

### ``configurator_update_all_config``
```c
void configurator_update_all_config(const void *new_config);
```
Prend un verrou d'écriture exclusif et met à jour toute la structure de configuration **uniquement en mémoire**.

### ``configurator_clean``
```c
void configurator_clean(void);
```
A appeler en fin de programme, elle nettoie correctement et ferme la librairie.