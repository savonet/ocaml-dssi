# check for one particular file of the sources
AC_INIT(ocaml-dssi, 0.1.0, savonet-users@lists.sourceforge.net)

VERSION=$PACKAGE_VERSION
AC_MSG_RESULT([configuring $PACKAGE_STRING])

# Add prefix to compilation variables
# if passed
if test "x$prefix" != "xNONE"; then
        CFLAGS="$CFLAGS -I$prefix/include"
        LDFLAGS="$LDFLAGS -L$prefix/lib"
        CPPFLAGS="$CPPFLAGS -I$prefix/include"
fi

AC_CHECK_OCAML_COMPILERS()

AC_PROG_CC()
AC_CHECK_TOOL([AR],[ar],no)
AC_SUBST(AR)

# Check for DSSI
AC_CHECK_HEADERS([dssi.h], [ac_have_dssi="yes"])
if test "$ac_have_dssi" != "yes"; then
   AC_MSG_ERROR([DSSI headers not found.])
fi

# Check for LADSPA
AC_ARG_WITH([ladspa-dir],AC_HELP_STRING([--with-ladspa-dir=path],
        [use "path" as the location of ocaml-ladspa (autodetected by default)]))
if test -z "$with_ladspa_dir"; then
  AC_MSG_CHECKING(for ocaml-ladspa)
  if ($OCAMLFIND query ladspa > /dev/null 2>&1); then
        OCAMLLADSPA_INC=`$OCAMLFIND query ladspa`
        AC_MSG_RESULT(ok)
  else
        AC_MSG_ERROR(not found.)
  fi
else
  echo $with_ladspa_dir | grep ^/ > /dev/null 2>&1 \
  || with_ladspa_dir=$PWD/$with_ladspa_dir
  OCAMLLADSPA_INC="$with_ladspa_dir"
fi
AC_SUBST(OCAMLLADSPA_INC)
CFLAGS="-I$OCAMLLADSPA_INC $CFLAGS"
AC_CHECK_HEADERS([ocaml_ladspa.h],[ac_have_ladspa="yes"])
if test "$ac_have_ladspa" != "yes"; then
   AC_MSG_ERROR([ocaml-ladspa headers not found.])
fi

# substitutions to perform
AC_SUBST(VERSION)
AC_SUBST(INC)
ALL_INC="`ocamlfind query -r -separator " " $requires` $INC"
AC_SUBST(ALL_INC)
AC_SUBST(requires)
all_requires="`ocamlfind query -r -separator " " -format "%p" $requires`"
AC_SUBST(all_requires)

# Finally create the Makefile and samples
AC_CONFIG_FILES([Makefile],[chmod a-w Makefile])
AC_CONFIG_FILES([src/META])
AC_CONFIG_FILES([src/Makefile])
AC_OUTPUT
