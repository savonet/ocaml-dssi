(lang dune 2.8)
(version 0.1.5)
(name dssi)
(source (github savonet/ocaml-dssi))
(license LGPL-2.1)
(authors "The Savonet Team <savonet-users@lists.sourceforge.net>")
(maintainers "The Savonet Team <savonet-users@lists.sourceforge.net>")

(generate_opam_files true)
(use_standard_c_and_cxx_flags false)

(package
 (name dssi)
 (synopsis "Bindings for the DSSI API which provides audio synthesizers")
 (depends
  dune
  dune-configurator
  (ladspa (>= 0.2.1))
  conf-dssi)
)
