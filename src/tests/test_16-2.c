#include "test_engine.h"

int main(int argc, char *argv[], char *envp[])
{
  /* |1|2| 3 | */
  char data[] = "\"1\",\"2\",\" 3 ";
  struct event results[] = 
    { {DELIM_COL, 0, 1, "1"},
      {DELIM_COL, 0, 1, "2"},
      {DELIM_COL, 0, 3, " 3 "},
      {DELIM_ROW, 0, 1, NULL}, {DELIM_END, 0, 0, NULL} };

  run_test("test_16-2", DELIM_STRICT, data, sizeof(data) - 1, results, ",", "\"");

  return test_failed;
}
