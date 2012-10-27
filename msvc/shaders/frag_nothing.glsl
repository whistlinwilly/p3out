varying vec2 tex_coord;

uniform sampler2DRect color;
uniform sampler2DRect depth;
uniform sampler2DRect normal;

void main(void)
{
    


		float vDepth = texture2DRect(depth, tex_coord).z;
		float vDepth2 = texture2DRect(depth, vec2(tex_coord.x - 1, tex_coord.y)).z;
		float vDepth3 = texture2DRect(depth, vec2(tex_coord.x + 1, tex_coord.y)).z;
		float vDepth4 = texture2DRect(depth, vec2(tex_coord.x, tex_coord.y - 1)).z;
		float vDepth5 = texture2DRect(depth, vec2(tex_coord.x, tex_coord.y + 1)).z;



		bool depthLeft = ((vDepth - vDepth2) > 0.04) || ((vDepth - vDepth2) < -0.04);
		bool depthRight = ((vDepth3 - vDepth) > 0.04) || ((vDepth3 - vDepth) < -0.04);
		bool depthTop = ((vDepth - vDepth4) > 0.04) || ((vDepth - vDepth4) < -0.04);
		bool depthBottom = ((vDepth5 - vDepth) > 0.04) || ((vDepth5 - vDepth) < -0.04);
			

		vec4 myNorm = texture2DRect(normal, tex_coord);
		float intensity = (myNorm.x + myNorm.y + myNorm.z)/3.0;

		vec4 myNorml = texture2DRect(normal, vec2(tex_coord.x - 0.7, tex_coord.y));
		float intensityl = (myNorml.x + myNorml.y + myNorml.z)/3.0;

		vec4 myNormr = texture2DRect(normal, vec2(tex_coord.x + 0.7, tex_coord.y));
		float intensityr = (myNormr.x + myNormr.y + myNormr.z)/3.0;

		vec4 myNormu = texture2DRect(normal, vec2(tex_coord.x, tex_coord.y - 0.7));
		float intensityu = (myNormu.x + myNormu.y + myNormu.z)/3.0;


		vec4 myNormd = texture2DRect(normal, vec2(tex_coord.x, tex_coord.y - 0.7));
		float intensityd = (myNormd.x + myNormd.y + myNormd.z)/3.0;

		bool lIntense = (intensityl - intensity > 0.01) || (intensity - intensityl > 0.01);
		bool rIntense = (intensityr - intensity > 0.01) || (intensity - intensityr > 0.01);
		bool uIntense = (intensityu - intensity > 0.01) || (intensity - intensityu > 0.01);
		bool dIntense = (intensityd - intensity > 0.01) || (intensity - intensityd > 0.01);

		if(lIntense || rIntense || uIntense || dIntense || depthLeft || depthRight || depthTop || depthBottom){
			gl_FragColor = vec4(0.0,0.0,0.0,1.0);
		}else{
			gl_FragColor = texture2DRect(color, tex_coord);
		} 


}

