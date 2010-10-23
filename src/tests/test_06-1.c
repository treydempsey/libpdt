#include "test_engine.h"

int main(int argc, char *argv[], char *envp[])
{
  /* | a, b ,c |a b  c|| */
  char data[] = "\" a, b ,c \", a b  c,";
  struct event results[] = 
    { {DELIM_COL, 0, 9, " a, b ,c "},
      {DELIM_COL, 0, 6, "a b  c"},
      {DELIM_COL, 0, 0, ""},
      {DELIM_ROW, 0, 1, NULL}, {DELIM_END, 0, 0, NULL} };

  run_test("test_06-1", 0, data, sizeof(data) - 1, results, ",", "\"");

  return test_failed;
}
