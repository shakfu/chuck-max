// api largely derived extracted chuck-globals, chuckdesigner

bool run_code(const char* code);
bool run_file(const char* filename);
bool run_file_with_args(const char* filename, const char* args);

bool set_global_int(const char* name, t_CKINT val);
bool get_global_int(const char* name, void (*callback)(t_CKINT));
bool get_named_global_int(const char* name, void (*callback)(const char*, t_CKINT));
bool get_global_int_with_id(t_CKINT callbackID, const char* name, void (*callback)(t_CKINT, t_CKINT));

bool set_global_float(const char* name, t_CKFLOAT val);
bool get_global_float(const char* name, void (*callback)(t_CKFLOAT));
bool get_named_global_float(const char* name, void (*callback)(const char*, t_CKFLOAT));
bool get_global_float_with_id(t_CKINT callbackID, const char* name, void (*callback)(t_CKINT, t_CKFLOAT));

bool set_global_string(const char* name, const char* val);
bool get_global_string(const char* name, void (*callback)(const char*));
bool get_named_global_string(const char* name, void (*callback)(const char*, const char*));
bool get_global_string_with_id(t_CKINT callbackID, const char* name, void (*callback)(t_CKINT, const char*));

bool signal_global_event(const char* name);
bool broadcast_global_event(const char* name);
bool listen_for_global_event_once(const char* name, void (*callback)(void));
bool listen_for_named_global_event_once(const char* name, void (*callback)(const char*));
bool listen_for_global_event_once_with_id(t_CKINT callbackID, const char* name, void (*callback)(t_CKINT));
bool start_listening_for_global_event(const char* name, void (*callback)(void));
bool start_listening_for_named_global_event(const char* name, void (*callback)(const char*));
bool start_listening_for_global_event_with_id(t_CKINT callbackID, const char* name, void (*callback)(t_CKINT));
bool stop_listening_for_global_event(const char* name, void (*callback)(void));
bool stop_listening_for_named_global_event(const char* name, void (*callback)(const char*));
bool stop_listening_for_global_event_with_id(t_CKINT callbackID, const char* name, void (*callback)(t_CKINT));

bool get_global_ugen_samples(const char* name, SAMPLE* buffer, int numSamples);

bool set_global_int_array(const char* name, t_CKINT arrayValues[], unsigned int numValues);
bool get_global_int_array(const char* name, void (*callback)(t_CKINT[], t_CKUINT));
bool get_named_global_int_array(const char* name, void (*callback)(const char*, t_CKINT[], t_CKUINT));
bool get_global_int_array_with_id(t_CKINT callbackID, const char* name, void (*callback)(t_CKINT, t_CKINT[], t_CKUINT));

bool set_global_int_array_value(const char* name, unsigned int index, t_CKINT value);
bool get_global_int_array_value(const char* name, unsigned int index, void (*callback)(t_CKINT));
bool get_named_global_int_array_value(const char* name, unsigned int index, void (*callback)(const char*, t_CKINT));
bool get_global_int_array_value_with_id(t_CKINT callbackID, const char* name, unsigned int index, void (*callback)(t_CKINT, t_CKINT));

bool set_global_associative_int_array_value(const char* name, char* key, t_CKINT value);
bool get_global_associative_int_array_value(const char* name, char* key, void (*callback)(t_CKINT));
bool get_named_global_associative_int_array_value(const char* name, char* key, void (*callback)(const char*, t_CKINT));
bool get_global_associative_int_array_value_with_id(t_CKINT callbackID, const char* name, char* key, void (*callback)(t_CKINT, t_CKINT));

bool set_global_float_array(const char* name, t_CKFLOAT arrayValues[], unsigned int numValues);
bool get_global_float_array(const char* name, void (*callback)(t_CKFLOAT[], t_CKUINT));
bool get_named_global_float_array(const char* name, void (*callback)(const char*, t_CKFLOAT[], t_CKUINT));
bool get_global_float_array_with_id(t_CKINT callbackID, const char* name, void (*callback)(t_CKINT, t_CKFLOAT[], t_CKUINT));

bool set_global_float_array_value(const char* name, unsigned int index, t_CKFLOAT value);
bool get_global_float_array_value(const char* name, unsigned int index, void (*callback)(t_CKFLOAT));
bool get_named_global_float_array_value(const char* name, unsigned int index, void (*callback)(const char*, t_CKFLOAT));
bool get_global_float_array_value_with_id(t_CKINT callbackID, const char* name, unsigned int index, void (*callback)(t_CKINT, t_CKFLOAT));

bool set_global_associative_float_array_value(const char* name, char* key, t_CKFLOAT value);
bool get_global_associative_float_array_value(const char* name, char* key, void (*callback)(t_CKFLOAT));
bool get_named_global_associative_float_array_value(const char* name, char* key, void (*callback)(const char*, t_CKFLOAT));
bool get_global_associative_float_array_value_with_id(t_CKINT callbackID, const char* name, char* key, void (*callback)(t_CKINT, t_CKFLOAT));
