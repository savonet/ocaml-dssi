type event =
  | Event_system of int * int
  | Event_result of int * int
  (* Event_note is forbidden by DSSI specification. *)
  | Event_note_on of int * int * int
  | Event_note_off of int * int * int

module Plugin =
struct
  type t

  exception Not_a_plugin

  external load : string -> t = "ocaml_dssi_open"

  external unload : t -> unit = "ocaml_dssi_close"
end

module Descriptor =
struct
  type t

  exception Not_implemented

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

  external ladspa : t -> Ladspa.Descriptor.t = "ocaml_dssi_ladspa"

  external configure : t -> Ladspa.Descriptor.instance -> string -> string -> string = "ocaml_dssi_configure"

  external get_program : t -> Ladspa.Descriptor.instance -> int -> int * int * string = "ocaml_dssi_get_program"

  external select_program : t -> Ladspa.Descriptor.instance -> int -> int -> unit = "ocaml_dssi_select_program"

  external get_midi_controller : t -> Ladspa.Descriptor.instance -> int -> int = "ocaml_dssi_get_midi_controller_for_port"

  external run_synth : t -> Ladspa.Descriptor.instance -> int -> (int * event) array -> unit = "ocaml_dssi_run_synth"
end

let init () =
  Callback.register_exception "ocaml_dssi_exn_not_found" Not_found;
  Callback.register_exception "ocaml_ladspa_exn_not_a_plugin" Plugin.Not_a_plugin;
  Callback.register_exception "ocaml_dssi_exn_not_implemented" Descriptor.Not_implemented

let () = init ()
