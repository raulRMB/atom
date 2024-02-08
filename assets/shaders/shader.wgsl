struct VertexInput {
	@location(0) position: vec3f,
	@location(1) color: vec4f,
}

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) color: vec4f,
}

struct MyUniforms {
    color : vec4f,
    time : f32,
    pad0 : f32,
    pad1 : f32,
    pad2 : f32,
}

@group(0) @binding(0) var<uniform> uMyUniforms: MyUniforms;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    var ratio: f32 = 640 / 480;
    var offset = vec2f(-0.5, -0.5);
    offset += 0.3 * vec2f(cos(uMyUniforms.time), sin(uMyUniforms.time));
    out.position = vec4f(in.position.x + offset.x, (in.position.y + offset.y) * ratio, in.position.z, 1.0);
    out.color = in.color;
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    let color = in.color.rgb * uMyUniforms.color.rgb;
    let corrected_color = pow(color, vec3f(2.2));
    return vec4f(corrected_color, uMyUniforms.color.a);
}