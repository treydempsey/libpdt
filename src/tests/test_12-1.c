#include "test_engine.h"

int main(int argc, char *argv[], char *envp[])
{
  /* empty */
  char data[] = "\x0a\x0a\x0a\x0a";
  struct event results[] = 
    { {DELIM_END, 0, 0, NULL} };

  run_test("test_12-1", 0, data, sizeof(data) - 1, results, ",", "\"");

  return test_failed;
}
