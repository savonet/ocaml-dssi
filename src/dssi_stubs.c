#include <caml/alloc.h>
#include <caml/callback.h>
#include <caml/custom.h>
#include <caml/fail.h>
#include <caml/memory.h>
#include <caml/mlvalues.h>
#include <caml/signals.h>

#include <assert.h>
#include <dlfcn.h>

#include <ladspa.h>
#include <dssi.h>
#include <ocaml_ladspa.h>

#define Descr_val(v) ((DSSI_Descriptor*)v)
#define Val_descr(d) ((value)d)

CAMLprim value ocaml_dssi_open(value fname)
{
  void *handle = dlopen(String_val(fname), RTLD_LAZY);
  DSSI_Descriptor_Function dssi_descriptor;

  /* TODO: raise exception */
  assert(handle);
  dssi_descriptor = (DSSI_Descriptor_Function)dlsym((void*)handle, "dssi_descriptor");
  if (dlerror() != NULL || !dssi_descriptor)
  {
    dlclose(handle);
    /* TODO: raise */
    assert(0);
    //caml_raise_constant(*caml_named_value("ocaml_ladspa_exn_not_a_plugin"));
  }

  return (value)handle;
}

CAMLprim value ocaml_dssi_close(value handle)
{
  dlclose((void*)handle);

  return Val_unit;
}

CAMLprim value ocaml_dssi_descriptor(value handle, value n)
{
  DSSI_Descriptor_Function dssi_descriptor = (DSSI_Descriptor_Function)dlsym((void*)handle, "dssi_descriptor");
  const DSSI_Descriptor *d = dssi_descriptor(Int_val(n));

  if (!d)
    caml_raise_constant(*caml_named_value("ocaml_dssi_exn_not_found"));

  return Val_descr(d);
}

CAMLprim value ocaml_dssi_api_version(value d)
{
  return Val_int(Descr_val(d)->DSSI_API_Version);
}

CAMLprim value ocaml_dssi_ladspa(value d)
{
  return Val_LADSPA_descr(Descr_val(d)->LADSPA_Plugin);
}

CAMLprim value ocaml_dssi_configure(value d, value i, value key, value v)
{
  char *ans;
  ans = Descr_val(d)->configure(Instance_val(i)->handle, String_val(key), String_val(v));
  /* TODO */
  assert(ans);
  value ret = caml_copy_string(ans);
  free(ans);
  return ret;
}

CAMLprim value ocaml_dssi_get_program(value d, value i, value n)
{
  CAMLparam0();
  CAMLlocal1(ret);
  const DSSI_Program_Descriptor *ans;

  ans = Descr_val(d)->get_program(Instance_val(i)->handle, Int_val(n));
  ret = caml_alloc_tuple(3);
  Store_field(ret, 0, Val_int(ans->Bank));
  Store_field(ret, 1, Val_int(ans->Program));
  Store_field(ret, 2, caml_copy_string(ans->Name));
  CAMLreturn(ret);
}

CAMLprim value ocaml_dssi_select_program(value d, value i, value bank, value program)
{
  // TODO: check that select_program exists
  Descr_val(d)->select_program(Instance_val(i)->handle, Int_val(bank), Int_val(program));
  return Val_unit;
}

CAMLprim value ocaml_dssi_get_midi_controller_for_port(value d, value i, value port)
{
  int ans;

  ans = Descr_val(d)->get_midi_controller_for_port(Instance_val(i)->handle, Int_val(port));

  return Val_int(ans);
}

static snd_seq_event_t* events_of_array(value ev)
{
  int event_count = Wosize_val(ev);
  snd_seq_event_t *events = malloc(event_count * sizeof(snd_seq_event_t));
  int i;
  value e;

  for (i = 0; i < event_count; i++)
  {
    events[i].time.tick = Int_val(Field(Field(ev, i), 0));
    e = Field(Field(ev, i), 1);

    if (Is_block(e))
    {
      switch (Tag_val(e))
      {
        case 2:
          events[i].type = SND_SEQ_EVENT_NOTEON;
          events[i].data.note.channel = Int_val(Field(e, 0));
          events[i].data.note.note = Int_val(Field(e, 1));
          events[i].data.note.velocity = Int_val(Field(e, 2));
          break;

        case 3:
          events[i].type = SND_SEQ_EVENT_NOTEOFF;
          events[i].data.note.channel = Int_val(Field(e, 0));
          events[i].data.note.note = Int_val(Field(e, 1));
          events[i].data.note.velocity = Int_val(Field(e, 2));
          break;
      }
    }
  }

  return events;
}

CAMLprim value ocaml_dssi_run_synth(value d, value i, value sc, value ev)
{
  DSSI_Descriptor* descr = Descr_val(d);
  LADSPA_Handle h = Instance_val(i)->handle;
  int sample_count = Int_val(sc);
  int event_count = Wosize_val(ev);
  snd_seq_event_t *events = events_of_array(ev);

  caml_enter_blocking_section();
  descr->run_synth(h, sample_count, events, event_count);
  caml_leave_blocking_section();

  free(events);
  return Val_unit;
}
