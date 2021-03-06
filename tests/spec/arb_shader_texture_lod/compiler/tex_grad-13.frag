/* [config]
 * expect_result: pass
 * glsl_version: 1.10
 * require_extensions: GL_ARB_shader_texture_lod
 * [end config]
 */
#extension GL_ARB_shader_texture_lod: require

uniform samplerCube s;
varying vec3 coord;
varying vec3 dPdx;
varying vec3 dPdy;

void main()
{
  gl_FragColor = textureCubeGradARB(s, coord, dPdx, dPdy);
}
