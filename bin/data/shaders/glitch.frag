uniform sampler2DRect tex0;

varying vec2 texCoords;

uniform float amount;
uniform sampler2DRect noiseTex;    // 1 pix width
uniform vec2 dimensions;

void main(){
    vec4 offset = texture2DRect(noiseTex, vec2(0, texCoords.y));
    vec2 offsetCoords = texCoords + vec2(pow(offset.r, 4.) * amount, 0);   // offset horizontally
    while(offsetCoords.x > dimensions.x)
        offsetCoords.x -= dimensions.x;

    vec4 color = texture2DRect(tex0, offsetCoords);
    gl_FragColor = color;
}