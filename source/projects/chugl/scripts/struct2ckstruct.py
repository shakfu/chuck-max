# converts a c struct into a ck struct (no pointer)
# e.g. 
# struct {
#     float a, b;
# }
# becomes
# class {
#     float a;
#     float b;
# }

out = ""
prefix = "b2"
original_type = "b2SegmentDistanceResult"
struct_name = original_type

class_doc = "Result of computing the distance between two line segments. https://box2d.org/documentation/group__distance.html#structb2_segment_distance_result"
# (type, name, comment)
struct_info = [
    # ("vec2", "gravity", "Gravity vector. Box2D has no up-vector defined."),
    # ("float", "restitutionThreshold", "Restitution velocity threshold, usually in m/s. Collisions above this speed have restitution applied (will bounce)."),
    # ("float", "contactPushoutVelocity", "This parameter controls how fast overlap is resolved and has units of meters per second"),
    # ("float", "hitEventThreshold", "Threshold velocity for hit events. Usually meters per second."),
    # ("float", "contactHertz", "Contact stiffness. Cycles per second."),
    # ("float", "contactDampingRatio", "Contact bounciness. Non-dimensional."),
    # ("float", "jointHertz", "Joint stiffness. Cycles per second."),
    # ("float", "jointDampingRatio", "Joint bounciness. Non-dimensional."),
    # ("int", "enableSleep", "Can bodies go to sleep to improve performance"),
    # ("int", "enableContinous", "Enable continuous collision"),
    # ("int", "workerCount", "Number of workers to use with the provided task system. Box2D performs best when using only performance cores and accessing a single L2 cache. Efficiency cores and hyper-threading provide little benefit and may even harm performance."),

    ("vec2", "closest1", "The closest point on the first segment"),
    ("vec2", "closest2", "The closest point on the second segment"),
    ("float", "fraction1", "The barycentric coordinate on the first segment"),
    ("float", "fraction2", "The barycentric coordinate on the second segment"),
    ("float", "distanceSquared", "The squared distance between the closest points"),
]

def l(s):

    global out
    out += s + '\n'

def ck_offset_name(name):
    return f"{struct_name}_{name}_offset"

def ck_ctor_name():
    return f"{struct_name}_ctor"

# member data offsets ---------------------------------------------
l(f"// {struct_name}")
for (type, name, comment) in struct_info:
    l(f"static t_CKUINT {ck_offset_name(name)} = 0;")
l(f"CK_DLL_CTOR({ck_ctor_name()});")
l("")
l(f"static void {original_type}_to_ckobj(Chuck_Object* ckobj, {original_type}* obj);")
l(f"static void ckobj_to_{original_type}({original_type}* obj, Chuck_Object* ckobj);")

# query
l(f"{{ // {struct_name} --------------------------------------")
l(f"BEGIN_CLASS(\"{struct_name}\", \"Object\");")
l(f"// clang-format off")
l(f"DOC_CLASS(\"{class_doc}\");")
l(f"// clang-format on")

l("")
l(f"CTOR({ck_ctor_name()});")
l("")

for (type, name, comment) in struct_info:
    # if type starts with prefix, add underscore
    # e.g. b2Filter -> b2_Filter
    if (type.startswith(prefix)):
        type = prefix + "_" + type[len(prefix):]

    l(f"{ck_offset_name(name)} = MVAR(\"{type}\", \"{(name)}\", false);")
    l(f"DOC_VAR(\"{comment}\");")
    l("")

l(f"END_CLASS();")
l(f"}} // {struct_name} ")


# impl ---------------------------------------------

l(f"// ============================================================================")
l(f"// {struct_name}")
l(f"// ============================================================================")
l("")
l(f"static void {original_type}_to_ckobj(Chuck_Object* ckobj, {original_type}* obj)")
l("{")
l("    CK_DL_API API = g_chuglAPI;")
for (type, name, comment) in struct_info:
    if (type == "int"):
        l(f"OBJ_MEMBER_INT(ckobj, {ck_offset_name(name)}) = obj->{name};")
    elif (type == "float"):
        l(f"OBJ_MEMBER_FLOAT(ckobj, {ck_offset_name(name)}) = obj->{name};")
    elif (type == "vec2"):
        l(f"OBJ_MEMBER_VEC2(ckobj, {ck_offset_name(name)}) = {{ obj->{name}.x, obj->{name}.y }};")
    else: # must be object
        l(f"{type}_to_ckobj(API, OBJ_MEMBER_OBJECT(ckobj, {ck_offset_name(name)}), &obj->{name});")
l("}")
l("")
l(f"static void ckobj_to_{original_type}({original_type}* obj, Chuck_Object* ckobj)")
l("{")
l("    CK_DL_API API = g_chuglAPI;")
for (type, name, comment) in struct_info:
    if (type == "int"):
        l(f"obj->{name} = OBJ_MEMBER_INT(ckobj, {ck_offset_name(name)});")
    elif (type == "float"):
        l(f"obj->{name} = (float) OBJ_MEMBER_FLOAT(ckobj, {ck_offset_name(name)});")
    elif (type == "vec2"):
        l(f"t_CKVEC2 {name}_vec2 = OBJ_MEMBER_VEC2(ckobj, {ck_offset_name(name)});")
        l(f"obj->{name} = {{ (float) {name}_vec2.x, (float) {name}_vec2.y }};")
    else: # must be object
        l(f"ckobj_to_{type}(API, &obj->{name}, OBJ_MEMBER_OBJECT(ckobj, {ck_offset_name(name)}));")
l("}")
l("")

l(f"CK_DLL_CTOR({ck_ctor_name()})")
l("{")
l("}")

print(out)


