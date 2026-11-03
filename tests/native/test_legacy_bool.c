/* Compile against each NPP h-basic.h, including when stdbool.h came first.
 * A non-standard bool_hack token must never escape the HAVE_STDBOOL_H path. */
#ifdef STDBOOL_FIRST
# include <stdbool.h>
#endif
#include "h-basic.h"

#ifndef HAVE_STDBOOL_H
# error "C99+ should detect stdbool.h"
#endif

int main(void)
{
    bool yes = TRUE;
    bool no = FALSE;
    return (yes && !no) ? 0 : 1;
}
