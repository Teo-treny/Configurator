#include <criterion/criterion.h>
#include <string.h>
#include <stdio.h>

#include <configurator/configurator.h>

void setup(void) {
    FILE *f = fopen("dummy_test.cfg", "w");
    fprintf(f, "# Fichier de test\n");
    fprintf(f, " port      = 8080\n");
    fprintf(f, "hostname =           localhost    \n");
    fclose(f);
}

void clean(void) {
    remove("dummy_test.cfg");
    configurator_clean();
}

// Configuration des séquences de test de criterion
TestSuite(configurator_suite, .init = setup, .fini = clean);

/**
 * @brief Test d'initialisation et lecture
 * 
 */
Test(configurator_suite, init_et_lecture) {

    // Ouverture du fichier
    FILE *f = fopen("dummy_test.cfg", "r");
    cr_assert_not_null(f, "Erreur d'ouverture du fichier de test");

    // Test de l'initialisation
    int params_lus = configurator_init(f);
    cr_assert_eq(params_lus, 2, "Le parser aurait dû trouver 2 paramètres valides. Paramètres trouvés : %d", params_lus);
    fclose(f);

    // Test de la lecture
    int port = configurator_get_int("port", 0);
    const char *hostname = configurator_get_string("hostname", NULL);

    cr_assert_eq(port, 8080, "Le port lu devrait être 8080. Lu : %d", port);
    cr_assert_str_eq(hostname, "localhost", "Le hostname lu devrait être localhost. Lu : [%s]", hostname);

}

/**
 * @brief Initialisation avec fichier NULL mais ça doit rester fonctionnel
 * 
 */
Test(configurator_suite, init_null) {
    int res = configurator_init(NULL);
    cr_assert_eq(res, -1, "L'init n'a pas retourné -1. Lu : [%d]", res);

    // on doit quand même pouvoir utiliser le dictionnaire de la lib
    res = configurator_set_int("port", 5000);
    cr_assert_eq(res, 0, "Le setter a retourné une erreur. Lu : [%d]", res);

    int port = configurator_get_int("port", 0);
    cr_assert_eq(port, 5000, "Le getter n'a pas renvoyé la bonne valeur. Lu : [%d]", port);

}

/**
 * @brief Initialiser des données et nettoyer le configurator, normalement on ne doit pouvoir plus lire 
 * 
 */
Test(configurator_suite, clean) {
    // Ouverture du fichier
    FILE *f = fopen("dummy_test.cfg", "r");
    cr_assert_not_null(f, "Erreur d'ouverture du fichier de test");

    // Test de l'initialisation
    int params_lus = configurator_init(f);
    cr_assert_eq(params_lus, 2, "Le parser aurait dû trouver 2 paramètres valides. Paramètres trouvés : %d", params_lus);
    fclose(f);

    configurator_clean();

    int port = configurator_get_int("port", 0);
    const char *hostname = configurator_get_string("hostname", "default");

    cr_assert_eq(port, 0, "Le port lu devrait être 0. Lu : %d", port);
    cr_assert_str_eq(hostname, "default", "Le hostname lu devrait être default. Lu : [%s]", hostname);
}

/**
 * @brief Tester le configurator sans l'init (sans charger de fichier)
 * 
 */
Test(configurator_suite, no_init) {
    // si on init pas, on aura que les valeurs par défaut

    int port = configurator_get_int("port", 0);
    const char *hostname = configurator_get_string("hostname", "default");

    cr_assert_eq(port, 0, "Le port renvoyé n'est pas à la valeur par défaut. Lu : [%d]", port);
    cr_assert_str_eq(hostname, "default", "Le hostname renvoyé n'est pas à la valeur par défaut. Lu : [%s]", hostname);
}

/**
 * @brief Avec des clefs inconnues 
 * 
 */
Test(configurator_suite, unknown_key) {
    // Ouverture du fichier
    FILE *f = fopen("dummy_test.cfg", "r");
    cr_assert_not_null(f, "Erreur d'ouverture du fichier de test");

    // Test de l'initialisation
    int params_lus = configurator_init(f);
    cr_assert_eq(params_lus, 2, "Le parser aurait dû trouver 2 paramètres valides. Paramètres trouvés : %d", params_lus);
    fclose(f);

    // demander une clef qui n'existe pas dans le fichier
    int res = configurator_get_int("cle_bizarre", 123);
    cr_assert_eq(res, 123, "La clef inexistante n'a pas retourné la valeur attendue. Lu : [%d]", res);

    // demander une clef null
    const char *str = configurator_get_string(NULL, "default");
    cr_assert_str_eq(str, "default", "La clef NULL aurait du renvoyé la valeur par défaut. Lu : [%s]", str);

    // faire mumuse avec les setters
    res = configurator_set_int(NULL, 4);
    cr_assert_eq(res, -1, "Le setter aurait du retourner une erreur");
    res = configurator_set_string(NULL, "coucou");
    cr_assert_eq(res, -1, "Le setter aurait du retourner une erreur");
    res = configurator_set_string("cle", NULL);
    cr_assert_eq(res, -1, "Le setter aurait du retourner une erreur");
    
}

/**
 * @brief Modification en direct d'une valeur
 * 
 */
Test(configurator_suite, modif_en_direct) {
   // Ouverture du fichier
    FILE *f = fopen("dummy_test.cfg", "r");
    cr_assert_not_null(f, "Erreur d'ouverture du fichier de test");

    // Test de l'initialisation
    int params_lus = configurator_init(f);
    cr_assert_eq(params_lus, 2, "Le parser aurait dû trouver 2 paramètres valides. Paramètres trouvés : %d", params_lus);
    fclose(f);
    
    int res = configurator_set_int("port", 234);
    cr_assert_eq(res, 0, "Erreur lors de set_int");
    int port = configurator_get_int("port", 0);
    cr_assert_eq(port, 234, "Erreur lors de set_int, mauvaise valeur. Lu : [%d]", port);

    res = configurator_set_string("hostname", "test");
    cr_assert_eq(res, 0, "Erreur lors de set_string");
    const char *str = configurator_get_string("hostname", "default");
    cr_assert_str_eq(str, "test", "Erreur lors de set_string, mauvaise valeur. Lu : [%s]", str);

}

/**
 * @brief Sauvegarde du fichier de configuration
 * 
 */
Test(configurator_suite, save) {
    // Ouverture du fichier
    FILE *f = fopen("dummy_test.cfg", "r");
    cr_assert_not_null(f, "Erreur d'ouverture du fichier de test");

    // Test de l'initialisation
    int params_lus = configurator_init(f);
    cr_assert_eq(params_lus, 2, "Le parser aurait dû trouver 2 paramètres valides. Paramètres trouvés : %d", params_lus);
    fclose(f);

    configurator_set_int("port", 9999);
    configurator_set_string("nouveau_param", "test");

    FILE *f_out = fopen("test_out.cfg", "w");
    cr_assert_not_null(f_out, "Erreur création fichier de sortie");

    int res = configurator_save_config(f_out);
    cr_assert_eq(res, 0, "Echec de la sauvegarde");
    fclose(f_out);

    configurator_clean();

    FILE *f_check = fopen("test_out.cfg", "r");
    cr_assert_not_null(f_check, "Erreur création fichier de sortie");
    configurator_init(f_check);
    fclose(f_check);

    int saved_port = configurator_get_int("port", 0);
    const char *saved_str = configurator_get_string("nouveau_param", "introuvable");

    cr_assert_eq(saved_port, 9999, "Le port lu depuis le fichier de sauvegarde n'est pas le bon. Lu : [%d]", saved_port);
    cr_assert_str_eq(saved_str, "test", "Le paramètre lu n'est pas à la valeur sauvegardé. Lu : [%s]", saved_str);

    remove("test_out.cfg");
}
