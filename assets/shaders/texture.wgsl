
@group(0) @binding(1) var gradientTexture: texture_2d<f32>;

fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    let color = textureLoad(gradientTexture, vec2i(in.position.xy), 0).rgb;
    // ...
}