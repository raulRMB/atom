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
    @location(2) uv: vec2f,
    @location(3) view_pos: vec3f
}

struct MyUniforms {
    model : mat4x4<f32>,
    view : mat4x4<f32>,
    proj : mat4x4<f32>,
    view_pos: vec3f,
    pad : f32
}

@group(0) @binding(0) var<uniform> u_uniforms: MyUniforms;
@group(0) @binding(1) var texture_sampler: sampler;

@group(0) @binding(2) var base_color_texture: texture_2d<f32>;
@group(0) @binding(3) var normal_texture: texture_2d<f32>;
@group(0) @binding(4) var roughness_texture: texture_2d<f32>;
@group(0) @binding(5) var metallic_texture: texture_2d<f32>;

const light_pos = vec3f(0.0, -1.0, 0.0);
const light_color = vec3f(1.0, 1.0, 1.0);

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    out.position = u_uniforms.proj * u_uniforms.view * u_uniforms.model * vec4f(in.position, 1.0);
    out.normal = in.normal;
    out.uv = in.uv;
    out.color = in.color;
    out.view_pos = u_uniforms.view_pos;
    return out;
}

fn mix_vec3(a: vec3<f32>, b: vec3<f32>, t: f32) -> vec3<f32> {
    return a * (1.0 - t) + b * t;
}

fn D(alpha: f32, n: vec3f, h: vec3f) -> f32 {
    let numerator = pow(alpha, 2.0);
    let n_dot_h = max(dot(n, h), 0.0);
    var denominator = pow(pow(n_dot_h, 2.0) * (numerator - 1.0) + 1.0, 2.0);
    denominator = max(denominator, 0.000001);

    return numerator / denominator;
}

fn G1(alpha: f32, n: vec3f, x: vec3f) -> f32 {
    let numerator = max(dot(n, x), 0.0);
    let k = alpha / 2.0;
    var denominator = max(dot(n, x), 0.0) * (1.0 - k) + k;
    denominator = max(denominator, 0.000001);

    return numerator / denominator;
}

fn G(alpha: f32, n: vec3f, v: vec3f, l: vec3f) -> f32 {
    return G1(alpha, n, v) * G1(alpha, n, l);
}

fn F(f0: vec3f, v: vec3f, h: vec3f) -> vec3f {
    return f0 + (vec3f(1.0) - f0) * pow(1.0 - max(dot(v, h), 0.0), 5.0);
}

fn PBR(color: vec3f, v: vec3f, h: vec3f, n: vec3f, l: vec3f) -> vec3f
{
    let f0 = vec3f(0.04);
    let alpha = 1.0;

    let ks = F(f0, v, h);
    let kd = vec3f(1.0) - ks;

    let lambertian = color / 3.14159265359;

    let cookTorranceNumerator = D(alpha, n, h) * G(alpha, n, v, l) * F(f0, v, h);
    var cookTorranceDenominator = 4.0 * max(dot(n, v), 0.0) * max(dot(n, l), 0.0);
    cookTorranceDenominator = max(cookTorranceDenominator, 0.000001);
    let cookTorrance = cookTorranceNumerator / cookTorranceDenominator;

    let brdf = kd * lambertian + cookTorrance;
    let outgoing = brdf * light_color * max(dot(n, l), 0.0);

    return outgoing;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    let n = normalize(mix_vec3(in.normal, textureSample(normal_texture, texture_sampler, in.uv).xyz, 0.5));
    let v = normalize(in.view_pos - in.position.xyz);
    let l = normalize(light_pos - in.position.xyz);
    let h = normalize(v + l);

    let color = textureSample(base_color_texture, texture_sampler, in.uv).rgb;

    return (vec4f(PBR(color, v, h, n, l) * 5. + color * 0.2, 1.0)) * in.color;
}