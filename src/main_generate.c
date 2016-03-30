
#include <stdio.h>

void generate_sptables();

/*Main function for generating stopping tables */
int main()
{
    fprintf(stdout, "Generating stopping tables. This may take several minutes.\n");
    generate_sptables();
    return 1;
}
