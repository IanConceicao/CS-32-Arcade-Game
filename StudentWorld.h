#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <list>
class Flame;
class Actor;
class Player;
class Citizen;

using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(string assetPath);
    virtual int init(); //Remember init is called automatically
    virtual int move();
    virtual void cleanUp();
    int distanceBetween(Actor* a, Actor* b);
    int distanceBetween(Actor* a, int x, int y);
    virtual ~StudentWorld();
    bool playerWouldBeBlocked(int x, int y);
    bool actorWouldBeBlocked(Actor *a, int x, int y);
    bool collisionCheck();
    int nearestZombieDistance(Citizen* cit);
    int nearestZombieDistance(int x, int y);
    bool shouldVomitBeMade(int x, int y);
    Actor* neareastPerson(Actor*);
    void addActor(Actor* a);
    bool canFlameExist(Flame *flame);

    int getCitizenCount(){
        return citizenCount;
    }
    void decreaseCitizenCount(){
        citizenCount--;
    }
    void setNextRound(){
        nextRound = true;
    }
    Player* getPlayer(){
        return player;
    }
private:
    string assetPath;
    bool playerAlive = true;
    int citizenCount = 0;
    list<Actor*> actors; //All actors, including the player
    Player* player;      //Specific player pointer needed for stats
    bool nextRound = false;
};

#endif // STUDENTWORLD_H_
