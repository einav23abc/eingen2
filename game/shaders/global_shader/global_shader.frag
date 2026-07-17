#version 330 core

in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;

uniform sampler2D u_texture;

uniform vec3 u_camera_position;

void main(){
    vec3 color = texture2D(u_texture, v_texcoord).xyz;

    gl_FragColor = vec4(color, 1.0);
}
