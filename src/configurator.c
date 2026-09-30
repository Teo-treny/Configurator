#include <string.h>
#include <stdlib.h>

#include "configurator.h"
#include "config_parser.h"

#ifdef _WIN32
// Réimplémentation strndup pour windows
char *strndup(const char *s, size_t n) {
    /*
    if (s == NULL)
        return NULL;
    */
   
    size_t len = 0;
    while (len < n && s[len] != '\0')
        len++;

    char *new_str = (char *)malloc(len + 1);
    if (new_str == NULL)
        return NULL;
    
    memcpy(new_str, s, len);
    new_str[len] = '\0';

    return new_str;
}

#endif


// Structure de données interne
typedef struct config_entry_t {
    char *key;
    char *value;
    struct config_entry_t *next;
} config_entry_t;

static config_entry_t *head = NULL; // Tete du dictionnaire

static config_entry_t *find_or_create(const char *key) {
    config_entry_t *curr = head;
    
    // find
    while (curr) {
        if (strcmp(curr->key, key) == 0)
            return curr;
        curr = curr->next;
    }

    // or create
    config_entry_t *new = (config_entry_t *)malloc(sizeof(config_entry_t));
    if (!new) 
        return NULL;

    // strdup fait une duplication d'une string
    new->key = strndup(key, CONFIGURATOR_MAX_BUFFER);
    new->value = NULL;
    
    new->next = NULL;
    
    if (head == NULL)
        head = new;
    else {
        config_entry_t *tail = head;
        while (tail->next != NULL)
            tail = tail->next;
        tail->next = new;
    }

    return new;
}

int configurator_init(FILE *f) {
    if (!f)
        return -1;

    return parse_config_file(f, configurator_set_string);
}

void configurator_clean(void) {
    config_entry_t *curr = head;
    while (curr) {
        config_entry_t *next = curr->next;
        free(curr->key);
        if (curr->value) free(curr->value);
        free(curr);
        curr = next;
    }
    head = NULL;
}

const char *configurator_get_string(const char *key, const char *default_value) {
    if (!key)
        return default_value;

    config_entry_t *curr = head;

    while (curr) {
        if (strcmp(curr->key, key) == 0)
            return curr->value;
        curr = curr->next;
    }

    return default_value;
}

int configurator_set_string(const char *key, const char *value) {
    if (!key || !value)
        return -1;

    config_entry_t *node = find_or_create(key);
    if (!node) 
        return -1;

    if (node->value)
        free(node->value);

    node->value = strndup(value, CONFIGURATOR_MAX_BUFFER);

    return 0;
}

int configurator_get_int(const char *key, int default_value) {
    const char *val_str = configurator_get_string(key, NULL);
    if (val_str)
        return atoi(val_str);
    return default_value;
}

int configurator_set_int(const char *key, int value) {
    if (!key)
        return -1;

    char buffer[CONFIGURATOR_MAX_BUFFER];
    snprintf(buffer, sizeof(buffer), "%d", value);
    return configurator_set_string(key, buffer);
}

int configurator_save_config(FILE *f) {
    if (!f)
        return -1;

    config_entry_t *curr = head;
    while (curr) {
        fprintf(f, "%s = %s\n", curr->key, curr->value ? curr->value : "");
        curr = curr->next;
    }

    return 0;
}
