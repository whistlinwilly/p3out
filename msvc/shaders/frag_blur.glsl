uniform sampler2DRect color;
uniform sampler2DRect blur;
uniform int frames;
varying vec2 tex_coord;


void main() {
	
	if(frames % 2 == 0){
	gl_FragColor = vec4(0.0,1.0,0.0,1.0);
	}
	else{
	gl_FragColor = vec4(1.0,0.0,0.0,1.0);
	}





}