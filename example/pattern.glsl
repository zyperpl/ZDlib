varying vec2 uv;
uniform sampler2D texture;
void main()
{
  gl_FragColor = texture2D(texture, uv);

  gl_FragColor *= sin(uv.x*30.0)*sin(uv.y*30.0)*10.0;
}


