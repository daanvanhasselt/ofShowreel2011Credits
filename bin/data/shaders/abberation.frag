uniform sampler2DRect tex0;
uniform sampler2DRect noiseTex;

uniform float amount;

varying vec2 texCoords;

void main(){
    vec4 color1 = texture2DRect(tex0, texCoords);

    vec4 noiseColor = texture2DRect(noiseTex, texCoords);
    noiseColor *= vec4(100.);
    vec4 color2 = texture2DRect(tex0, vec2(texCoords.x + (noiseColor.r * amount), texCoords.y + (noiseColor.g * amount)));
    vec4 color3 = texture2DRect(tex0, vec2(texCoords.x - (noiseColor.r * amount), texCoords.y - (noiseColor.g * amount)));
    vec4 color = vec4(color1.r, color2.g, color3.b, 1.0);
    
    gl_FragColor = color;
}