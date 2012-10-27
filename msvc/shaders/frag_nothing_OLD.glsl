// set by the vertex shader


varying vec2 tex_coord;

uniform sampler2DRect color;

void main(void)
{
    

// render a color based on the texture coordinate
    
// gl_FragColor is the primary "output" of the fragment shader
  

  
gl_FragColor = texture2DRect(color, tex_coord);




}







		vec4 aNorm = texture2DRect(normal,vec2(tex_coord.x - 2, tex_coord.y));
		vec4 bNorm = texture2DRect(normal,vec2(tex_coord.x + 2, tex_coord.y));
		vec4 cNorm = texture2DRect(normal,vec2(tex_coord.x, tex_coord.y - 2));
		vec4 dNorm = texture2DRect(normal,vec2(tex_coord.x, tex_coord.y + 2));

		float lrSum = (((aNorm.x - 0.5) * 2.0)*((bNorm.x - 0.5) * 2.0)) + (((aNorm.y - 0.5) * 2.0)*((bNorm.y - 0.5) * 2.0)) + (((aNorm.z - 0.5)*2.0)*((bNorm.z - 0.5)*2.0));
		float udSum = (((cNorm.x - 0.5) * 2.0)*((dNorm.x - 0.5) * 2.0)) + (((cNorm.y - 0.5) * 2.0)*((dNorm.y - 0.5) * 2.0)) + (((cNorm.z - 0.5)*2.0)*((dNorm.z - 0.5)*2.0));

		bool udEdge = (udSum > 2.0) || (udSum < -2.0);
		bool lrEdge = (lrSum > 2.0) || (lrSum < -2.0);















vec4 myNorm = texture2DRect(normal, tex_coord);
		float intensity = (myNorm.x + myNorm.y + myNorm.z)/3.0;

		vec4 myNorml = texture2DRect(normal, vec2(tex_coord.x - 1.0, tex_coord.y));
		float intensityl = (myNorml.x + myNorml.y + myNorml.z)/3.0;

		vec4 myNormr = texture2DRect(normal, vec2(tex_coord.x + 1.0, tex_coord.y));
		float intensityr = (myNormr.x + myNormr.y + myNormr.z)/3.0;

		vec4 myNormu = texture2DRect(normal, vec2(tex_coord.x, tex_coord.y - 1.0));
		float intensityu = (myNormu.x + myNormu.y + myNormu.z)/3.0;


		vec4 myNormd = texture2DRect(normal, vec2(tex_coord.x, tex_coord.y + 1.0));
		float intensityd = (myNormd.x + myNormd.y + myNormd.z)/3.0;

		bool lIntense = (intensityl - intensity > 0.1) || (intensity - intensityl > 0.1);
		bool rIntense = (intensityr - intensity > 0.1) || (intensity - intensityr > 0.1);
		bool uIntense = (intensityu - intensity > 0.1) || (intensity - intensityu > 0.1);
		bool dIntense = (intensityd - intensity > 0.1) || (intensity - intensityd > 0.1);











































		float red = texture2DRect(color, tex_coord).x;
		float green = texture2DRect(color, tex_coord).y;
		float vDepth = texture2DRect(depth, tex_coord).z;
		float vDepth2 = texture2DRect(depth, vec2(tex_coord.x - 1, tex_coord.y)).z;
		float vDepth3 = texture2DRect(depth, vec2(tex_coord.x + 1, tex_coord.y)).z;
		float vDepth4 = texture2DRect(depth, vec2(tex_coord.x, tex_coord.y - 1)).z;
		float vDepth5 = texture2DRect(depth, vec2(tex_coord.x, tex_coord.y + 1)).z;



		bool depthLeft = ((vDepth - vDepth2) > 0.2) || ((vDepth - vDepth2) < -0.2);
		bool depthRight = ((vDepth - vDepth3) > 0.7) || ((vDepth - vDepth3) < -0.7);
		bool depthTop = ((vDepth - vDepth4) > 0.2) || ((vDepth - vDepth4) < -0.2);
		bool depthBottom = ((vDepth - vDepth5) > 0.7) || ((vDepth - vDepth5) < -0.7);


		

		bool edgeLeft = depthLeft;
		bool edgeRight = depthRight;
		bool edgeTop = depthTop;
		bool edgeBottom = depthBottom;

