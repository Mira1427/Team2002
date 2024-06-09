#pragma once

#include "../../Library/GameObject/GameObject.h"


class EnemySpawnerComponent;

// --- 敵のスポナーの処理 ---
class EnemySpawnerBehavior final : public Behavior
{
public:
	void Execute(GameObject* obj, float elapsedTime) override;
	void Hit(GameObject* src, GameObject* dst, float elapsedTime) override {};

private:
	void AddEnemy(GameObject* obj);

	void Wave1Enemy(GameObject* obj, EnemySpawnerComponent* spawner);
	void Wave1_2Enemy(GameObject* obj, EnemySpawnerComponent* spawner);
	void Wave2Enemy(GameObject* obj, EnemySpawnerComponent* spawner);
	void Wave3Enemy(GameObject* obj, EnemySpawnerComponent* spawner);
	void Wave4Enemy(GameObject* obj, EnemySpawnerComponent* spawner);
	void Wave5Enemy(GameObject* obj, EnemySpawnerComponent* spawner);
	void Wave6Enemy(GameObject* obj, EnemySpawnerComponent* spawner);
	void Wave7Enemy(GameObject* obj, EnemySpawnerComponent* spawner);
	void Wave8Enemy(GameObject* obj, EnemySpawnerComponent* spawner);
	void Wave9Enemy(GameObject* obj, EnemySpawnerComponent* spawner);
	void Wave10Enemy(GameObject* obj, EnemySpawnerComponent* spawner);

	void Enemy(GameObject* obj, EnemySpawnerComponent* spawner, float life, int color, int angle, int item = -1);

};