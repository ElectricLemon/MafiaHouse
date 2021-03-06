#pragma once
#include "GameObject.h"
class MissingItem : public Sprite {
public:
	CREATE_WITH_FRAME(MissingItem);
	CREATE_WITH_FRAME_NAME(MissingItem, "items/fist.png");
	GameObject* owner = NULL;
};
class Item : public GameObject
{
public:
	Item();
	~Item() {}
	CREATE_SPRITE_FUNC(Item, "knife2.png");
	CREATE_WITH_FRAME(Item);
	CREATE_WITH_FRAME_NAME(Item, "items/knife.png");

	virtual void initObject(Vec2 startPos = Vec2(0,0));
	virtual void initRadius();

	enum AttackType {
		STAB,
		SWING,
		SHOOT
	};
	enum Effect {
		NONE,
		KNOCKOUT,
		KILL
	};
	enum State {
		GROUND,
		HELD,
		THROWN,
		FALLING
	};

	State getState() { return state; }

	AttackType getAttackType() { return attackType; }
	Effect getEffect() { return effect; }
	float getStartTime() { return startTime; }
	float getAttackTime() { return attackTime; }
	float getLagTime() { return lagTime; }
	int getRange() { return range; }
	int getRangeRadius() { return rangeRadius; }

	Vec2 angleToDirection(float angle);

	virtual void initPickedUpItem();
	virtual void initHeldItem();
	virtual void initCrouchHeldItem();
	virtual void initOffhand();//for enemies only
	virtual void initDroppedItem(Vec2 pos, bool flip);
	virtual void initGroundItem();
	virtual void initThrownItem();
	virtual void initFallItem();

	virtual void prepareThrow(float angle);
	virtual void prepareCrouchThrow(float angle);
	virtual void throwItem(float angle, Vec2 pos, bool flipped);
	void spin();

	bool checkBroken();
	virtual void breakItem();
	void used();

	void hitWall();

	void prepareShoot(float angle);
	void prepareCrouchShoot(float angle);
	void enemyShoot(Vec2 target);
	void playerShoot(float angle);

	virtual void prepareStab(float angle);
	virtual void prepareSwing(float angle);
	virtual void prepareCrouchStab(float angle);
	virtual void prepareCrouchSwing(float angle);

	virtual void stabSequence(float angle, bool flip);
	virtual void swingSequence(float angle, bool flip);

	virtual void fallAttack();

	virtual void checkThrownSpeed();
	virtual void checkFallingSpeed();
	virtual void checkGroundSpeed();

	void startHeld() { 
		state = HELD;
		startedHeld = true;
	}

	void rotatePickUpRadius(float degrees);

	void stealRange(Node* player);
	virtual void playerInRange(Node* player);
	void hasMoved();
	bool enemyCanUse = false;

	float maxHP = 4;
	float hp = 1;//keeps track fo how much item has been used
	float dmg = 0;
	float doorDmg = 0;
	Vec2 knockback = Vec2(0, 0);
	float hitstun = 0.0f;//amount of time attack prevents you from acting
	int priority = 0;//determines which attacks beat which
	int powerLevel = 0;

	float noiseLevel = 0.5f;

	bool canBreakDoor = false;
	bool isKey = false;

	bool didHitWall = false;
	bool makeNoise = false;

	bool enemyItem = false;//if it's being held by an enemy

	float thrownTime = -1;

	bool holderFlipped = false;

	bool isUnderObject = false;

	bool startedHeld = false;

	void initMissingItem();

	MissingItem* missingItem = NULL;

	float shotTime = -1;
	bool wasShot = false;
	Vec2 endpoint;
	Vec2 startpoint;

protected:
	string itemFile = "items/fist.png";
	State state = GROUND;
	State prevState = GROUND;
	Effect effect = KILL;
	Node * pickUpRadius;
	//attack/use action parameters:
	AttackType attackType;
	float startTime;//startup time of attack
	float attackTime;//duration of active frames
	float lagTime;//enlag time of attack
	int range;//range of the weapons attack, for enemy ai
	float rangeRadius = 100;//for swinging weapons, possible for stabbing weapons once they have multi-directional attacks
};

class Gun : public Item//enemy attacking barehand, actually an invisible item
{
public:
	Gun();
	~Gun() {}
	CREATE_WITH_FRAME(Gun);
	CREATE_WITH_FRAME_NAME(Gun, "items/gun.png");
	void initHeldItem();
};

class Fist : public Item//enemy attacking barehand, actually an invisible item
{
public:
	Fist();
	~Fist() {}
	CREATE_WITH_FRAME(Fist);
	CREATE_WITH_FRAME_NAME(Fist, "items/fist.png");
	void initObject(Vec2 startPos = Vec2(0, 0));
	void initHeldItem();
};

class Knife : public Item
{
public:
	Knife();
	~Knife() {}
	CREATE_WITH_FRAME(Knife);
	CREATE_WITH_FRAME_NAME(Knife, "items/knife.png");
};

class Screwdriver : public Item
{
public:
	Screwdriver();
	~Screwdriver() {}
	CREATE_WITH_FRAME(Screwdriver);
	CREATE_WITH_FRAME_NAME(Screwdriver, "items/screwdriver.png");
};

class Key : public Item
{
public:
	Key();
	~Key() {}
	CREATE_WITH_FRAME(Key);
	CREATE_WITH_FRAME_NAME(Key, "items/key.png");
};

class Hammer : public Item
{
public:
	Hammer();
	~Hammer() {}
	CREATE_WITH_FRAME(Hammer);
	CREATE_WITH_FRAME_NAME(Hammer, "items/hammer.png");
};

class Mug : public Item
{
public:
	Mug();
	~Mug() {}
	CREATE_WITH_FRAME(Mug);
	CREATE_WITH_FRAME_NAME(Mug, "items/mug.png");
};

class Crowbar : public Item
{
public:
	Crowbar();
	~Crowbar() {}
	CREATE_WITH_FRAME(Crowbar);
	CREATE_WITH_FRAME_NAME(Crowbar, "items/crowbar.png");
};

class IronBar : public Item
{
public:
	IronBar();
	~IronBar() {}
	CREATE_WITH_FRAME(IronBar);
	CREATE_WITH_FRAME_NAME(IronBar, "items/ironbar.png");
};
