#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include "Actor.h"
#include "Level.h"
#include <sstream>
#include <iomanip>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

int StudentWorld::init()
{
    loadLevel();
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you type q
    if(m_levelComplete){
        m_levelComplete = false;
        increaseScore(2000);
        resetBonusPoints();
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    if(getAvatar()->isDead()) {
        numOfCrystals = 0;
        return GWSTATUS_PLAYER_DIED;
    }
    setBonusPoints();
    setDisplayText();
    
//    for(vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end();it++){
//        //will auto check to see if dead, will only do something if alive
//        if((*it)==nullptr)cout<<"balls";
//        else if((*it)!=nullptr){
//            (*it)->doSomething();
//        }
//    }
    int n = m_actors.size();

    for (int i = 0; i < n; i++) {
        if (m_actors[i] == nullptr) cout << "balls";
        else m_actors[i]->doSomething();
    }
    
    //delete if dead
    for(vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end();){
        if((*it)->isDead()){
            delete (*it);
            it = m_actors.erase(it);
        }
        else {
            it++;
        }
    }
    m_avatar->doSomething();

    
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() {
    delete m_avatar;
    m_avatar = nullptr;
    // Use an iterator to delete and remove actors from the vector
    for(vector<Actor*>::iterator it = m_actors.begin();it!=m_actors.end();){
        delete(*it);
        it = m_actors.erase(it);
    }
}

StudentWorld::~StudentWorld(){
    cleanUp();
}



int StudentWorld::loadLevel()
{
    stringstream next;
    next.fill('0');
    next << setw(2) << getLevel();
    string levelnum = next.str();
    
    string curLevel = "level" + levelnum + ".txt";
    Level lev(assetPath());
    Level::LoadResult result = lev.loadLevel(curLevel);
    if (result == Level::load_fail_file_not_found || levelnum == "100") //if file not found or at level 100 then you won
        {
            return GWSTATUS_PLAYER_WON;
        }

        if (result == Level::load_fail_bad_format)
        {
            return GWSTATUS_LEVEL_ERROR; // something bad happened!
        }
if (result == Level::load_fail_file_not_found || result == Level:: load_fail_bad_format)
    return -1; // something bad happened!
// otherwise the load was successful and you can access the
// contents of the level – here’s an example

    //iterate through the level to find player
    for(int x = 0; x < VIEW_WIDTH; x++)
    {
        for(int y = 0; y < VIEW_HEIGHT; y++)
        {
            Level::MazeEntry item = lev.getContentsOf(x, y); //get what is at this position
            if (item == Level::player) m_avatar = new Avatar(x, y, this);
            if(item == Level::wall) m_actors.push_back(new Wall(x,y, this));
            if(item == Level::marble) m_actors.push_back(new Marble(x, y, this));
            if(item == Level::pit) m_actors.push_back(new Pit(x, y, this));
            if(item == Level::crystal) {
                m_actors.push_back(new Crystal(x,y,this));
                changeNumOfCrystals(1);
            }
            if(item == Level::exit) m_actors.push_back(new Exit(x,y, this));
            if(item == Level::ammo) m_actors.push_back(new AmmoGoodies(x, y, this));
            if(item == Level::restore_health) m_actors.push_back(new RestoreHealthGoodies(x, y, this));
            if(item == Level::extra_life) m_actors.push_back(new ExtraLifeGoodies(x, y, this));
            if(item == Level::vert_ragebot) m_actors.push_back(new VerticalRageBot(x, y, this));
            if(item == Level::horiz_ragebot) m_actors.push_back(new HorizontalRageBot(x,y, this));
            if(item == Level::thiefbot_factory) m_actors.push_back(new RobotFactory(x, y, this, 0));
            if(item == Level::mean_thiefbot_factory) m_actors.push_back(new RobotFactory(x, y, this, 1));

        }
    }
//Level::MazeEntry item = lev.getContentsOf(x, y);
//if (item == Level::player)
//std::cout << "The player should be placed at 0,5 in the maze\n";
//x = 10;
//y = 7;
//item = lev.getContentsOf(x, y);
//if (item == Level::wall)
//    std::cout << "There should be a wall at 10,7 in the maze\n";
    return 1;
}

// Implementation of getActor
Actor* StudentWorld::getActor(int x, int y, Actor* actor) const{
    //prioritize getting the robots so I don't go through them
    for (vector<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); ++it) {
        if ((*it)->getX() == x && (*it)->getY() == y && (*it) != actor && (*it)->hiddenExistance()) {//visible means thief bot did not pick up, careful about the exit which is false
            // Found an actor at the specified (x, y) location
            return *it;
        }
    }
    for (vector<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); ++it) {
        if ((*it)->getX() == x && (*it)->getY() == y && (*it) != actor && (*it)->isVisible() && (*it)->isEnemy()) {//visible means thief bot did not pick up, careful about the exit which is false
            // Found an actor at the specified (x, y) location
            return *it;
        }
    }
    for (vector<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); ++it) {
        if ((*it)->getX() == x && (*it)->getY() == y && (*it) != actor && (*it)->isVisible()) {//visible means thief bot did not pick up
            // Found an actor at the specified (x, y) location
            return *it;
        }
    }
    // No actor found at the specified (x, y) location
    return nullptr;
}

Actor* StudentWorld::getActorThatCanBeShot(int x, int y, Actor* actor) const{
    //prioritize getting the things that can take Damage
    
    if(m_avatar->getX() == x && m_avatar->getY() == y)
    {
        return m_avatar;
    }
    
    for (vector<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); ++it) {
        if ((*it)->getX() == x && (*it)->getY() == y && (*it) != actor && (*it)->canTakeDamage()) {
            return *it;
        }
    }
    //priortize getting the things that can be Shot
    for (vector<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); ++it) {
        if ((*it)->getX() == x && (*it)->getY() == y && (*it) != actor && (*it)->canBeShot()) {
            return *it;
        }
    }
    //get literaly anything else
    for (vector<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); ++it) {
        if ((*it)->getX() == x && (*it)->getY() == y && (*it) != actor && (*it)->isVisible()) {
            return *it;
        }
    }
    // No actor found at the specified (x, y) location
    return nullptr;
}

int StudentWorld::getCountThiefBot(int x, int y) const{
    int count = 0;
    for (vector<Actor*>::const_iterator it = m_actors.begin(); it != m_actors.end(); ++it) {
        if ((*it)->getX() == x && (*it)->getY() == y && (*it)->getWorld()->getAvatar()->canSteal()) {
            // Found an actor at the specified (x, y) location
            count++;
        }
    }
    // No actor found at the specified (x, y) location
    return count;
}


void StudentWorld::createActor(Actor* actor){
    m_actors.push_back(actor);
}

Actor* StudentWorld::getAvatar() const{
    return m_avatar;
}

int StudentWorld::getNumOfCrystals() const{
    return numOfCrystals;
}

void StudentWorld::changeNumOfCrystals(int num){
    numOfCrystals+=num;
}

void StudentWorld::setDisplayText()
{
 int score = getScore();
 int level = getLevel();
// unsigned int bonus = getBonus();
 int lives = getLives();
 // Next, create a string from your statistics, of the form:
 // Score: 0000100 Level: 03 Lives: 3 Health: 70% Ammo: 216 Bonus: 34
    int health = getAvatar()->getHealth();
    int ammo = getAvatar()->getAmmo();
    int bonus = m_BonusPoints;
 string s = someFunctionToFormatThingsNicely(score, level, lives, health, ammo, bonus);
// Finally, update the display text at the top of the screen with your
// newly created stats
 setGameStatText(s); // calls our provided GameWorld::setGameStatText
}

string StudentWorld::someFunctionToFormatThingsNicely(int score, int level, int lives, int health, int ammo, unsigned int bonus){
    ostringstream oss;
    oss.fill('0');
    oss << "Score: " << setw(7) << score;
    oss << "  Level: " << setw(2) << level;
    oss.fill(' ');
    oss << "  Lives: " << setw(2) << lives << "  Health: " << setw(3) << health <<"%" << "  Ammo: " << setw(3) << ammo << setw(4) << "  Bonus: "<<bonus;
    return oss.str();
}
void StudentWorld::resetBonusPoints(){
    m_BonusPoints = 1000;
}

void StudentWorld::setBonusPoints(){
    if(m_BonusPoints == 0)return;
    m_BonusPoints--;
}

void StudentWorld::finishedLevel(){
    m_levelComplete = true;
}

void StudentWorld::createThiefBot(int type, int x, int y){
    if(type==0){ //thief bot
        m_actors.push_back(new ThiefBot(x, y, this, 5, IID_THIEFBOT));
    } //else mean thief bot
    else {
        m_actors.push_back(new MeanThiefBot(x, y, this));
    }
}

void StudentWorld::createBullet(int x, int y, int direction){
    m_actors.push_back(new Bullet(x, y, this, direction));
}
