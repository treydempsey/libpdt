#include "test_engine.h"

int main(int argc, char *argv[], char *envp[])
{
  char data[] = "\"I call our world Flatland,\x0a"
                       "not because we call it so,\x0a"
                       "but to make its nature clearer\x0a"
                       "to you, my happy readers,\x0a"
                       "who are privileged to live in Space.\"";
  struct event results[] = 
    { {DELIM_COL, 0, 147, "I call our world Flatland,\x0a"
                     "not because we call it so,\x0a"
                     "but to make its nature clearer\x0a"
                     "to you, my happy readers,\x0a"
                     "who are privileged to live in Space."},
      {DELIM_ROW, 0, 1, NULL}, {DELIM_END, 0, 0, NULL} };

  run_test("test_04-2", DELIM_STRICT, data, sizeof(data) - 1, results, ",", "\"");

  return test_failed;
}
