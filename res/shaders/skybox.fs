#version 330 core
in vec3 TexCoords;
in vec3 camToPixel;
out vec4 color;

uniform samplerCube skybox;
uniform vec3 viewPos;
uniform vec3 camDir;
uniform vec3 lightPos;
uniform float time;

#define PI 3.14159265359

// https://www.shadertoy.com/view/llSSDR

float saturatedDot( in vec3 a, in vec3 b )
{
    return max( dot( a, b ), 0.0 );   
}

vec3 YxyToXYZ( in vec3 Yxy )
{
    float Y = Yxy.r;
    float x = Yxy.g;
    float y = Yxy.b;

    float X = x * ( Y / y );
    float Z = ( 1.0 - x - y ) * ( Y / y );

    return vec3(X,Y,Z);
}

vec3 XYZToRGB( in vec3 XYZ )
{
    // CIE/E
    mat3 M = mat3
    (
         2.3706743, -0.9000405, -0.4706338,
        -0.5138850,  1.4253036,  0.0885814,
         0.0052982, -0.0146949,  1.0093968
    );

    return XYZ * M;
}


vec3 YxyToRGB( in vec3 Yxy )
{
    vec3 XYZ = YxyToXYZ( Yxy );
    vec3 RGB = XYZToRGB( XYZ );
    return RGB;
}

void calculatePerezDistribution( in float t, out vec3 A, out vec3 B, out vec3 C, out vec3 D, out vec3 E )
{
    A = vec3(  0.1787 * t - 1.4630, -0.0193 * t - 0.2592, -0.0167 * t - 0.2608 );
    B = vec3( -0.3554 * t + 0.4275, -0.0665 * t + 0.0008, -0.0950 * t + 0.0092 );
    C = vec3( -0.0227 * t + 5.3251, -0.0004 * t + 0.2125, -0.0079 * t + 0.2102 );
    D = vec3(  0.1206 * t - 2.5771, -0.0641 * t - 0.8989, -0.0441 * t - 1.6537 );
    E = vec3( -0.0670 * t + 0.3703, -0.0033 * t + 0.0452, -0.0109 * t + 0.0529 );
}

vec3 calculateZenithLuminanceYxy( in float t, in float thetaS )
{
    float chi       = ( 4.0 / 9.0 - t / 120.0 ) * ( PI - 2.0 * thetaS );
    float Yz        = ( 4.0453 * t - 4.9710 ) * tan( chi ) - 0.2155 * t + 2.4192;

    float theta2    = thetaS * thetaS;
    float theta3    = theta2 * thetaS;
    float T         = t;
    float T2        = t * t;

    float xz =
      ( 0.00165 * theta3 - 0.00375 * theta2 + 0.00209 * thetaS + 0.0)     * T2 +
      (-0.02903 * theta3 + 0.06377 * theta2 - 0.03202 * thetaS + 0.00394) * T +
      ( 0.11693 * theta3 - 0.21196 * theta2 + 0.06052 * thetaS + 0.25886);

    float yz =
      ( 0.00275 * theta3 - 0.00610 * theta2 + 0.00317 * thetaS + 0.0)     * T2 +
      (-0.04214 * theta3 + 0.08970 * theta2 - 0.04153 * thetaS + 0.00516) * T +
      ( 0.15346 * theta3 - 0.26756 * theta2 + 0.06670 * thetaS + 0.26688);

    return vec3( Yz, xz, yz );
}

vec3 calculatePerezLuminanceYxy( in float theta, in float gamma, in vec3 A, in vec3 B, in vec3 C, in vec3 D, in vec3 E )
{
    return ( 1.0 + A * exp( B / cos( theta ) ) ) * ( 1.0 + C * exp( D * gamma ) + E * cos( gamma ) * cos( gamma ) );
}

vec3 calculateSkyLuminanceRGB( in vec3 s, in vec3 e, in float t )
{
    vec3 A, B, C, D, E;
    calculatePerezDistribution( t, A, B, C, D, E );

    float thetaS = acos( saturatedDot( s, vec3(0,1,0) ) );
    float thetaE = acos( saturatedDot( e, vec3(0,1,0) ) );
    float gammaE = acos( saturatedDot( s, e )          );

    vec3 Yz = calculateZenithLuminanceYxy( t, thetaS );

    vec3 fThetaGamma = calculatePerezLuminanceYxy( thetaE, gammaE, A, B, C, D, E );
    vec3 fZeroThetaS = calculatePerezLuminanceYxy( 0.0,    thetaS, A, B, C, D, E );

    vec3 Yp = Yz * ( fThetaGamma / fZeroThetaS );

    return YxyToRGB( Yp );
}
const vec3 UP = vec3(0.0, 1.0, 0.0);                        // An up vector.

vec3 sky( in vec3 d, in vec3 ld )
{
    // Get the sky color.
    vec3 sky = calculateSkyLuminanceRGB(ld, d, 3.0);
    
    // How night time is it? This variable will tell you.
    float night = smoothstep(-0.0, -0.5, clamp(dot(ld, UP),-0.5, -0.0));
    // Set a general brightness level so we don't just have a white screen,
    // and artificially darken stuff at night so it looks good.
    sky *= .040-.035*night;
    
    // Create a spot for the sun. This version gives us some nice edges
    // without having a pow(x,VERY_LARGE_NUMBER) call.
    float sunspot = smoothstep(.99935, .99965, max(dot(d,ld),0.0));
    sunspot += smoothstep(.98000, 1.0, max(dot(d,ld),0.0))*.05; // Corona.
    
    // Mix the sky with the sun.
    sky = sky*(1.0+sunspot);
    
    // Also add in the stars.
    return sky;// + stars(d)*.5;
}

void main()
{    
	vec3 viewDir = normalize( vec3(TexCoords));// (vec2(TexCoords.x,TexCoords.y+1.0) * 2.0) - 1.0 , 1.0 ) );
    //color = texture(skybox, TexCoords);
    color = vec4(sky(viewDir,lightPos), 1.0);;
}


/*
void doScatter(inout vec3 dSC, inout vec3 sEC, in vec3 sp, in vec3 l, in vec3 r, in vec3 o){
    vec3 ctpn = normalize(camToPixel);

    //direct sun color
    float directSun = dot(ctpn, normalize(l-o));
    directSun = pow( clamp(directSun,0.0,1.0), 600.0);

    // atmosphere
    float sunEarthScatering = 1.0-dot(ctpn, vec3(0.0,1.0,0.0));
    sunEarthScatering = pow( clamp(sunEarthScatering,0.0,1.0), 5.0);
    vec4 skyBlend = pow(1.*vec4(.1, .75, .8, 1), vec4(4.*(1.*TexCoords.y-.1)));

    // learp or mix based on directional lights 'y' coord/dir
    sunEarthTop = mix(.65*skyBlend.rgb, sunEarthTop, l.y);
    sunEarthBottom = mix(vec3(.5)*sunEarthBottom, sunEarthBottom, l.y);
    
    //lerp color and add contribution of sun and atmosphere
    dSC = mix(sunPeriph,sunCenter,directSun)*directSun;
    sEC = mix(sunEarthTop,sunEarthBottom,sunEarthScatering);
}
*/