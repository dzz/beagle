
/* define functions which python application must
 * provide which may be called by the host */

CLIENT_FUNCTION( init,                 "init"                 )
CLIENT_FUNCTION( tick,                 "tick"                 )
CLIENT_FUNCTION( immediate_cycle,      "immediate_cycle"      )
CLIENT_FUNCTION( dispatch_mouseup,     "dispatch_mouseup"     )
CLIENT_FUNCTION( dispatch_mousedown,   "dispatch_mousedown"   )
CLIENT_FUNCTION( dispatch_mousemotion, "dispatch_mousemotion" )
CLIENT_FUNCTION( dispatch_key,         "dispatch_key"         )
CLIENT_FUNCTION( dispatch_text,        "dispatch_text"        )
CLIENT_FUNCTION( finalize,             "finalize"             )
CLIENT_FUNCTION( render,               "render"               )
//CLIENT_FUNCTION( render_test,          "render_test"               )
CLIENT_FUNCTION( set_screensize,       "set_screensize"       )
CLIENT_FUNCTION( map_keycode,          "map_keycode"              )

