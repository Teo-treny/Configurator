#include <criterion/criterion.h>
#include <string.h>
#include <stdio.h>

#include <configurator/configurator.h>

// Moule de configuration pour nos tests
typedef struct {
    int port;
    char hostname[64];
} TestConfig_t;

// Le handler pour nos tests
int test_handler(void *user_data, const char *key, const char *value) {
    TestConfig_t *cfg = (TestConfig_t *)user_data;
    if (strcmp(key, "port") == 0) {
        cfg->port = atoi(value);
        return 0;
    }
    else if (strcmp(key, "hostname") == 0) {
        strncpy(cfg->hostname, value, 63);
        return 0;
    }
    return -1;
}

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
    TestConfig_t def = {80, "default"};

    // Test de l'initialisation
    int params_lus = configurator_init("dummy_test.cfg", sizeof(TestConfig_t), test_handler, &def);
    cr_assert_eq(params_lus, 2, "Le parser aurait dû trouver 2 paramètres valides. Paramètres trouvés : %d", params_lus);

    // Test de la lecture
    const TestConfig_t *cfg = (const TestConfig_t *)configurator_acquire_read();
    cr_assert_not_null(cfg, "Le pointeur de configuration ne devrait pas être NULL");

    cr_assert_eq(cfg->port, 8080, "Le port lu devrait être 8080. Lu : %d", cfg->port);
    cr_assert_str_eq(cfg->hostname, "localhost", "Le hostname lu devrait être localhost. Lu : [%s]", cfg->hostname);

    // Libération
    configurator_release_read();
}

/**
 * @brief Test les valeurs par défaut
 * 
 */
Test(configurator_suite, valeurs_par_defaut) {
    TestConfig_t def = {404, "not_found"};

    // On tente de lire un fichier qui n'existe pas
    int res = configurator_init("fichier_inexistant.cfg", sizeof(TestConfig_t), test_handler, &def);
    cr_assert_eq(res, -1, "Init devrait retourner -1 si le fichier n'existe pas. Lu : %d", res);

    const TestConfig_t *cfg = (const TestConfig_t *)configurator_acquire_read();
    cr_assert_not_null(cfg, "Le pointeur de configuration ne devrait pas être NULL");

    // Même si la lecture du fichier a planté, on doit quand même avoir nos paramètres par défaut
    cr_assert_eq(cfg->port, 404, "Le port par défaut devrait être appliqué à 404. Ici : %d", cfg->port);
    cr_assert_str_eq(cfg->hostname, "not_found", "Le hostname par défaut devrait être appliqué à not_found. Ici : [%s]", cfg->hostname);

    configurator_release_read();
}

/**
 * @brief Modification en direct d'une valeur
 * 
 */
Test(configurator_suite, modif_en_direct) {
    TestConfig_t def = {80, "default"};
    configurator_init("dummy_test.cfg", sizeof(TestConfig_t), test_handler, &def);

    // Test de la fonction set_value
    int res = configurator_set_value("port", "9000");
    cr_assert_eq(res, 0, "La mise à jour aurait du réussir.");

    // On vérifie que la valeur a bien changé en mémoire
    const TestConfig_t *cfg = (const TestConfig_t *)configurator_acquire_read();
    cr_assert_eq(cfg->port, 9000, "Le port en mémoire aurait du passer à 9000. Lu : %d", cfg->port);

    configurator_release_read();
}

/**
 * @brief Modification d'une clé inconnue
 * 
 */
Test(configurator_suite, modif_cle_inconnue) {
    TestConfig_t def = {80, "default"};
    configurator_init("dummy_test.cfg", sizeof(TestConfig_t), test_handler, &def);
    
    // Injection d'une clé au pif
    int res = configurator_set_value("cle_au_pif", "856321");
    cr_assert_eq(res, -1, "Le handler de clé devrait rejeter la clé car elle est inconnue");
}
