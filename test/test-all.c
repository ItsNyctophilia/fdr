#include <check.h>

int main(void) {
    SRunner *sr = srunner_create(NULL);

    srunner_run_all(sr, CK_VERBOSE);

    srunner_free(sr);
}
