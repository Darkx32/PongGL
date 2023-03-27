#version 330 core
out vec4 FragColor;
in vec4 aColor;

in float aRadius;
in vec2 aPosition;

void main()
{
	float d = 1.0 - length(vec3(aPosition, 0.0));
	float circle = smoothstep(0.0, 0.1, d);
	circle *= smoothstep(1.005, 1.0, d);

	if (circle == 0.0)
		discard;

	FragColor = vec4(aColor.rgb, aColor.a * circle);

//    vec2 uv = gl_FragCoord.xy/aPosition;
//    float d = length(gl_PointCoord.xy - vec2(aRadius, 0.0));
//    float t = 1.0 - smoothstep(aRadius, aRadius, d);
//    gl_FragColor = vec4(aColor.rgb, aColor.a * t);
}