#include "CSolidCube.h"

CSolidCube::CSolidCube()
{
	m_iNumVtx = SOLIDCUBE_NUM;
	m_pPoints = NULL; m_pNormals = NULL; m_pTex = NULL;

	m_pPoints  = new vec4[m_iNumVtx];
	m_pNormals = new vec3[m_iNumVtx];
	m_pColors  = new vec4[m_iNumVtx]; 
	m_pTex     = new vec2[m_iNumVtx];

    m_vertices[0] = point4( -0.5, -0.5,  0.5, 1.0 );
    m_vertices[1] = point4( -0.5,  0.5,  0.5, 1.0 );
    m_vertices[2] = point4(  0.5,  0.5,  0.5, 1.0 );
    m_vertices[3] = point4(  0.5, -0.5,  0.5, 1.0 );
    m_vertices[4] = point4( -0.5, -0.5, -0.5, 1.0 );
    m_vertices[5] = point4( -0.5,  0.5, -0.5, 1.0 );
    m_vertices[6] = point4(  0.5,  0.5, -0.5, 1.0 );
	m_vertices[7] = point4(  0.5, -0.5, -0.5, 1.0 );

	m_iIndex = 0;
	// generate 12 triangles: 36 vertices and 36 colors
    Quad( 1, 0, 3, 2 );
    Quad( 2, 3, 7, 6 );
    Quad( 3, 0, 4, 7 );
    Quad( 6, 5, 1, 2 );
    Quad( 4, 5, 6, 7 );
    Quad( 5, 4, 0, 1 );

	// �w�]�N�Ҧ��������]�w���Ǧ�
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

	// Create and initialize a buffer object �A�N���������]�w���J SetShader ��
//	CreateBufferObject();

	// �]�w����
	SetMaterials(vec4(0), vec4(0.5f, 0.5f, 0.5f, 1), vec4(1.0f, 1.0f, 1.0f, 1.0f));
	SetKaKdKsShini(0, 0.8f, 0.2f, 1);
}

void CSolidCube::Quad( int a, int b, int c, int d )
{
    // Initialize temporary vectors along the quad's edge to
    //   compute its face normal 
    vec4 u = m_vertices[b] - m_vertices[a];
    vec4 v = m_vertices[c] - m_vertices[b];
    vec3 normal = normalize( cross(u, v) );

    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[a]; m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[b]; m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[c]; m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[a]; m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[c]; m_iIndex++;
    m_pNormals[m_iIndex] = normal; m_pPoints[m_iIndex] = m_vertices[d]; m_iIndex++;
}

void CSolidCube::Draw()
{
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Change to wireframe mode
	DrawingSetShader();
	glDrawArrays( GL_TRIANGLES, 0, SOLIDCUBE_NUM );
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Return to solid mode
}


void CSolidCube::DrawW()
{
	DrawingWithoutSetShader();
	glDrawArrays( GL_TRIANGLES, 0, SOLIDCUBE_NUM );
}


void CSolidCube::RenderWithFlatShading(vec4 vLightPos, color4 vLightI)
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

void CSolidCube::RenderWithFlatShading(const LightSource &Lights)
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

void CSolidCube::RenderWithGouraudShading(vec4 vLightPos, color4 vLightI)
{
	vec4 vCentroidP;
	for( int i = 0 ; i < m_iNumVtx ; i += 6 ) {
		m_pColors[i] = m_pColors[i + 3] = PhongReflectionModel(m_pPoints[i], m_pNormals[i], vLightPos, vLightI);
		m_pColors[i + 2] = m_pColors[i + 4] = PhongReflectionModel(m_pPoints[i + 2], m_pNormals[i + 2], vLightPos, vLightI);
		m_pColors[i + 1] = PhongReflectionModel(m_pPoints[i + 1], m_pNormals[i + 1], vLightPos, vLightI);
		m_pColors[i + 5] = PhongReflectionModel(m_pPoints[i + 5], m_pNormals[i + 5], vLightPos, vLightI);
	}
	glBindBuffer( GL_ARRAY_BUFFER, m_uiBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx+sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors ); // vertcies' Color
}

void CSolidCube::RenderWithGouraudShading(const LightSource &Lights)
{
	vec4 vCentroidP;
	for (int i = 0; i < m_iNumVtx; i += 6) {
		m_pColors[i] = m_pColors[i + 3] = PhongReflectionModel(m_pPoints[i], m_pNormals[i], Lights);
		m_pColors[i + 2] = m_pColors[i + 4] = PhongReflectionModel(m_pPoints[i + 2], m_pNormals[i + 2], Lights);
		m_pColors[i + 1] = PhongReflectionModel(m_pPoints[i + 1], m_pNormals[i + 1], Lights);
		m_pColors[i + 5] = PhongReflectionModel(m_pPoints[i + 5], m_pNormals[i + 5], Lights);
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_uiBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4)*m_iNumVtx + sizeof(vec3)*m_iNumVtx, sizeof(vec4)*m_iNumVtx, m_pColors); // vertcies' Color
}


// ���B�ҵ��� vLightPos �����O�@�ɮy�Ъ��T�w�����m
void CSolidCube::Update(float dt, point4 vLightPos, color4 vLightI)
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


void CSolidCube::Update(float dt, const LightSource &Lights)
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


void CSolidCube::Update(const LightSource *Lights, float dt)
{
	//if (m_bViewUpdated || m_bTRSUpdated) {
	//	m_mxMVFinal = m_mxView * m_mxTRS;
	//	m_mxMV3X3Final = mat3(	// �u���e���� 3X3 �x�}�����e
	//		m_mxMVFinal._m[0].x, m_mxMVFinal._m[1].x, m_mxMVFinal._m[2].x,
	//		m_mxMVFinal._m[0].y, m_mxMVFinal._m[1].y, m_mxMVFinal._m[2].y,
	//		m_mxMVFinal._m[0].z, m_mxMVFinal._m[1].z, m_mxMVFinal._m[2].z);
	//	m_bViewUpdated = m_bTRSUpdated = false;
	//}



	//for (int i = 0; i < LIGHTCOUNT; i++)
	//{
	//	m_AmbientProduct[i] = m_Material.ka * m_Material.ambient  * Lights[i].ambient;
	//	lightType[i] = Lights[i].type;
	//	m_LightDir[i] = m_mxMV3X3Final * Lights[i].spotDirection;
	//	m_SpotExponent[i] = Lights[i].spotExponent;
	//	m_vLightInView[i] = m_mxView * Lights[i].position;
	//	m_DiffuseProduct[i] = m_Material.kd * m_Material.diffuse  * Lights[i].diffuse;
	//	m_Diffuse[i] = Lights[i].diffuse;
	//	m_SpecularProduct[i] = m_Material.ks * m_Material.specular * Lights[i].specular;
	//	m_spotCosCutoff[i] = Lights[i].spotCosCutoff;
	//	m_iLighting[i] = Lights[i].isLighting;
	//}

	if (m_bViewUpdated || m_bTRSUpdated) {
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_bViewUpdated = m_bTRSUpdated = false;
	}

	for (int i = 0; i < LIGHTCOUNT; i++)
	{
		lightType[i] = Lights[i].type;
		m_vLightInView[i] = m_mxView * Lights[i].position;		// �N Light �ഫ�����Y�y�ЦA��
		m_vSpotTarget[i] = m_mxView * Lights[i].spotTarget;
		// ��X AmbientProduct DiffuseProduct �P SpecularProduct �����e
		m_AmbientProduct[i] = m_Material.ka * m_Material.ambient  * Lights[i].ambient;
		m_DiffuseProduct[i] = m_Material.kd * m_Material.diffuse  * Lights[i].diffuse;
		m_SpecularProduct[i] = m_Material.ks * m_Material.specular * Lights[i].specular;
		m_iLighting[i] = Lights[i].isLighting;
	}


}

void CSolidCube::Update(float dt)
{
	if (m_bViewUpdated || m_bTRSUpdated) { // Model View �������x�}���e�����
		m_mxMVFinal = m_mxView * m_mxTRS;
		m_mxITView = InverseTransposeMatrix(m_mxMVFinal);
		m_bViewUpdated = m_bTRSUpdated = false;
	}
}