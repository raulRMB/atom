struct VertexInput {
	@location(0) position: vec3f,
	@location(1) color: vec4f,
}

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) color: vec4f,
}

@group(0) @binding(0) var<uniform> uTime: f32;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    var ratio: f32 = 640 / 480;
    var offset = vec2f(-0.5, -0.5);
    offset += 0.3 * vec2f(cos(uTime), sin(uTime));
    out.position = vec4f(in.position.x + offset.x, (in.position.y + offset.y) * ratio, 0.0, 1.0);
    out.color = in.color;
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    return vec4f(.4, .4, 0., 1.);
}