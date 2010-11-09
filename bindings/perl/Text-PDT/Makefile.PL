use 5.008009;
use ExtUtils::MakeMaker;
WriteMakefile(
    NAME              => 'Text::PDT',
    VERSION_FROM      => 'lib/Text/PDT.pm',
    PREREQ_PM         => {}, # e.g., Module::Name => 1.1
    ($] >= 5.005 ?     ## Add these new keywords supported since 5.005
      (ABSTRACT_FROM  => 'lib/Text/PDT.pm',
       AUTHOR         => 'Trey Dempsey <trey.dempsey@gmail.com>') : ()),
    LIBS              => ['-L/usr/local/lib -ltgcl'],
    INC               => '-I. -I../../.. -I../../../src -I/usr/local/include/tgcl',
    OBJECT            => '$(O_FILES) ../../../src/.libs/libpdt.a'
);
