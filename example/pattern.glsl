varying vec2 uv;
uniform sampler2D texture;
uniform float time;

const float PI = 3.1415;
const float LWIDTH = 0.001;

vec2 pos = vec2(0.0,0.0);

float line(vec2 pt1, vec2 pt2)
{
  vec2 lineDir = pt2 - pt1;
  vec2 perpDir = vec2(lineDir.y, -lineDir.x);
  vec2 dirToPt1 = pt1 - pos;
  float dstToLine = abs(dot(normalize(perpDir), dirToPt1));
  float dstToPSum = length(pos-pt1) + length(pos-pt2);

  if (dstToPSum > length(lineDir)+dstToLine) 
  {
    return min(length(pos-pt1), length(pos-pt2));
  }

  return dstToLine;
}

vec3 drawLine(vec2 from, vec2 to, vec3 color)
{
  float d = line(from, to);
  if (d <= LWIDTH)
  {
    return color*smoothstep(1.0,0.6,d*(1./LWIDTH));
  }

  return vec3(0,0,0);
}

void main()
{
  vec3 color = vec3(0.1,0.1,0.1);
  pos = uv-0.50;
  
  vec3 blue = vec3(0.0,0.5,1.0);
  vec3 red  = vec3(1.0,0.1,0.3);

  color = max(drawLine(vec2(-0.4, -0.5), vec2(0.0,0.0), blue),
              drawLine(vec2(0.3, -0.2),  vec2(0.1,0.0), red)
      );
  
  gl_FragColor = vec4(color, 0.0);
}


