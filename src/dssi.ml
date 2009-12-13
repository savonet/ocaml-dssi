let () =
  Callback.register_exception "ocaml_dssi_exn_not_found" Not_found

module Plugin =
struct
  type t

  exception Not_a_plugin

  external load : string -> t = "ocaml_dssi_open"

  external unload : t -> unit = "ocaml_dssi_close"
end

let () =
  Callback.register_exception "ocaml_ladspa_exn_not_a_plugin" Plugin.Not_a_plugin

module Descriptor =
struct
  type t

  external descriptor : Plugin.t -> int -> t = "ocaml_dssi_descriptor"

  let rec descriptors p n =
    try
      let d = descriptor p n in
      let dd = descriptors p (n+1) in
        d::dd
    with
      | Not_found -> []

  let descriptors p = Array.of_list (descriptors p 0)

  external api_version : t -> int = "ocaml_dssi_api_version"

  external configure : t -> Ladspa.Descriptor.instance -> string -> string -> string = "ocaml_dssi_configure"
end
