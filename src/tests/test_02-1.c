#include "test_engine.h"

int main(int argc, char *argv[], char *envp[])
{
  /* ||||||| */
  char data[] = ",,,,,\x0a";
  struct event results[] =
    { {DELIM_COL, 0, 0, ""},
      {DELIM_COL, 0, 0, ""},
      {DELIM_COL, 0, 0, ""},
      {DELIM_COL, 0, 0, ""},
      {DELIM_COL, 0, 0, ""},
      {DELIM_COL, 0, 0, ""},
      {DELIM_ROW, '\x0a', 1, NULL}, {DELIM_END, 0, 0, NULL} };

  run_test("test_02-1", 0, data, sizeof(data) - 1, results, ",", "\"");

  return test_failed;
}
