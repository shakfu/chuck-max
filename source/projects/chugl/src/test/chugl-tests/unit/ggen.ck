// component ID
GGen A; GGen B;
T.assert(A.id() + 1 == B.id(), "ids not sequential");

// component Name
T.assert(A.name() == "", "name not empty");
// A.name("A");
"A" => A.name;
T.assert(A.name() == "A", "Expected name A, but name was: " + A.name());
"1234567890" + "1234567890" + "1234567890" + "1234567890" + "1234567890" + "1234567890" + "1234567890" => A.name;
// make sure name is truncated properly to 63 chars
T.assert(A.name() == "123456789012345678901234567890123456789012345678901234567890123", "name not truncated");
T.assert(A.name().length() == 63, "name not truncated to 63 chars");

// identity transform
T.assert(
    A.pos() == @(0, 0, 0) && A.posX() == 0 && A.posY() == 0 && A.posZ() == 0 &&
    A.rot() == @(0, 0, 0) && A.rotX() == 0 && A.rotY() == 0 && A.rotZ() == 0 &&
    A.sca() == @(1, 1, 1) && A.scaX() == 1 && A.scaY() == 1 && A.scaZ() == 1,
    "transform not identity"
);

// translation
A.translate(@(1,2,3));
T.assert(
    A.pos() == @(1, 2, 3) && A.posX() == 1 && A.posY() == 2 && A.posZ() == 3,
    "translation not set correctly"
);

// translate x, y, z
A.translateX(-1);
A.translateY(-2);
A.translateZ(-3);
T.assert(
    A.pos() == @(0, 0, 0) && A.posX() == 0 && A.posY() == 0 && A.posZ() == 0,
    "translate x, y, z not set correctly"
);

// rotation
A.rotateOnLocalAxis(@(0, 0, 1), Math.pi/2);
T.assert(
    T.feq(A.rotZ(), Math.pi/2) && A.rotX() == 0 && A.rotY() == 0,
    "rotation not set correctly"
);

// A.rot(@(Math.pi/2, 0, 3));
// A.rotateZ(Math.pi/2);
// <<< A.rotZ(), Math.pi/2 >>>;

// scenegraph relationships
T.assert(
    A.parent() == null && A.numChildren() == 0,
    "gruck 0"
);

A --> B;

T.assert(
    A.parent() == B && A.numChildren() == 0
    && B.child() == A && B.numChildren() == 1,
    "gruck 1"
);

// forming a cycle 
B --> A;

T.assert(
    A.parent() == B && A.numChildren() == 0
    && B.child() == A && B.numChildren() == 1,
    "no cycles allowed"
);

GG.scene().numChildren() => int num_children;

T.assert(A.scene() == null, "scene not set");
A --> GG.scene();
T.assert(A.scene() == GG.scene(), "scene set");

T.assert(
    A.parent() == GG.scene() 
    && GG.scene().numChildren() == (num_children + 1)
    && GG.scene().child(num_children) == A,
    "grucking to scene"
);

GGen C;

C --> A;
B --> A;
A --> GG.scene();

// TODO add refcounting tests after impl gc

A.detachParent();
T.assert( A.parent() == null && A.numChildren() == 2, "detaching parent");
// T.assert( Machine.refcount(A) == refcount - 1, "detach parent refcount");

A-->GG.scene();
A.detachChildren();
T.assert( A.parent() == GG.scene() && A.numChildren() == 0, "detaching children");
// T.assert( Machine.refcount(A) == refcount - 2, "detach children refcount");

C --> A;
B --> A;
A.detach();
T.assert( A.parent() == null && A.numChildren() == 0, "detaching all");
// T.assert( Machine.refcount(A) == refcount - 3, "detach all refcount");
