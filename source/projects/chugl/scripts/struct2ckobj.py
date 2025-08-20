out = ""
original_type = "b2Polygon"
struct_name = "b2_"
data_offset = "b2_shape_def_data_offset"
class_doc = '''
    Used to create a shape.
    This is a temporary object used to bundle shape creation parameters. You may use
    the same shape definition to create multiple shapes.
    Must be initialized using b2DefaultShapeDef().
'''


# (type, name, info)
struct_info = [
    # ("vec2", "gravity", "Gravity vector. Box2D has no up-vector defined."),
]

def l(s):
    global out
    out += s + '\n'

def ck_dll_mfun_getter_name(field_name):
    return f"{struct_name}_get_{field_name}"
def ck_dll_mfun_setter_name(field_name):
    return f"{struct_name}_set_{field_name}"

l(f"static t_CKUINT {data_offset} = 0;")
l("")

# impl ---------------------------------------------

l(f"// ============================================================================")
l(f"// {struct_name}")
l(f"// ============================================================================")
l("")

# ctor
l(f"CK_DLL_CTOR({struct_name}_ctor)")
l("{")
l(f"OBJ_MEMBER_UINT(SELF, {data_offset}) = (t_CKUINT)(new {original_type});")
l("}")
l("")


# dtor
l(f"CK_DLL_DTOR({struct_name}_dtor)")
l("{")
l(f"CHUGIN_SAFE_DELETE({original_type}, {data_offset});")
l("}")
l("")


# getters
for type, name, comment in struct_info:
    l(f"CK_DLL_MFUN({ck_dll_mfun_getter_name(name)})")
    l("{")
    l(f"{original_type}* data = ({original_type}*)OBJ_MEMBER_UINT(SELF, {data_offset});")
    if (type == "vec2"):
        l(f"RETURN->v_{type} = {{  data->{name}.x, data->{name}.y }};")
    else:
        l(f"RETURN->v_{type} = data->{name};")
    l("}")
    l("")


# setters
for type, name, comment in struct_info:
    l(f"CK_DLL_MFUN({ck_dll_mfun_setter_name(name)})")
    l("{")
    l(f"{original_type}* data = ({original_type}*)OBJ_MEMBER_UINT(SELF, {data_offset});")
    if (type == "vec2"):
        l(f"t_CKVEC2 {name}_vec2 = GET_NEXT_VEC2(ARGS);")
        l(f"data->{name} = {{ (float) {name}_vec2.x, (float) {name}_vec2.y }};")
    else:
        l(f"data->{name} = GET_NEXT_{type.upper()}(ARGS);")
    l("}")
    l("")

# query ---------------------------------------------
l(f"// {struct_name} --------------------------------------")
l(f"BEGIN_CLASS(\"{struct_name}\", \"Object\");")
l(f"DOC_CLASS(\"{class_doc}\");")
l(f"{data_offset} = MVAR(\"int\", \"@{data_offset}\", false);")
l(f"CTOR({struct_name}_ctor);")
l(f"DTOR({struct_name}_dtor);")
l(f"")
# member getters
l(f"// getters")
for type, name, comment in struct_info:
    l(f"MFUN({ck_dll_mfun_getter_name(name)}, \"{type}\", \"{name}\");")
    l(f"DOC_FUNC(\"{comment}\");")
    l(f"")
# member setters
l(f"// setters")
for type, name, comment in struct_info:
    l(f"MFUN({ck_dll_mfun_setter_name(name)}, \"void\", \"{name}\");")
    l(f"ARG(\"{type}\", \"{name}\");")
    l(f"DOC_FUNC(\"{comment}\");")
    l(f"")

l(f"END_CLASS(); // {struct_name}")

print(out)


