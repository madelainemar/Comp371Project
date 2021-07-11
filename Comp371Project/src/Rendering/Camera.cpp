#include "Camera.h"
#include "../Dependencies/glm-0.9.9.8/glm/gtc/matrix_transform.hpp"
#include "../Core/Application.h"

void Camera::SetOrthographic(float size, float nearClip, float farClip)
{
	m_projectionType = ProjectionType::Orthographic;

	m_orthographicSize = size;
	m_orthographicNear = nearClip;
	m_orthographicFar = farClip;

	RecalculateProjection();
}

void Camera::SetPerspective(float fov, float nearClip, float farClip)
{
	m_projectionType = ProjectionType::Perspective;

	m_perspectiveFOV = fov;
	m_perspectiveNear = nearClip;
	m_perspectiveFar = farClip;

	RecalculateProjection();
}

void Camera::SetViewportSize(unsigned int width, unsigned int height)
{
	m_aspectRatio = (float)width / (float)height;
	RecalculateProjection();

}

void Camera::UpdateAspectRatio()
{
	int width, height;
	glfwGetWindowSize(Application::GetWindow(), &width, &height);
	m_aspectRatio = (float)width / (float)height;
	RecalculateProjection();
}

void Camera::RecalculateProjection()
{
	if (m_projectionType == ProjectionType::Perspective)
	{
		m_projectionMatrix = glm::perspective(m_perspectiveFOV, m_aspectRatio, m_perspectiveNear, m_perspectiveFar);
	}
	else //Orthographic
	{
		float orthoLeft = -m_orthographicSize * m_aspectRatio * 0.5f;
		float orthoRight = m_orthographicSize * m_aspectRatio * 0.5f;
		float orthoBottom = -m_orthographicSize * 0.5f;
		float orthoTop = m_orthographicSize * 0.5f;

		m_projectionMatrix = glm::ortho(orthoLeft, orthoRight,
			orthoBottom, orthoTop, m_orthographicNear, m_orthographicFar);
	}
}