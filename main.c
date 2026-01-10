#include <stdbool.h>
#include "menu.h"
#include "defs.h"

int main(/* int argc, char **argv */) {
    Ledger ledger = {0};
    while(menu(&ledger));
    return 0;
}
