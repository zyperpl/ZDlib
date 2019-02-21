varying vec2 uv;
uniform sampler2D texture;

const float PI = 3.1415;

void main()
{
  float x = uv.x;
  float y = uv.y;

  vec2 center = vec2(0.5,0.5);

  gl_FragColor = texture2D(texture,uv);

  float dst = length(uv-mod(center,6.5) );

  gl_FragColor *= clamp(1.0-(sin(dst*PI*30.0)),0.8, 2.0);

}


