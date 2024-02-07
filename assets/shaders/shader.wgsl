struct VertexInput {
	@location(0) position: vec3f,
	@location(1) color: vec4f,
}

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) color: vec4f,
}

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    out.position = vec4f(in.position, 1.0);
    out.color = in.color; // forward to the fragment shader
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    return in.color;
}