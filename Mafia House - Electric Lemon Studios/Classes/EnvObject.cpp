#include "EnvObject.h"

HideObject::HideObject()
{
	//sprite properties
	zOrder = 3;
	scale = 1.0f;
	name = "hide_object";//used for collision detection
	//physics body properties
	tag = 20000;//each object will have a unique tag number
	dynamic = false;
	category = 16;
	collision = 1;
}
HideObject::~HideObject(){
}

void HideObject::initHideObject(string name) {
	GameObject::initObject();
	createOutline("objects/" + name + "_outline.png");
	outline->setColor(ccc3(255, 235, 50));//yellow
	outline->setVisible(false);
}

void HideObject::playerInRange() {
	if (hiding == false) {
		if (playerRange == true) {
			outline->setVisible(true);
			//outline->setColor(ccc3(255, 235, 50));//yellow
		}
		else {
			outline->setVisible(false);
			//outline->setColor(ccc3(155, 155, 155));//grey
		}
	}
	playerRange = false;
}

void HideObject::hide() {
	hiding = true;
	setOpacity(175);
	outline->setVisible(false);
}
void HideObject::unhide() {
	hiding = false;
	setOpacity(255);
	outline->setVisible(true);
}

//Physical Objects:
PhysObject::PhysObject(){
	//sprite properties
	zOrder = 6;
	scale = 1.0f;
	name = "phys_object";//used for collision detection
	//physics body properties
	tag = 50000;//each object will have a unique tag number
	dynamic = false;
	category = 0xFFFFFFFF;
	collision = 41;
}

void PhysObject::initObject(){
	initObjectNoPhysics();
	initBoxBody(surfaceSize);
	getPhysicsBody()->setPositionOffset(surfaceOffset);
	if (hasHideBox == true) {
		initHideBox();
		initHideBox2();
	}
}

void PhysObject::initHideBox() {
	hideBox = Node::create();
	hideBox->setName("hide_radius");
	auto body = PhysicsBody::createBox(hideBoxSize);
	body->setDynamic(false);
	body->setCategoryBitmask(4);
	body->setCollisionBitmask(9);
	body->setContactTestBitmask(0xFFFFFFFF);
	hideBox->setPhysicsBody(body);
	hideBox->setPosition(hideBoxPosition);
	addChild(hideBox);
}

void PhysObject::initHideBox2() {
	hideBox2 = Node::create();
	hideBox2->setName("hide_radius");
	auto body = PhysicsBody::createBox(hideBoxSize);
	body->setDynamic(false);
	body->setCategoryBitmask(4);
	body->setCollisionBitmask(9);
	body->setContactTestBitmask(0xFFFFFFFF);
	hideBox2->setPhysicsBody(body);
	hideBox2->setPosition(hideBoxPosition);
	addChild(hideBox2);
}

//Table:
Table::Table(){
	PhysObject::PhysObject();
	surfaceSize = Size(140, 10);
	surfaceOffset = Vec2(0, 24);
	hideBoxSize = Size(86, 38);
	hideBoxPosition = Vec2(70, 20);
	surfaceHeight = 55;
}
//Kitchen Table:
KitchenTable::KitchenTable() {
	PhysObject::PhysObject();
	surfaceSize = Size(143, 10);
	surfaceOffset = Vec2(0, 10);
	hideBoxSize = Size(120, 38);
	hideBoxPosition = Vec2(92.5, 20);
	surfaceHeight = 55;
}

//Fountain:
Fountain::Fountain() {
	PhysObject::PhysObject();
	surfaceSize = Size(140, 10);
	surfaceOffset = Vec2(0, 24);
	hideBoxSize = Size(86, 38);
	hideBoxPosition = Vec2(70, 20);
	surfaceHeight = 55;
}

//Wall Shelf:
WallShelf::WallShelf() {
	PhysObject::PhysObject();
	surfaceSize = Size(120, 15);
	surfaceOffset = Vec2(0, -5);
	hideBoxSize = Size(70, 20);
	hideBoxPosition = Vec2(60, 18);
	surfaceHeight = 10;
}

//Vent Cover:
VentCover::VentCover() {
	PhysObject::PhysObject();
	hasHideBox = false;
	surfaceSize = Size(40, 5);
	surfaceOffset = Vec2(0, 0);
	surfaceHeight = 5;
}
