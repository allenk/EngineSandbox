#pragma once

#include "Systems/Light.h"
#include "Systems/Entity.h"
#include "Systems/Material.h"
#include "Systems/Camera/Camera.h"
#include "Systems/Rendering/Shader.h"

class RenderPass
{
public:

};


class ShadowRenderPass
{
public:
	void GatherLights(std::vector<std::shared_ptr<ILight>>& lights) 
	{

	}

	void GatherObjects(std::vector<std::shared_ptr<Entity>>& entities) 
	{
		
	}

	void Initialize()
	{
		// configure depth map FBO
		// -----------------------
		glGenFramebuffers(1, &depthMapFBO);
		// create depth texture
		glGenTextures(1, &depthMap);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		// attach depth texture as FBO's depth buffer
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void SetShader(Shader shader)
	{
		m_shader = shader;
	}

	void SetWindowParams(WindowParams params)
	{
		m_windowParams = params;
	}

	void Render(Camera& camera,
		const std::vector<std::shared_ptr<ILight>>& lights, 
		const std::vector<std::shared_ptr<Entity>>& entities,
		Material& mat, std::function<void(Shader)> renderScene) 
	{
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		for (auto& entity : entities)
		{
			for (auto& light : lights)
			{
				entity->ApplyLightingPass(*light);
			}
			glm::mat4 model = entity->GetTransform().GetModelMat();
			entity->Draw(model, camera.GetView(), camera.GetProjection(), mat);
		}

		// TEMP: extra scene
		renderScene(m_shader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		glViewport(0, 0, m_windowParams.Width, m_windowParams.Height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	unsigned int GetDepthMap() const { return depthMap; }

	Shader m_shader;
	WindowParams m_windowParams;

	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	unsigned int depthMap;
};