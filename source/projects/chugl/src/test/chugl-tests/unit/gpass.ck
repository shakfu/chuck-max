T.assert(GG.rootPass() != null, "root pass is not null");

ScreenPass pass0, pass1;

T.assert(pass0.next() == null, "default next is NULL");
pass0 --> pass1;
T.assert(pass0.next() == pass1, "next is pass1");
T.assert(pass1.next() == null, "pass1 next is NULL");
pass0 --< pass1;
T.assert(pass0.next() == null, "ungruck pass is null");


RenderPass rpass;
T.assert(rpass.colorOutput() == null, "default target is null");
Texture target;
rpass.colorOutput(target);
T.assert(rpass.colorOutput() == target, "target is target");
rpass.colorOutput(null);
T.assert(rpass.colorOutput() == null, "target is null again");

T.assert(rpass.clear(), "default auto clear color is true");
rpass.clear(false);
T.assert(!rpass.clear(), "auto clear color is false");

T.assert(GG.renderPass().next() == GG.outputPass(), "default render pass next is output pass");

OutputPass opass;
T.assert(opass.gamma(), "default apply gamma correction");