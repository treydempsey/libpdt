#include "test_engine.h"

int main(int argc, char *argv[], char *envp[])
{
  /* empty */
  char data[] = "";
  struct event results[] = 
    { {DELIM_END, 0, 0, NULL} };
                     
  run_test("test_09-1", 0, data, sizeof(data) - 1, results, ",", "\"");

  return test_failed;
}
