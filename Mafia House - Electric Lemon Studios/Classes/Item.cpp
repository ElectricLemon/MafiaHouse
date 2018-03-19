#include "Item.h"

Item::Item()
{
	name = "item";
	//sprite properties
	zOrder = 6;
	//physics body properties
	category = 32;
	collision = 8;
	tag = 10000;//eac_h item type will be identified by the second and third digit: 10100 - 10199 for knives
	dynamic = true;
	rotate = true;
	attackType = STAB;
	startTime = 10 FRAMES;
	attackTime = 10 FRAMES;
	lagTime = 20 FRAMES;
}
Item::~Item(){
}

void Item::initObject(Vec2 startPos)
{
	GameObject::initObject(startPos);
	retain();
	initRadius();
	Texture2D::TexParams texParams = { GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE };
	outline = Sprite::createWithSpriteFrameName("items/knife_outline.png");
	outline->getTexture()->setTexParameters(texParams);
	outline->setPositionNormalized(Vec2(0.5, 0.5));
	outline->setGlobalZOrder(6);
	addChild(outline);
}
//initializing pickup radius:
void Item::initRadius() {
	Size pickUpBox = getContentSize() * 1.7;
	pickUpRadius = Node::create();
	pickUpRadius->setPositionNormalized(Vec2(0.5, 0.5));
	pickUpRadius->setName("item_radius");

	auto pickUpRadiusBody = PhysicsBody::createBox(pickUpBox);
	pickUpRadiusBody->setDynamic(false);
	pickUpRadiusBody->setCategoryBitmask(4);
	pickUpRadiusBody->setCollisionBitmask(3);
	pickUpRadiusBody->setContactTestBitmask(0xFFFFFFFF);
	pickUpRadiusBody->setTag(10000);
	pickUpRadiusBody->setName("item_radius");
	pickUpRadius->setPhysicsBody(pickUpRadiusBody);

	addChild(pickUpRadius);
}

//used when player picks up item
void Item::initHeldItem() {
	outline->setVisible(false);
	pickUpRadius->getPhysicsBody()->setEnabled(false);
	getPhysicsBody()->setEnabled(false);
	getPhysicsBody()->setDynamic(true);
	getPhysicsBody()->setGravityEnable(false);
	getPhysicsBody()->setCategoryBitmask(8);
	getPhysicsBody()->setCollisionBitmask(42);
	setName("held_item");
	getPhysicsBody()->setName("held_item");
	setPosition(Vec2(50, 32));
	setRotation(-45.0f);
	if (flippedX == true) {
		flipX();
	}
}
//used when player drops item
void Item::initDroppedItem(Vec2 pos, bool flip) {
	outline->setVisible(true);
	getPhysicsBody()->setCategoryBitmask(32);
	getPhysicsBody()->setCollisionBitmask(8);
	getPhysicsBody()->setEnabled(true);
	getPhysicsBody()->setGravityEnable(true);
	getPhysicsBody()->setDynamic(true);
	setName("item");
	getPhysicsBody()->setName("item");
	setPosition(pos);
	//setRotation(20);
	if (flip == true) {
		flipX();
		setAnchorPoint(Vec2(0, 0));
		setRotation(-getRotation());
	}
	pickUpRadius->getPhysicsBody()->setEnabled(true);
}

void Item::breakItem() {
	release();
	getParent()->removeChild(this, true);
}

void Item::used() {
	hp = 0;
}

void Item::hitWall() {
	didHitWall = true;
	getPhysicsBody()->setEnabled(false);
}

void Item::beginStab() {
	setPosition(Vec2(25, 45));
	setRotation(0);
	//auto prepare = MoveBy::create(5 FRAMES, Vec2(-12, 6));
	//runAction(prepare);
}

void Item::beginSwing() {
	setRotation(-135);
	setPosition(Vec2(10, 45));
	//auto prepare = MoveBy::create(10 FRAMES, Vec2(-16, 20));
	//auto rotate = RotateBy::create(10 FRAMES, -90);
	//runAction(Spawn::create(prepare,rotate));
}

void Item::stabSequence() {
	auto move = MoveBy::create(attackTime * 0.125, Vec2(25, 6));
	auto hold = MoveBy::create(attackTime * 0.75, Vec2(0, 0));
	auto moveback = MoveBy::create(attackTime * 0.125, Vec2(-25, -6));
	auto sequence = Sequence::create(move, hold, moveback, NULL);
	runAction(sequence);
}

void Item::swingSequence() {
	auto move = MoveBy::create(4 FRAMES, Vec2(20, -25));
	auto rotate = RotateBy::create(4 FRAMES, 135);

	auto hold = MoveBy::create(6 FRAMES, Vec2(0, 0));

	//auto moveback = MoveBy::create(6 FRAMES, Vec2(-10, 5));
	//auto rotateback = RotateBy::create(6 FRAMES, -135);
	//Spawn::create(moveback, rotateback)

	auto sequence = Sequence::create(Spawn::create(move, rotate), hold, NULL);
	runAction(sequence);
}

//Knife Class:
Knife::Knife()
{
	Item::Item();
	category = 32;
	collision = 8;
	hp = 2;
	dmg = 5;
	canBreakDoor = true;//temporary
	tag = 10100;//10100 - 10199 for knives
	attackType = STAB;
	startTime = 6 FRAMES;
	attackTime = 8 FRAMES;
	lagTime = 10 FRAMES;
}
Knife::~Knife(){
}
