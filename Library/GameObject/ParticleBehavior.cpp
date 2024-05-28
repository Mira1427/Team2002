#include "ParticleBehavior.h"

#include "../Library/Library.h"

void ParticleBehavior::Execute(GameObject* obj, float elapsedTime)
{
	switch (obj->state_)
	{
	case 0:

	{
		ParticleComponent* particle = obj->GetComponent<ParticleComponent>();

		particle->Initialize(RootsLib::DX11::GetDeviceContext());
	}

	obj->state_++;

	case 1:

	{
		ParticleComponent* particle = obj->GetComponent<ParticleComponent>();

		particle->Update(RootsLib::DX11::GetDeviceContext(), elapsedTime);
	}

	break;
	}
}
