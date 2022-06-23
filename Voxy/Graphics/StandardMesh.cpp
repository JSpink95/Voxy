#include "Graphics/StandardMesh.h"
#include "Graphics/VertexMesh.h"

struct DebugVertex
{
	static inline List<GLenum> Attributes = { GL_FLOAT_VEC3, GL_FLOAT_VEC4 };

	glm::vec3 a_Position;
	glm::vec4 a_Color;
};

struct ScreenQuadVertex
{
	static inline List<GLenum> Attributes = { GL_FLOAT_VEC2/*, GL_FLOAT_VEC2*/ };

	glm::vec2 a_Position;
};

struct SkyDomeVertex
{
	static inline List<GLenum> Attributes = { GL_FLOAT_VEC3/*, GL_FLOAT_VEC2*/ };

	glm::vec3 a_Position;
	//glm::vec2 a_Texcoord;
};

struct LineVertex
{
	static inline List<GLenum> Attributes = { GL_FLOAT_VEC3, GL_FLOAT_VEC4 };

	glm::vec3 a_Position;
	glm::vec4 a_Color;
};

struct BlockVertex
{
	static inline List<GLenum> Attributes = { GL_FLOAT_VEC3, GL_FLOAT_VEC3, GL_FLOAT_VEC2, GL_FLOAT };

	glm::vec3 a_Position, a_Normal;
	glm::vec2 a_Texcoord;
	f32 a_TextureID;
};

struct AnimatedTextureVertex
{
	static inline List<GLenum> Attributes = { GL_FLOAT_VEC3, GL_FLOAT_VEC3, GL_FLOAT_VEC2 };

	glm::vec3 a_Position, a_Normal;
	glm::vec2 a_Texcoord;
};

struct UIVertex
{
	static inline List<GLenum> Attributes = { GL_FLOAT_VEC2, GL_FLOAT_VEC2 };

	glm::vec2 a_Position, a_Texcoord;
};

struct ImageVertex
{
	static inline List<GLenum> Attributes = { GL_FLOAT_VEC2 };

	glm::vec2 a_Position;
};

namespace Graphics
{
	template<VertexType T>
	void InitialiseVertexMeshSpecification(List<u32> const& indices, List<T> const& vertices, VertexMeshSpecification& specification)
	{
		specification.attributes = T::Attributes;

		if (!indices.empty())
		{
			specification.indexCount = indices.size();
			specification.indices = indices.data();
		}

		specification.vertexCount = vertices.size();
		specification.vertices = vertices.data();
	}

	Reference<VertexMesh> CreateScreenQuad()
	{
		List<ScreenQuadVertex> vertices =
		{
			ScreenQuadVertex{ glm::vec2(-1.0f, -1.0f) },
			ScreenQuadVertex{ glm::vec2( 1.0f, -1.0f) },
			ScreenQuadVertex{ glm::vec2( 1.0f,  1.0f) },
			ScreenQuadVertex{ glm::vec2(-1.0f,  1.0f) },
		};

		List<u32> indices =
		{
			0, 1, 2,
			0, 2, 3,
		};

		VertexMeshSpecification specification = {};
		InitialiseVertexMeshSpecification<ScreenQuadVertex>(indices, vertices, specification);

		return VertexMesh::Create(specification);
	}

	Reference<VertexMesh> CreateSkyDomeMesh()
	{
		constexpr u64 Stacks = 8;
		constexpr u64 Slices = 8;

		List<SkyDomeVertex> vertices = {};

		auto toSphereCoord = [](f32 theta, f32 phi) -> glm::vec3
		{
			f32 x = glm::sin(phi) * glm::cos(theta);
			f32 y = glm::cos(phi);
			f32 z = glm::sin(phi) * glm::sin(theta);

			return glm::vec3(x, y, z);
		};

		for (s32 t = 0; t < Stacks; t++) // stacks are ELEVATION so they count theta
		{
			f32 theta1 = ((f32)(t) / Stacks) * glm::pi<f32>();
			f32 theta2 = ((f32)(t + 1) / Stacks) * glm::pi<f32>();

			for (s32 p = 0; p < Slices; p++) // slices are ORANGE SLICES so the count azimuth
			{
				f32 phi1 = ((f32)(p) / Slices) * glm::two_pi<f32>(); // azimuth goes around 0 .. 2*PI
				f32 phi2 = ((f32)(p + 1) / Slices) * glm::two_pi<f32>();

				//phi2   phi1
				// |      |
				// 2------1 -- theta1
				// |\ _   |
				// |    \ |
				// 3------4 -- theta2
				//

				glm::vec3 vertex1 = toSphereCoord(theta1, phi1);
				glm::vec3 vertex2 = toSphereCoord(theta1, phi2);
				glm::vec3 vertex3 = toSphereCoord(theta2, phi2);
				glm::vec3 vertex4 = toSphereCoord(theta2, phi1);

				if (t == 0)
				{
					vertices.push_back({ vertex1 });
					vertices.push_back({ vertex3 });
					vertices.push_back({ vertex4 });
				}
				else if ((t + 1) == Stacks)
				{
					vertices.push_back({ vertex3 });
					vertices.push_back({ vertex1 });
					vertices.push_back({ vertex2 });
				}
				else
				{
					vertices.push_back({ vertex1 });
					vertices.push_back({ vertex2 });
					vertices.push_back({ vertex4 });

					vertices.push_back({ vertex2 });
					vertices.push_back({ vertex3 });
					vertices.push_back({ vertex4 });
				}
			}
		}

		VertexMeshSpecification specification = {};
		specification.attributes = SkyDomeVertex::Attributes;
		//specification.indexCount = indices.size();
		//specification.indices = indices.data();
		specification.vertexCount = vertices.size();
		specification.vertices = vertices.data();

		return VertexMesh::Create(specification);
	}

	Reference<VertexMesh> CreateBlockMesh()
	{
		List<BlockVertex> vertices =
		{
			// bottom face
			BlockVertex{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), 0.0f },
			BlockVertex{ glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), 0.0f },
			BlockVertex{ glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f), 0.0f },
			BlockVertex{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), 0.0f },
			BlockVertex{ glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f), 0.0f },
			BlockVertex{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f), 0.0f },

			// back face
			BlockVertex{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f), 1.0f },
			BlockVertex{ glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f), 1.0f },
			BlockVertex{ glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f), 1.0f },
			BlockVertex{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f), 1.0f },
			BlockVertex{ glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f), 1.0f },
			BlockVertex{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f), 1.0f },

			// front face
			BlockVertex{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f,  1.0f), glm::vec2(0.0f, 0.0f), 1.0f },
			BlockVertex{ glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f,  1.0f), glm::vec2(1.0f, 0.0f), 1.0f },
			BlockVertex{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f,  1.0f), glm::vec2(1.0f, 1.0f), 1.0f },
			BlockVertex{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f,  1.0f), glm::vec2(0.0f, 0.0f), 1.0f },
			BlockVertex{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f,  1.0f), glm::vec2(1.0f, 1.0f), 1.0f },
			BlockVertex{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f,  1.0f), glm::vec2(0.0f, 1.0f), 1.0f },

			// left face
			BlockVertex{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), 1.0f },
			BlockVertex{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), 1.0f },
			BlockVertex{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 1.0f },
			BlockVertex{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), 1.0f },
			BlockVertex{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 1.0f },
			BlockVertex{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), 1.0f },

			// right face
			BlockVertex{ glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), 1.0f },
			BlockVertex{ glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), 1.0f },
			BlockVertex{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 1.0f },
			BlockVertex{ glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), 1.0f },
			BlockVertex{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), 1.0f },
			BlockVertex{ glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), 1.0f },

			// top face
			BlockVertex{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f), 2.0f },
			BlockVertex{ glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f), 2.0f },
			BlockVertex{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), 2.0f },
			BlockVertex{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f), 2.0f },
			BlockVertex{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), 2.0f },
			BlockVertex{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f), 2.0f },
		};

		VertexMeshSpecification specification = {};
		InitialiseVertexMeshSpecification<BlockVertex>({}, vertices, specification);

		return VertexMesh::Create(specification);
	}

	Reference<VertexMesh> CreateAnimatedTextureBlockMesh()
	{
		List<AnimatedTextureVertex> vertices =
		{
			// bottom face
			AnimatedTextureVertex{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
			AnimatedTextureVertex{ glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
			AnimatedTextureVertex{ glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
			AnimatedTextureVertex{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
			AnimatedTextureVertex{ glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
			AnimatedTextureVertex{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f) },

			// back face
			AnimatedTextureVertex{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f) },
			AnimatedTextureVertex{ glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f) },
			AnimatedTextureVertex{ glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f) },
			AnimatedTextureVertex{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f) },
			AnimatedTextureVertex{ glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f) },
			AnimatedTextureVertex{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f) },

			// front face
			AnimatedTextureVertex{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f,  1.0f), glm::vec2(0.0f, 0.0f) },
			AnimatedTextureVertex{ glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f,  1.0f), glm::vec2(1.0f, 0.0f) },
			AnimatedTextureVertex{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f,  1.0f), glm::vec2(1.0f, 1.0f) },
			AnimatedTextureVertex{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, 0.0f,  1.0f), glm::vec2(0.0f, 0.0f) },
			AnimatedTextureVertex{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f,  1.0f), glm::vec2(1.0f, 1.0f) },
			AnimatedTextureVertex{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 0.0f,  1.0f), glm::vec2(0.0f, 1.0f) },

			// left face
			AnimatedTextureVertex{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
			AnimatedTextureVertex{ glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
			AnimatedTextureVertex{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
			AnimatedTextureVertex{ glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
			AnimatedTextureVertex{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
			AnimatedTextureVertex{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f) },

			// right face
			AnimatedTextureVertex{ glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
			AnimatedTextureVertex{ glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
			AnimatedTextureVertex{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
			AnimatedTextureVertex{ glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
			AnimatedTextureVertex{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
			AnimatedTextureVertex{ glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f) },

			// top face
			AnimatedTextureVertex{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
			AnimatedTextureVertex{ glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
			AnimatedTextureVertex{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
			AnimatedTextureVertex{ glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
			AnimatedTextureVertex{ glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
			AnimatedTextureVertex{ glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f) },
		};

		VertexMeshSpecification specification = {};
		InitialiseVertexMeshSpecification({}, vertices, specification);

		return VertexMesh::Create(specification);
	}
	
	Reference<VertexMesh> CreateBlockMeshOutline(glm::vec4 const &color /* = { 1.0f, 0.0f, 1.0f, 1.0f } */)
	{
		Array<LineVertex, 8> vertices =
		{
			LineVertex{ glm::vec3(-0.5f, -0.5f, -0.5f), color },
			LineVertex{ glm::vec3( 0.5f, -0.5f, -0.5f), color },
			LineVertex{ glm::vec3( 0.5f, -0.5f,  0.5f), color },
			LineVertex{ glm::vec3(-0.5f, -0.5f,  0.5f), color },

			LineVertex{ glm::vec3(-0.5f,  0.5f, -0.5f), color },
			LineVertex{ glm::vec3( 0.5f,  0.5f, -0.5f), color },
			LineVertex{ glm::vec3( 0.5f,  0.5f,  0.5f), color },
			LineVertex{ glm::vec3(-0.5f,  0.5f,  0.5f), color },
		};

		Array<u32, 24> indices =
		{
			// bottom face
			0, 1, 1, 2, 2, 3, 3, 0,
			// top face
			4, 5, 5, 6, 6, 7, 7, 4,
			// pillars
			0, 4, 1, 5, 2, 6, 3, 7,
		};

		VertexMeshSpecification specification = {};
		specification.attributes = LineVertex::Attributes;
		specification.indexCount = indices.size();
		specification.indices = indices.data();
		specification.vertexCount = vertices.size();
		specification.vertices = vertices.data();

		return VertexMesh::Create(specification);
	}

	Reference<VertexMesh> CreateUIElementMesh_Centre()
	{
		Array<UIVertex, 4> vertices =
		{
			UIVertex{ glm::vec2(-0.5f, -0.5f), glm::vec2(0.0f, 0.0f) },
			UIVertex{ glm::vec2( 0.5f, -0.5f), glm::vec2(1.0f, 0.0f) },
			UIVertex{ glm::vec2( 0.5f,  0.5f), glm::vec2(1.0f, 1.0f) },
			UIVertex{ glm::vec2(-0.5f,  0.5f), glm::vec2(0.0f, 1.0f) },
		};

		Array<u32, 6> indices =
		{
			0, 1, 2,
			0, 2, 3,
		};

		VertexMeshSpecification specification = {};
		specification.attributes = UIVertex::Attributes;
		specification.indexCount = indices.size();
		specification.indices = indices.data();
		specification.vertexCount = vertices.size();
		specification.vertices = vertices.data();

		return VertexMesh::Create(specification);
	}

	Reference<VertexMesh> CreateUIElementMesh_BL()
	{
		List<UIVertex> vertices =
		{
			UIVertex{ glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
			UIVertex{ glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
			UIVertex{ glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f) },
			UIVertex{ glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 1.0f) },
		};

		List<u32> indices =
		{
			0, 1, 2,
			0, 2, 3,
		};

		VertexMeshSpecification specification = {};
		InitialiseVertexMeshSpecification<UIVertex>(indices, vertices, specification);

		return VertexMesh::Create(specification);
	}

	Reference<VertexMesh> CreateImageMesh()
	{
		List<ImageVertex> vertices =
		{
			ImageVertex{ glm::vec2(0.0f) }
		};

		VertexMeshSpecification specification = {};
		InitialiseVertexMeshSpecification<ImageVertex>({}, vertices, specification);

		return VertexMesh::Create(specification);
	}
}