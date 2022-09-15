#version 450 core

uniform vec2 u_resolution;

float plot(vec2 st) {
    return smoothstep(0.02, 0.0, abs(st.y - st.x));
}

float grid(vec2 st, float res) {
  vec2 grid = fract(st*res);
  return (step(res,grid.x) * step(res,grid.y));
}

void main() {
	vec2 st = gl_FragCoord.st/u_resolution.xy;
    st.x *= u_resolution.x/u_resolution.y;

    vec2 grid_st = st * 300;
    vec4 color = vec4(1, 0, 1, 0.5) * grid(grid_st, 0.01);

    gl_FragColor = color;
}