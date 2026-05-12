#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "config_parser.h"

#define MAX_BUFFER 256

/**
 * @brief Retourne un pointeur vers une substring de la string originelle en supprimant les espaces.
 * Issu de : https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
 *
 * @warning Attention à la manipulation de pointeur avec cette fonction ! 
 * 
 * @param string Châine à traiter
 * @return char* Substring traitée
 */
static char *whitespace_trimmer(char *str) {
    char *end;

    // isspace retourne un nombre positif si la classe testée (ici un espace) correspond
    // Donc tant qu'on a des espaces, on avance le pointeur de chaine
    while(isspace((unsigned char) *str)) str++;

    // Arrivé ici, si on a 0 c'est que la chaine est constitué à 100% d'espace
    if (*str == 0)  
        return str;

    end = str + strlen(str) - 1; // on se positionne en fin de chaine

    // Maintenant on retire les espaces depuis la fin de chaine
    while (end > str && isspace((unsigned char) *end))
        end--;

    // On finit proprement la chaine
    end[1] = '\0';

    return str;
}

int parse_config_file(const char *filename, config_handler_t handler, void *user_data) {
    // On vient lire ligne par ligne le fichier de configuration
    // Si on tombe sur une ligne vide ou un ligne commençant par #, on ignore
    // Si on tombe sur une ligne du style "clé=valeur" il faut la traiter
    // On récupère la clé, la valeur et on appelle le handler avec en paramètre

    // Attention il faut supprimer les espaces dans les lignes lues

    // 1. Il faut ouvrir le fichier
    // bah oui on va pas deviner les lignes...


    int count = 0;
    FILE *file = fopen(filename, "r"); // en mode lecture
    if (file == NULL)
        return -1; // on n'a pas réussi à ouvrir le fichier

    // Maintenant on lit en boucle jusqu'à la fin

    char read[MAX_BUFFER];
    while(fgets(read, MAX_BUFFER, file)) {

        // on "nettoie" la ligne lue
        char *cleaned = whitespace_trimmer(read);

        // si la ligne est vide OU que c'est un commentaire, on l'ignore
        if (cleaned[0] == '\0' || cleaned[0] == '#')
            continue;   // prochain pas de boucle

        // Ici on devrait avoir quelque chose de la forme
        // clé=valeur
        // il faut séparer la clé de la valeur en cherchant '='
        
        count++;    // On incrémente le compte

        char *delimiter = strchr(cleaned, '='); // on récupère la position du '='
        if (delimiter != NULL) {
            *delimiter = '\0'; // On finit la string à la place du '='
            char *key = whitespace_trimmer(cleaned);    // On renettoie un coup
            char *value = whitespace_trimmer(delimiter+1);

            // debug
            // printf("[parser] %s = %s\n", key, value);

            // On a la clé et la valeur, la suite c'est pour l'utilisateur
            // (ca rime !)
            if (handler) {
                // Si on rencontre une erreur
                if (handler(user_data, key, value) == -1) {
                    // on passe juste à la prochaine itération, mais on va quand même signaler l'erreur
                    fprintf(stderr, "[config_parser] Erreur du handler sur la clé %s\n", key);
                }
            }
        }
    }

    // on ferme le fichier et on sort
    fclose(file);
    return count;
}
