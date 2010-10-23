#include "test_engine.h"

int main(int argc, char *argv[], char *envp[])
{
  /* |a\0b\0c| */
  char data[] = " a\0b\0c ";
  struct event results[] = 
    { {DELIM_COL, 0, 5, "a\0b\0c"},
      {DELIM_ROW, 0, 1, NULL}, {DELIM_END, 0, 0, NULL} };

  run_test("test_17-1", 0, data, sizeof(data) - 1, results, ",", "\"");

  return test_failed;
}
