#pragma once
#include "GameObject.h"
#include "Item.h"
USING_NS_CC;

class Player : public GameObject
{
public:
	Player();
	~Player();

	static Player* create(const std::string& filename = "player.png");

	void initObject(Vec2 startPos = Vec2(0, 0));

	//functions for player actions:
	void pickUpItem();

	int itemToPickUp = -1;//the tag of the item the player is picking up

	int objectHidingBehind = -1;//the tag of the object the player is hiding behind

	bool hidden = false;
	bool hideStart = false;//used for setting the object you are hiding behind transparent

	int stairEntered = -1;//the tag of the stairway the player has used

	Item* heldItem = NULL;

	float maxSpeed = 0.6f;

private:
	std::vector<Item*> items;//items the player is carrying
};
