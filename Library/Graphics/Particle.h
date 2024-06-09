#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "../Math/Vector.h"

#define NUMTHREADS_X 16


struct ParticleData
{
	Vector4 color_ = Vector4::White_;
	Vector3 position_;
	float age_{};
	Vector3 velocity_;
	int state_{};
};


class Particle
{
public:
	Particle(ID3D11Device* device, size_t count);
	~Particle() {};
	Particle(const Particle&) = delete;
	Particle& operator=(const Particle&) = delete;
	Particle(Particle&&) noexcept = delete;
	Particle& operator=(Particle&&) noexcept = delete;

	void Initialize(ID3D11DeviceContext* dc, float deltaTime, const Vector3& position);
	void Update(ID3D11DeviceContext* dc, float deltaTime, const Vector3& position);
	void Draw(ID3D11DeviceContext* dc, ID3D11ShaderResourceView** srv);

	const size_t maxParticleCount_;

	struct Constants
	{
		Vector3 emitterPosition_;
		float particleSize_{ 0.08f };
		float time_{};
		float deltaTime_{};
		float pad_[2];
	};

	Constants particleData_;

	Microsoft::WRL::ComPtr<ID3D11Buffer>				buffer_;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>	bufferUAV_;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	bufferSRV_;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vs_;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		ps_;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	gs_;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>		cs_;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>		initializerCS_;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			constantBuffer_;
};

