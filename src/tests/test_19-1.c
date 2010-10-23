#include "test_engine.h"

int main(int argc, char *argv[], char *envp[])
{
  /* |a|b\_^|c| */
  char data[] = "a\\_^_/b\\_^\\_^_/c\x0a";
  struct event results[] =
    { {DELIM_COL, 0, 1, "a"},
      {DELIM_COL, 0, 4, "b\\_^"},
      {DELIM_COL, 0, 1, "c"},
      {DELIM_ROW, '\x0a', 1, NULL}, {DELIM_END, 0, 0, NULL} };

  run_test("test_19-1", 0, data, sizeof(data) - 1, results, "\\_^_/", "");

  return test_failed;
}
