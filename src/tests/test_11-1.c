#include "test_engine.h"

int main(int argc, char *argv[], char *envp[])
{
  /* |1|2|3|4| */
  char data[] = "1,2 ,3,4\x0a";
  struct event results[] = 
    { {DELIM_COL, 0, 1, "1"},
      {DELIM_COL, 0, 1, "2"},
      {DELIM_COL, 0, 1, "3"}, 
      {DELIM_COL, 0, 1, "4"},
      {DELIM_ROW, '\x0a', 1, NULL}, {DELIM_END, 0, 0, NULL} };

  run_test("test_11-1", 0, data, sizeof(data) - 1, results, ",", "\"");

  return test_failed;
}
