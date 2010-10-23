#include "test_engine.h"

int main(int argc, char *argv[], char *envp[])
{
  /* |\x0a|\x0a|\x0a|\x0a| */
  char data[] = "\x0a\x0a\x0a\x0a";
  struct event results[] = 
    { {DELIM_COL, 0, 1, "\x0a"},
      {DELIM_ROW, '\x0a', 1, NULL},
      {DELIM_COL, 0, 1, "\x0a"},
      {DELIM_ROW, '\x0a', 1, NULL},
      {DELIM_COL, 0, 1, "\x0a"},
      {DELIM_ROW, '\x0a', 1, NULL},
      {DELIM_COL, 0, 1, "\x0a"},
      {DELIM_ROW, '\x0a', 1, NULL}, {DELIM_END, 0, 0, NULL} };

  run_test("test_12-2", DELIM_REPALL_NL, data, sizeof(data) - 1, results, ",", "\"");

  return test_failed;
}
