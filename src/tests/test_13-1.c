#include "test_engine.h"

int main(int argc, char *argv[], char *envp[])
{
  /* |abc| */
  char data[] = "\"abc\"";
  struct event results[] =
    { {DELIM_COL, 0, 3, "abc"},
      {DELIM_ROW, 0, 1, NULL}, {DELIM_END, 0, 0, NULL} };

  run_test("test_13-1", 0, data, sizeof(data) - 1, results, ",", "\"");

  return test_failed;
}
