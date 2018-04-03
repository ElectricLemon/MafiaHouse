#include "Player.h"
#include "Wall.h"

Player::Player()
{
	FRAME_OFFSET = 22;
	bodySize = Size(26, 90);
	standSize = bodySize;
	crouchSize = Size(30, 35);
	//sprite properties
	zOrder = 5;
	scale = 1.0f;
	name = "player";
	//physics body properties
	tag = 1;
	dynamic = true;
	category = 1;
	collision = 30;
	//max movement speed
	baseSpeed = 70;
	//initializing animations
	stand = GameAnimation(STAND, "player/stand/%03d.png", 24, 10.5 FRAMES, true);
	moonwalk = GameAnimation(MOONWALK, "player/walk_moon/%03d.png", 7, 8 FRAMES, true);
	walking = GameAnimation(WALK, "player/walk/%03d.png", 6, 10 FRAMES, true);
	throwing = GameAnimation(THROW, "player/throw/%03d.png", 3, 5 FRAMES, false);
	stab = GameAnimation(STAB, "player/stab/%03d.png", 2, 10 FRAMES, false);
	swing = GameAnimation(SWING, "player/swing/%03d.png", 4, 2 FRAMES, false);
	crouchstab = GameAnimation(STAB, "player/crouch_stab/%03d.png", 2, 10 FRAMES, false);
	crouchswing = GameAnimation(SWING, "player/crouch_swing/%03d.png", 2, 6 FRAMES, false);
	fallstab = GameAnimation(FALLATK, "player/fall_stab/%03d.png", 1, 10 FRAMES, false);
	fallswing = GameAnimation(FALLATK, "player/fall_swing/%03d.png", 1, 10 FRAMES, false);
	crouchwalk = GameAnimation(CROUCHWALK, "player/crouch_walk/%03d.png", 4, 14 FRAMES, true);
	climbing = GameAnimation(CLIMB, "player/climb/%03d.png", 5, 10 FRAMES, false);
	jumping = GameAnimation(JUMP, "player/jump/%03d.png", 2, 10 FRAMES, false);
	falling = GameAnimation(FALL, "player/fall/%03d.png", 1, 10 FRAMES, true);
	rolling = GameAnimation(ROLL, "player/roll/%03d.png", 5, 6 FRAMES, false);
	dying = GameAnimation(DEATH, "player/roll/%03d.png", 5, 6 FRAMES, false);
}
Player::~Player(){}

void Player::initObject(Vec2 startPos) {
	Character::initObject(startPos);
	//initializing crouching physics body
	crouchBody = PhysicsBody::createBox(crouchSize);//player is half height when crouching
	crouchBody->setContactTestBitmask(0xFFFFFFFF);
	crouchBody->setTag(1);
	crouchBody->setName("player");
	crouchBody->setCategoryBitmask(1);
	crouchBody->setCollisionBitmask(30);
	crouchBody->setDynamic(true);
	crouchBody->setRotationEnable(false);
	crouchBody->retain();
	//intializing hitbox for item pickup radius
	auto body = PhysicsBody::createBox(Size(bodySize.width, 54));//player is half height when crouching
	body->setContactTestBitmask(0xFFFFFFFF);
	body->setCategoryBitmask(64);
	body->setCollisionBitmask(4);
	body->setDynamic(false);
	pickUpRadius = Node::create();
	pickUpRadius->setPhysicsBody(body);
	pickUpRadius->setName("player_pickup");
	pickUpRadius->setPosition(Vec2(35, 64));
	addChild(pickUpRadius);
	//intializing aim indicator
	Texture2D::TexParams texParams = { GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE };
	aimMarker = Sprite::createWithSpriteFrameName("icons/aim.png");
	aimMarker->setAnchorPoint(Vec2(-2.2, 0.5));
	aimMarker->setPositionNormalized(Vec2(0.5, 0.5));
	aimMarker->getTexture()->setTexParameters(texParams);
	aimMarker->setGlobalZOrder(10);
	aimMarker->setVisible(false);
	addChild(aimMarker);
}

void Player::flipX() {
	Character::flipX();
	if (heldBody != NULL) {
		heldBody->knockback *= -1;
	}
}

//functions for player actions:
void Player::resetCollisionChecks(float time) {
	isHidingUnder = false;
	touchingFloor = false;
	touchingWall = false;
	objectToClimb = NULL;
	isHit = false;
	doorToUse = NULL;
	stairToUse = NULL;
	objectToHideBehind = NULL;
	itemToPickUp = NULL;
	bodyToPickUp = NULL;
	if (time - seenTime >= visionResetTime || seenTime == -1) {
		inVision = false;
	}
	//bodyToPickUp = NULL;
}

void Player::walkPrepareAttack(Input input, float time) {
	if (input == MOVE_LEFT) {
		if (moveDirection == 0) {
			walkingSound = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/walk.wav", true);
			moveDirection = 1;
			if (isCrouched == false) {
				//startAnimation(WALK, walking);
			}
			else {
				//startAnimation(CROUCHWALK, crouchwalk);
			}
			setSpeed(moveSpeed);
		}
		walking.action->setSpeed(moveSpeed);
		moveAbsolute(Vec2(-7.0f * moveSpeed, 0));
	}
	if (input == MOVE_RIGHT) {
		if (moveDirection == 0) {
			walkingSound = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/walk.wav", true);
			moveDirection = 2;
			if (isCrouched == false) {
				//startAnimation(WALK, walking);
			}
			else {
				//startAnimation(CROUCHWALK, crouchwalk);
			}
			setSpeed(moveSpeed);
		}
		walking.action->setSpeed(moveSpeed);
		moveAbsolute(Vec2(7.0f * moveSpeed, 0));
	}
	if (input == STOP) {
		stopX();
		CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(walkingSound);
		moveDirection = 0;
		//stopAnimation(WALK);
		//stopAnimation(CROUCHWALK);
		if (isCrouched == false) {
			setSpriteFrame(stand.animation->getFrames().at(0)->getSpriteFrame());//run standing animation here
		}
		else {
			setSpriteFrame(crouchwalk.animation->getFrames().at(0)->getSpriteFrame());//run standing animation here
		}
	}
}

void Player::walk(Input input, float time) {
	if (input == MOVE_LEFT) {
		if (moveDirection == 0){
			walkingSound = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/walk.wav", true);
			moveDirection = 1;
			if (turned == false) {
				turned = true;
				flipX();
			}
			stopAnimation(STAND);
			stopAnimation(MOONWALK);
			startAnimation(WALK, walking);
			setSpeed(moveSpeed);
		}
		if (moveDirection == 1) {
			if (getActionByTag(MOONWALK) == NULL) {
				stopAnimation(STAND);
				startAnimation(WALK, walking);
			}
			walking.action->setSpeed(moveSpeed);
			moveAbsolute(Vec2(-9.0f * moveSpeed, 0));
			//run walking animation
		}
		else if (moveDirection == 2) {
			stopAnimation(STAND);
			stopAnimation(WALK);
			startAnimation(MOONWALK, moonwalk);
			setSpeed(moveSpeed * 1.4f);
			moonwalk.action->setSpeed(moveSpeed);
			if (turned == false) {
				turned = true;
				flipX();
				moonwalking = true;
			}
		}
	}
	if (input == MOVE_RIGHT) {
		if (moveDirection == 0) {
			walkingSound = CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/walk.wav", true);
			moveDirection = 2;
			if (turned == true) {
				turned = false;
				flipX();
			}
			stopAnimation(STAND);
			stopAnimation(MOONWALK);
			startAnimation(WALK, walking);
			setSpeed(moveSpeed);
		}
		if (moveDirection == 2) {
			if (getActionByTag(MOONWALK) == NULL) {
				stopAnimation(STAND);
				startAnimation(WALK, walking);
			}
			walking.action->setSpeed(moveSpeed);
			moveAbsolute(Vec2(9.0f * moveSpeed, 0));
			//run walking animation
		}
		else if (moveDirection == 1) {
			stopAnimation(STAND);
			stopAnimation(WALK);
			startAnimation(MOONWALK, moonwalk);
			setSpeed(moveSpeed * 1.4f);
			moonwalk.action->setSpeed(moveSpeed);
			if (turned == true) {
				turned = false;
				flipX();
				moonwalking = true;
			}
		}
	}
	if (input == STOP) {
		if (time - prevStopTime >= stopDelay || prevStopTime == -1) {
			prevStopTime = time;
			if (moonwalking == false) {
				stopX();
			}
			moonwalking = false;
		}
		CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(walkingSound);
		moveDirection = 0;
		stopAnimation(WALK);
		stopAnimation(MOONWALK);
		startAnimation(STAND, stand);
	}
}

void Player::crouchWalk(Input input, float time) {
	if (input == MOVE_LEFT) {
		if (moveDirection == 0) {
			moveDirection = 1;
			if (turned == false) {
				turned = true;
				flipX();
			}
			startAnimation(CROUCHWALK, crouchwalk);
			setSpeed(moveSpeed);
		}
		if (moveDirection == 1) {
			startAnimation(CROUCHWALK, crouchwalk);
			walking.action->setSpeed(moveSpeed);
			moveAbsolute(Vec2(-9.0f * moveSpeed, 0));
		}
		else if (moveDirection == 2) {
			setSpeed(moveSpeed * 1.4f);
			if (turned == false) {
				turned = true;
				flipX();
				moonwalking = true;
			}
		}
	}
	if (input == MOVE_RIGHT) {
		if (moveDirection == 0) {
			moveDirection = 2;
			if (turned == true) {
				turned = false;
				flipX();
			}
			startAnimation(CROUCHWALK, crouchwalk);
			setSpeed(moveSpeed);
		}
		if (moveDirection == 2) {
			startAnimation(CROUCHWALK, crouchwalk);
			walking.action->setSpeed(moveSpeed);
			moveAbsolute(Vec2(9.0f * moveSpeed, 0));
			//run walking animation
		}
		else if (moveDirection == 1) {
			setSpeed(moveSpeed * 1.4f);
			moonwalk.action->setSpeed(moveSpeed);
			if (turned == true) {
				turned = false;
				flipX();
				moonwalking = true;
			}
		}
	}
	if (input == STOP) {
		if (time - prevStopTime >= stopDelay || prevStopTime == -1) {
			prevStopTime = time;
			if (moonwalking == false) {
				stopX();
			}
			moonwalking = false;
		}
		moveDirection = 0;
		stopAnimation(CROUCHWALK);
		setSpriteFrame(frameCache->getSpriteFrameByName("player/crouch_walk/crouch.png"));
	}
}

void Player::jump() {
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/jump.wav");
	if (touchingFloor == true) {
		auto callback = CallFunc::create([this]() {
			hasJumped = true;
			move(Vec2(0, 120));//apply force straight up
		});
		auto wait = MoveBy::create(10 FRAMES, Vec2(0, 0));
		auto sequence = Sequence::create(wait, callback, nullptr);//runs the stair use animation and then has character take the stairs
	
		runAction(sequence);
	}
}

void Player::pickUpBody(GameLayer* mainLayer) {
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/grabbody.wav");
	if (bodyToPickUp != NULL) {
		if (heldItem != NULL) {
			heldItem->setVisible(false);
		}
		bodyToPickUp->removeFromParent();
		heldBody = bodyToPickUp;

		addChild(heldBody);
		if (isCrouched == false) {
			heldBody->initPickedUpBody();
		}
		else {
			heldBody->initCrouchPickedUpBody();
		}
		if (flippedX == true) {
			heldBody->knockback *= -1;
		}

		bodyToPickUp = NULL;
	}
}

void Player::dropBody(GameLayer* mainLayer) {
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/dropbody.wav");
	if (heldBody != NULL) {
		if (heldItem != NULL) {
			heldItem->setVisible(true);
		}

		heldBody->initDroppedBody(convertToWorldSpace(heldBody->getPosition()), flippedX);

		removeChild(heldBody, true);
		mainLayer->addChild(heldBody);
		heldBody = NULL;
	}
}

void Player::beginThrowBody(float time) {
	if (heldBody != NULL) {
		if (isCrouched == false) {
			heldBody->prepareThrow();
		}
		else {
			heldBody->prepareCrouchThrow();
		}
		aimMarker->setVisible(true);
		float percentage = (time - attackPrepareTime) / (heldBody->getStartTime() * 2);
		percentage = percentage > 1 ? 1 : percentage;
		aimMarker->setScaleY(percentage);
		aimMarker->setRotation(aimAngle);
	}
}

void Player::throwBody(GameLayer* mainLayer, float time) {
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/throw.wav");
	if (heldBody != NULL) {
		heldBody->thrownTime = time;
		heldBody->throwItem(aimAngle, convertToWorldSpace(heldBody->getPosition()), flippedX);
		removeChild(heldBody, true);
		mainLayer->addChild(heldBody);
		heldBody->thrownTime = time;
		thrownItem = heldBody;
		heldBody = NULL;
		if (heldItem != NULL) {
			heldItem->setVisible(true);
		}
		aimMarker->setVisible(false);
	}
}

void Player::pickUpItem(GameLayer* mainLayer) {
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/equip.wav");
	Character::pickUpItem(mainLayer);
	if (isCrouched == true) {
		if (heldItem != NULL) {
			heldItem->initCrouchHeldItem();
		}
	}
	if (heldItem != NULL) {
		if (heldItem->enemyItem == true) {
			heldItem->enemyItem = false;//item was stolen from enemy
		}
	}
}

void Player::dropItem(GameLayer* mainLayer) {
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/unequip.wav");
	Character::dropItem(mainLayer);
}

void Player::beginThrowItem(float time) {
	if (heldItem != NULL) {
		if (isCrouched == false) {
			heldItem->prepareThrow(aimAngle);
			setSpriteFrame(frameCache->getSpriteFrameByName("player/throw/start.png"));
		}
		else {
			heldItem->prepareCrouchThrow(aimAngle);
			setSpriteFrame(frameCache->getSpriteFrameByName("player/crouch_swing/start.png"));
		}
		aimMarker->setVisible(true);
		float percentage = (time - attackPrepareTime) / (heldItem->getStartTime() * 2);
		percentage = percentage > 1 ? 1 : percentage;
		aimMarker->setScaleY(percentage);
		aimMarker->setRotation(aimAngle);
	}
}

void Player::throwItem(GameLayer* mainLayer, float time) {
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/throw.wav");
	Character::throwItem(mainLayer, time);
	aimMarker->setVisible(false);
	if (isCrouched == false){
		startAnimation(THROW, throwing);
	}
	else {
		startAnimation(SWING, crouchswing);
	}
}

void Player::beginUseItem(float angle) {
	if (heldItem != NULL) {
		if (isCrouched == false) {
			if (heldItem->getAttackType() == Item::STAB) {
				heldItem->prepareStab(angle);
				setSpriteFrame(stab.animation->getFrames().at(0)->getSpriteFrame());//setting player sprite to first frame of stab animation
			}
			else if (heldItem->getAttackType() == Item::SWING) {
				heldItem->prepareSwing(angle);
				setSpriteFrame(frameCache->getSpriteFrameByName("player/swing/start.png"));
			}
			else if (heldItem->getAttackType() == Item::SHOOT) {
				heldItem->prepareShoot(angle);
				setSpriteFrame(stab.animation->getFrames().at(1)->getSpriteFrame());//first frame of shooting animation
			}
		}
		else {
			if (heldItem->getAttackType() == Item::STAB) {
				heldItem->prepareCrouchStab(angle);
				setSpriteFrame(crouchstab.animation->getFrames().at(0)->getSpriteFrame());//setting player sprite to first frame of stab animation
			}
			else if (heldItem->getAttackType() == Item::SWING) {
				heldItem->prepareCrouchSwing(angle);
				setSpriteFrame(frameCache->getSpriteFrameByName("player/crouch_swing/start.png"));
			}
			else if (heldItem->getAttackType() == Item::SHOOT) {
				heldItem->prepareCrouchShoot(angle);
				setSpriteFrame(crouchstab.animation->getFrames().at(1)->getSpriteFrame());//first frame of shooting animation
			}
		}
	}
}

void Player::useItem(float angle) {
	if (heldItem != NULL) {
		if (isCrouched == false) {
			heldItem->getPhysicsBody()->setEnabled(true);
			if (heldItem->getAttackType() == Item::STAB) {
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/stab.wav");
				heldItem->stabSequence(angle, flippedX);
				setSpriteFrame(stab.animation->getFrames().at(1)->getSpriteFrame());
			}
			else if (heldItem->getAttackType() == Item::SWING) {
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/swing.wav");
				heldItem->swingSequence(angle, flippedX);
				startAnimation(SWING, swing);
			}
			else if (heldItem->getAttackType() == Item::SHOOT) {
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/gun_shoot.wav");
				heldItem->playerShoot(angle);
			}
		}
		else {
			heldItem->getPhysicsBody()->setEnabled(true);
			if (heldItem->getAttackType() == Item::STAB) {
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/stab.wav");
				heldItem->stabSequence(angle, flippedX);
				setSpriteFrame(crouchstab.animation->getFrames().at(1)->getSpriteFrame());
			}
			else if (heldItem->getAttackType() == Item::SWING) {
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/swing.wav");
				heldItem->swingSequence(angle, flippedX);
				startAnimation(SWING, crouchswing);
			}
			else if (heldItem->getAttackType() == Item::SHOOT) {
				CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/gun_shoot.wav");
				heldItem->playerShoot(angle);
			}
		}
	}
}

void Player::finishUseItem() {
	if (isCrouched == false) {
		if (heldItem->getAttackType() == Item::STAB) {
			setSpriteFrame(stab.animation->getFrames().at(0)->getSpriteFrame());
		}
		else if (heldItem->getAttackType() == Item::SWING) {
			setSpriteFrame(frameCache->getSpriteFrameByName("player/swing/end.png"));
		}
	}
	else {
		if (heldItem->getAttackType() == Item::STAB) {
			setSpriteFrame(crouchstab.animation->getFrames().at(0)->getSpriteFrame());//setting player sprite to first frame of stab animation
		}
		else if (heldItem->getAttackType() == Item::SWING) {
			//setSpriteFrame(crouchswing.animation->getFrames().at(0)->getSpriteFrame());//first frame of the swing animation
		}
	}
	heldItem->getPhysicsBody()->setEnabled(false);
}

void Player::fallAttack() {
	heldItem->fallAttack();
	if (heldItem->getAttackType() == Item::STAB) {
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/stab.wav");
		setSpriteFrame(fallstab.animation->getFrames().at(0)->getSpriteFrame());//setting player sprite to first frame of stab animation
	}
	else if (heldItem->getAttackType() == Item::SWING) {
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/swing.wav");
		setSpriteFrame(fallswing.animation->getFrames().at(0)->getSpriteFrame());//setting player sprite to first frame of stab animation
	}
}

void Player::useDoor() {
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/openDoor.wav");
	Character::useDoor();
}

void Player::useStair(GameLayer* mainLayer) {
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/stairs.wav");
	Character::useStair(mainLayer);
}

void Player::hide() {
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Audio/hide.wav");
	if (hidden == false) {
		wasSeen = false;
		if (inVision == true) {//if player was in enemy vision upon entering hiding
			wasSeen = true;
		}
		hidden = true;
		setOpacity(200);
		setTag(getTag() + 10);//for enemy vision rays
		setGlobalZOrder(2);
		if (heldItem != NULL) {
			heldItem->setGlobalZOrder(2);
		}
		if (heldBody != NULL) {
			heldBody->isHidden = true;
			heldBody->setGlobalZOrder(2);
			heldBody->setOpacity(200);
			heldBody->outline->setGlobalZOrder(2);
			heldBody->outline->setOpacity(200);
		}
		hideObject->hide();
	}
	else {
		hidden = false;
		wasSeen = false;
		setOpacity(255);
		setTag(getTag() - 10);//for enemy vision rays
		setGlobalZOrder(5);
		if (heldItem != NULL) {
			heldItem->setGlobalZOrder(6);
		}
		if (heldBody != NULL) {
			heldBody->isHidden = false;
			heldBody->setGlobalZOrder(5);
			heldBody->setOpacity(255);
			heldBody->outline->setGlobalZOrder(5);
			heldBody->outline->setOpacity(255);
		}
		hideObject->unhide();
	}
}

void Player::stayWithin(HideObject* object) {
	Vec2 range = Vec2(object->getContentSize().width / 2, object->getContentSize().height / 2);
	Vec2 displacement = (getPosition() + Vec2(getSize().width / 2, getSize().height / 2)) - (object->getPosition() + Vec2(object->getContentSize().width / 2, object->getContentSize().height / 2));
	if (displacement.x > range.x - getSize().width / 2) {
		if (hittingRight == true) {
			setSpeed(0.3);
			moveAbsolute(Vec2(-3, 0));
		}
		else {
			hittingRight = true;
			stop();
			moveAbsolute(Vec2(-3, 0));
		}
	}
	else {
		if (hittingLeft == false) {
			setSpeed(1.0);
		}
		hittingRight = false;
	}
	if (displacement.x < -(range.x - getSize().width / 2)) {
		if (hittingLeft == true) {
			setSpeed(0.3);
			moveAbsolute(Vec2(3, 0));
		}
		else {
			hittingLeft = true;
			stop();
			moveAbsolute(Vec2(3, 0));
		}
	}
	else {
		if (hittingRight == false) {
			setSpeed(1.0);
		}
		hittingLeft = false;
	}
}
//have player stay behind object they are hiding behind
void Player::hiding() {
	stayWithin(hideObject);
}

void Player::hearNoise(string name) {
	//CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(("Audio/" + name + ".wav").c_str());
}

void Player::wasHit(Item* item, float time) {
	if (item->didHitWall == false) {
		auto emitter = ParticleFireworks::create();
		emitter->setStartColor(Color4F(255, 0, 0, 1));
		emitter->setEndColor(Color4F(200, 0, 0, 1));//red
		emitter->setDuration(0.1f);
		emitter->setStartSize(4.0f);
		emitter->setStartSizeVar(1.0f);
		emitter->setEndSize(0.5f);
		emitter->setAngleVar(55);
		emitter->setGravity(Vec2(0, -300));
		emitter->setSpeed(75);
		emitter->setSpeedVar(200.0f);
		emitter->setLife(0.05f);
		emitter->setLifeVar(1.5f);
		emitter->setTextureWithRect(frameCache->getSpriteFrameByName("particles/pixel.png")->getTexture(), frameCache->getSpriteFrameByName("particles/pixel.png")->getRect());
		emitter->setGlobalZOrder(30);
		emitter->setPosition(getPosition() + getSize() / 2);
		director->getRunningScene()->addChild(emitter);
		stopAllActions();
		wasInHitStun = true;
		pauseSchedulerAndActions();
		hitStunStart = time;
		hitStunTime = item->hitstun;
		//item->used();//enemy items don't break
		isHit = true;
		if (item->getAttackType() == Item::SHOOT) {
			if (item->wasShot == true) {
				hp -= 75;
			}
			else {
				hp -= 25;
			}
		}
		else {
			hp -= item->dmg;//taking damage from attack
		}
		hp = hp < 0 ? 0 : hp;
		if (touchingWall == false && touchingFloor == true) {
			stop();
			moveAbsoluteNoLimit(item->knockback);
		}
	}
}

//Update Checking:
void Player::update(GameLayer* mainLayer, float time) {
	updateFloor(mainLayer->floors);//checking if floor has changed
	updateRoom(mainLayer->floors[currentFloor].rooms);//checking if room has changed
	if (itemHitBy != NULL) {
		if (itemHitBy->getState() == Item::THROWN || itemHitBy->getState() == Item::FALLING){
			if (itemHitBy == thrownItem && (time - itemHitBy->thrownTime >= thrownItemDelay)) {
				wasHit(itemHitBy, time);
				thrownItem = NULL;
			}
			else if (itemHitBy != thrownItem){
				wasHit(itemHitBy, time);
			}
		}
		else {
			wasHit(itemHitBy, time);
		}
		itemHitBy = NULL;
	}
	if (isHidingUnder == true) {
		if (inVision == true) {//if player was in enemy vision upon entering hiding
			wasSeen = true;
		}
		setOpacity(200);
		hidden = true;
	}
	else{//if they are no longer under object
		setOpacity(255);
		if (state->type != "hide") {//and not in a hide state
			hidden = false;
			wasSeen = false;
		}
	}
	newState = state->update(this, mainLayer, time);
	if (newState != NULL)
	{
		state->exit(this, mainLayer, time);

		if (prevState != NULL && newState != prevState) { delete prevState; }
		prevState = state;
		state = newState;
		newState = NULL;

		state->enter(this, mainLayer, time);
	}
}

//Input Handling:
void Player::handleInput(GameLayer* mainLayer, float time, Input input) {
	if (time - hitStunStart >= hitStunTime || hitStunStart == -1) {//only allow player input if hitstun is over, of if histun never began
		resumeSchedulerAndActions();
		hitStunStart = -1;
		newState = state->handleInput(this, mainLayer, time, input);
		if (newState != NULL)
		{
			state->exit(this, mainLayer, time);

			if (prevState != NULL && newState != prevState) { delete prevState; }
			prevState = state;
			state = newState;
			newState = NULL;

			state->enter(this, mainLayer, time);
		}
		wasInHitStun = false;
	}
}

//Player States:
void Player::State::enter(Player* player, GameLayer* mainLayer, float time) {}
Player::State* Player::State::update(Player* player, GameLayer* mainLayer, float time) {return nullptr;}
Player::State* Player::State::handleInput(Player* player, GameLayer* mainLayer, float time, Input input) {return nullptr;}
void Player::State::exit(Player* player, GameLayer* mainLayer, float time) {}

//Neutral State:
void Player::NeutralState::enter(Player* player, GameLayer* mainLayer, float time) {
	player->isCrouched = false;
	player->stopAllActions();
	player->startAnimation(STAND, player->stand);
	if (player->prevState->type != "attack" && player->prevState->type != "throw" && player->prevState->type != "hide") {
		player->mainBody->setVelocity(player->getPhysicsBody()->getVelocity());
		player->stop();
		player->setPhysicsBody(player->mainBody);
		player->bodySize = player->standSize;
		player->getPhysicsBody()->setPositionOffset(Vec2(0, 17));
		player->pickUpRadius->setPosition(Vec2(35, 64));
		if (player->heldItem != NULL) {
			player->heldItem->initHeldItem();
		}
		if (player->heldBody != NULL) {
			player->heldBody->initHeldBody();
		}
		//player->startAnimation(STANDUP, player->standup);
	}
	player->moveSpeed = 1.0f;
	player->setSpeed(player->moveSpeed);
}
Player::State* Player::NeutralState::update(Player* player, GameLayer* mainLayer, float time) {
	if (player->checkDead() == true) { return new DeathState; }
	if (player->touchingFloor == false && player->getPhysicsBody()->getVelocity().y < -50) {
		return new FallState;
	}
	return nullptr;
}
Player::State* Player::NeutralState::handleInput(Player* player, GameLayer* mainLayer, float time, Input input) {
	if (input == USE_DOOR) {
		player->useDoor();
	}
	if (input == USE_STAIR) {
		player->useStair(mainLayer);
	}
	if (input == DROP) {
		if (player->heldBody == NULL) {
			player->dropItem(mainLayer);
		}
		else {
			player->dropBody(mainLayer);
		}
	}
	if (input == USE_ITEM) {
		if (player->heldBody == NULL) {
			return new AttackState;
		}
	}
	if (input == THROW_ITEM) {
		return new ThrowState;
	}
	if (input == PICKUP) {
		if (player->heldBody == NULL) {
			if (player->itemToPickUp != NULL && player->heldItem == NULL) {
				player->pickUpItem(mainLayer);
			}
			else if (player->bodyToPickUp != NULL){
				player->pickUpBody(mainLayer);
			}
		}
	}
	if (input == HIDE) {
		player->hidden = false;
		return new HideState;
	}
	if (input == MOVE_LEFT || input == MOVE_RIGHT || input == STOP) {
		if (player->wasFalling == true) {
			player->wasFalling = false;
			player->stop();
			player->moveDirection = 0;
		}
		player->walk(input, time);
	}
	if (input == MOVE_UP) {
		if (player->objectToClimb == NULL && player->touchingFloor == true) {//only jump if not colliding with a physical object
			if (player->heldBody == NULL) {
				return new JumpState;
			}
		}
		else if (player->objectToClimb != NULL && player->heldBody == NULL){//player has an object to climb
			player->climbObject = player->objectToClimb;
			return new ClimbState;
		}
	}
	if (input == MOVE_DOWN) {
		if (player->touchingFloor == true) {
			return new CrouchState;
		}
	}
	if (input == ROLL) {
		return new RollState;
	}
	if (input == NO_CLIP) {
		return new NoClipState;
	}
	return nullptr;
}
void Player::NeutralState::exit(Player* player, GameLayer* mainLayer, float time) {
	CocosDenshion::SimpleAudioEngine::getInstance()->stopEffect(player->walkingSound);
}

//Crouching State:
void Player::CrouchState::enter(Player* player, GameLayer* mainLayer, float time) {
	player->isCrouched = true;
	player->stopAllActions();
	if (player->prevState->type != "attack" && player->prevState->type != "throw" && player->prevState->type != "hide") {
		player->crouchBody->setVelocity(player->getPhysicsBody()->getVelocity());
		player->stop();
		player->setPhysicsBody(player->crouchBody);
		player->bodySize = player->crouchSize;
		if (player->prevState->type == "fall") {
			player->getPhysicsBody()->setPositionOffset(Vec2(0, -35));
		}
		else {
			player->getPhysicsBody()->setPositionOffset(Vec2(0, -27.5));
		}
		player->pickUpRadius->setPosition(Vec2(35, 15));
		if (player->heldItem != NULL) {
			player->heldItem->initCrouchHeldItem();
		}
		if (player->heldBody != NULL) {
			player->heldBody->initCrouchHeldBody();
		}
		//player->startAnimation(CROUCH, player->crouch);
	}
	player->setSpriteFrame(player->frameCache->getSpriteFrameByName("player/crouch_walk/crouch.png"));
	player->moveSpeed = 0.5f;
	player->setSpeed(player->moveSpeed);
}
Player::State* Player::CrouchState::update(Player* player, GameLayer* mainLayer, float time) {
	if (player->checkDead() == true) { return new DeathState; }
	if (player->getPhysicsBody()->getVelocity().y < -50 && player->touchingFloor == false) {
		return new FallState;
	}
	return nullptr;
}
Player::State* Player::CrouchState::handleInput(Player* player, GameLayer* mainLayer, float time, Input input) {
	if (input == USE_DOOR) {
		player->useDoor();
	}
	if (input == USE_STAIR) {
		player->useStair(mainLayer);
	}
	if (input == DROP) {
		if (player->heldBody == NULL) {
			player->dropItem(mainLayer);
		}
		else {
			player->dropBody(mainLayer);
		}
	}
	if (input == USE_ITEM) {
		if (player->heldBody == NULL) {
			return new AttackState;
		}
	}
	if (input == THROW_ITEM) {
		return new ThrowState;
	}
	if (input == PICKUP) {
		if (player->heldBody == NULL) {
			if (player->itemToPickUp != NULL && player->heldItem == NULL) {
				player->pickUpItem(mainLayer);
			}
			else if (player->bodyToPickUp != NULL) {
				player->pickUpBody(mainLayer);
			}
		}
	}
	if (input == HIDE) {
		player->hidden = false;
		return new HideState;
	}
	if (input == MOVE_LEFT || input == MOVE_RIGHT || input == STOP) {
		if (player->wasFalling == true) {
			player->wasFalling = false;
			player->stop();
			player->moveDirection = 0;
		}
		if (player->wasClimbing == true) {
			player->wasClimbing = false;
			player->stop();
			player->moveDirection = 0;
		}
		player->crouchWalk(input, time);
	}
	if (input == MOVE_UP) {
		return new NeutralState;
	}
	if (input == ROLL) {
		return new RollState;
	}
	return nullptr;
}

//Jump State:
void Player::JumpState::enter(Player* player, GameLayer* mainLayer, float time) {
	player->stopAllActions();
	player->jump();
	player->startAnimation(JUMP, player->jumping);
}
Player::State* Player::JumpState::update(Player* player, GameLayer* mainLayer, float time) {
	if (player->checkDead() == true) { return new DeathState; }
	if (player->wasInHitStun == true) {
		return new NeutralState;
	}
	if (player->hasJumped == true && player ->getPhysicsBody()->getVelocity().y <= 0) {//when player's vertical speed has stopped
		return new FallState;
	}
	return nullptr;
}
Player::State* Player::JumpState::handleInput(Player* player, GameLayer* mainLayer, float time, Input input) {
	if (input == DROP) {
		player->dropItem(mainLayer);
	}
	if (input == PICKUP) {
		player->pickUpItem(mainLayer);
	}
	if (input == MOVE_UP) {
		if (player->objectToClimb != NULL) {//player has an object to climb
			player->climbObject = player->objectToClimb;
			return new ClimbState;
		}
	}
	return nullptr;
}
void Player::JumpState::exit(Player* player, GameLayer* mainLayer, float time) {
	player->hasJumped = false;
}

//Fall State:
void Player::FallState::enter(Player* player, GameLayer* mainLayer, float time) {
	player->wasFalling = true;
	player->stopAllActions();
	player->startAnimation(FALL, player->falling);
	if (player->prevState->type == "crouch" || player->prevState->type == "roll") {
		player->mainBody->setVelocity(player->getPhysicsBody()->getVelocity());
		player->stop();
		player->setPhysicsBody(player->mainBody);
		player->bodySize = player->standSize;
		player->getPhysicsBody()->setPositionOffset(Vec2(0, 17));
		player->pickUpRadius->setPosition(Vec2(35, 64));
		if (player->heldItem != NULL) {
			player->heldItem->initHeldItem();
		}
		if (player->heldBody != NULL) {
			player->heldBody->initHeldBody();
		}
		//player->setPositionY(player->getPositionY() - 35);
	}
}
Player::State* Player::FallState::update(Player* player, GameLayer* mainLayer, float time) {
	if (player->checkDead() == true) { return new DeathState; }
	//if (player->touchingCeiling == true) {
	//	player->move(Vec2(0, -300));
	//}
	if (player->touchingFloor == true && (player->getPhysicsBody()->getVelocity().y > -20 && player->getPhysicsBody()->getVelocity().y < 20)){//if player has hit the ground
		//player->startAnimation(LAND, landing);
		player->setPhysicsBody(player->crouchBody);
		player->bodySize = player->crouchSize;
		player->getPhysicsBody()->setPositionOffset(Vec2(0, -28));
		player->createNoise(25, 0.5, time, player->getPosition() + Vec2(player->getSize().width / 2, 0), Vec2(player->currentFloor, player->currentRoom), "player_land", &mainLayer->noises);
		return new CrouchState;
	}
	//if (player->touchingFloor == true && player->getActionByTag(LAND) == NULL) {//player's landing animation has finished
	//	return new CrouchState;
	//}
	return nullptr;
}
Player::State* Player::FallState::handleInput(Player* player, GameLayer* mainLayer, float time, Input input) {
	if (input == DROP) {
		player->dropItem(mainLayer);
	}
	if (input == USE_ITEM) {
		if (player->getPhysicsBody()->getVelocity().y < -49) {
			return new AttackState;
		}
	}
	if (input == PICKUP) {
		if (player->heldBody == NULL) {
			player->pickUpItem(mainLayer);
		}
	}
	if (input == MOVE_UP) {
		if (player->objectToClimb != NULL) {//player has an object to climb
			player->climbObject = player->objectToClimb;
			return new ClimbState;
		}
	}
	return nullptr;
}
void Player::FallState::exit(Player* player, GameLayer* mainLayer, float time) {
}

//Climb State:
void Player::ClimbState::enter(Player* player, GameLayer* mainLayer, float time) {
	player->wasFalling = true;
	player->wasClimbing = true;
	player->stopAllActions();
	//setting physics body to crouching body
	player->crouchBody->setVelocity(player->getPhysicsBody()->getVelocity());
	player->stop();
	player->setPhysicsBody(player->crouchBody);
	player->stopX();
	player->getPhysicsBody()->setGravityEnable(false);
	player->bodySize = player->crouchSize;
	player->getPhysicsBody()->setPositionOffset(Vec2(0, 0));
	player->maxSpeedY = 100;
	player->startClimbTime = time;
	player->startAnimation(CLIMB, player->climbing);
	if (player->touchingFloor == true) {
		player->startClimbDelay = 0.25f;
	}
	else {
		player->startClimbDelay = 0.5f;
	}
}
Player::State* Player::ClimbState::update(Player* player, GameLayer* mainLayer, float time) {
	if (player->checkDead() == true) { return new DeathState; }
	if (time - player->startClimbTime <= player->startClimbDelay) {
		if (player->getPositionY() + player->getSize().height < player->climbObject->getPositionY() + player->climbObject->surfaceHeight / 2) {
			player->move(Vec2(0, 200));
		}
		else {
			player->stop();
		}
	}
	else{
		player->maxSpeedY = 80;
		if (player->getPositionY() < player->climbObject->getPositionY() + player->climbObject->surfaceHeight) {
			player->move(Vec2(0, 100));
		}
		else {
			return new CrouchState;
		}
	}

	return nullptr;
}
void Player::ClimbState::exit(Player* player, GameLayer* mainLayer, float time) {
	player->getPhysicsBody()->setGravityEnable(true);
	player->objectToClimb = NULL;
}

//Throw State(throwing items):
void Player::ThrowState::enter(Player* player, GameLayer* mainLayer, float time) {
	player->aimAngle = 0;
	player->stopAllActions();
	player->attackStartTime = -1;
	player->attackEndTime = -1;
	player->attackPrepareTime = time;
	if (player->heldBody == NULL) {
		player->beginThrowItem(time);
	}
	else {
		player->beginThrowBody(time);
	}
}
Player::State* Player::ThrowState::update(Player* player, GameLayer* mainLayer, float time) {
	if (player->checkDead() == true) { return new DeathState; }

	if (player->attackRelease == false) {
		if (player->heldBody == NULL) {
			player->beginThrowItem(time);
		}
		else {
			player->beginThrowBody(time);
		}
	}
	else if (player->heldItem != NULL || player->heldBody != NULL) {
		if (player->heldBody == NULL) {
			if (time - player->attackPrepareTime <= player->heldItem->getStartTime() * 2) {
				player->beginThrowItem(time);
			}
		}
		else {
			if (time - player->attackPrepareTime <= player->heldBody->getStartTime() * 2) {
				player->beginThrowBody(time);
			}
		}
	}
	if (player->heldBody == NULL) {
		if (player->attackRelease == true && player->attackPrepareTime != -1.0f && time - player->attackPrepareTime >= player->heldItem->getStartTime() * 2) {
			player->attackStartTime = time;
			player->throwItem(mainLayer, time);
			player->attackPrepareTime = -1.0f;
		}
	}
	else {
		if (player->attackRelease == true && player->attackPrepareTime != -1.0f && time - player->attackPrepareTime >= player->heldBody->getStartTime() * 2) {
			player->attackStartTime = time;
			player->throwBody(mainLayer, time);
			player->attackPrepareTime = -1.0f;
		}
	}
	if (player->thrownItem != NULL) {
		if (player->attackStartTime != -1.0f && time - player->attackStartTime >= player->thrownItem->getAttackTime()) {
			player->attackEndTime = time;
			player->attackStartTime = -1.0f;
		}
		if (player->attackEndTime != -1.0f && time - player->attackEndTime >= player->thrownItem->getLagTime()) {
			player->attackEndTime = -1.0f;
			return player->prevState;
		}
	}
	else if (player->attackStartTime != -1.0f || player->attackEndTime != -1.0f){
		return player->prevState;
	}

	return nullptr;
}
Player::State* Player::ThrowState::handleInput(Player* player, GameLayer* mainLayer, float time, Input input) {
	if (player->wasInHitStun == true) {//player is in hitstun
		player->attackRelease = true;//forced to release attack
		//return player->prevState;
	}
	if (input == USE_ITEM) {//cancel your throw
		if (player->heldItem != NULL) {
			if (player->isCrouched == false) {
				player->heldItem->initHeldItem();
			}
			else {
				player->heldItem->initCrouchHeldItem();
			}
			return player->prevState;
		}
	}
	//input for releasing attack
	if (input == THROW_RELEASE) {
		player->attackRelease = true;
	}
	//only register inputs if player is still preparing throw
	if (player->attackRelease == false) {
		//input for aiming
		if (input == AIM_UP) {
			player->aimAngle = 270;
		}
		else if (input == AIM_DOWN) {
			if (player->isCrouched == false) {
				player->aimAngle = 90;
			}
			else {
				player->aimAngle = 45;
			}
		}
		else if (input == AIM_UP_LEFT) {
			if (player->flippedX == false) {
				player->flipX();
				player->turned = true;
			}
			player->aimAngle = 315;
		}
		else if (input == AIM_LEFT) {
			if (player->flippedX == false) {
				player->flipX();
				player->turned = true;
			}
			player->aimAngle = 0;
		}
		else if (input == AIM_DOWN_LEFT) {
			if (player->flippedX == false) {
				player->flipX();
				player->turned = true;
			}
			player->aimAngle = 45;
		}
		else if (input == AIM_UP_RIGHT) {
			if (player->flippedX == true) {
				player->flipX();
				player->turned = false;
			}
			player->aimAngle = 315;
		}
		else if (input == AIM_RIGHT) {
			if (player->flippedX == true) {
				player->flipX();
				player->turned = false;
			}
			player->aimAngle = 0;
		}
		else if (input == AIM_DOWN_RIGHT) {
			if (player->flippedX == true) {
				player->flipX();
				player->turned = false;
			}
			player->aimAngle = 45;
		}
	}
	return nullptr;
}
void Player::ThrowState::exit(Player* player, GameLayer* mainLayer, float time) {
	player->aimMarker->setVisible(false);
	player->attackRelease = false;
	player->aimAngle = 0;
	player->attackStartTime = -1.0f;
	player->attackEndTime = -1.0f;
}

//Attack State(using items):
void Player::AttackState::enter(Player* player, GameLayer* mainLayer, float time) {
	player->wasFalling = false;
	player->stopAllActions();
	if (player->isCrouched == true) {
		player->moveSpeed = (0.2f);
		player->setSpeed(player->moveSpeed);
	}
	else {
		player->moveSpeed = (0.4f);
		player->setSpeed(player->moveSpeed);
	}
	player->beginUseItem(0);
	player->attackPrepareTime = time;
}
Player::State* Player::AttackState::update(Player* player, GameLayer* mainLayer, float time) {
	if (player->checkDead() == true) { return new DeathState; }
	if (player->heldItem->didHitWall == true) {
		player->move(Vec2(-50, 0));
		//player->moveAbsolute(-player->heldItem->knockback / 5);
	}
	if (player->touchingFloor == false && player->getPhysicsBody()->getVelocity().y < -50) {//player is falling
		player->wasFalling = true;
		player->inFallingAttack = true;
		player->attackRelease = true;
		player->fallAttack();
	}
	if (player->inFallingAttack == false) {
		if (player->attackRelease == false) {
			player->beginUseItem(player->aimAngle);
		}
		if (player->attackRelease == true && player->attackPrepareTime != -1.0f && time - player->attackPrepareTime >= player->heldItem->getStartTime()) {
			player->attackStartTime = time;

			player->useItem(player->aimAngle);
			player->attackPrepareTime = -1.0f;
		}
		if (player->attackStartTime != -1.0f && time - player->attackStartTime >= player->heldItem->getAttackTime()) {
			player->finishUseItem();
			player->attackEndTime = time;
			player->attackStartTime = -1.0f;
		}
		if (player->attackEndTime != -1.0f && time - player->attackEndTime >= player->heldItem->getLagTime()) {
			player->attackEndTime = -1.0f;
			return player->prevState;
		}
	}
	else {//player is performing a falling attack
		if (player->touchingFloor == true) {//they hit the ground
			player->createNoise(60, 0.6, time, player->getPosition() + Vec2(player->getSize().width, 0), Vec2(player->currentFloor, player->currentRoom), "player_land", &mainLayer->noises);
			return new CrouchState;
		}
	}
	return nullptr;
}
Player::State* Player::AttackState::handleInput(Player* player, GameLayer* mainLayer, float time, Input input) {
	if (player->wasInHitStun == true) {//player is in hitstun
		player->attackRelease = true;//forced to release attack
		player->walkPrepareAttack(STOP, time);
		//return player->prevState;
	}
	if (input == THROW_ITEM) {//cancel your attack
		if (player->heldItem != NULL) {
			if (player->isCrouched == false) {
				player->heldItem->initHeldItem();
			}
			else {
				player->heldItem->initCrouchHeldItem();
			}
			return player->prevState;
		}
	}
	//input for releasing attack
	if (input == USE_RELEASE) {
		player->attackRelease = true;
		//player->walkPrepareAttack(STOP, time);
		player->stop();
	}
	//only register inputs if player is still preparing attack
	if (player->attackRelease == false) {
		//input for aiming
		if (input == AIM_UP) {
			player->aimAngle = 270;
		}
		if (input == AIM_DOWN) {
			if (player->isCrouched == false) {
				player->aimAngle = 90;
			}
			else {
				player->aimAngle = 45;
			}
		}
		if (player->flippedX == true) {//only register left inputs for aiming while facing left
			if (input == AIM_UP_LEFT) {
				player->aimAngle = 315;
			}
			if (input == AIM_LEFT) {
				player->aimAngle = 0;
			}
			if (input == AIM_DOWN_LEFT) {
				player->aimAngle = 45;
			}
		}
		if (player->flippedX == false) {//only register right inputs for aiming while facing right
			if (input == AIM_UP_RIGHT) {
				player->aimAngle = 315;
			}
			if (input == AIM_RIGHT) {
				player->aimAngle = 0;
			}
			if (input == AIM_DOWN_RIGHT) {
				player->aimAngle = 45;
			}
		}
		//input for moving
		if ((input == MOVE_LEFT || input == MOVE_RIGHT || input == STOP)) {
			player->walkPrepareAttack(input, time);
		}
	}
	return nullptr;
}
void Player::AttackState::exit(Player* player, GameLayer* mainLayer, float time) {
	player->aimAngle = 0;
	player->heldItem->didHitWall = false;
	player->moveSpeed = (1.0f);
	player->setSpeed(player->moveSpeed);
	if (player->heldItem->hp <= 0) {//if item is broken, no hp left
		if (player->isCrouched == false) {
			player->heldItem->initHeldItem();
		}
		else {
			player->heldItem->initCrouchHeldItem();
		}
		player->breakItem(mainLayer);
	}
	else{
		if (player->isCrouched == false) {
			player->heldItem->initHeldItem();
		}
		else {
			player->heldItem->initCrouchHeldItem();
		}
	}
	player->attackStartTime = -1.0f;
	player->attackEndTime = -1.0f;
	player->inFallingAttack = false;
	player->attackRelease = false;
}

//Rolling State:
void Player::RollState::enter(Player* player, GameLayer* mainLayer, float time) {
	player->wasFalling = true;
	if (player->prevState->type == "neutral") {
		player->isCrouched = true;
		player->crouchBody->setVelocity(player->getPhysicsBody()->getVelocity());
		player->stop();
		player->setPhysicsBody(player->crouchBody);
		player->bodySize = player->crouchSize;
		player->getPhysicsBody()->setPositionOffset(Vec2(0, -27));
		player->pickUpRadius->setPosition(Vec2(35, 15));
		if (player->heldItem != NULL) {
			player->heldItem->initCrouchHeldItem();
		}
	}
	if (player->heldItem != NULL) {
		player->heldItem->setVisible(false);
	}
	player->stopAllActions();
	player->stop();
	player->startAnimation(ROLLING, player->rolling);
	player->getPhysicsBody()->setLinearDamping(1.0f);
	player->moveNoLimit(Vec2(420, 0));//applying force for the roll
	if (player->heldBody != NULL) {
		player->dropBody(mainLayer);
	}
	//player->createNoise(35, 0.4, time, player->getPosition() + Vec2(player->getSize().width, 0), Vec2(player->currentFloor, player->currentRoom), "player_roll", &mainLayer->noises);
}
Player::State* Player::RollState::update(Player* player, GameLayer* mainLayer, float time) {
	if (player->checkDead() == true) { return new DeathState; }
	if (player->getPhysicsBody()->getVelocity().x > -2.5f && player->getPhysicsBody()->getVelocity().x < 2.5f) {//when player's horizontal speed has stopped
		return new CrouchState;
	}
	else if (player->touchingFloor == false && player->getPhysicsBody()->getVelocity().y < -75) {
		return new FallState;
	}
	return nullptr;
}
Player::State* Player::RollState::handleInput(Player* player, GameLayer* mainLayer, float time, Input input) {
	if (input == DROP) {
		player->dropItem(mainLayer);
	}
	if (input == PICKUP) {
		player->pickUpItem(mainLayer);
	}
	return nullptr;
}
void Player::RollState::exit(Player* player, GameLayer* mainLayer, float time) {
	if (player->heldItem != NULL) {
		player->heldItem->setVisible(true);
	}
	player->getPhysicsBody()->setLinearDamping(0.0f);
}

//Hide State:
void Player::HideState::enter(Player* player, GameLayer* mainLayer, float time) {
	player->hideObject = player->objectToHideBehind;
	if (player->isCrouched == false) {
		player->isCrouched = true;
		player->stopAllActions();
		player->crouchBody->setVelocity(player->getPhysicsBody()->getVelocity());
		player->stop();
		player->setPhysicsBody(player->crouchBody);
		player->bodySize = player->crouchSize;
		player->getPhysicsBody()->setPositionOffset(Vec2(0, -27.5));
		player->pickUpRadius->setPosition(Vec2(35, 15));
		if (player->heldItem != NULL) {
			player->heldItem->initCrouchHeldItem();
		}
		if (player->heldBody != NULL) {
			player->heldBody->initCrouchHeldBody();
		}
		//player->startAnimation(CROUCH, player->crouch);
		player->setSpriteFrame(player->crouchwalk.animation->getFrames().at(0)->getSpriteFrame());
		player->moveSpeed = 0.5f;
		player->setSpeed(player->moveSpeed);
	}
	player->hide();
}
Player::State* Player::HideState::update(Player* player, GameLayer* mainLayer, float time) {
	if (player->checkDead() == true) { return new DeathState; }
	if (player->isHit == true) {//force exit hiding if hit by enemy
		if (player->isCrouched == true) {
			return new CrouchState;
		}
		else {
			return new NeutralState;
		}
	}
	player->hiding();
	return nullptr;
}
Player::State* Player::HideState::handleInput(Player* player, GameLayer* mainLayer, float time, Input input) {
	if (input == HIDE) {
		if (player->isCrouched == true) {
			return new CrouchState;
		}
		else {
			return new NeutralState;
		}
	}
	if (input == DROP) {
		if (player->heldBody != NULL) {
			player->dropBody(mainLayer);
		}
	}
	if (input == PICKUP) {
		if (player->heldBody == NULL) {
			if (player->bodyToPickUp != NULL) {
				player->pickUpBody(mainLayer);
			}
		}
	}
	if (input == MOVE_LEFT) {
		if (player->hittingLeft == false) {
			if (player->isCrouched == false) {
				player->walk(input, time);
			}
			else {
				player->crouchWalk(input, time);
			}
		}
	}
	else if (input == MOVE_RIGHT) {
		if (player->hittingRight == false) {
			if (player->isCrouched == false) {
				player->walk(input, time);
			}
			else {
				player->crouchWalk(input, time);
			}
		}
	}
	else if (input == STOP) {
		if (player->isCrouched == false) {
			player->walk(input, time);
		}
		else {
			player->crouchWalk(input, time);
		}
	}
	/*if (player->hittingLeft == false && player->hittingRight == false) {
		if (input == MOVE_UP) {
			if (player->isCrouched == true) {
				player->isCrouched = false;
				player->stopAllActions();
				player->mainBody->setVelocity(player->getPhysicsBody()->getVelocity());
				player->stop();
				player->setPhysicsBody(player->mainBody);
				player->bodySize = player->standSize;
				player->getPhysicsBody()->setPositionOffset(Vec2(0, 17));
				player->pickUpRadius->setPosition(Vec2(35, 64));
				if (player->heldItem != NULL) {
					player->heldItem->initHeldItem();
				}
				if (player->heldBody != NULL) {
					player->heldBody->initHeldBody();
				}
				//player->startAnimation(STANDUP, player->standup);
				player->setSpriteFrame(player->stand.animation->getFrames().at(0)->getSpriteFrame());
				player->moveSpeed = 1.0f;
				player->setSpeed(player->moveSpeed);
			}
		}
		else if (input == MOVE_DOWN) {
			if (player->isCrouched == false) {
				player->isCrouched = true;
				player->stopAllActions();
				player->crouchBody->setVelocity(player->getPhysicsBody()->getVelocity());
				player->stop();
				player->setPhysicsBody(player->crouchBody);
				player->bodySize = player->crouchSize;
				player->getPhysicsBody()->setPositionOffset(Vec2(0, -27.5));
				player->pickUpRadius->setPosition(Vec2(35, 15));
				if (player->heldItem != NULL) {
					player->heldItem->initCrouchHeldItem();
				}
				if (player->heldBody != NULL) {
					player->heldBody->initCrouchHeldBody();
				}
				//player->startAnimation(CROUCH, player->crouch);
				player->setSpriteFrame(player->crouchwalk.animation->getFrames().at(0)->getSpriteFrame());
				player->moveSpeed = 0.5f;
				player->setSpeed(player->moveSpeed);
			}
		}
	}
	*/
	return nullptr;
}
void Player::HideState::exit(Player* player, GameLayer* mainLayer, float time) {
	player->moveSpeed = 1.0f;
	player->setSpeed(player->moveSpeed);
	player->hide();
}

//Death State:
void Player::DeathState::enter(Player* player, GameLayer* mainLayer, float time) {
	player->stopAllActions();
	player->getPhysicsBody()->setEnabled(false);
	player->startAnimation(DEATH, player->dying);
}
Player::State* Player::DeathState::update(Player* player, GameLayer* mainLayer, float time) {
	if (player->getActionByTag(DEATH) == NULL) {//player's death animation is over
		player->isDead = true;
		return new DeathState;
	}
	return nullptr;
}
void Player::DeathState::exit(Player* player, GameLayer* mainLayer, float time) {
}


//No Clip state:
void Player::NoClipState::enter(Player* player, GameLayer* mainLayer, float time) {
	player->noclip = true;
	player->getPhysicsBody()->setVelocityLimit(250);
	player->getPhysicsBody()->setGravityEnable(false);
	player->getPhysicsBody()->setCollisionBitmask(0);
	player->moveSpeed = 4;
	player->setSpeed(player->moveSpeed);
	player->hidden = true;
	player->setTag(10);
}
Player::State* Player::NoClipState::handleInput(Player* player, GameLayer* mainLayer, float time, Input input) {
	if (input == USE_DOOR) {
		player->useDoor();
	}
	if (input == USE_STAIR) {
		player->useStair(mainLayer);
	}
	if (input == USE_ITEM) {
		return new AttackState;
	}
	if (input == HIDE) {
		return new HideState;
	}
	if (input == MOVE_LEFT || input == MOVE_RIGHT || input == STOP) {
		player->walk(input, time);
	}
	if (input == MOVE_UP) {
		player->move(Vec2(0, 30));
	}
	else if (input == MOVE_DOWN) {
		player->move(Vec2(0, -30));
	}
	if (input == NO_CLIP) {
		return new NeutralState;
	}
	return nullptr;
}
void Player::NoClipState::exit(Player* player, GameLayer* mainLayer, float time) {
	player->noclip = false;
	player->getPhysicsBody()->setVelocityLimit(10000);
	player->getPhysicsBody()->setGravityEnable(true);
	player->getPhysicsBody()->setCollisionBitmask(30);
	player->moveSpeed = 1;
	player->setSpeed(player->moveSpeed);
	player->hidden = false;
	player->setTag(1);
}
