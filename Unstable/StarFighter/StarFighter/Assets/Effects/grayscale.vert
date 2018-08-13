uniform sampler2D texture;
uniform float ratio;

void main()
{
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
    gl_FragColor = gl_Color * pixel;
	
	float gray = (0.2126*gl_FragColor.r + 0.7152*gl_FragColor.g + 0.0722*gl_FragColor.b);
	
	gl_FragColor.r = gray*ratio + gl_FragColor.r*(1-ratio);
	gl_FragColor.g = gray*ratio + gl_FragColor.g*(1-ratio);
	gl_FragColor.b = gray*ratio + gl_FragColor.b*(1-ratio);
}