
/*! 
 * \file purify_about.c
 * Program to display summary information, including version and build numbers.
 */

#include <stdio.h>

int main(int argc, char *argv[]) {

  printf("%s\n", "==========================================================");
  printf("%s\n", "  PURIFY for radio interferometric imaging");
  printf("%s\n", "  By R. E. Carrillo, J. D. McEwen & Y. Wiaux");

  printf("%s\n", "  See README.txt for more information.");
  printf("%s\n", "  See LICENSE.txt for license information.");

  printf("%s%s\n", "  Version: ", PURIFY_VERSION);
  printf("%s%s\n", "  Build:   ", PURIFY_BUILD);
  printf("%s\n", "==========================================================");

  return 0;

}
