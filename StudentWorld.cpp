#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h"
#include "Actor.h"
#include <list>
#include <string>
#include <iostream>
#include "math.h"
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    this->assetPath = assetPath;
}

int StudentWorld::init()
{
    int currentLevelInt = this->getLevel();
    string currentLevel = "level0" + to_string(currentLevelInt) + ".txt";
    Level lev(this->assetPath);
    Level::LoadResult result = lev.loadLevel(currentLevel);
    
    if (result == Level::load_fail_file_not_found)
        cerr << "Cannot find level01.txt data file" << endl;
    else if (result == Level::load_fail_bad_format)
        cerr << "Your level was improperly formatted" << endl;
    else if (result == Level::load_success)
    {
        //Add stuff
        for(int row = 0; row < VIEW_WIDTH; row++){
            for(int col = 0; col < VIEW_HEIGHT; col++){
                Level::MazeEntry e = lev.getContentsOf(row, col);
                
                switch(e){
                    case Level::empty:
                        break;
                    case Level::player:
                        player = new Player(SPRITE_WIDTH * row, SPRITE_HEIGHT * col, this);
                        actors.push_back(player);
                        break;
                    case Level::wall:
                        actors.push_back(new Wall(SPRITE_WIDTH * row, SPRITE_HEIGHT * col, this));
                        break;
                    case Level::exit:
                        actors.push_back(new Exit(SPRITE_WIDTH * row, SPRITE_WIDTH * col, this));
                        break;
                    case Level::pit:
                        actors.push_back(new Pit(SPRITE_WIDTH * row, SPRITE_WIDTH * col, this));
                        break;
                    case Level::dumb_zombie:
                        actors.push_back(new DumbZombie(SPRITE_WIDTH * row, SPRITE_WIDTH * col, this));
                        break;
                    case Level::smart_zombie:
                        actors.push_back(new SmartZombie(SPRITE_WIDTH * row, SPRITE_WIDTH * col, this));
                        break;
                    case Level::gas_can_goodie:
                        actors.push_back(new GasCanGoodie(SPRITE_WIDTH * row, SPRITE_WIDTH * col, this));
                        break;
                    case Level::vaccine_goodie:
                        actors.push_back(new VaccineGoodie(SPRITE_WIDTH * row, SPRITE_WIDTH * col, this));
                        break;
                    case Level::landmine_goodie:
                        actors.push_back(new LandmineGoodie(SPRITE_WIDTH * row, SPRITE_WIDTH * col, this));
                        break;
                    case Level::citizen:
                        actors.push_back(new Citizen(SPRITE_WIDTH * row, SPRITE_WIDTH * col, this));
                        citizenCount++;
                        break;
                    default:
                        break;
                }
            }
        }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    nextRound = false;
    //Have everything do something, like move
    list<Actor*>::iterator it = actors.begin();
    while(it != actors.end()){
        (*it)->doSomething();
        it++;
    }
    
    
    //********** Check for collisions*********//
    
    list<Actor*>::iterator a;
    list<Actor*>::iterator b;
    for(a = actors.begin(); a != actors.end(); a++){
        //Loops compare every actor against every other actor, twice in both situations
        for(b = actors.begin(); b != actors.end(); b++){
            if((*a) != (*b) && distanceBetween((*a), (*b)) <= 10){ //collision!
                
                //If its an activator call the activator function
                if(dynamic_cast<ActivatingObject*>(*a) != nullptr)
                    dynamic_cast<ActivatingObject*>(*a)->activate((*b));
                
                
                //Need to add zombie and citizen implementations
            }
        }
    }
    
    //Check if an exit was activated
    if(nextRound)
        return GWSTATUS_FINISHED_LEVEL; //Between an exit and a person
    
    //Remove dead stuff
    it = actors.begin();
    while(it != actors.end()){
        if(!(*it)->isAlive()){
            //Award points / sounds
            if(dynamic_cast<Player*>(*it) != nullptr){
                decLives();
                return GWSTATUS_PLAYER_DIED; //cleanup method will be called and delete player and everything else
            }
            delete (*it);
            actors.erase(it);
            it = actors.begin(); //lists can get out of order if you delete something, better to just start at beginning
        }
        
        it++;
    }
    
    setGameStatText(   "Score: " + to_string(getScore())
                    + "  Level: " + to_string(getLevel())
                    + "  Lives: " + to_string(getLives())
                    + "  Vacc: " + to_string(player->getVaccCount())
                    + "  Flames: " + to_string(player->getFlameCount())
                    + "  Mines: " + to_string(player->getLandMineCount())
                    + "  Infected: " + to_string(player->getInfectionCount()));
    
    return GWSTATUS_CONTINUE_GAME;
}



StudentWorld::~StudentWorld(){
    this->cleanUp();
}

void StudentWorld::cleanUp()
{
    list<Actor*>::iterator it = actors.begin(); //Free all actors
    while(it != actors.end()){
        delete *it;
        it++;
        actors.pop_front();
    }
}

//** HELPER FUNCTIONS FOR MOVEMENT **//

//Returns if an actor would be blocked, helpful for movement functions of agents!
bool StudentWorld::actorWouldBeBlocked(Actor* a, int x, int y){
    
    list<Actor*>::iterator it;
    it = actors.begin();
    while(it != actors.end())
    {
        if(a != (*it) && (*it)->doesBlockAgents()){
            
            if((y >= (*it)->getY() && //Player is going down into something
                (*it)->getY() + SPRITE_HEIGHT-1) >= y &&
               (*it)->getX() <= (x + SPRITE_WIDTH-1) &&
               ((*it)->getX() + SPRITE_WIDTH-1) >= x)
            {
                return true;
            }
            if(y <= ((*it)->getY() + SPRITE_HEIGHT-1)&&  //Player is going up into something
               (*it)->getY() <= y + SPRITE_HEIGHT-1 &&
               (*it)->getX() <= (x + SPRITE_WIDTH-1) &&
               ((*it)->getX() + SPRITE_WIDTH-1) >= x)
            {
                return true;
            }
            if(x <= ((*it)->getX() + SPRITE_WIDTH-1)&& //Player is going left into something
               (*it)->getX() <= x + SPRITE_WIDTH-1 &&
               ((*it)->getY() + SPRITE_HEIGHT-1) >= y&&
               (*it)->getY() <= (y + SPRITE_HEIGHT-1))
            {
                return true;
            }
            if(x >= (*it)->getX() && //Player is going right into something
               ((*it)->getX() + SPRITE_WIDTH-1) >= x &&
               ((*it)->getY() + SPRITE_HEIGHT-1) >= y &&
               (*it)->getY() <= (y + SPRITE_HEIGHT-1))
            {
                return true;
            }
        }
        it++;
    }
    return false;
}

//Let's a flame know if it can be made somewhere!
bool StudentWorld::canFlameExist(Flame* flame){
    list<Actor*>::iterator c;
    for(c = actors.begin(); c != actors.end(); c++){
        if((*c) != flame && distanceBetween((*c), flame) <= 10 && (*c)->doesBlockFlame()){ //collision!
            return false;
        }
    }
    return true;
}

//Find the distance between stuff!
int StudentWorld::distanceBetween(Actor* a, Actor* b){
    int aXcenter, aYcenter, bXcenter, bYcenter;
    aXcenter = a->getX() + (SPRITE_WIDTH / 2);
    aYcenter = a->getY() + (SPRITE_HEIGHT / 2);
    bXcenter = b->getX() + (SPRITE_WIDTH / 2);
    bYcenter = b->getY() + (SPRITE_HEIGHT / 2);
    
    int deltaX = aXcenter - bXcenter;
    int deltaY = aYcenter - bYcenter;
    return sqrt(pow(deltaX, 2) + pow(deltaY, 2));
}

int StudentWorld::distanceBetween(Actor* a, int x, int y){
    int aXcenter, aYcenter, bXcenter, bYcenter;
    aXcenter = a->getX() + (SPRITE_WIDTH / 2);
    aYcenter = a->getY() + (SPRITE_HEIGHT / 2);
    bXcenter = x + (SPRITE_WIDTH / 2);
    bYcenter = y + (SPRITE_HEIGHT / 2);
    
    int deltaX = aXcenter - bXcenter;
    int deltaY = aYcenter - bYcenter;
    return sqrt(pow(deltaX, 2) + pow(deltaY, 2));
}

//Find the distance of the nearest zombie for a citizen!

int StudentWorld::nearestZombieDistance(Citizen* cit){
    int nearestZombie = INT_MAX;
    list<Actor*>::iterator it;
    it = actors.begin();
    while(it != actors.end())
    {
        if(dynamic_cast<Zombie* >(*it) != nullptr){
            int d = distanceBetween((*it), cit);;
            if(d < nearestZombie)
                nearestZombie = d;
        }
        it++;
    }
  
    return nearestZombie;
}

int StudentWorld::nearestZombieDistance(int x, int y){
    int nearestZombie = INT_MAX;
    list<Actor*>::iterator it;
    it = actors.begin();
    while(it != actors.end())
    {
        if(dynamic_cast<Zombie* >(*it) != nullptr){
            int d = distanceBetween((*it), x, y);;
            if(d < nearestZombie)
                nearestZombie = d;
        }
        it++;
    }
    return nearestZombie;
}

bool StudentWorld::shouldVomitBeMade(int x, int y){
    list<Actor*>::iterator it;
    it = actors.begin();
    while(it != actors.end())
    {
        if((*it)->canItGetInfected() && distanceBetween((*it), x, y) <= 10)
            return true;
        it++;
    }
    return false;
}

Actor* StudentWorld::neareastPerson(Actor* zombie){
    Actor* p = player;
    int nearestDistance = INT_MAX;
    list<Actor*>::iterator it;
    it = actors.begin();
    while(it != actors.end())
    {
        int a = distanceBetween((*it), zombie);
        if(dynamic_cast<Person*>(*it) != nullptr && a <= nearestDistance){
            nearestDistance = a;
            p = (*it);
        }
        it++;
    }

    return p;
}

void StudentWorld::addActor(Actor *a){
    actors.push_back(a);
}
