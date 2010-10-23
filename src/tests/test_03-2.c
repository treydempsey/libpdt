#include "test_engine.h"

int main(int argc, char *argv[], char *envp[])
{
  /* |,|,|| */
  char data[] = "\",\",\",\",\"\"";
  struct event results[] = 
    { {DELIM_COL, 0, 1, ","},
      {DELIM_COL, 0, 1, ","},
      {DELIM_COL, 0, 0, ""},
      {DELIM_ROW, 0, 1, NULL}, {DELIM_END, 0, 0, NULL} };

  run_test("test_03-2", DELIM_STRICT, data, sizeof(data) - 1, results, ",", "\"");

  return test_failed;
}
