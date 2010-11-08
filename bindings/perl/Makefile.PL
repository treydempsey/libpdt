use 5.008009;
use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile(
    NAME              => 'PDT',
    VERSION_FROM      => 'lib/PDT.pm', # finds $VERSION
    PREREQ_PM         => {}, # e.g., Module::Name => 1.1
    ($] >= 5.005 ?     ## Add these new keywords supported since 5.005
      (ABSTRACT_FROM  => 'lib/PDT.pm', # retrieve abstract from module
       AUTHOR         => 'Trey Dempsey <trey.dempsey@gmail.com>') : ()),
    LIBS              => ['-L/usr/local/lib -ltgcl'], # e.g., '-lm'
    DEFINE            => '', # e.g., '-DHAVE_SOMETHING'
    INC               => '-I. -I../.. -I../../src -I/usr/local/include/tgcl', # e.g., '-I. -I/usr/include/other'
    OBJECT            => '$(O_FILES) ../../src/.libs/libpdt.a'
);
