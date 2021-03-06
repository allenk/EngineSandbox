
#include "Model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer/Shader.h"

#include "Assets/AssetManager.h"
#include "Systems/Rendering/Vertex.h"


Model::Model()
{
}

Model::Model(const Model& copy)
	: m_meshes(copy.m_meshes)
	, m_materials(copy.m_materials)
	, m_materialOverride(copy.m_materialOverride)
{
}

Model::Model(Model&& move)
	: m_meshes(move.m_meshes)
	, m_materials(move.m_materials)
	, m_materialOverride(move.m_materialOverride)
{
	
}

Model& Model::operator=(const Model& assign)
{
	m_meshes = assign.m_meshes;
	m_materials = assign.m_materials;
	m_materialOverride = assign.m_materialOverride;
	return *this;
}

Model::~Model()
{
}

void Model::Initialize()
{
	const unsigned int meshCount = static_cast<unsigned int>(m_meshes.size());
	for (unsigned int i = 0; i < meshCount; ++i)
	{
		m_meshes[i].CreateBuffers();
	}
}

void Model::SetShader(Shader* shader)
{
	const unsigned int materialCount = static_cast<unsigned int>(m_materials.size());
	for (unsigned int i = 0; i < materialCount; ++i)
	{
		m_materials[i].SetShader(shader);
	}
}

void Model::Draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
{
	const unsigned int meshCount = static_cast<unsigned int>(m_meshes.size());

	if (m_materialOverride.IsValid())
	{
		m_materialOverride.SetMVP(model, view, projection);
		m_materialOverride.BindTextures();
	}

	for (unsigned int i = 0; i < meshCount; ++i)
	{
		if (m_materialOverride.IsValid())
		{
			m_meshes[i].Draw();
		}
		else
		{
			unsigned int matIndex = m_meshes[i].GetMaterialIndex();
			const bool hasMaterial = matIndex >= 0 && matIndex < m_materials.size();
			if (hasMaterial)
			{
				Material mat = m_materials[matIndex];
				mat.SetMVP(model, view, projection);
				mat.BindTextures();

				m_meshes[i].Draw();
			}
		}
	}
}

void Model::Draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, Material material)
{
	const unsigned int meshCount = static_cast<unsigned int>(m_meshes.size());
	for (unsigned int i = 0; i < meshCount; ++i)
	{
		material.SetMVP(model, view, projection);
		material.BindTextures();
		m_meshes[i].Draw();
	}
}

void Model::Draw(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const Shader* shader)
{
	shader->Use();
	shader->SetMatrix("view", view);
	shader->SetMatrix("projection", projection);
	shader->SetMatrix("model", model);

	const unsigned int meshCount = static_cast<unsigned int>(m_meshes.size());
	for (unsigned int i = 0; i < meshCount; ++i)
	{
		m_meshes[i].Draw();
	}
}

void Model::AddMesh(Mesh mesh)
{
	m_meshes.push_back(mesh);
}

void Model::AddMaterial(Material material)
{
	m_materials.push_back(material);
}

void Model::ApplyLight(ILight& light)
{
	const unsigned int matCount = static_cast<unsigned int>(m_materials.size());

	if (m_materialOverride.IsValid())
	{
		m_materialOverride.GetShader()->Use();
		light.SetProperties(m_materialOverride.GetShader());
	}
	else
	{
		for (unsigned int i = 0; i < matCount; ++i)
		{
			m_materials[i].GetShader()->Use();
			light.SetProperties(m_materials[i].GetShader());
		}
	}
}
