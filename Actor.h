#ifndef ACTOR_H_
#define ACTOR_H_
#include "GraphObject.h"
class StudentWorld;
class Player; //So Goodie knows about player
using namespace std;

/*CLASS HIERACHY:
 GraphObject
    Actor
        Wall
        ActivatingObject
            Exit
            Pit
            Flame
            Vomit
            Landmine
            Goodie
                VaccineGoodie
                GasCanGoodie
                LandmineGoodie
        Agent
            Person
                Player
                Citizen
            Zombie
                DumbZombie
                SmartZombie
 */

//***** ACTOR PURE ABSTRACT CLASS***//
class Actor: public GraphObject{
public:
    
    //SIMPLE CONSTRUCTOR//
    Actor(int imageID, int startX, int startY, StudentWorld* gameWorld):
    GraphObject(imageID, startX, startY, right, 0, 1.0)
    // depth, size
    {
        gameWorldPtr = gameWorld;
    }
    
    //COMPLICATED CONSTRUCTOR//
    Actor(int imageID, int startX, int startY, Direction dir, int depth, double size, StudentWorld* gameWorld):
    GraphObject(imageID, startX, startY, dir, depth, size)
    {
        gameWorldPtr = gameWorld;
    }
    
    virtual ~Actor(){}
    
    //DO SOMETHING METHOD, EVERYTHING NEEDS THIS
    virtual void doSomething() = 0;
    
    //ACCESS TO WORLD
    StudentWorld* getGameWorldPtr() const{
        return gameWorldPtr;
    }
    
    //SET DEAD/IS ALIVE
    virtual void setDead(){                 //Is it alive
        alive = false;
    }
    bool isAlive(){
        return alive;
    }
    
    //BLOCKING ATTRIBUTES
    bool doesBlockAgents() const{        //Does it block agents
        return blocksAgents;
    }
    bool doesBlockFlame() const{        //Does it block flames
        return blocksFlame;
    }
    bool doesTriggerLandmine() const{    //Does it trigger landmines
        return triggersLandmine;
    }
    void setBlocksFlame(bool a){
        blocksFlame = a;
    }
    void setBlocksAgents(bool a){
        blocksAgents = a;
    }
    void setTriggersLandmine(bool a){
        triggersLandmine = a;
    }
    
    //GETTING HURT
    bool canGetFlameDamaged() const{       //Can it get flamed damaged?
        return canFlameDamage;
    }
    bool canItGetInfected() const{        //Can it get infected?
        return canGetInfected;
    }
    void setFlameDamaged(bool a){
        canFlameDamage = a;
    }
    void setCanGetInfected(bool a){
        canGetInfected = a;
    }
    
private:
    //ACCESS TO WORLD
    StudentWorld* gameWorldPtr = nullptr;
    
    //IS ALIVE?
    bool alive = true;
    
    //BLOCKING ATTRIBUTES
    bool blocksAgents = false;
    bool blocksFlame = false;
    
    //GETTING HURT ATTRIBUTES
    bool triggersLandmine = false;
    bool canFlameDamage = false;
    bool canGetInfected = false;
};

//** WALL CLASS**//
class Wall: public Actor{
public:
    Wall(int startX, int startY, StudentWorld* gameWorld):
    Actor(IID_WALL, startX, startY, gameWorld)
    {
        setBlocksFlame(true);
        setBlocksAgents(true);
    }
    
    virtual void doSomething(){
        //Nothing needed, walls don't do much
    }
};

//** ACTIVATING OBJECT PURE ABSTRACT CLASS**//
class ActivatingObject: public Actor{
public:
    ActivatingObject(int IID, int startX, int startY, StudentWorld* GameWorld)
    :Actor(IID, startX, startY, GameWorld)
    {
    }
    ActivatingObject(int IID, int startX, int startY, Direction dir, int depth, double size, StudentWorld* gameWorld):
    Actor(IID, startX, startY,  dir,  depth,  size, gameWorld)
    {
    }
    void doSomething();
    virtual void activate(Actor* actor) = 0; //Must have an activate!
};

//*** EXIT CLASS **//
class Exit: public ActivatingObject{
public:
    Exit(int startX, int startY, StudentWorld* gameWorld)
    : ActivatingObject(IID_EXIT, startX,startY, right, 1, 1.0, gameWorld)
    {
        setBlocksFlame(true);
    }
    virtual void activate(Actor* actor); //Change to human
};

//** PIT CLASS **//
class Pit: public ActivatingObject{
public:
    Pit(int startX, int startY, StudentWorld* gameWorld):
    ActivatingObject(IID_PIT, startX, startY, gameWorld)
    {
    }
    void activate(Actor* actor);
};

//** FLAME CLASS **//
class Flame: public ActivatingObject{
public:
    Flame(int startX, int startY, Direction dir, StudentWorld* gameworld):
    ActivatingObject(IID_FLAME, startX, startY, dir, 0, 1.0, gameworld)
    {
        setTriggersLandmine(true);
    }
    virtual void doSomething();
    virtual void activate(Actor* actor);
private:
    int tick = 0;
};

//** VOMIT CLASS **//
class Vomit: public ActivatingObject{
public:
    Vomit(int startX, int startY, Direction dir, StudentWorld* gameworld):
    ActivatingObject(IID_VOMIT, startX, startY, dir, 0, 1.0, gameworld)
    {
    }
    virtual void activate(Actor* actor);
};

//** LANDMINE CLASS **//
class Landmine: public ActivatingObject{
public:
    Landmine(int startX, int startY, StudentWorld* gameWorld)
    :ActivatingObject(IID_LANDMINE, startX, startY, gameWorld)
    {
    }
    virtual void activate(Actor* actor);
    virtual void doSomething();//Landmine will need more todo each tick
private:
    int safetyTicks = 30;
    bool active = false;
};

//*** GOODIE PURE ABSTRACT CLASS ***///
class Goodie: public ActivatingObject{
public:
    Goodie(int IID, int startX, int startY, StudentWorld* gameWorld):
    ActivatingObject(IID, startX, startY, gameWorld){
        setFlameDamaged(true);
    }
    virtual void activate(Actor* actor) = 0; //When a goodie is used of course
};

//**VACCINE CLASS**//
class VaccineGoodie: public Goodie{
public:
    VaccineGoodie(int startX, int startY, StudentWorld* gameWorld):
    Goodie(IID_VACCINE_GOODIE, startX, startY, gameWorld)
    {
    }
    virtual void activate(Actor* actor);
};

//**GAS CAN GOODIE**//
class GasCanGoodie: public Goodie{
public:
    GasCanGoodie(int startX, int startY, StudentWorld* gameWorld):
    Goodie(IID_GAS_CAN_GOODIE, startX, startY, gameWorld)
    {
    }
    virtual void activate(Actor* actor);
};

//**LANDMINE GOODIE**//
class LandmineGoodie: public Goodie{
public:
    LandmineGoodie(int startX, int startY, StudentWorld* gameWorld):
    Goodie(IID_LANDMINE_GOODIE, startX, startY, gameWorld)
    {
    }
    virtual void activate(Actor* activator);
};

//** AGENT ABSTRACT CLASS **//
class Agent: public Actor{
public:
    Agent(int IID, int startX, int startY, StudentWorld* gameWorld)
    :Actor(IID, startX, startY, gameWorld){
        setTriggersLandmine(true);
        setBlocksAgents(true);
        setFlameDamaged(true);
    }
    virtual void doSomething() = 0;
};

//**PERSON ABSTRACT CLASS**//

class Person: public Agent{
public:
    Person(int IID, int startX, int startY, StudentWorld* gameWorld):
    Agent(IID, startX, startY, gameWorld){
        setCanGetInfected(true);
    }
    
    //INFECTED METHODS
    virtual void setInfected(bool input){           //Set a person's infection status
        infected = input;
        if(input == false){
            infectionCount = 0; //Cured!
        }
    }
    bool isInfected(){
        return infected;
    }
    int getInfectionCount(){
        return infectionCount;
    }
    void increaseInfectionCount(){
        infectionCount++;
    }
    bool checkInfectionCount(){             //All Persons should run this method
        if(this->isInfected()){              //Returns true if the person died!
            this->increaseInfectionCount();
        }
        if(this->getInfectionCount() >= 500){
            this->setDead();
            return true;
        }
        return false;
    }
    virtual void setDead() = 0; //Must overwrite dead method now
    
private:
    bool infected = false;
    int infectionCount = 0;
};

//PLAYER CLASS//
class Player: public Person{
public:
    Player(int startX, int startY, StudentWorld* gameWorld):
    Person(IID_PLAYER, startX, startY, gameWorld)
    {
    }
    virtual void doSomething();
    //PLAYERS POWER UPS
    void increaseVaccCount(int i){ // vacc count
        vaccCount += i;
    }
    void increaseFlameCount(int i){ // flame count
        flameCount += i;
    }
    void increaseLandmineCount(int i){ //land mine count
        landmineCount += i;
    }
    int getVaccCount(){
        return vaccCount;
    }
    int getFlameCount(){
        return flameCount;
    }
    int getLandMineCount(){
        return landmineCount;
    }
    virtual void setDead();
private:
    int vaccCount = 0;
    int flameCount = 0;
    int landmineCount = 0;
    
};

//** CITIZEN CLASS**//
class Citizen: public Person{
public:
    Citizen(int startX, int startY, StudentWorld* gameWorld):
    Person(IID_CITIZEN, startX, startY, gameWorld)
    {
    }
    virtual void doSomething();
    virtual void setInfected(bool input);
    void exited();
    virtual void setDead(); //Want its own setDead method
    
private:
    bool paralyzed =  false;
    int dist_p; //distance to player
    int dist_z; //distance to nearest zombie
};

//** ZOMBIE CLASS **//
class Zombie: public Agent{
public:
    Zombie(int startX, int startY, StudentWorld* gameWorld):
    Agent(IID_ZOMBIE, startX, startY, gameWorld)
    {
    }
    virtual void setDead();
    virtual void doSomething() = 0;
    void vomitCalculations();
    bool isParalyzed(){
        return paralyzed;
    }
    void setParalyzed(bool a){
        paralyzed = a;
    }
    int getMovementPlanDistance(){
        return movementPlanDistance;
    }
    void setMovementPlanDistance(int i){
        movementPlanDistance = i;
    }
    bool hasVomitted(){
        return vomitted;
    }
    void tryAndMove();
    void chooseRandomDir();
private:
    int movementPlanDistance = 0;
    bool paralyzed = false;
    bool vomitted = false;
};

//Some dead zombies drop vaccines, going to want to change the setDead method to do this
class DumbZombie: public Zombie{
public:
    DumbZombie(int startX, int startY, StudentWorld* gameWorld):
    Zombie(startX, startY, gameWorld)
    {
    }
    virtual void doSomething();
    virtual void setDead();
};

class SmartZombie: public Zombie{
public:
    SmartZombie(int startX, int startY, StudentWorld* gameWorld):
    Zombie(startX, startY, gameWorld)
    {
    }
    virtual void setDead();
    virtual void doSomething();
};
#endif // ACTOR_H_
