#include <stdio.h>
#include "magmaTerm.h"

int main(void) {
    MagmaTerm *magma = initialize_magmaterm();
    set_bg_color(magma->window, 255, 98, 35, 207);
    Terminal *term = new_terminal(magma);
    magma->term = term;
    run(magma);
    cleanup(magma);
    return 0;
}