// Actor.h
#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld; // Forward declaration

// Actor class declaration
class Actor : public GraphObject {
public:
    Actor(int imageID, double startX, double startY, int startDirection, StudentWorld* world, int health);
    virtual void doSomething() = 0; // Pure virtual function
    void decHealth(int decrement);
    int getHealth();
    StudentWorld* getWorld() ;
    virtual bool isBlocking(int direction);
    virtual bool blockRobot() ;
    virtual bool isPushableInto() ;
    void setDead();
    bool isDead() ;
    virtual int getAmmo() ;
    virtual void decrementAmmo();
    virtual void recieveAmmoCrate();
    void restoreHealth();
    void extraLife();
    virtual bool isConsumable() { return false;}
    virtual bool canSteal() {return false;}
    virtual bool isEnemy()  {return false;}
    virtual bool hiddenExistance() {return false;}
    virtual void takeDamage() {}
    virtual bool canTakeDamage(){return false;}
    virtual bool canBeShot(){return false;}
    virtual bool blockRobotVision(){return false;}
private:
    StudentWorld* m_world;
    bool m_dead = false;
    int m_health = 999;
};

// Avatar class declaration
class Avatar : public Actor {
public:
    Avatar(double startX, double startY, StudentWorld* world);
    virtual void doSomething(); // Implementation of Avatar's actions
    virtual bool blockRobot() ;
    virtual int getAmmo() ;
    virtual void recieveAmmoCrate();
    virtual void decrementAmmo();
    virtual bool canTakeDamage(){return true;}
    virtual bool canBeShot(){return true;}
    virtual void takeDamage();
private:
    int m_ammo = 20;
};

//Wall class declaration
class Wall : public Actor {
public:
    Wall(double startX, double startY, StudentWorld* world);
    virtual void doSomething();
    virtual bool isBlocking(int direction);
    virtual bool blockRobot() ;
    virtual bool canBeShot(){return true;}
    virtual bool blockRobotVision(){return true;}
};

//Marble class declaration
class Marble : public Actor {
public:
    Marble(double startX, double startY, StudentWorld* world);
    virtual void doSomething();
    virtual bool isBlocking(int direction);
    virtual bool blockRobot() ;
    virtual void takeDamage() {decHealth(2);}
    virtual bool canBeShot(){return true;}
    virtual bool canTakeDamage(){return true;}
    virtual bool blockRobotVision(){return true;}
};

//Pit class declaration
class Pit : public Actor {
public:
    Pit(double startX, double startY, StudentWorld* world);
    virtual void doSomething();
    virtual bool isBlocking(int direction);
    virtual bool isPushableInto() ;
    virtual bool blockRobot() ;

};

class Crystal : public Actor {
public:
    Crystal(double startX, double startY, StudentWorld* world);
    virtual void doSomething();
};

class Exit : public Actor {
public:
    Exit(double startX, double startY, StudentWorld* world);
    virtual void doSomething();
    virtual bool hiddenExistance() {return true;}
};

class Goodies : public Actor {
public:
    Goodies(double startX, double startY, StudentWorld* world, int IID);
    virtual void doSomething() = 0; // Pure virtual function
    virtual bool isConsumable() { return true;}
};

class RestoreHealthGoodies : public Goodies {
public:
    RestoreHealthGoodies(double startX, double startY, StudentWorld* world);
    virtual void doSomething();
};

class AmmoGoodies : public Goodies {
public:
    AmmoGoodies(double startX, double startY, StudentWorld* world);
    virtual void doSomething();
};

class ExtraLifeGoodies : public Goodies {
public:
    ExtraLifeGoodies(double startX, double startY, StudentWorld* world);
    virtual void doSomething();
};

class Robot : public Actor {
public:
    Robot(double startX, double startY, StudentWorld* world, int IID, int direction, int health);
    virtual void doSomething() = 0; // Pure virtual function
    bool isBlocking(int direction);
    virtual bool isEnemy() {return true;}
    virtual bool blockRobot() ;
    int getTick() ;
    void setTick(int tick);
    virtual void takeDamage();
    virtual bool canTakeDamage(){return true;}
    virtual bool canBeShot(){return true;}
    virtual bool blockRobotVision(){return true;}
    void executeShot();
private:
    bool shootPlayer();
    int m_tick;
};

class RageBot : public Robot {
public:
    RageBot(double startX, double startY, StudentWorld* world, int direction);
    void flipDirection(int direction);
    virtual void doSomething() = 0;
private:
    int direction;
};

class HorizontalRageBot : public RageBot {
public:
    HorizontalRageBot(double startX, double startY, StudentWorld* world);
    virtual void doSomething();
};

class VerticalRageBot : public RageBot {
public:
    VerticalRageBot(double startX, double startY, StudentWorld* world);
    virtual void doSomething();
};

class ThiefBot : public Robot {
public:
    ThiefBot(double startX, double startY, StudentWorld* world, int health, int image);
    virtual void doSomething();
    virtual bool canSteal() {return true;}
    virtual void takeDamage();
    bool getHasPickedUpGoodie(){return hasPickedUpGoodie;}
    Actor* getGoodie() {return goodie;}

private:
    int distanceBeforeTurning;
    bool hasPickedUpGoodie;
    Actor* goodie;
};

class MeanThiefBot : public ThiefBot {
public:
    MeanThiefBot(double startX, double startY, StudentWorld* world);
    virtual void doSomething();
    virtual void takeDamage();
};

class RobotFactory: public Actor {
public:
    RobotFactory(double startX, double startY, StudentWorld* world, int type);
    virtual void doSomething();
    virtual bool isBlocking(int direction){return true;}
    virtual bool blockRobot()  {return true;}
    virtual bool canBeShot(){return true;}
    virtual bool blockRobotVision(){return true;}
private:
    int m_type;
};

class Bullet: public Actor {
public:
    Bullet(double startX, double startY, StudentWorld* world, int direction);
    virtual void doSomething();
};

#endif // ACTOR_H_
