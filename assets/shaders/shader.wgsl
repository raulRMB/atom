struct VertexInput {
	@location(0) position: vec3f,
    @location(1) normal: vec3f,
	@location(2) color: vec4f,
    @location(3) uv: vec2f
}

struct VertexOutput {
	@builtin(position) position: vec4f,
	@location(0) color: vec4f,
    @location(1) normal: vec3f,
    @location(2) uv: vec2f
}

struct MyUniforms {
    model : mat4x4<f32>,
    view : mat4x4<f32>,
    proj : mat4x4<f32>
}

@group(0) @binding(0) var<uniform> u_uniforms: MyUniforms;
@group(0) @binding(1) var texture: texture_2d<f32>;
@group(0) @binding(2) var texture_sampler: sampler;

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    out.position = u_uniforms.proj * u_uniforms.view * u_uniforms.model * vec4f(in.position, 1.0);
    out.normal = in.normal;
    out.uv = in.uv;
    out.color = in.color;
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    let texel_coords = vec2i(in.uv * vec2f(textureDimensions(texture)));
    let color = textureSample(texture, texture_sampler, in.uv).rgb;
    
    return vec4f(color.xyz, 1.0) * in.color;
}