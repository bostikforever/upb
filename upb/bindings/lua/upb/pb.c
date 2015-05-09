/*
 * upb - a minimalist implementation of protocol buffers.
 *
 * Copyright (c) 2014 Google Inc.  See LICENSE for details.
 * Author: Josh Haberman <jhaberman@gmail.com>
 *
 * A Lua extension for upb.pb.
 *
 * Exposes all the types defined in upb/pb/{*}.h
 * Also defines a few convenience functions on top.
 */

#include "upb/bindings/lua/upb.h"
#include "upb/pb/decoder.h"

#define LUPB_PBDECODERMETHOD "lupb.pb.decodermethod"

#define MSGDEF_INDEX 1

static upb_pbdecodermethod *lupb_pbdecodermethod_check(lua_State *L, int narg) {
  return lupb_refcounted_check(L, narg, LUPB_PBDECODERMETHOD);
}

static int lupb_pbdecodermethod_new(lua_State *L) {
  const upb_handlers *handlers = lupb_msg_newwritehandlers(L, 1, &handlers);

  upb_pbdecodermethodopts opts;
  upb_pbdecodermethodopts_init(&opts, handlers);

  const upb_pbdecodermethod *m = upb_pbdecodermethod_new(&opts, &m);
  upb_handlers_unref(handlers, &handlers);
  lupb_refcounted_pushnewrapper(L, UPB_UPCAST(m), LUPB_PBDECODERMETHOD, &m);

  // We need to keep a pointer to the MessageDef (in Lua space) so we can
  // construct new messages in parse().
  lua_newtable(L);
  lua_pushvalue(L, 1);
  lua_rawseti(L, -2, MSGDEF_INDEX);
  lua_setuservalue(L, -2);

  return 1;  // The DecoderMethod wrapper.
}

// We implement upb's allocation function by allocating a Lua userdata.
// This is a raw hunk of memory that will be GC'd by Lua.
static void *lua_alloc(void *ud, size_t size) {
  lua_State *L = ud;
  return lua_newuserdata(L, size);
}

// Unlike most of our exposed Lua functions, this does not correspond to an
// actual method on the underlying DecoderMethod.  But it's convenient, and
// important to implement in C because we can do stack allocation and
// initialization of our runtime structures like the Decoder and Sink.
static int lupb_pbdecodermethod_parse(lua_State *L) {
  size_t len;
  const upb_pbdecodermethod *method = lupb_pbdecodermethod_check(L, 1);
  const char *pb = lua_tolstring(L, 2, &len);

  const upb_handlers *handlers = upb_pbdecodermethod_desthandlers(method);

  lua_getuservalue(L, 1);
  lua_rawgeti(L, -1, MSGDEF_INDEX);
  lupb_assert(L, !lua_isnil(L, -1));
  lupb_msg_pushnew(L, -1);  // Push new message.
  void *msg = lua_touserdata(L, -1);

  // Handlers need this.
  lua_getuservalue(L, -1);

  upb_status status = UPB_STATUS_INIT;
  upb_env env;
  upb_env_init(&env);
  upb_env_reporterrorsto(&env, &status);
  upb_sink sink;
  upb_sink_reset(&sink, handlers, msg);
  upb_pbdecoder *decoder = upb_pbdecoder_create(&env, method, &sink);
  upb_bufsrc_putbuf(pb, len, upb_pbdecoder_input(decoder));

  // This won't get called in the error case, which longjmp's across us.  But
  // since we made our alloc function allocate only GC-able memory, that
  // shouldn't matter.  It *would* matter if the environment had references to
  // any non-memory resources (ie.  filehandles).  As an alternative to this we
  // could make the environment itself a userdata.
  upb_env_uninit(&env);

  lupb_checkstatus(L, &status);

  lua_pop(L, 1);  // Uservalue.

  return 1;
}

static const struct luaL_Reg lupb_pbdecodermethod_m[] = {
  {"parse", lupb_pbdecodermethod_parse},
  {NULL, NULL}
};

static const struct luaL_Reg toplevel_m[] = {
  {"DecoderMethod", lupb_pbdecodermethod_new},
  {NULL, NULL}
};

int luaopen_upb_pb(lua_State *L) {
  luaopen_upb(L);

  static char module_key;
  if (lupb_openlib(L, &module_key, "upb.pb", toplevel_m)) {
    return 1;
  }

  lupb_register_type(L, LUPB_PBDECODERMETHOD, lupb_pbdecodermethod_m, NULL,
                     true);

  return 1;
}