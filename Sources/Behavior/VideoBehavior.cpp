#include "VideoBehavior.h"


void VideoBehavior::Execute(GameObject* obj, float elapsedTime)
{
	auto* video = obj->GetComponent<VideoComponent>();
	video->Update(elapsedTime);
}
