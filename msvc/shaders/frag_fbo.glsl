varying vec2 tex_coord;
uniform sampler2DRect color;


void main() {

	gl_FragColor = texture2DRect(color, tex_coord);



}