#include "test_engine.h"

int main(int argc, char *argv[], char *envp[])
{
  /* | " " " " | */
  char data[] = "\" \"\" \" \" \"\" \"";
  struct event results[] = 
    { {DELIM_ERR, 0, 0, NULL} };

  run_test("test_07-2", DELIM_STRICT, data, sizeof(data) - 1, results, ",", "\"");

  return test_failed;
}
