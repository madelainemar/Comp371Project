#pragma once
#define GLEW_STATIC
#include "../Dependencies/glew-2.1.0/include/GL/glew.h"
#include "../Core/Script.h"
#include "../GraphicsAPI/OpenGLBuffer.h"
#include "../GraphicsAPI/OpenGLVertexArray.h"
#include "../GraphicsAPI/OpenGLShader.h"
#include "../GraphicsAPI/OpenGLFramebuffer.h"
#include "../GraphicsAPI/OpenGLCubeMap.h"
#include "../Rendering/CameraController.h"

#include <vector>
#include <sstream>

//https://learnopengl.com/code_viewer_gh.php?code=src/2.lighting/6.multiple_lights/multiple_lights.cpp
//https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.1.3.shadow_mapping/shadow_mapping.cpp
//https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/3.2.2.point_shadows_soft/point_shadows_soft.cpp

struct LightVertexData
{
	glm::vec3 position;
	glm::vec3 normal;
	Material mat;
};

class PointLightTest : public Script
{
public:
	void OnStart()
	{
		Application::SetBackgroundColor({ 1, 1, 1, 1 });
		PrepareCubeAndPlane();
			
		m_framebuffer = std::make_shared<OpenGLFramebuffer>(1024, 1024, true);
		m_framebuffer->Unbind();

		m_shader = std::make_shared<OpenGLShader>("Resources/Shaders/PointLightShader.glsl");
		m_shadowMapShader = std::make_shared<OpenGLShader>("Resources/Shaders/ShadowMap.glsl");
		GenerateShadowMap(m_shadowMapShader, m_framebuffer, m_light.position);
		//SetPointLight(m_shader, m_light.position, {1, 1, 1, 1}, 0);

		m_shader->Bind();
		m_shader->SetFloat3("lightPos", m_light.position);
		m_shader->SetInt("shadows", 0);
		m_shader->SetFloat("far_plane", Application::GetCamera()->GetPerspectiveFarClip());
		m_shader->SetInt("u_shadowMap", 0);
	}

	void OnUpdate()
	{
		if (Input::IsKeyPressed(GLFW_KEY_W))
		{
			m_light.position.z -= 5*Time::GetDeltaTime();
		}

		if (Input::IsKeyPressed(GLFW_KEY_S))
		{
			m_light.position.z += 5*Time::GetDeltaTime();
		}
	}

	void OnRender()
	{
		/*
		auto camera = Application::GetCamera();
		glm::mat4 camTransform = camera->GetTransform();
		glm::mat4 viewProjectionMatrix = camera->GetProjectionMatrix() * camTransform;

		m_shader->Bind();
		m_shader->SetMat4("u_viewProjMatrix", viewProjectionMatrix);
		m_shader->SetMat4("u_lightSpaceMatrix", m_lightSpaceMatrix);
		m_shader->SetInt("u_shadowMap", 0);
		m_shader->SetFloat3("viewPos", Application::GetCameraController()->GetCamPos());
		*/
		
		m_framebuffer->Unbind();
		int windowWidth, windowHeight;
		glfwGetWindowSize(Application::GetWindow(), &windowWidth, &windowHeight);
		glm::mat4 lookAtMat = glm::lookAt(m_light.position, m_cube.position, { 0, 1, 0 });
		glm::mat4 perspectiveMat = glm::perspective(glm::radians(90.0f), (float)windowWidth / (float)windowHeight,
			Application::GetCamera()->GetPerspectiveNearClip(), 100.0f);
		m_lightSpaceMatrix = perspectiveMat * lookAtMat;

		m_shadowMapShader->Bind();
		m_shadowMapShader->SetMat4("u_lightSpaceMatrix", m_lightSpaceMatrix);

		glActiveTexture(GL_TEXTURE0);
		m_vao->Bind();
		m_ibo->Bind();
		glDrawElements(GL_TRIANGLES, m_ibo->GetCount(), GL_UNSIGNED_INT, nullptr);

		/*
		m_vaoLight->Bind();
		m_iboLight->Bind();
		glDrawElements(GL_TRIANGLES, m_iboLight->GetCount(), GL_UNSIGNED_INT, nullptr);
		*/

	}

private:

	void PrintMat4(const glm::mat4& m)
	{
		for (unsigned int i = 0; i < 4; i++)
		{
			for (unsigned int j = 0; j < 4; j++)
			{
				std::cout << m[j][i] << " ";
			}
			std::cout << "\n";
		}
	}

	void PrepareCubeAndPlane()
	{
		m_vao = std::make_shared<OpenGLVertexArray>();
		/*//back face
		1, 0, 2,
		2, 3, 1,

		//left face
		0, 4, 6,
		6, 2, 0,

		//front face
		4, 5, 7, 
		7, 6, 4,

		//right face
		5, 1, 3,
		3, 7, 5,

		//top face
		6, 7, 3, 
		3, 2, 6,

		//bottom face
		0, 1, 5,
		5, 4, 0
		*/

		glm::vec3 posAndNormals[] = {
			//back face
			{  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f },
			{ -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f },
			{ -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f },
			{  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f },

			//left face
			{ -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f },
			{ -0.5f, -0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f },
			{ -0.5f,  0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f },
			{ -0.5f,  0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f },

			//front face
			{ -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f },
			{  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f },
			{  0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f },
			{ -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f },

			//right face
			{  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f },
			{  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f },
			{  0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f },
			{  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f },

			//top face
			{ -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f },
			{  0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f },
			{  0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f },
			{ -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f },

			//bottom face
			{ -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f },
			{  0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f },
			{  0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f },
			{ -0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f }

			/* actual vertices
			{ -0.5f, -0.5f, -0.5f }, //0
			{  0.5f, -0.5f, -0.5f }, //1
			{ -0.5f,  0.5f, -0.5f }, //2
			{  0.5f,  0.5f, -0.5f }, //3
			{ -0.5f, -0.5f,  0.5f }, //4
			{  0.5f, -0.5f,  0.5f }, //5
			{ -0.5f,  0.5f,  0.5f }, //6
			{  0.5f,  0.5f,  0.5f }, //7
			*/
		};

		constexpr unsigned int numVertices = sizeof(posAndNormals) / (sizeof(glm::vec3) * 2);
		LightVertexData cubePlanePos[2 * numVertices];
		LightVertexData lightPosAndNormal[numVertices];

		unsigned int index = 0;
		//cube
		for (int i = 0; i < numVertices * 2; i+=2)
		{
			cubePlanePos[index].position = m_cube.GetTransformMatrix() * glm::vec4(posAndNormals[i], 1);
			cubePlanePos[index].normal = glm::vec3(m_cube.GetTransformMatrix() * glm::vec4(posAndNormals[i + 1], 0));
			cubePlanePos[index].mat = m_testMat;
			index++;
		}


		//plane
		for (int i = 0; i < numVertices * 2; i += 2)
		{
			cubePlanePos[index].position = m_plane.GetTransformMatrix() * glm::vec4(posAndNormals[i], 1);
			cubePlanePos[index].normal = glm::vec3(m_plane.GetTransformMatrix() * glm::vec4(posAndNormals[i + 1], 0));
			cubePlanePos[index].mat = m_testMat;
			index++;
		}

		index = 0;

		//light
		for (int i = 0; i < numVertices * 2; i += 2)
		{
			lightPosAndNormal[index].position = m_light.GetTransformMatrix() * glm::vec4(posAndNormals[i], 1);
			lightPosAndNormal[index].normal = m_light.GetTransformMatrix() * glm::vec4(posAndNormals[i + 1], 0);
			lightPosAndNormal[index].mat = m_defaultMat;
			index++;
		}

		m_vbo = std::make_shared<OpenGLVertexBuffer>((float*)(void*)cubePlanePos, sizeof(cubePlanePos));
		m_vbo->SetLayout({ 
			{ShaderDataType::Float3, "position"}, 
			{ShaderDataType::Float3, "normal"}, 
			{ShaderDataType::Float3, "ambiant"}, 
			{ShaderDataType::Float3, "diffuse"}, 
			{ShaderDataType::Float3, "specular"}, 
			{ShaderDataType::Float, "shininess"}
		});

		m_vboLight = std::make_shared<OpenGLVertexBuffer>((float*)(void*)lightPosAndNormal, sizeof(lightPosAndNormal));
		m_vboLight->SetLayout({
			{ShaderDataType::Float3, "position"},
			{ShaderDataType::Float3, "normal"},
			{ShaderDataType::Float3, "ambiant"},
			{ShaderDataType::Float3, "diffuse"},
			{ShaderDataType::Float3, "specular"},
			{ShaderDataType::Float, "shininess"}
			});

		m_vao->AddVertexBuffer(m_vbo);

		m_vaoLight = std::make_shared<OpenGLVertexArray>();
		m_vaoLight->AddVertexBuffer(m_vboLight);

		unsigned int indices[] = {
			//back face
			0, 1, 2,
			2, 3, 0,

			//left face
			4, 5, 6,
			6, 7, 4,

			//front face
			8, 9, 10,
			10, 11, 8,

			//right face
			12, 13, 14,
			14, 15, 12,

			//top face
			16, 17, 18,
			18, 19, 16,

			//bottom face
			20, 21, 22,
			22, 23, 20
		};

		constexpr unsigned int indicesCount = sizeof(indices) / sizeof(unsigned int);
		unsigned int indexData[2 * indicesCount];
		unsigned int lightIndices[indicesCount];

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < indicesCount; j++)
			{
				indexData[i * indicesCount + j] = indices[j] + (i * numVertices);
			}
		}

		for (int j = 0; j < indicesCount; j++)
		{
			lightIndices[j] = indices[j];
		}

		m_ibo = std::make_shared<OpenGLIndexBuffer>(indexData, sizeof(indexData) / sizeof(unsigned int));
		m_iboLight = std::make_shared<OpenGLIndexBuffer>(lightIndices, sizeof(lightIndices) / sizeof(unsigned int));
	}

	void SetPointLight(std::shared_ptr<OpenGLShader>& shader, glm::vec3 position, glm::vec4 color, int index)
	{
		shader->Bind();
		std::stringstream ss;
		ss << "u_pointLights[" << index << "].";
		

		shader->SetFloat3((ss.str() + "position").c_str(), position);
		shader->SetFloat4((ss.str() + "color").c_str(), color);
	}

	void GenerateShadowMap(std::shared_ptr<OpenGLShader>& shader,
		std::shared_ptr<OpenGLFramebuffer>& framebuffer, glm::vec3 position)
	{
		//to avoid peter panning we cull the front face when generating the 
		//shadow map (works only for solid objects like cubes)
		glCullFace(GL_FRONT);

		//might need to change since currently calculating as a directional light
		int windowWidth, windowHeight;
		glfwGetWindowSize(Application::GetWindow(), &windowWidth, &windowHeight);
		glm::mat4 lookAtMat = glm::lookAt(position, m_cube.position, { 0, 1, 0 });
		glm::mat4 perspectiveMat = glm::perspective(glm::radians(90.0f), (float)windowWidth / (float)windowHeight,
			Application::GetCamera()->GetPerspectiveNearClip(), Application::GetCamera()->GetPerspectiveFarClip());
		m_lightSpaceMatrix = perspectiveMat * lookAtMat;

		std::cout << position.x << ", " << position.y << ", " << position.z << "\n\n";
		PrintMat4(lookAtMat);
		shader->Bind();
		shader->SetMat4("u_lightSpaceMatrix", m_lightSpaceMatrix);

		//store the current window width and height and change the 
		//viewport size to fit the whole shadow map texture
		glViewport(0, 0, framebuffer->GetWidth(), framebuffer->GetHeight());
		
		glClear(GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE0);
		framebuffer->Bind();
		glBindTexture(GL_TEXTURE_2D, framebuffer->GetDepthAttachment());

		//render the scene
		shader->Bind();
		m_vao->Bind();
		m_ibo->Bind();
		glDrawElements(GL_TRIANGLES, m_ibo->GetCount(), GL_UNSIGNED_INT, nullptr);
		////////////////////

		//revert the cull face settings and the viewport size to their previous values for the actual rendering
		framebuffer->Unbind();
		glViewport(0, 0, windowWidth, windowHeight);
		glCullFace(GL_BACK);
	}

	std::shared_ptr<OpenGLVertexArray> m_vao;
	std::shared_ptr<OpenGLVertexBuffer> m_vbo;
	std::shared_ptr<OpenGLIndexBuffer> m_ibo;
	std::shared_ptr<OpenGLShader> m_shader;
	std::shared_ptr<OpenGLShader> m_shadowMapShader;

	std::shared_ptr<OpenGLVertexArray> m_vaoLight;
	std::shared_ptr<OpenGLVertexBuffer> m_vboLight;
	std::shared_ptr<OpenGLIndexBuffer> m_iboLight;

	std::shared_ptr<OpenGLFramebuffer> m_framebuffer;

	glm::mat4 m_lightSpaceMatrix;

	Transform m_cube = Transform({0, 0, -3});
	Transform m_plane = Transform({ 0, -3, -3 }, { 0, 0, 0 }, {10, 0.5f, 10});
	Transform m_light = Transform({ 2.0f, 2.0f, -6.0f }, { 0, 0, 0 }, {0.1f, 0.1f, 0.1f});

	Material m_defaultMat = { {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1, 1, 1}, 32 };
	Material m_testMat = { {0.1f, 0.0f, 0.0f}, {0.5f, 0.0f, 0.0f}, {0.75f, 0, 0}, 32 };


	//temp
	unsigned int m_fbo;
	unsigned int m_cubemap;
	CameraController* m_camController;
};