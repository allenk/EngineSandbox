#pragma once

#include "Model.h"
#include "Transform.h"
#include "Renderer/Light.h"

class Entity
{
public:
	Entity();
	Entity(const Entity& copy);
	Entity(Entity&& move);
	Entity& operator=(const Entity& assign);

	~Entity() = default;

	void SetTransform(const Transform& newTransform) { m_transform = newTransform; }
	Transform& GetTransform() { return m_transform; }

	Model& GetModel()
	{
		return m_model;
	}

	void SetModel(Model& m) 
	{ 
		m_model = m; 
	}

	void Update(float deltaTime);
	void Draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, Material& material);
	void Draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const Shader* shader);
	void Draw(const glm::mat4& view, const glm::mat4& projection);
	void ApplyLightingPass(ILight& light);

private:
	unsigned int m_id;

	Transform m_transform;

	// TODO: make a model component
	Model m_model;

	static unsigned int ID;
};
