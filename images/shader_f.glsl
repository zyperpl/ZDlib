#version 330

in vec2 uv;
uniform sampler2D sampler;

out vec4 fragColor;

void main()
{
  fragColor = texture(sampler, uv);
  fragColor.a = 1.0;
  vec2 center = vec2(0.25, 0.25);
  vec2 nuv = uv - center;
  vec3 col = vec3(0.0,0.4,0.0);
  float dst = length(nuv - center);
  col += vec3(1.0,1.0,0.0) * sin(dst * 300.0);
  col += vec3(1.0,0.0,1.0) * cos(dst * 79.04) * 3.0;
  fragColor.rgb += col;
}
