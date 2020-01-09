#include <cmath>
#include "CQuad.h"
// Example 4 �}�l
// ���P Example 3 �¤W(Y�b)
// �C�@�� Vertex �W�[ Normal �A�令�~���� CShape�A�@�ֳB�z�������]�w�ݨD

CQuad::CQuad()
{
	m_iNumVtx = QUAD_NUM;
	m_pPoints = NULL; m_pNormals = NULL; m_pTex = NULL;

	m_pPoints  = new vec4[m_iNumVtx];
	m_pNormals = new vec3[m_iNumVtx];
	m_pColors  = new vec4[m_iNumVtx]; 

	m_pPoints[0] = vec4( -0.5f, 0.0f,  0.5f, 1.0f);
	m_pPoints[1] = vec4(  0.5f, 0.0f,  0.5f, 1.0f);
	m_pPoints[2] = vec4(  0.5f, 0.0f, -0.5f, 1.0f);
	m_pPoints[3] = vec4( -0.5f, 0.0f,  0.5f, 1.0f);
	m_pPoints[4] = vec4(  0.5f, 0.0f, -0.5f, 1.0f);
	m_pPoints[5] = vec4( -0.5f, 0.0f, -0.5f, 1.0f);

	m_pNormals[0] = vec3(  0, 1.0f, 0);  // Normal Vector �� W �� 0
	m_pNormals[1] = vec3(  0, 1.0f, 0);
	m_pNormals[2] = vec3(  0, 1.0f, 0);
	m_pNormals[3] = vec3(  0, 1.0f, 0);
	m_pNormals[4] = vec3(  0, 1.0f, 0);
	m_pNormals[5] = vec3(  0, 1.0f, 0);

	m_pColors[0] = vec4( 1.0f, 1.0f,  1.0f, 1.0f);  // (r, g, b, a)
	m_pColors[1] = vec4( 1.0f, 1.0f,  1.0f, 1.0f);
	m_pColors[2] = vec4( 1.0f, 1.0f,  1.0f, 1.0f);
	m_pColors[3] = vec4( 1.0f, 1.0f,  1.0f, 1.0f);
	m_pColors[4] = vec4( 1.0f, 1.0f,  1.0f, 1.0f);
	m_pColors[5] = vec4( 1.0f, 1.0f,  1.0f, 1.0f);

	for( int i = 0 ; i < m_iNumVtx ; i++ ) m_pColors[i] = vec4(-1.0f,-1.0f,-1.0f,1.0f);

#ifdef LIGHTING_WITHCPU
	// Default Set shader's name
	SetShaderName("vsLighting_CPU.glsl", "fsLighting_CPU.glsl");
#else // lighting with GPU
#ifdef PERVERTEX_LIGHTING
	SetShaderName("vsLighting_GPU.glsl", "fsLighting_GPU.glsl");
#else
	SetShaderName("vsPerPixelLighting.glsl", "fsPerPixelLighting.glsl");
#endif
#endif  
}

void CQuad::RenderWithFlatShading(vec4 vLightPos, color4 vLightI)
{
	// �H�C�@�ӭ����T�ӳ��I�p��䭫�ߡA�H�ӭ��ߧ@���C��p�⪺�I��
	// �ھ� Phong lighting model �p��۹������C��A�ñN�C���x�s�즹�T���I��
	// �]���C�@�ӥ��������I�� Normal ���ۦP�A�ҥH���B�èS���p�⦹�T�ӳ��I������ Normal

	vec4 vCentroidP;
	for( int i = 0 ; i < m_iNumVtx ; i += 3 ) {
		// �p��T���Ϊ�����
		vCentroidP = (m_pPoints[i] + m_pPoints[i+1] + m_pPoints[i+2])/3.0f;
		m_pColors[i] = m_pColors[i + 1] = m_pColors[i + 2] = PhongReflectionModel(vCentroidP, m_pNormals[i], vLightPos, vLightI);
	}
	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx+sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors ); // vertcies' Color
}

void CQuad::RenderWithFlatShading(const LightSource &Lights)
{
	// �H�C�@�ӭ����T�ӳ��I�p��䭫�ߡA�H�ӭ��ߧ@���C��p�⪺�I��
	// �ھ� Phong lighting model �p��۹������C��A�ñN�C���x�s�즹�T���I��
	// �]���C�@�ӥ��������I�� Normal ���ۦP�A�ҥH���B�èS���p�⦹�T�ӳ��I������ Normal
	vec4 vCentroidP;
	for (int i = 0; i < m_iNumVtx; i += 3) {
		// �p��T���Ϊ�����
		vCentroidP = (m_pPoints[i] + m_pPoints[i + 1] + m_pPoints[i + 2]) / 3.0f;
		m_pColors[i] = m_pColors[i + 1] = m_pColors[i + 2] = PhongReflectionModel(vCentroidP, m_pNormals[i], Lights.position, Lights.diffuse);
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_uiBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx + sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors); // vertcies' Color
}

void CQuad::RenderWithGouraudShading(vec4 vLightPos, color4 vLightI)
{
	// �H vertex �����A�ھڸ��I����m�P��k�V�q�A�Q�� Phong lighting model �p��۹������C��
	// �N���C���x�s�^�ӳ��I

	// ���ӳ��I�c������ӤT����
	// �p�� 0 1 2 5 �|�ӳ��I���C��Y�i�A0 �P 3�B2 �P 4 ���C��ۦP
	m_pColors[0] = m_pColors[3] = PhongReflectionModel(m_pPoints[0], m_pNormals[0], vLightPos,  vLightI);
	m_pColors[2] = m_pColors[4] = PhongReflectionModel(m_pPoints[2], m_pNormals[2], vLightPos,  vLightI);
	m_pColors[1] = PhongReflectionModel(m_pPoints[1], m_pNormals[1], vLightPos,  vLightI);
	m_pColors[5] = PhongReflectionModel(m_pPoints[5], m_pNormals[5], vLightPos,  vLightI);

	// �p�G�n�]�w Spot Light�A�b���B�p������P�Q�ө��I�������A���� vLightI �����e�A�W���b�I�s�ǤJ�Y�i

	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx+sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors ); // vertcies' Color
}

void CQuad::RenderWithGouraudShading(const LightSource &Lights)
{
	// �H vertex �����A�ھڸ��I����m�P��k�V�q�A�Q�� Phong lighting model �p��۹������C��
	// �N���C���x�s�^�ӳ��I

	// ���ӳ��I�c������ӤT����
	// �p�� 0 1 2 5 �|�ӳ��I���C��Y�i�A0 �P 3�B2 �P 4 ���C��ۦP
	m_pColors[0] = m_pColors[3] = PhongReflectionModel(m_pPoints[0], m_pNormals[0], Lights);
	m_pColors[2] = m_pColors[4] = PhongReflectionModel(m_pPoints[2], m_pNormals[2], Lights);
	m_pColors[1] = PhongReflectionModel(m_pPoints[1], m_pNormals[1], Lights);
	m_pColors[5] = PhongReflectionModel(m_pPoints[5], m_pNormals[5], Lights);

	glBindBuffer(GL_ARRAY_BUFFER, m_uiBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx + sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors); // vertcies' Color
}

// ���B�ҵ��� vLightPos �����O�@�ɮy�Ъ��T�w�����m
void CQuad::Update(float dt, point4 vLightPos, color4 vLightI)
{
#ifdef LIGHTING_WITHCPU
	if (m_bViewUpdated || m_bTRSUpdated) { // Model View �������x�}���e�����
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_mxMV3X3Final = mat3(	// �u���e���� 3X3 �x�}�����e
			m_mxMVFinal._m[0].x, m_mxMVFinal._m[1].x, m_mxMVFinal._m[2].x,
			m_mxMVFinal._m[0].y, m_mxMVFinal._m[1].y, m_mxMVFinal._m[2].y,
			m_mxMVFinal._m[0].z, m_mxMVFinal._m[1].z, m_mxMVFinal._m[2].z);
#ifdef GENERAL_CASE
		m_mxITMV = InverseTransposeMatrix(m_mxMVFinal);
#endif
		m_bViewUpdated = m_bTRSUpdated = false;
	}
	if (m_iMode == FLAT_SHADING) RenderWithFlatShading(vLightPos, vLightI);
	else RenderWithGouraudShading(vLightPos, vLightI);

#else // Lighting With GPU
	if (m_bViewUpdated || m_bTRSUpdated) {
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_bViewUpdated = m_bTRSUpdated = false;
	}
	for (int i = 0; i < 2; i++)
	{
		m_vLightInView[i] = m_mxView * vLightPos;		// �N Light �ഫ�����Y�y�ЦA�ǤJ

		m_DiffuseProduct[i] = m_Material.kd * m_Material.diffuse  * vLightI;
		m_SpecularProduct[i] = m_Material.ks * m_Material.specular * vLightI;
		m_AmbientProduct[i] = m_Material.ka * m_Material.ambient  * vLightI;
	}

	// ��X AmbientProduct DiffuseProduct �P SpecularProduct �����e


#endif

}

void CQuad::Update(float dt, const LightSource &Lights)
{
#ifdef LIGHTING_WITHCPU
	if (m_bViewUpdated || m_bTRSUpdated) { // Model View �������x�}���e�����
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_mxMV3X3Final = mat3(
			m_mxMVFinal._m[0].x, m_mxMVFinal._m[1].x, m_mxMVFinal._m[2].x,
			m_mxMVFinal._m[0].y, m_mxMVFinal._m[1].y, m_mxMVFinal._m[2].y,
			m_mxMVFinal._m[0].z, m_mxMVFinal._m[1].z, m_mxMVFinal._m[2].z);
#ifdef GENERAL_CASE
		m_mxITMV = InverseTransposeMatrix(m_mxMVFinal);
#endif
		m_bViewUpdated = m_bTRSUpdated = false;
	}
	if (m_iMode == FLAT_SHADING) RenderWithFlatShading(Lights);
	else RenderWithGouraudShading(Lights);

#else // Lighting With GPU
	if (m_bViewUpdated || m_bTRSUpdated) {
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_bViewUpdated = m_bTRSUpdated = false;
	}
	for (int i = 0; i < 2; i++)
	{
		m_vLightInView[i] = m_mxView * Lights.position;		// �N Light �ഫ�����Y�y�ЦA�ǤJ

		m_DiffuseProduct[i] = m_Material.kd * m_Material.diffuse  * Lights.diffuse;
		m_SpecularProduct[i] = m_Material.ks * m_Material.specular * Lights.specular;
		m_spotCosCutoff[i] = Lights.spotCosCutoff;
		m_AmbientProduct[i] = m_Material.ka * m_Material.ambient  * Lights.ambient;
	}
	// ��X AmbientProduct DiffuseProduct �P SpecularProduct �����e


#endif

}


void CQuad::Update(const LightSource *Lights, float dt)
{
//	if (m_bViewUpdated || m_bTRSUpdated) {
//		m_mxMVFinal = m_mxView * m_mxTRS;
//		m_mxMV3X3Final = mat3(	// �u���e���� 3X3 �x�}�����e
//			m_mxMVFinal._m[0].x, m_mxMVFinal._m[1].x, m_mxMVFinal._m[2].x,
//			m_mxMVFinal._m[0].y, m_mxMVFinal._m[1].y, m_mxMVFinal._m[2].y,
//			m_mxMVFinal._m[0].z, m_mxMVFinal._m[1].z, m_mxMVFinal._m[2].z);
//		m_bViewUpdated = m_bTRSUpdated = false;
//	}
//
//
//
//	for (int i = 0; i < LIGHTCOUNT; i++)
//	{
//		m_AmbientProduct[i] = m_Material.ka * m_Material.ambient  * Lights[i].ambient;
//		lightType[i] = Lights[i].type;
//		m_LightDir[i] = m_mxMV3X3Final * Lights[i].spotDirection;
//		m_SpotExponent[i] = Lights[i].spotExponent;
//		m_vLightInView[i] = m_mxView * Lights[i].position;
//		m_DiffuseProduct[i] = m_Material.kd * m_Material.diffuse  * Lights[i].diffuse;
//		m_Diffuse[i] = Lights[i].diffuse;
//		m_SpecularProduct[i] = m_Material.ks * m_Material.specular * Lights[i].specular;
//		m_spotCosCutoff[i] = Lights[i].spotCosCutoff;
//		m_iLighting[i] = Lights[i].isLighting;
//	}


	// Lighting With GPU
	if (m_bViewUpdated || m_bTRSUpdated) {
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_bViewUpdated = m_bTRSUpdated = false;
	}

	for (int i = 0; i < LIGHTCOUNT; i++)
	{
		lightType[i] = Lights[i].type;

		m_vLightInView[i]= m_mxView * Lights[i].position;		// �N Light �ഫ�����Y�y�ЦA��
		m_vSpotTarget[i] = m_mxView * Lights[i].spotTarget;
		// ��X AmbientProduct DiffuseProduct �P SpecularProduct �����e
		m_AmbientProduct[i] = m_Material.ka * m_Material.ambient  * Lights[i].ambient;
		m_DiffuseProduct[i] = m_Material.kd * m_Material.diffuse  * Lights[i].diffuse;
		m_SpecularProduct[i] = m_Material.ks * m_Material.specular * Lights[i].specular;
		m_iLighting[i] = Lights[i].isLighting;
	}

}

void CQuad::Update(float dt)
{
	if (m_bViewUpdated || m_bTRSUpdated) { // Model View �������x�}���e�����
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_mxITView = InverseTransposeMatrix(m_mxMVFinal);
		m_bViewUpdated = m_bTRSUpdated = false;
	}
}

void CQuad::SetVtxColors(vec4 vLFColor, vec4 vLRColor, vec4 vTRColor, vec4 vTLColor)
{
	m_pColors[3] = m_pColors[0] = vLFColor;
	m_pColors[1] = vLRColor;
	m_pColors[4] = m_pColors[2] = vTRColor;
	m_pColors[5] = vTLColor;

	// �N�Ҧ� vertices �C���s�� VBO ��
	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx+sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors ); // vertcies' Color
}

void CQuad::SetVtxColors(vec4 vFColor, vec4 vSColor) // �]�w��ӤT���Ϊ��C��
{
	m_pColors[0] = m_pColors[1] = m_pColors[2] = vFColor;
	m_pColors[3] = m_pColors[4] = m_pColors[5] = vSColor;

	// �N�Ҧ� vertices �C���s�� VBO ��
	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx+sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors ); // vertcies' Color
}

void CQuad::Draw()
{
	DrawingSetShader();
	glDrawArrays( GL_TRIANGLES, 0, QUAD_NUM );
}

void CQuad::DrawW()
{
	DrawingWithoutSetShader();
	glDrawArrays( GL_TRIANGLES, 0, QUAD_NUM );
}