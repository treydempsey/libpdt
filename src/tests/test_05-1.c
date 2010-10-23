#include "test_engine.h"

int main(int argc, char *argv[], char *envp[])
{
  /* |"a,b"|| "" |"" | ""|""| */
  char data[] = "\"\"\"a,b\"\"\",,\" \"\"\"\" \",\"\"\"\"\" \",\" \"\"\"\"\",\"\"\"\"\"\"";
  struct event results[] = 
    { {DELIM_COL, 0, 5, "\"a,b\""},
      {DELIM_COL, 0, 0, ""},
      {DELIM_COL, 0, 4, " \"\" "},
      {DELIM_COL, 0, 3, "\"\" "},
      {DELIM_COL, 0, 3, " \"\""},
      {DELIM_COL, 0, 2, "\"\""},
      {DELIM_ROW, 0, 1, NULL}, {DELIM_END, 0, 0, NULL} };

  run_test("test_05-1", 0, data, sizeof(data) - 1, results, ",", "\"");

  return test_failed;
}
