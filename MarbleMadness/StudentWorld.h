#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <vector>

class Actor;

class StudentWorld : public GameWorld {
public:
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool canMoveTo(double x, double y);
    virtual ~StudentWorld();
    Actor* getActor(int x, int y, Actor* actor) const;
    Actor* getActorThatCanBeShot(int x, int y, Actor* actor) const;
    int getCountThiefBot(int x, int y) const;
    void createActor(Actor *actor);
    void createThiefBot(int type, int x, int y);
    Actor* getAvatar() const;
    int getNumOfCrystals() const;
    void changeNumOfCrystals(int num);
    void setBonusPoints();
    int getBonusPoints(){return m_BonusPoints;}
    void finishedLevel();
    void createBullet(int x, int y, int direction);

private:
    int loadLevel();
    void setDisplayText();
    std::string someFunctionToFormatThingsNicely(int score, int level, int lives, int health, int ammo, unsigned int bonus);
    int numOfCrystals = 0;
    std::vector<Actor*> m_actors;
    Actor* m_avatar;
    int m_BonusPoints = 1000;
    bool m_levelComplete = false;
};

#endif // STUDENTWORLD_H_
