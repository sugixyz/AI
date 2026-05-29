#pragma once
#include ".\Library\GameObject.h"
#include "global.h"


class Enemy :
    public GameObject
{
	int hImage_;//画像ID
	Point pos_;//位置
	DIR dir_;//移動方向
public:
	Enemy();
	~Enemy();
	void Update() override;
	void Draw() override;
private:
	//追跡中かどうか
	bool isChase = false;
	//距離
	Point dist;
	//プレイヤーが見えるかどうか
	bool isVisibility = false;
private:
	//追跡
	void Chase();
	void Move();
	//視界内に入っているかどうか
	bool CheckVisibility();

	Pointf VectorNormalize(const Point& p);

	Point GetDir();
	void ToPlayerDir(Point toPlayer);
};

