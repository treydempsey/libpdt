#include "test_engine.h"

int main(int argc, char *argv[], char *envp[])
{
  /* |a| */
  char data[] = "a\x0a";
  struct event results[] = 
    { {DELIM_COL, 0, 1, "a"},
      {DELIM_ROW, '\x0a', 1, NULL}, {DELIM_END, 0, 0, NULL} };

  run_test("test_10-1", 0, data, sizeof(data) - 1, results, ",", "\"");

  return test_failed;
}
