#version 330 core

in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;

uniform sampler2D u_sun_shadow_map_texture;

uniform mat4 u_camera_world_view_projection_matrix;

uniform vec3 u_sun_vector;
uniform mat4 u_sun_shadow_map_wvp_mat;

uniform vec3 u_camera_position;

// for shadows
const vec2 poisson_sampling_disk[16] = vec2[](
    vec2(-0.94201624, -0.39906216 ),
    vec2( 0.94558609, -0.76890725 ),
    vec2(-0.094184101,-0.92938870 ),
    vec2( 0.34495938,  0.29387760 ),
    vec2(-0.91588581,  0.45771432 ),
    vec2(-0.81544232, -0.87912464 ),
    vec2(-0.38277543,  0.27676845 ),
    vec2( 0.97484398,  0.75648379 ),
    vec2( 0.44323325, -0.97511554 ),
    vec2( 0.53742981, -0.47373420 ),
    vec2(-0.26496911, -0.41893023 ),
    vec2( 0.79197514,  0.19090188 ),
    vec2(-0.24188840,  0.99706507 ),
    vec2(-0.81409955,  0.91437590 ),
    vec2( 0.19984126,  0.78641367 ),
    vec2( 0.14383161, -0.14100790 )
);
// for shadows
float random(vec3 seed, int i) {
    vec4 seed4 = vec4(seed, i);
    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
}

// for grass patch
float hash(float n) { return fract(sin(n) * 1e4); }
// for grass patch
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }
// for grass patch
float noise(vec2 x) {
	vec2 i = floor(x);
	vec2 f = fract(x);

	// Four corners in 2D of a tile
	float a = hash(i);
	float b = hash(i + vec2(1.0, 0.0));
	float c = hash(i + vec2(0.0, 1.0));
	float d = hash(i + vec2(1.0, 1.0));

	// Simple 2D lerp using smoothstep envelope between the values.
	// return (mix(mix(a, b, smoothstep(0.0, 1.0, f.x)),
	// 			mix(c, d, smoothstep(0.0, 1.0, f.x)),
	// 			smoothstep(0.0, 1.0, f.y)));

	// Same code, with the clamps in smoothstep and common subexpressions
	// optimized away.
	vec2 u = f * f * (3.0 - 2.0 * f);
	return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}
// for grass patch
float layered_noise(vec2 p) {
    vec2 transform = vec2(
        noise((p + vec2(536.26, 864.13)) / 2.0),
        noise((p + vec2(-954.82, -163.37)) / 2.0)
    ) * 5.0;
    
    return (
        noise((((p + transform) + vec2(-178.29, 23.641))) / 53.0) * 0.7 +
        noise(((p + transform) + vec2(451.12, -912.74)) / 27.0) * 0.2 +
        noise(((p + transform) + vec2(-336.12, -125.74)) / 7.0) * 0.1
    );
}

float freq8(float x) {
    float s = sin(x);
    float s2 = s*s;
    float s4 = s2*s2;
    float s8 = s4*s4;
    float c = cos(x);
    float c2 = c*c;
    float c4 = c2*c2;
    float c8 = c4*c4;
    return (s8 + c8);
}

float MAX_HEIGHT = 16.0;
float height_map(vec2 p) {
    // return noise(p / 1.57) * MAX_HEIGHT;
    return noise(p / 3.0) * MAX_HEIGHT;
    // return freq8(p.x / 3.0) * freq8(p.y / 3.8) * MAX_HEIGHT;
}

float round(float x) {
    return floor(x + 0.5);
}

float calculate_depth(vec3 p) {
    vec4 t = vec4(p, 1.0) * u_camera_world_view_projection_matrix;
    return (t.z / t.w) * 0.5 + 0.5;
}

void main(){
    vec3 normal = v_normal;
    vec3 position = v_position;

    /*
        this parallaxing is wrong since i am going upwards instead of downwards.
        due to this, in a situation like so: 

                              xxx   
                       ___ xxx      
                      / xxB         
                     xxx   \        
                  xxx       \       
               xxx /         \___   
        ____Axx___/              \__

        where `x` is the line of stepping, and the `_/\_` indicates the height map if
        disected from the side.

        we have a unique issue that on the first iteration we already satisfy the condition
        that the parallaxed depth is smaller then the parallaxed step's depth. which causes
        us to exit the loop and thus we select point A as our position instead of point B.
    */

    int PARALAX_ITERATIONS = 4;

    vec3 view_dir = u_camera_position - position;
    vec3 normalized_view_dir = normalize(view_dir);
    vec3 parallax_step = (-dot(normalized_view_dir, normal) * normalized_view_dir) * MAX_HEIGHT / float(PARALAX_ITERATIONS);
    
    vec3 parallaxed_step_position = position;
    float parallaxed_step_depth = calculate_depth(parallaxed_step_position);
    vec3 parallaxed_position = parallaxed_step_position + height_map(parallaxed_step_position.xz) * normal;
    float parallaxed_depth = calculate_depth(parallaxed_position);

    vec3 last_parallaxed_step_position = parallaxed_step_position;
    float last_parallaxed_step_depth = parallaxed_step_depth;
    vec3 last_parallaxed_position = parallaxed_position;
    float last_parallaxed_depth = parallaxed_depth;
    for (int i = 0; i < PARALAX_ITERATIONS && parallaxed_step_depth <= parallaxed_depth; i++) {
        last_parallaxed_step_position = parallaxed_step_position;
        last_parallaxed_step_depth = parallaxed_step_depth;
        last_parallaxed_position = parallaxed_position;
        last_parallaxed_depth = parallaxed_depth;
        
        parallaxed_step_position = last_parallaxed_step_position + parallax_step;
        parallaxed_step_depth = calculate_depth(parallaxed_step_position);
        parallaxed_position = parallaxed_step_position + height_map(parallaxed_step_position.xz) * normal;
        parallaxed_depth = calculate_depth(parallaxed_position);
    }

    // linear interpolate
    vec3 a = last_parallaxed_position - last_parallaxed_step_position;
    vec3 b = parallaxed_position - parallaxed_step_position;
    float c = sqrt(dot(parallax_step, parallax_step)) * sqrt(dot(a, a));
    float x = c / (c + sqrt(dot(b, b)));
    vec3 final_parallaxed_position = last_parallaxed_position + x * parallax_step;
    float final_parallaxed_depth = calculate_depth(final_parallaxed_position);

    position = final_parallaxed_position;
    // float depth = gl_FragCoord.z; // same as calculate_depth(position)
    float depth = final_parallaxed_depth;


    float normal_dot_sun = dot(normal, -u_sun_vector);
    float light = normal_dot_sun;

    vec3 sun_shadow_map_position = (vec4(position, 1.0) * u_sun_shadow_map_wvp_mat).xyz * 0.5 + 0.5;

    float sun_current_depth = sun_shadow_map_position.z;
    float bias = max(0.00005, 0.0002 * (1.0 - normal_dot_sun));
    float shadow = 1.0;
    for (int i = 0; i < 4; i++) {
        int index = int(16.0 * random(gl_FragCoord.xyy, i)) % 16;
        float sun_closest_depth = texture2D(u_sun_shadow_map_texture, sun_shadow_map_position.xy + poisson_sampling_disk[index]/(1080.0*2.0)).r;
        if (sun_closest_depth + bias < sun_current_depth) {
            shadow -= 0.25;
        }
    }
    
    float lighting = clamp(light * shadow, 0.0, 1.0) * 0.5 + 0.5;
    // float lighting = clamp(light, 0.0, 1.0) * 0.75 + 0.25;

    // float aerial_mixing = max(0.0,min(1.0, 1 - 0.001 * distance(position, u_camera_position)));
    // const vec3 aerial_color = vec3(0.2, 0.2, 0.3);

    float grass_patch = layered_noise(position.xz);

    float color_modifier = (round(grass_patch) * 0.3 + 0.7) * lighting;

    vec3 color = (vec3(84.0, 106.0, 0.0) / 256.0) * color_modifier;
    // color = (color * aerial_mixing) + (aerial_color * (1 - aerial_mixing));

    // color = vec3(height_map(position.xz) / MAX_HEIGHT);
    // color = vec3(((position.xz + 200.0) / 400.0), 0.0);

    gl_FragDepth = depth;
    gl_FragColor = vec4(color, 1.0);
}
