#include "test_engine.h"

int main(int argc, char *argv[], char *envp[])
{
  /* |1|2| 3 | */
  char data[] = "\"1\",\"2\",\" 3 ";
  struct event results[] = 
    { {DELIM_COL, 0, 1, "1"},
      {DELIM_COL, 0, 1, "2"},
      {DELIM_ERR, 0, 0, NULL} };

  run_test("test_16-3", DELIM_STRICT | DELIM_STRICT_FINI, data, sizeof(data) - 1, results, ",", "\"");

  return test_failed;
}
