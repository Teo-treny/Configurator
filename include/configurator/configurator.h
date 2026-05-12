/**
 * @file configurator.h
 * @author Téo Trény
 * @brief Coffre fort de la librairie. Il garde en mémoire la configuration et gère les accès (R/W) de manière thread-safe.
 * Non compatible windows pour l'instant
 * @version 0.1
 * @date 2026-04-21
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <stddef.h>
#include "config_parser.h"

/**
 * @brief Permet d'initialiser le gestionnaire de configuration
 * 
 * @param filename Nom du fichier de configuration
 * @param struct_size Taille de la structure de configuration du projet de l'utilisateur (sizeof(AppConfig))
 * @param handler Fonction de traduction
 * @param default_values Structure par défaut, si jamais une lecture de ligne rate c'est bien de la fournir
 * @return int 0 si succès, -1 si erreur
 */
int configurator_init(const char *filename, size_t struct_size, config_handler_t handler, const void *default_values);

/**
 * @brief Fonction à appeler en fin de programme pour bien nettoyer le configurateur
 * 
 */
void configurator_clean(void);

/**
 * @brief Donne l'accès en écriture au pointeur renvoyé.
 * /!\ Attention à bien appeler release_read après /!\
 * /!\ Je conseille de ne pas sauvegarder le pointeur dans une variable /!\
 * 
 * @return const void* Pointeur de la configuration (faites pas n'imp !)
 */
const void *configurator_acquire_read(void);

/**
 * @brief Va de paire avec acquire_read. Il faut l'appeler pour rendre la ressource.
 * 
 */
void configurator_release_read(void);

/**
 * @brief Met à jour toute la configuration en une seule fois.
 * 
 * @param new_config Nouvelle configuration
 */
void configurator_update_all_config(const void *new_config);

/**
 * @brief Met à jour une valeur du configurateur.
 * La fonction se base sur la fonction user donnée à l'initialisation.
 * 
 * @param key Clé à mettre à jour
 * @param value Valeur à mettre à jour
 * @return int -1 si erreur, 0 sinon
 */
int configurator_set_value(const char *key, const char *value);


#endif