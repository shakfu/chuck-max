//-----------------------------------------------------------------------------
// name: geo_and_mat.ck
// desc: UI-driven example for geometry + material
// requires: ChuGL + chuck-1.5.3.0 or higher
//-----------------------------------------------------------------------------
// camera
GOrbitCamera cam --> GG.scene();
// select main camera
GG.scene().camera( cam );
// mesh
GMesh mesh --> GG.scene();

[
    null,
    new PlaneGeometry,
    new SuzanneGeometry,
    new SphereGeometry,
    new CubeGeometry,
    new CircleGeometry,
    new TorusGeometry,
    new CylinderGeometry,
    new KnotGeometry,
    new PolygonGeometry,
] @=> Geometry geometries[];

UI_Int geometry_index;
[ "None" ] @=> string builtin_geometries[];
for (1 => int i; i < geometries.size(); i++) {
    builtin_geometries << Type.of(geometries[i]).name();
}

UVMaterial uv_material;
NormalMaterial normal_material;
WireframeMaterial wireframe_material;
FlatMaterial flat_material;
PhongMaterial phong_material;
PBRMaterial pbr_material;

[
    null,
    uv_material,
    normal_material,
    wireframe_material,
    flat_material,
    phong_material,
    pbr_material,
] @=> Material materials[];

UI_Int material_index;
[ "None" ] @=> string builtin_materials[];
for (1 => int i; i < materials.size(); i++) {
    builtin_materials << Type.of(materials[i]).name();
}

// Material params
UI_Int material_topology_index(3); // default to triangle list
[
    "PointList",
    "LineList",
    "LineStrip",
    "TriangleList",
    "TriangleStrip",
] @=> string material_topologies[];

// Wireframe material params
UI_Float wireframe_thickness(wireframe_material.thickness());
UI_Float wireframe_alpha_cutoff(wireframe_material.alphaCutoff());
UI_Float3 wireframe_color(wireframe_material.color());

// Normal material params
UI_Bool normal_material_worldspace(normal_material.worldspaceNormals());

// Phong material params
UI_Float3 phong_specular(phong_material.specular());
UI_Float3 phong_diffuse(phong_material.color());
UI_Float phong_shine(phong_material.shine());
UI_Float3 phong_emission(phong_material.emission());
UI_Float phong_normal_factor(phong_material.normalFactor());
UI_Float phong_ao_factor(phong_material.aoFactor());

// PBR material params
UI_Float3 pbr_albedo(pbr_material.color());
UI_Float pbr_metallic(pbr_material.metallic());
UI_Float pbr_roughness(pbr_material.roughness());
UI_Float pbr_ao_factor(pbr_material.aoFactor());
UI_Float pbr_normal_factor(pbr_material.normalFactor());

// Plane geometry params
geometries[1] $ PlaneGeometry @=> PlaneGeometry@ plane_geo;
UI_Float plane_width(plane_geo.width());
UI_Float plane_height(plane_geo.height());
UI_Int plane_width_segments(plane_geo.widthSegments());
UI_Int plane_height_segments(plane_geo.heightSegments());
fun void buildPlane() {
    plane_geo.build(
        plane_width.val(),
        plane_height.val(),
        plane_width_segments.val(),
        plane_height_segments.val()
    );
}

// sphere geometry params
geometries[3] $ SphereGeometry @=> SphereGeometry@ sphere_geo;
UI_Float sphere_radius(sphere_geo.radius());
UI_Int sphere_width(sphere_geo.widthSegments());
UI_Int sphere_height(sphere_geo.heightSegments());
UI_Float sphere_phi_start(sphere_geo.phiStart());
UI_Float sphere_phi_length(sphere_geo.phiLength());
UI_Float sphere_theta_start(sphere_geo.thetaStart());
UI_Float sphere_theta_length(sphere_geo.thetaLength());
fun void buildSphere() {
    sphere_geo.build(
        sphere_radius.val(),
        sphere_width.val(),
        sphere_height.val(),
        sphere_phi_start.val(),
        sphere_phi_length.val(),
        sphere_theta_start.val(),
        sphere_theta_length.val()
    );
}

// box geometry params
geometries[4] $ CubeGeometry @=> CubeGeometry@ cube_geo;
UI_Float box_width(cube_geo.width());
UI_Float box_height(cube_geo.height());
UI_Float box_depth(cube_geo.depth());
UI_Int box_width_segments(cube_geo.widthSegments());
UI_Int box_height_segments(cube_geo.heightSegments());
UI_Int box_depth_segments(cube_geo.depthSegments());
fun void buildBox() {
    cube_geo.build(
        box_width.val(),
        box_height.val(),
        box_depth.val(),
        box_width_segments.val(),
        box_height_segments.val(),
        box_depth_segments.val()
    );
}

// circle geometry params
geometries[5] $ CircleGeometry @=> CircleGeometry@ circle_geo;
UI_Float circle_radius(circle_geo.radius());
UI_Int circle_segments(circle_geo.segments());
UI_Float circle_theta_start(circle_geo.thetaStart());
UI_Float circle_theta_length(circle_geo.thetaLength());
fun void buildCircle() {
    circle_geo.build(
        circle_radius.val(),
        circle_segments.val(),
        circle_theta_start.val(),
        circle_theta_length.val()
    );
}

// torus geometry params
geometries[6] $ TorusGeometry @=> TorusGeometry@ torus_geo;
UI_Float torus_radius(torus_geo.radius());
UI_Float torus_tube_radius(torus_geo.tubeRadius());
UI_Int torus_radial_segments(torus_geo.radialSegments());
UI_Int torus_tubular_segments(torus_geo.tubularSegments());
UI_Float torus_arc_length(torus_geo.arcLength());
fun void buildTorus() {
    torus_geo.build(
        torus_radius.val(),
        torus_tube_radius.val(),
        torus_radial_segments.val(),
        torus_tubular_segments.val(),
        torus_arc_length.val()
    );
}

// cylinder geometry params
geometries[7] $ CylinderGeometry @=> CylinderGeometry@ cylinder_geo;
UI_Float cylinder_radius_top(cylinder_geo.radiusTop());
UI_Float cylinder_radius_bottom(cylinder_geo.radiusBottom());
UI_Float cylinder_height(cylinder_geo.height());
UI_Int cylinder_radial_segments(cylinder_geo.radialSegments());
UI_Int cylinder_height_segments(cylinder_geo.heightSegments());
UI_Bool cylinder_open_ended(cylinder_geo.openEnded());
UI_Float cylinder_theta_start(cylinder_geo.thetaStart());
UI_Float cylinder_theta_length(cylinder_geo.thetaLength());
fun void buildCylinder() {
    cylinder_geo.build(
        cylinder_radius_top.val(),
        cylinder_radius_bottom.val(),
        cylinder_height.val(),
        cylinder_radial_segments.val(),
        cylinder_height_segments.val(),
        cylinder_open_ended.val(),
        cylinder_theta_start.val(),
        cylinder_theta_length.val()
    );
}

// Knot geometry params
geometries[8] $ KnotGeometry @=> KnotGeometry@ knot_geo;
UI_Float knot_radius(knot_geo.radius());
UI_Float knot_tube(knot_geo.tube());
UI_Int knot_tubular_segments(knot_geo.tubularSegments());
UI_Int knot_radial_segments(knot_geo.radialSegments());
UI_Int knot_p(knot_geo.p());
UI_Int knot_q(knot_geo.q());
fun void buildKnot() {
    knot_geo.build(
        knot_radius.val(),
        knot_tube.val(),
        knot_tubular_segments.val(),
        knot_radial_segments.val(),
        knot_p.val(),
        knot_q.val()
    );
}

UI_Bool rotate;
fun void ui() {
    while (true) {
        GG.nextFrame() => now; 
        UI.setNextWindowBgAlpha(0.00);
        if (UI.begin("Geometry and Material Example")) {

            UI.scenegraph(GG.scene());

            UI.checkbox("rotate", rotate);

            if (UI.listBox("builtin geometries", geometry_index, builtin_geometries)) {
                mesh.geometry(geometries[geometry_index.val()]);
            }

            if (UI.listBox("builtin materials", material_index, builtin_materials)) {
                mesh.material(materials[material_index.val()]);

                // update material params
                if (mesh.material() != null) {
                    material_topology_index.val() => mesh.material().topology;
                }
            }

            UI.separatorText("Base Material Params");

            if (mesh.material() != null) {

                // material topology
                if (UI.listBox("topology", material_topology_index, material_topologies)) {
                    <<< "setting topology to", material_topology_index.val() >>>;
                    mesh.material().topology(material_topology_index.val());
                }
            }

            if (mesh.material() == normal_material) {
                UI.separatorText("Normal Material Params");

                if (UI.checkbox("worldspace normals", normal_material_worldspace)) {
                    normal_material.worldspaceNormals(normal_material_worldspace.val());
                }
            }

            if (mesh.material() == wireframe_material) {
                UI.separatorText("Wireframe Material Params");

                if (UI.slider("thickness", wireframe_thickness, 0, 5)) {
                    wireframe_thickness.val() => wireframe_material.thickness;
                }

                if (UI.slider("alpha cutoff", wireframe_alpha_cutoff, 0, 1)) {
                    wireframe_alpha_cutoff.val() => wireframe_material.alphaCutoff;
                }

                if (UI.colorEdit("color", wireframe_color, 0)) {
                    wireframe_color.val() => wireframe_material.color;
                }
            }

            if (mesh.material() == phong_material) {
                UI.separatorText("Phong Material Params");
                if (UI.colorEdit("specular", phong_specular, 0)) {
                    phong_specular.val() => phong_material.specular;
                }

                if (UI.colorEdit("diffuse", phong_diffuse, 0)) {
                    phong_diffuse.val() => phong_material.color;
                }

                if (UI.slider("shine", phong_shine, 0, 10)) {
                    phong_shine.val() => phong_material.shine;
                }

                if (UI.colorEdit("emission", phong_emission, 0)) {
                    phong_emission.val() => phong_material.emission;
                }

                if (UI.slider("normal factor", phong_normal_factor, 0, 1)) {
                    phong_normal_factor.val() => phong_material.normalFactor;
                }

                if (UI.slider("ao factor", phong_ao_factor, 0, 1)) {
                    phong_ao_factor.val() => phong_material.aoFactor;
                }
            }

            if (mesh.material() == pbr_material)
            {
                UI.separatorText("PBR Material Params");
                if (UI.colorEdit("albedo", pbr_albedo, 0)) {
                    pbr_albedo.val() => pbr_material.color;
                }

                if (UI.slider("metallic", pbr_metallic, 0, 1)) {
                    pbr_metallic.val() => pbr_material.metallic;
                }

                if (UI.slider("roughness", pbr_roughness, 0, 1)) {
                    pbr_roughness.val() => pbr_material.roughness;
                }

                if (UI.slider("ao factor", pbr_ao_factor, 0, 1)) {
                    pbr_ao_factor.val() => pbr_material.aoFactor;
                }

                if (UI.slider("normal factor", pbr_normal_factor, 0, 1)) {
                    pbr_normal_factor.val() => pbr_material.normalFactor;
                }
            }

            UI.separatorText("Geometry Params");

            // plane geometry params
            if (mesh.geometry() == plane_geo) {
                if (UI.slider("width", plane_width, 0.1, 10)) buildPlane();
                if (UI.slider("height", plane_height, 0.1, 10)) buildPlane();
                if (UI.slider("width segments", plane_width_segments, 1, 64)) buildPlane();
                if (UI.slider("height segments", plane_height_segments, 1, 64)) buildPlane();
            }
            
            // sphere geometry params
            if (mesh.geometry() == sphere_geo) {
                if (UI.slider("radius", sphere_radius, 0.1, 10)) buildSphere();
                if (UI.slider("width segments", sphere_width, 3, 64)) buildSphere();
                if (UI.slider("height segments", sphere_height, 2, 64)) buildSphere();
                if (UI.slider("phi start", sphere_phi_start, 0, 2 * Math.PI)) buildSphere();
                if (UI.slider("phi length", sphere_phi_length, 0, 2 * Math.PI)) buildSphere();
                if (UI.slider("theta start", sphere_theta_start, 0, Math.PI)) buildSphere();
                if (UI.slider("theta length", sphere_theta_length, 0, Math.PI)) buildSphere();
            }

            // box geometry params
            if (mesh.geometry() == cube_geo) {
                if (UI.slider("width", box_width, 0.1, 10)) buildBox();
                if (UI.slider("height", box_height, 0.1, 10)) buildBox();
                if (UI.slider("depth", box_depth, 0.1, 10)) buildBox();
                if (UI.slider("width segments", box_width_segments, 1, 64)) buildBox();
                if (UI.slider("height segments", box_height_segments, 1, 64)) buildBox();
                if (UI.slider("depth segments", box_depth_segments, 1, 64)) buildBox();
            }

            // circle geometry params
            if (mesh.geometry() == circle_geo) {
                if (UI.slider("radius", circle_radius, 0.1, 10)) buildCircle();
                if (UI.slider("segments", circle_segments, 1, 64)) buildCircle();
                if (UI.slider("theta start", circle_theta_start, 0, 2 * Math.PI)) buildCircle();
                if (UI.slider("theta length", circle_theta_length, 0, 2 * Math.PI)) buildCircle();
            }

            // torus geometry params
            if (mesh.geometry() == torus_geo) {
                if (UI.slider("radius", torus_radius, 0.1, 10)) buildTorus();
                if (UI.slider("tube radius", torus_tube_radius, 0.1, 10)) buildTorus();
                if (UI.slider("radial segments", torus_radial_segments, 3, 64)) buildTorus();
                if (UI.slider("tubular segments", torus_tubular_segments, 3, 64)) buildTorus();
                if (UI.slider("arc length", torus_arc_length, 0, 2 * Math.PI)) buildTorus();
            }

            // cylinder geometry params
            if (mesh.geometry() == cylinder_geo) {
                if (UI.slider("radius top", cylinder_radius_top, 0.1, 10)) buildCylinder();
                if (UI.slider("radius bottom", cylinder_radius_bottom, 0.1, 10)) buildCylinder();
                if (UI.slider("height", cylinder_height, 0.1, 10)) buildCylinder();
                if (UI.slider("radial segments", cylinder_radial_segments, 3, 64)) buildCylinder();
                if (UI.slider("height segments", cylinder_height_segments, 1, 64)) buildCylinder();
                if (UI.checkbox("open ended", cylinder_open_ended)) buildCylinder();
                if (UI.slider("theta start", cylinder_theta_start, 0, 2 * Math.PI)) buildCylinder();
                if (UI.slider("theta length", cylinder_theta_length, 0, 2 * Math.PI)) buildCylinder();
            }

            // knot geometry params
            if (mesh.geometry() == knot_geo) {
                if (UI.slider("radius", knot_radius, 0.1, 10)) buildKnot();
                if (UI.slider("tube", knot_tube, 0.1, 10)) buildKnot();
                if (UI.slider("tubular segments", knot_tubular_segments, 3, 64)) buildKnot();
                if (UI.slider("radial segments", knot_radial_segments, 3, 64)) buildKnot();
                if (UI.slider("p", knot_p, 1, 20)) buildKnot();
                if (UI.slider("q", knot_q, 1, 20)) buildKnot();
            }
        }
        UI.end();
    }
}
spork ~ ui();

while (true) {
    GG.nextFrame() => now;
    if (rotate.val()) {
        GG.dt() * .3 => mesh.rotateY;
    }
}

// TODO
/*
Add builtin textures to apply to materials
Add builtin skybox + IBL lighting
*/
