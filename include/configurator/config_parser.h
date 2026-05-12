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


/**
 * @brief Type de fonction de callback attendue
 * Cette fonction de callback est appelée à chaque paire "clé=valeur" lue, elle traitera par elle même ces valeurs
 * @param user_data Pointeur vers un void * pour que l'utilisateur puisse passer ce qu'il veut dans son handler. Peut etre laissé à NULL
 * @param key Chaine de caractère contenant la clé de la ligne de configuration
 * @param value Chaine de caractère contenant la valeur associée à la clé de la ligne de configuration
 * @return  0 si succès
 *          -1 si erreur
 * 
 */
typedef int (*config_handler_t) (void *user_data, const char* key, const char *value);

/**
 * @brief Fonction de parsing. Elle va lire un fichier et appeler handler à chaque paire clé=valeur
 * 
 * @param filename Fichier de configuration à lrie
 * @param handler Fonction de callback propre à l'utilisateur
 * @param user_data Pointeur void * qui sera passé au handler, faites-en ce que vous en voulez
 * @return int Nombre de paramètres lus
 */
int parse_config_file(const char *filename, config_handler_t handler, void *user_data);

#endif