require 'mkmf'
$CFLAGS = "#{$CFLAGS} -I.."
$LFLAGS = "#{$LFLAGS} ../libedf.a"
create_makefile("EDF")
