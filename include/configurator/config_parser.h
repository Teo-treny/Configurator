/**
 * @file config_parser.h
 * @author Téo Trény
 * @brief "Moulinette" lisant les fichiers de configuration. Il fait appel à une fonction de callback, à définir par l'utilisateur.
 * Cette fonction de callback est appelée à chaque paramètre lu et offre le traitement à l'utilisateur.
 * @version 0.1
 * @date 2026-04-21
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <stdio.h>

#define CONFIGURATOR_MAX_BUFFER 256

/**
 * @brief Type de callback attendue
 * 
 */
typedef int (*config_handler_t)(const char *key, const char *value);

/**
 * @brief Fonction de parsing. Elle va lire un fichier et appeler handler à chaque paire clé=valeur
 * 
 * @param file pointeur vers le fichier
 * @param handler Fonction de callback appelée à chaque paire lue
 * @return int Nombre de paramètres lus
 */
int parse_config_file(FILE *file, config_handler_t handler);

#endif