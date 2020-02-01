varying vec2 uv;
uniform sampler2D texture;
uniform float time;

const float PI = 3.1415;

void main()
{
  float x = uv.x;
  float y = uv.y;

  vec2 center = vec2(0.5,0.5);

  //gl_FragColor = texture2D(texture,uv);
  gl_FragColor = vec4(0.0,0.2,0.3,1.0);

  float dst = length(uv-mod(center,1.) );

  //gl_FragColor *= clamp(1.0-(sin(dst*PI*90.0)),0.8, 2.0);

  gl_FragColor.g = dst;
  gl_FragColor.r = sin(dst*50.);
  
  gl_FragColor = texture2D(texture, uv);
  if (length(texture2D(texture, uv-0.001) - texture2D(texture, uv)) > 0.05 ) {
    gl_FragColor -= vec4(0.8,0.8,0.8,0.0);
  }

  x -= center.x;
  y -= center.y;

  float t = sin(time/76.0)*1000.0;

  float d = dst;

  //t = 120.0;
  float ta = t;
  float off = 3.141595;
  if (y < 0.0) { 
    ta = -ta;
    //off -= 3.14;
  }


  //float a = cos(x*54.+ta)*x*cos(d*t);
  float a = cos(x*t*dst*3.+ta)*y*x*cos(d*t);
  float b = cos(y*d*t);
  float p = a*30. + b*30.;
  gl_FragColor = vec4(a*5.*b, a*4.*dst, a*p, 1.0);

  //gl_FragColor *= texture2D(texture ,uv);
}


