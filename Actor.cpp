#include "Actor.h"
#include "GameConstants.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

//*************************///
//***ACTIVATING OBJECTS****//
//************************//
//Need: doSomething() and activate(Actor* actor)

//Should all have the same doSomething that does nothing
void ActivatingObject::doSomething(){
}
//** EXIT **//
void Exit::activate(Actor* actor){
    if(dynamic_cast<Player*>(actor) != nullptr && this->getGameWorldPtr()->getCitizenCount() == 0){
        this->getGameWorldPtr()->setNextRound();
    }
    else if(dynamic_cast<Citizen*>(actor) != nullptr){
        dynamic_cast<Citizen*>(actor)->exited();
    }
}

//** PIT **//
void Pit::activate(Actor* actor){
    if(dynamic_cast<Agent*>(actor) != nullptr)
        actor->setDead();
}

//** FLAME **//

void Flame::doSomething(){
    if(!this->isAlive())
        return;
    tick++;
    if(tick > 2)
        this->setDead();
}

void Flame::activate(Actor *actor){
    if(actor->canGetFlameDamaged()){
        actor->setDead();
    }
    else if(actor->doesBlockFlame())
        this->setDead();
}

//** VOMIT **//
void Vomit::activate(Actor* actor){
    if(actor->canItGetInfected()){
        dynamic_cast<Person*>(actor)->setInfected(true);
    }
}

//** LANDMINE **//
void Landmine::doSomething(){
    if(!this->isAlive())
        return;
    safetyTicks--;
    if(!active && safetyTicks <= 0)
        active = true;
}


void Landmine::activate(Actor *actor){
    if(active && actor->doesTriggerLandmine()){
        this->setDead();
        this->getGameWorldPtr()->playSound(SOUND_LANDMINE_EXPLODE);
        
        //Add flame objects
        for(int x = getX() - SPRITE_WIDTH; x <= getX() + SPRITE_WIDTH; x += SPRITE_WIDTH){
            for(int y = getY() - SPRITE_HEIGHT; y <= getY() + SPRITE_HEIGHT; y += SPRITE_HEIGHT){
                if(x != getX() || y != getY())
                    getGameWorldPtr()->addActor(new Flame(x, y, right, getGameWorldPtr()));
            }
        }
        //Add pit
        getGameWorldPtr()->addActor(new Pit(getX(), getY(), getGameWorldPtr()));
    }
}


//****GOODIES****//
void Goodie::activate(Actor *activator){
    this->getGameWorldPtr()->playSound(SOUND_GOT_GOODIE);
    this->getGameWorldPtr()->increaseScore(50);
    this->setDead();
}

//**VACCINE GOODIE

void VaccineGoodie::activate(Actor* actor){
    if(dynamic_cast<Player*>(actor) == nullptr)
        return;
    Player* player = dynamic_cast<Player*>(actor);
    player->increaseVaccCount(1);
    this->Goodie::activate(actor);
}

//**GAS CAN GOODIE

void GasCanGoodie::activate(Actor* actor){
    if(dynamic_cast<Player*>(actor) == nullptr)
        return;
    Player* player = dynamic_cast<Player*>(actor);
    player->increaseFlameCount(5);
    this->Goodie::activate(actor);
}

//**LANDMINE GOODIE

void LandmineGoodie::activate(Actor *actor){
    if(dynamic_cast<Player*>(actor) == nullptr)
        return;
    Player* player = dynamic_cast<Player*>(actor);
    player->increaseLandmineCount(2);
    this->Goodie::activate(actor);
}

//**PLAYER **//

void Player::setDead(){
    this->Actor::setDead();
    getGameWorldPtr()->playSound(SOUND_PLAYER_DIE);
}

void Player::doSomething(){
    if(!this->isAlive() || checkInfectionCount()){
        return;
    }
    
    // KEY INPUT //
    int keyPress = 0;
    if(this->getGameWorldPtr()->getKey(keyPress)){ //Returns false if a key isn't hit
        
        //MOVEMENT
        int x = this->getX();
        int y = this->getY();
        int dest_x = x;
        int dest_y = y;
        
        if(keyPress == KEY_PRESS_LEFT){
            this->setDirection(left);
            dest_x -= 4;
        }
        else if(keyPress == KEY_PRESS_RIGHT){
            this->setDirection(right);
            dest_x += 4;
        }
        else if(keyPress == KEY_PRESS_UP){
            this->setDirection(up);
            dest_y += 4;
        }
        else if(keyPress == KEY_PRESS_DOWN){
            this->setDirection(down);
            dest_y -= 4;
        }
        
        if(!getGameWorldPtr()->actorWouldBeBlocked(this,dest_x, dest_y)){
            this->moveTo(dest_x, dest_y);
        }
        
        //**POWER UPS
        
        //Flames
        if(keyPress == KEY_PRESS_SPACE && flameCount > 0){
            
            int flameX = getX();
            int flameY = getY();
            bool flameMade = false;
            for(int flamesMade = 0; flamesMade < 3; flamesMade++){
                if(flameCount > 0){
                    if(this->getDirection() == right)
                        flameX += SPRITE_WIDTH;
                    else if(this->getDirection() == left)
                        flameX -= SPRITE_WIDTH;
                    else if(this->getDirection() == up)
                        flameY += SPRITE_HEIGHT;
                    else if(this->getDirection() == down)
                        flameY -= SPRITE_HEIGHT;
                    Flame* flame = new Flame(flameX, flameY, this->getDirection(), this->getGameWorldPtr());
                    if(getGameWorldPtr()->canFlameExist(flame)){
                        flameMade = true;
                        getGameWorldPtr()->addActor(flame);
                                            }
                    else{ //Flame position not valid
                        delete flame;
                        break;
                    }
                }
            }
            if(flameMade){
                flameCount--;
                getGameWorldPtr()->playSound(SOUND_PLAYER_FIRE);
            }
        }
        
        //Landmines
        if(keyPress == KEY_PRESS_TAB && landmineCount > 0){
            getGameWorldPtr()->addActor(new Landmine(getX(), getY(), getGameWorldPtr()));
            landmineCount--;
        }
        
        //Vaccines
        if(keyPress == KEY_PRESS_ENTER && vaccCount > 0){
            setInfected(false);
            vaccCount--;
        }
        
        //END OF KEY INPUT//
    }
}

//** CITIZEN **//

void Citizen::exited(){
    getGameWorldPtr()->increaseScore(500);
    getGameWorldPtr()->playSound(SOUND_CITIZEN_SAVED);
    getGameWorldPtr()->decreaseCitizenCount();
    this->Actor::setDead();//just set it dead, don't use its own set dead method
}

void Citizen::setDead(){
    this->Actor::setDead();
    getGameWorldPtr()->increaseScore(-1000);
    getGameWorldPtr()->playSound(SOUND_CITIZEN_DIE);
    getGameWorldPtr()->decreaseCitizenCount();
}

void Citizen::setInfected(bool input){
    if(!this->isInfected() && input) //if its just getting infected
        getGameWorldPtr()->playSound(SOUND_CITIZEN_INFECTED);
    this->Person::setInfected(input);
}

void Citizen::doSomething(){
    if(!this->isAlive()){
        return;
    }
    if(checkInfectionCount()){ //Check if infected, returns true if it becomes a zombie
        this->setDead();
        getGameWorldPtr()->playSound(SOUND_ZOMBIE_BORN);
        if(rand() % 100 + 1 <= 70){
            getGameWorldPtr()->addActor(new DumbZombie(getX(), getY(), getGameWorldPtr()));
        }
        else{
            getGameWorldPtr()->addActor(new SmartZombie(getX(), getY(), getGameWorldPtr()));
        }
        return;
    }
    if(paralyzed){ //Citizen doesn't do stuff every other tick
        paralyzed = false;
        return;
    }
    paralyzed = true;
    dist_p = getGameWorldPtr()->distanceBetween(this, getGameWorldPtr()->getPlayer());
    dist_z = getGameWorldPtr()->nearestZombieDistance(this);
    
    //Move the citizen!
    if(dist_p <= dist_z && dist_p <= 80){ //citizen wants to follow player
        //PlayerCol
        int playerCol = getGameWorldPtr()->getPlayer()->getX() / SPRITE_WIDTH;
        int playerRow = getGameWorldPtr()->getPlayer()->getY() / SPRITE_HEIGHT;
        int citCol = getX() / SPRITE_WIDTH;
        int citRow = getY() / SPRITE_HEIGHT;
        
        
        //Determine where the player is in relation
        bool playerIsRight = false; bool playerIsLeft = false; bool playerIsUp = false; bool playerIsDown = false;
        bool sameCol = true;
        bool sameRow = true;
        if(playerCol > citCol){
            playerIsRight = true;
            sameCol = false;
        }
        if(playerCol < citCol){
            playerIsLeft = true;
            sameCol = false;
        }
        if(playerRow > citRow){
            playerIsUp = true;
            sameRow = false;
        }
        if(playerRow < citRow){
            playerIsDown = true;
            sameRow = false;
        }
        
        //Same Row or columns
        if(playerIsRight && sameRow){ //Try and move right
            if(!getGameWorldPtr()->actorWouldBeBlocked(this, getX()+2, getY())){
                setDirection(right);
                moveTo(getX()+2, getY());
                return;
            }
        }
        else if(playerIsLeft && sameRow){ //Try and move left
            if(!getGameWorldPtr()->actorWouldBeBlocked(this, getX()-2, getY())){
                setDirection(left);
                moveTo(getX()-2, getY());
                return;
            }
        }
        else if(playerIsUp && sameCol){ //Try and move up
            if(!getGameWorldPtr()->actorWouldBeBlocked(this, getX(), getY()+2)){
                setDirection(up);
                moveTo(getX(), getY()+2);
                return;
            }
        }
        else if(playerIsDown && sameCol){ //Try and move down
            if(!getGameWorldPtr()->actorWouldBeBlocked(this, getX(), getY()-2)){
                setDirection(down);
                moveTo(getX(), getY()-2);
                return;
            }
        }
        
        if(!sameRow && !sameCol){  //Different Row and Columns
            //Randomly choose to move horizontally or vertically
            bool moveHorizontally = false;
            if(rand() % 100 + 1 <= 50)
                moveHorizontally = true;
            
            if(playerIsRight && moveHorizontally){ //Try and move right
                if(!getGameWorldPtr()->actorWouldBeBlocked(this, getX()+2, getY())){
                    setDirection(right);
                    moveTo(getX()+2, getY());
                    return;
                }
            }
            else if(!playerIsRight && moveHorizontally){ //Try and move left
                if(!getGameWorldPtr()->actorWouldBeBlocked(this, getX()-2, getY())){
                    setDirection(left);
                    moveTo(getX()-2, getY());
                    return;
                }
            }
            else if(playerIsUp && !moveHorizontally){ //Try and move up
                if(!getGameWorldPtr()->actorWouldBeBlocked(this, getX(), getY()+2)){
                    setDirection(up);
                    moveTo(getX(), getY()+2);
                    return;
                }
            }
            else if(!playerIsUp && !moveHorizontally){ //Try and move down
                if(!getGameWorldPtr()->actorWouldBeBlocked(this, getX(), getY()-2)){
                    setDirection(down);
                    moveTo(getX(), getY()-2);
                    return;
                }
            }
        }
    }
    
    //Move away from zombie!
    if(dist_z <= 80){
        int distZRight = 0;
        int distZLeft = 0;
        int distZUp = 0;
        int distZDown = 0;
        
        //See how far nearest zombies are
        
        if(!getGameWorldPtr()->actorWouldBeBlocked(this, getX()+2, getY())){
            
            distZRight = getGameWorldPtr()->nearestZombieDistance(getX()+2, getY());
        }
        if(!getGameWorldPtr()->actorWouldBeBlocked(this, getX()-2, getY())){
            
            distZLeft = getGameWorldPtr()->nearestZombieDistance(getX()-2, getY());
        }
        if(!getGameWorldPtr()->actorWouldBeBlocked(this, getX(), getY()+2)){
            
            distZUp = getGameWorldPtr()->nearestZombieDistance(getX(), getY()+2);
        }
        if(!getGameWorldPtr()->actorWouldBeBlocked(this, getX(), getY()-2)){
            distZDown = getGameWorldPtr()->nearestZombieDistance(getX(), getY()-2);
        }
        
        if(distZRight <= dist_z && distZLeft <= dist_z
           && distZRight <= dist_z && distZDown <= dist_z){ //if none of the options are better!
            return;
        }
        if(distZRight >= distZLeft && distZRight >= distZUp && distZRight >= distZDown){
            setDirection(right);
            moveTo(getX()+2, getY());
            return;
        }
        if(distZLeft >= distZRight && distZLeft >= distZUp && distZLeft >= distZDown){
            setDirection(left);
            moveTo(getX()-2, getY());
            return;
        }
        if(distZUp >= distZRight && distZUp >= distZLeft && distZUp >= distZDown){
            setDirection(up);
            moveTo(getX(), getY()+2);
            return;
        }
        if(distZDown >= distZRight && distZDown >= distZLeft && distZDown >= distZUp){
            setDirection(down);
            moveTo(getX(), getY()-2);
            return;
        }
    }
    
}

//** ZOMBIE CLASS **//

void Zombie::setDead(){
    this->Actor::setDead();
    getGameWorldPtr()->playSound(SOUND_ZOMBIE_DIE);
}

void Zombie::vomitCalculations(){
    //Vomit Calculations
    vomitted = false;
    int dest_x = this->getX();
    int dest_y = this->getY();
    if(getDirection() == right)
        dest_x += SPRITE_WIDTH;
    if(getDirection() == left)
        dest_x -= SPRITE_WIDTH;
    if(getDirection() == up)
        dest_y += SPRITE_HEIGHT;
    if(getDirection() == down)
        dest_y -= SPRITE_HEIGHT;
    
    if(getGameWorldPtr()->shouldVomitBeMade(dest_x, dest_y) && rand() % 99 + 1 <= 33){
        getGameWorldPtr()->addActor(new Vomit(dest_x, dest_y, getDirection(), getGameWorldPtr()));
        getGameWorldPtr()->playSound(SOUND_ZOMBIE_VOMIT);
        vomitted = true;
    }
}

void Zombie::tryAndMove(){
    if(getDirection() == right && !getGameWorldPtr()->actorWouldBeBlocked(this, getX()+1, getY())){
        moveTo(getX()+1, getY());
        movementPlanDistance--;
    }
    else if(getDirection() == left && !getGameWorldPtr()->actorWouldBeBlocked(this, getX()-1, getY())){
        moveTo(getX()-1, getY());
        movementPlanDistance--;
    }
    else if(getDirection() == up && !getGameWorldPtr()->actorWouldBeBlocked(this, getX(), getY()+1)){
        moveTo(getX(), getY()+1);
        movementPlanDistance--;
    }
    else if(getDirection() == down && !getGameWorldPtr()->actorWouldBeBlocked(this, getX(), getY()-1)){
        moveTo(getX()-1, getY()-1);
        movementPlanDistance--;
    }
    else{
        movementPlanDistance = 0;
    }
}
void Zombie::chooseRandomDir(){
    setMovementPlanDistance(rand() % 8 + 3);
    
    int random = rand() % 4;
    if(random < 1)
        setDirection(right);
    else if(random < 2)
        setDirection(left);
    else if(random < 3)
        setDirection(up);
    else
        setDirection(down);
}

//** DUMB ZOMBIE **//

void DumbZombie::setDead(){
    this->Zombie::setDead();
    getGameWorldPtr()->increaseScore(10000);
    if(rand() % 10 < 1)
        getGameWorldPtr()->addActor(new VaccineGoodie(getX(), getY(), getGameWorldPtr()));
}

void DumbZombie::doSomething(){
    if(!this->isAlive()){
        return;
    }
    if(isParalyzed()){
        setParalyzed(false);
        return;
    }
    setParalyzed(true); //for the next tick
    this->vomitCalculations();
    if(hasVomitted())
        return;
    if(getMovementPlanDistance() == 0){
        chooseRandomDir();
    }
    tryAndMove();
}
//** SMART ZOMBIE **//
void SmartZombie::setDead(){
    this->Zombie::setDead();
    getGameWorldPtr()->increaseScore(2000);
}

void SmartZombie::doSomething(){
    if(!this->isAlive()){
        return;
    }
    if(isParalyzed()){
        setParalyzed(false); //Code has to be repeated because it has a return
        return;
    }
    setParalyzed(true); //for the next tick
    this->vomitCalculations();
    if(hasVomitted())
        return;
    
    if(getMovementPlanDistance() == 0){
        setMovementPlanDistance(rand() % 8 + 3);
        Actor* p = getGameWorldPtr()->neareastPerson(this);
        if(getGameWorldPtr()->distanceBetween(this, p) > 80){
            chooseRandomDir();
        }
        else{
            int targetCol = p->getX() / SPRITE_WIDTH;
            int targetRow = p->getY() / SPRITE_HEIGHT;
            int zombieCol =  getX() / SPRITE_WIDTH;
            int zombieRow =  getY() / SPRITE_HEIGHT;
            
            
            if(zombieRow == targetRow && targetCol > zombieCol){
                setDirection(right);
            }
            else if(zombieRow == targetRow && targetCol < zombieCol){
                setDirection(left);
            }
            else if(zombieCol == targetCol && targetRow > zombieCol){
                setDirection(up);
            }
            else if(zombieCol == targetCol && targetRow < zombieCol){
                setDirection(down);
            }
            else{// go random
                bool moveHorizontally = false;
                if(rand() % 100 + 1 < 50)
                    moveHorizontally = true;
                
                if(moveHorizontally && targetRow > zombieRow)
                    setDirection(right);
                else if(moveHorizontally && targetRow < zombieRow)
                    setDirection(left);
                else if(targetCol > zombieCol)
                    setDirection(up);
                else
                    setDirection(down);
            }
        }
    }
    tryAndMove();
}
