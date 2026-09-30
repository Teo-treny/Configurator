# Configurator
Une librairie C pensée légère et dynamique sur base de dictionnaire pour lire, stocker, modifier et sauvegarder des configurations.

## Points clés
-**Agnostique** : La librairie s'adapte à n'importe quelle structure de projet. Vous demandez une clef, la librairie vous la donne.
-**Zéro dépendance** : La librairie ne dépend que des librairies standard C (libc). Elle s'intègre dans n'importe quel projet. Normalement compatible windows et linux.
-**Haute performance** : Très peu d'accès mémoire complexe, offrant un accès et une édition rapide des champs.
-**Persistance** : Enregistrez toutes les modifications d'un seul coût

## Metrique de couplage
L'un des objectifs de cette lib est de minimiser l'impact architectural sur le code client. La librairie respecte des métriques de couplage strictes :
- **Couplage efférent (Ce) : 0**, la librairie ne dépend d'aucun module externe (hormis les lib standards)
- **Couplage afférent (Ca) : élevé**, conçue pour être appelée massivement par l'application cliente
- **Indice d'instabilité (I) : ~0**, la librairie est très stable

## Guide d'utilisation (quick start)
Grâce à son modèle en dictionnaire, l'utilisation est immédiate : on charge, on lit, on modifie.
Le dictionnaire est persistant : on peut le lire depuis n'importe quel endroit **tant qu'il a été initialisé au moins une fois**.

### Exemple
```cfg
# Fichier de configuration
ipv4 = 192.168.1.10
port = 8000
server_name = Mon super serveur
```

```c
#include <stdio.h>
#include <configurator/configurator.h>

int main(void) {
    // 1. Initialisation de la librairie au démarrage
    FILE *f = fopen("config.cfg", "r");
    if (f) {
        configurator_init(f);
        fclose(f); // Le fichier peut être fermé immédiatement
    }

    // 2. LECTURE (Avec valeurs par défaut sécurisées)
    int port = configurator_get_int("port", 8080);
    const char *ipv4 = configurator_get_string("ipv4", "127.0.0.1");
    const char *server = configurator_get_string("server_name", "Serveur par defaut");
    
    printf("Serveur démarré : %s (%s:%d)\n", server, ipv4, port);
    
    // 3. MODIFICATION À LA VOLÉE
    configurator_set_int("port", 9050);
    configurator_set_string("nouvelle_cle", "Ajout dynamique !");

    // 4. SAUVEGARDE SUR LE DISQUE
    FILE *f_save = fopen("config_backup.cfg", "w");
    if (f_save) {
        configurator_save_config(f_save);
        fclose(f_save);
    }

    // 5. Nettoyage en fin de programme
    configurator_clean();
    return 0;
}
```
test
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
int configurator_init(FILE *f);
```
Initialise le dictionnaire en mémoire et parse le fichier fourni. Peut prendre `NULL` pour initialiser un dictionnaire vide. Retourne le nombre de paramètres lus ou `-1` en cas d'erreur.

### ``configurator_get_string / configurator_get_int``
```c
const char *configurator_get_string(const char *key, const char *default_value);
int configurator_get_int(const char *key, int default_value);
```
Cherche une clé dans le dictionnaire. Si la clé est introuvable ou si une erreur survient, la fonction retourne silencieusement la valeur par défaut spécifiée en second paramètre.

### ``configurator_set_string / configuration_set_int``
```c
int configurator_set_string(const char *key, const char *value);
int configurator_set_int(const char *key, int value);
```
Ajoute une nouvelle paire ou met à jour une clé existante dans le dictionnaire en mémoire. Retourne `0` en cas de succès, `-1` en cas d'erreur.

### ``configurator_save_config``
```c
int configurator_save_config(FILE *f);
```
Parcourt le dictionnaire en mémoire et écrit son contenu sous forme `clé = valeur` dans le flux de fichier fourni (qui doit être ouvert en mode écriture `w`).

### ``configurator_clean``
```c
void configurator_clean(void);
```
Libère l'intégralité de la mémoire allouée pour le dictionnaire. À appeler impérativement avant la fin du programme pour éviter les fuites de mémoire.