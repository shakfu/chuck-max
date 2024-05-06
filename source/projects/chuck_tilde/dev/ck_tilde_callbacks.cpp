
// captures callback api (not all of this is used in chuck_tilde)

// typedefs
typedef void (*ck_named_event_callback)(const char*);
typedef std::unordered_map<std::string, ck_named_event_callback> callback_map;


// callback registeration
t_max_err ck_register(t_ck* x, t_symbol* s);
t_max_err ck_unregister(t_ck* x, t_symbol* s);

// callback demo
t_max_err ck_demo(t_ck* x);


// callbacks (events)
void cb_global_event(void);
void cb_global_named_event(const char* name);
void cb_global_event_with_id(t_CKINT cb_id);

// callbacks (variables)
void cb_get_global_int(t_CKINT val);
void cb_get_global_named_int(const char* name, t_CKINT val);
void cb_get_global_int_with_id(t_CKINT cb_id, t_CKINT val);

void cb_get_global_float(t_CKFLOAT val);
void cb_get_global_named_float(const char* name, t_CKFLOAT val);
void cb_get_global_float_with_id(t_CKINT cb_id, t_CKFLOAT val);

void cb_get_global_string(const char* val);
void cb_get_global_named_string(const char* name, const char* val);
void cb_get_global_string_with_id(t_CKINT cb_id, const char* val);

void cb_get_global_int_array(t_CKINT array[], t_CKUINT n);
void cb_get_global_named_int_array(const char* name, t_CKINT array[], t_CKUINT n);
void cb_get_global_int_array_with_id(t_CKINT cb_id, t_CKINT array[], t_CKUINT n);

void cb_get_global_int_array_value(t_CKINT value);
void cb_get_global_named_int_array_value(const char* name, t_CKINT value);
void cb_get_global_int_array_value_with_id(t_CKINT cb_id, t_CKINT value);

void cb_get_global_float_array(t_CKFLOAT array[], t_CKUINT n);
void cb_get_global_named_float_array(const char* name, t_CKFLOAT array[], t_CKUINT n);
void cb_get_global_float_array_with_id(t_CKINT cb_id, t_CKFLOAT array[], t_CKUINT n);

void cb_get_global_float_array_value(t_CKFLOAT value);
void cb_get_global_named_float_array_value(const char* name, t_CKFLOAT value);
void cb_get_global_float_array_value_with_id(t_CKINT cb_id, t_CKFLOAT value);

void cb_get_global_assoc_int_array_value(t_CKINT val);
void cb_get_global_named_assoc_int_array_value(const char* name, t_CKINT val);
void cb_get_global_assoc_int_array_value_with_id(t_CKINT cb_id, t_CKINT val);

void cb_get_global_assoc_float_array_value(t_CKFLOAT val);
void cb_get_global_named_assoc_float_array_value(const char* name, t_CKFLOAT val);
void cb_get_global_assoc_float_array_value_with_id(t_CKINT cb_id, t_CKFLOAT val);

// dump all global variables
void cb_get_all_global_vars(const std::vector<Chuck_Globals_TypeValue> & list, void * data);



//-----------------------------------------------------------------------------------------------
// global event callbacks

void cb_global_event(void)
{
    post("cb_global_event");
}

void cb_global_named_event(const char* name)
{
    post("cb_global_named_event: %s", name);
}

void cb_global_event_with_id(t_CKINT cb_id)
{
    post("cb_global_event_with_id: id: %d", cb_id);
}


//-----------------------------------------------------------------------------------------------
// global variable callbacks

/* nothing useful here yet just minimal demos */

void cb_get_global_int(t_CKINT val)
{
    post("cb_get_global_int: %d", val);
}

void cb_get_global_named_int(const char* name, t_CKINT val)
{
     post("cb_get_global_named_int: name: %s value: %d", name, val);
}

void cb_get_global_int_with_id(t_CKINT cb_id, t_CKINT val)
{
     post("cb_get_global_int_with_id: id: %d value: %d", cb_id, val);
}

void cb_get_global_float(t_CKFLOAT val)
{
    post("cb_get_global_float: %f", val);
}

void cb_get_global_named_float(const char* name, t_CKFLOAT val)
{
     post("cb_get_global_named_float: name: %s value: %f", name, val);
}

void cb_get_global_float_with_id(t_CKINT cb_id, t_CKFLOAT val)
{
     post("cb_get_global_float_with_id: id: %d value: %f", cb_id, val);
}

void cb_get_global_string(const char* val)
{
    post("cb_get_global_string: %s", val);
}

void cb_get_global_named_string(const char* name, const char* val)
{
     post("cb_get_global_named_string: name: %s value: %s", name, val);
}

void cb_get_global_string_with_id(t_CKINT cb_id, const char* val)
{
     post("cb_get_global_string_with_id: id: %d value: %s", cb_id, val);
}

void cb_get_global_int_array(t_CKINT array[], t_CKUINT n)
{
    post("cb_get_global_int_array: %d", n);
    for (int i = 0; i < n; i++) {
        post("array[%d] = %d", i, array[i]);
    }
}

void cb_get_global_named_int_array(const char* name, t_CKINT array[], t_CKUINT n)
{
    post("cb_get_global_named_int_array: name: %s size: %d", name, n);
    for (int i = 0; i < n; i++) {
        post("array[%d] = %d", i, array[i]);
    }
}

void cb_get_global_int_array_with_id(t_CKINT cb_id, t_CKINT array[], t_CKUINT n)
{
    post("cb_get_global_int_array_with_id: cb_id: %d size: %d", cb_id, n);
    for (int i = 0; i < n; i++) {
        post("array[%d] = %d", i, array[i]);
    }
}

void cb_get_global_int_array_value(t_CKINT value)
{
    post("cb_get_global_int_array_value: value: %d", value);
}

void cb_get_global_named_int_array_value(const char* name, t_CKINT value)
{
    post("cb_get_global_named_int_array_value: name: %s value: %d", name, value);
}

void cb_get_global_int_array_value_with_id(t_CKINT cb_id, t_CKINT value)
{
    post("cb_get_global_int_array_value_with_id: cb_id: %d value: %d", cb_id, value);
}


void cb_get_global_float_array(t_CKFLOAT array[], t_CKUINT n)
{
    post("cb_get_global_float_array: %d", n);
    for (int i = 0; i < n; i++) {
        post("array[%d] = %d", i, array[i]);
    }
}

void cb_get_global_named_float_array(const char* name, t_CKFLOAT array[], t_CKUINT n)
{
    post("cb_get_global_named_float_array: name: %s size: %d", name, n);
    for (int i = 0; i < n; i++) {
        post("array[%d] = %d", i, array[i]);
    }
}

void cb_get_global_float_array_with_id(t_CKINT cb_id, t_CKFLOAT array[], t_CKUINT n)
{
    post("cb_get_global_float_array_with_id: cb_id: %d size: %d", cb_id, n);
    for (int i = 0; i < n; i++) {
        post("array[%d] = %d", i, array[i]);
    }
}


void cb_get_global_float_array_value(t_CKFLOAT value)
{
    post("cb_get_global_float_array_value: value: %d", value);
}

void cb_get_global_named_float_array_value(const char* name, t_CKFLOAT value)
{
    post("cb_get_global_named_float_array_value: name: %s value: %d", name, value);
}

void cb_get_global_float_array_value_with_id(t_CKINT cb_id, t_CKFLOAT value)
{
    post("cb_get_global_float_array_value_with_id: cb_id: %d value: %d", cb_id, value);
}


void cb_get_all_global_vars(const std::vector<Chuck_Globals_TypeValue> & list, void * data)
{
    post("cb_get_all_global_vars:");
    for (auto v : list) {
        post("type: %s name: %s", v.type.c_str(), v.name.c_str());
    }
}


void cb_get_global_assoc_int_array_value(t_CKINT val)
{
    post("cb_get_global_assoc_int_array_value: %d", val);
}

void cb_get_global_named_assoc_int_array_value(const char* name, t_CKINT val)
{
     post("cb_get_global_named_assoc_int_array_value: name: %s value: %d", name, val);
}

void cb_get_global_assoc_int_array_value_with_id(t_CKINT cb_id, t_CKINT val)
{
     post("cb_get_global_associative_int_array_value_with_id: id: %d value: %d", cb_id, val);
}


void cb_get_global_assoc_float_array_value(t_CKFLOAT val)
{
    post("cb_get_global_assoc_float_array_value: %f", val);
}

void cb_get_global_named_assoc_float_array_value(const char* name, t_CKFLOAT val)
{
     post("cb_get_global_named_assoc_float_array_value: name: %s value: %f", name, val);
}

void cb_get_global_assoc_float_array_value_with_id(t_CKINT cb_id, t_CKFLOAT val)
{
     post("cb_get_global_assoc_float_array_value_with_id: id: %d value: %f", cb_id, val);
}

//-----------------------------------------------------------------------------------------------
// callback handlers


t_max_err ck_register(t_ck* x, t_symbol* s)
{
    if (!x->cb_map.count(s->s_name)) {
        error("event/callback not found: %s", s->s_name);
        return MAX_ERR_GENERIC;
    }
    std::string key = std::string(s->s_name);
    ck_named_event_callback cb = x->cb_map[key];
    // false: for a one off call, strue: called everytime it is called
    if (x->chuck->vm()->globals_manager()->listenForGlobalEvent(s->s_name, cb, false)) {
        post("%s event/callback registered", s->s_name);
        return MAX_ERR_NONE;
    };
    return MAX_ERR_GENERIC;
}


t_max_err ck_unregister(t_ck* x, t_symbol* s)
{
    if (!x->cb_map.count(s->s_name)) {
        error("event/callback not found: %s", s->s_name);
        return MAX_ERR_GENERIC;
    }
    std::string key = std::string(s->s_name);
    ck_named_event_callback cb = x->cb_map[key];
    if (x->chuck->vm()->globals_manager()->stopListeningForGlobalEvent(s->s_name, cb)) {
        post("%s event/callback unregistered", s->s_name);
        return MAX_ERR_NONE;
    };
    return MAX_ERR_GENERIC;
}

// t_max_err ck_demo_gevents(t_ck* x)
// {
//     x->chuck->vm()->globals_manager()->listenForGlobalEvent("gevent", cb_global_event, false);
//     x->chuck->vm()->globals_manager()->listenForGlobalEvent("gevent", cb_global_named_event, false);
//     x->chuck->vm()->globals_manager()->listenForGlobalEvent("gevent", 3, cb_global_event_with_id, false);
//     x->chuck->vm()->globals_manager()->stopListeningForGlobalEvent( const char * name, void (*callback)(void) );
//     x->chuck->vm()->globals_manager()->stopListeningForGlobalEvent( const char * name, void (*callback)(const char*) );
//     x->chuck->vm()->globals_manager()->stopListeningForGlobalEvent( const char * name, t_CKINT callbackID, void (*callback)(t_CKINT) );
// }

// t_max_err ck_demo_set_global_arrays(t_ck* x)
// {
//     x->chuck->vm()->globals_manager()->setGlobalIntArray( const char * name, t_CKINT arrayValues[], t_CKUINT numValues );
//     x->chuck->vm()->globals_manager()->setGlobalIntArrayValue( const char * name, t_CKUINT index, t_CKINT value );
//     x->chuck->vm()->globals_manager()->setGlobalAssociativeIntArrayValue( const char * name, const char * key, t_CKINT value );
//     x->chuck->vm()->globals_manager()->setGlobalFloatArray( const char * name, t_CKFLOAT arrayValues[], t_CKUINT numValues );
//     x->chuck->vm()->globals_manager()->setGlobalFloatArrayValue( const char * name, t_CKUINT index, t_CKFLOAT value );
//     x->chuck->vm()->globals_manager()->setGlobalAssociativeFloatArrayValue( const char * name, const char * key, t_CKFLOAT value );
// }

t_max_err ck_demo_min(t_ck* x)
{
    // x->chuck->vm()->globals_manager()->getGlobalUGenSamples( const char * name, SAMPLE* buffer, int numFrames );

    // primitives
    x->chuck->vm()->globals_manager()->getGlobalInt("gint", cb_get_global_named_int);
    x->chuck->vm()->globals_manager()->getGlobalFloat("gfloat", cb_get_global_named_float);
    x->chuck->vm()->globals_manager()->getGlobalString("gstring", cb_get_global_named_string);

    // arrays
    x->chuck->vm()->globals_manager()->getGlobalIntArray("gints", cb_get_global_named_int_array);
    x->chuck->vm()->globals_manager()->getGlobalFloatArray("gfloats", cb_get_global_named_float_array);

    // array values    
    x->chuck->vm()->globals_manager()->getGlobalIntArrayValue("gints", 0, cb_get_global_named_int_array_value);
    x->chuck->vm()->globals_manager()->getGlobalFloatArrayValue("gfloats", 0, cb_get_global_named_float_array_value);

    // associative array values
    x->chuck->vm()->globals_manager()->getGlobalAssociativeIntArrayValue("imap", "a", cb_get_global_named_assoc_int_array_value);
    x->chuck->vm()->globals_manager()->getGlobalAssociativeFloatArrayValue("fmap", "a", cb_get_global_named_assoc_float_array_value);

    return MAX_ERR_NONE;
}

t_max_err ck_demo(t_ck* x)
{
    // ck_spork_last_id(x);
    // ck_spork_next_id(x);

    x->chuck->vm()->globals_manager()->getGlobalInt("gint", cb_get_global_int);
    x->chuck->vm()->globals_manager()->getGlobalInt("gint", cb_get_global_named_int);
    x->chuck->vm()->globals_manager()->getGlobalInt("gint", 3, cb_get_global_int_with_id);

    x->chuck->vm()->globals_manager()->getGlobalFloat("gfloat", cb_get_global_float);
    x->chuck->vm()->globals_manager()->getGlobalFloat("gfloat", cb_get_global_named_float);
    x->chuck->vm()->globals_manager()->getGlobalFloat("gfloat", 3, cb_get_global_float_with_id);

    x->chuck->vm()->globals_manager()->getGlobalString("gstring", cb_get_global_string);
    x->chuck->vm()->globals_manager()->getGlobalString("gstring", cb_get_global_named_string);
    x->chuck->vm()->globals_manager()->getGlobalString("gstring", 3, cb_get_global_string_with_id);

    // x->chuck->vm()->globals_manager()->getGlobalUGenSamples( const char * name, SAMPLE* buffer, int numFrames );

    x->chuck->vm()->globals_manager()->getGlobalIntArray("gints", cb_get_global_int_array);
    x->chuck->vm()->globals_manager()->getGlobalIntArray("gints", cb_get_global_named_int_array);
    x->chuck->vm()->globals_manager()->getGlobalIntArray("gints", 3, cb_get_global_int_array_with_id);

    x->chuck->vm()->globals_manager()->getGlobalIntArrayValue("gints", 0, cb_get_global_int_array_value);
    x->chuck->vm()->globals_manager()->getGlobalIntArrayValue("gints", 0, cb_get_global_named_int_array_value);
    x->chuck->vm()->globals_manager()->getGlobalIntArrayValue("gints", 3, 0, cb_get_global_int_array_value_with_id);

    x->chuck->vm()->globals_manager()->getGlobalAssociativeIntArrayValue("imap", "a", cb_get_global_assoc_int_array_value);
    x->chuck->vm()->globals_manager()->getGlobalAssociativeIntArrayValue("imap", "a", cb_get_global_named_assoc_int_array_value);
    x->chuck->vm()->globals_manager()->getGlobalAssociativeIntArrayValue("imap", 3, "a", cb_get_global_assoc_int_array_value_with_id);

    x->chuck->vm()->globals_manager()->getGlobalFloatArray("gfloats", cb_get_global_float_array);
    x->chuck->vm()->globals_manager()->getGlobalFloatArray("gfloats", cb_get_global_named_float_array);
    x->chuck->vm()->globals_manager()->getGlobalFloatArray("gfloats", 3, cb_get_global_float_array_with_id);

    x->chuck->vm()->globals_manager()->getGlobalFloatArrayValue("gfloats", 0, cb_get_global_float_array_value);
    x->chuck->vm()->globals_manager()->getGlobalFloatArrayValue("gfloats", 0, cb_get_global_named_float_array_value);
    x->chuck->vm()->globals_manager()->getGlobalFloatArrayValue("gfloats", 3, 0, cb_get_global_float_array_value_with_id);

    x->chuck->vm()->globals_manager()->getGlobalAssociativeFloatArrayValue("fmap", "a", cb_get_global_assoc_float_array_value);
    x->chuck->vm()->globals_manager()->getGlobalAssociativeFloatArrayValue("fmap", "a", cb_get_global_named_assoc_float_array_value);
    x->chuck->vm()->globals_manager()->getGlobalAssociativeFloatArrayValue("fmap", 3, "a", cb_get_global_assoc_float_array_value_with_id);

    return MAX_ERR_NONE;
}

