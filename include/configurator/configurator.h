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

#include <stdio.h>
#include <stddef.h>

/**
 * @brief Permet d'initialiser le gestionnaire de configuration
 * 
 * @param f Fichier de configuration
 * @return int 0 si succès, -1 si erreur
 */
int configurator_init(FILE *f);

/**
 * @brief Fonction à appeler en fin de programme pour bien nettoyer le configurateur
 * 
 */
void configurator_clean(void);

const char *configurator_get_string(const char *key, const char *default_value);

int configurator_set_string(const char *key, const char *value);

int configurator_get_int(const char *key, int default_value);

int configurator_set_int(const char *key, int value);

int configurator_save_config(FILE *f);

#endif