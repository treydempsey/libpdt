#include "test_engine.h"

int main(int argc, char *argv[], char *envp[])
{
  char data[] = "\" abc\"                                             "
                       "                                                     "
                       "                                                     "
                       "                                                     "
                       "                                                     "
                       "                                                     "
                       "                                                     "
                       "                                                     "
                       "                                          \", \"123\"";
  struct event results[] = 
    { {DELIM_COL, 0, 463,   " abc\"                                               "
                       "                                                     "
                       "                                                     "
                       "                                                     "
                       "                                                     "
                       "                                                     "
                       "                                                     "
                       "                                                     "
                       "                                        "},
      {DELIM_COL, 0, 3, "123"},
      {DELIM_ROW, 0, 1, NULL}, {DELIM_END, 0, 0, NULL} };
                     
  run_test("test_08-1", 0, data, sizeof(data) - 1, results, ",", "\"");

  return test_failed;
}
