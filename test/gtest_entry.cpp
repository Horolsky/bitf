#include "gtest_solid_func.hpp"
#include "gtest_solid_cls.hpp"
// #include "gtest_chunked.hpp"

int
main (int argc, char **argv)
{
  printf ("Running %s...\n", argv[0]);

  testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS ();
}