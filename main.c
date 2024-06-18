#include "./machine/dm.h"
#include "./dasm/asm.h"

int main(void) {
    const char *filename = "./main.asm";
    dm_init(&dm);
    dm_assemble(&dm, filename);
    dm_execute(&dm);
    dm_write_prog(&dm);
    dm_clean(&dm);
    return 0;
}