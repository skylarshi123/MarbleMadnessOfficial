
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
    int getHealth()const;
    StudentWorld* getWorld();
    virtual bool isBlocking(int direction);
    virtual bool blockRobot() const;
    virtual bool isPushableInto() const;
    void setDead();
    bool isDead() const;
    virtual int getAmmo() const;
    virtual void decrementAmmo();
    virtual void recieveAmmoCrate();
    void restoreHealth();
    void extraLife();
    virtual bool isConsumable() const { return false;}
    virtual bool canSteal() const {return false;}
    virtual bool isEnemy() const  {return false;}
    virtual bool hiddenExistance() const {return false;}
    virtual void takeDamage() {}
    virtual bool canTakeDamage() const{return false;}
    virtual bool canBeShot() const{return false;}
    virtual bool blockRobotVision() const{return false;}
    void setRobbed(bool robbed) {this->isRobbed = robbed;}
    bool getRobbed() {return isRobbed;}
    void changeCoordinates(int &predX, int &predY, int direction);
private:
    StudentWorld* m_world;
    bool m_dead = false;
    int m_health = 999;
    bool isRobbed = false;
};

// Avatar class declaration
class Avatar : public Actor {
public:
    Avatar(double startX, double startY, StudentWorld* world);
    virtual void doSomething(); // Implementation of Avatar's actions
    virtual bool blockRobot() const ;
    virtual int getAmmo() const;
    virtual void recieveAmmoCrate();
    virtual void decrementAmmo();
    virtual bool canTakeDamage() const{return true;}
    virtual bool canBeShot() const{return true;}
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
    virtual bool blockRobot() const ;
    virtual bool canBeShot() const{return true;}
    virtual bool blockRobotVision() const{return true;}
};

//Marble class declaration
class Marble : public Actor {
public:
    Marble(double startX, double startY, StudentWorld* world);
    virtual void doSomething();
    virtual bool isBlocking(int direction);
    virtual bool blockRobot() const ;
    virtual void takeDamage() {decHealth(2);}
    virtual bool canBeShot() const{return true;}
    virtual bool canTakeDamage() const{return true;}
    virtual bool blockRobotVision() const{return true;}
};

//Pit class declaration
class Pit : public Actor {
public:
    Pit(double startX, double startY, StudentWorld* world);
    virtual void doSomething();
    virtual bool isBlocking(int direction);
    virtual bool isPushableInto() const;
    virtual bool blockRobot() const ;

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
    virtual bool hiddenExistance() const {return true;}
private:
    bool m_isOpen = false;
};

class Goodies : public Actor {
public:
    Goodies(double startX, double startY, StudentWorld* world, int IID);
    virtual void doSomething() = 0; // Pure virtual function
    virtual bool isConsumable() const { return true;}
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
    virtual bool isEnemy() const {return true;}
    virtual bool blockRobot() const ;
    int getTick() ;
    void setTick(int tick);
    virtual void takeDamage();
    virtual bool canTakeDamage() const{return true;}
    virtual bool canBeShot() const{return true;}
    virtual bool blockRobotVision() const{return true;}
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
    virtual bool canSteal() const {return true;}
    virtual void takeDamage();
    void takeDamageHelper(int increaseScore);
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
    virtual bool blockRobot() const  {return true;}
    virtual bool canBeShot() const{return true;}
    virtual bool blockRobotVision() const{return true;}
private:
    int m_meaness;
};

class Bullet: public Actor {
public:
    Bullet(double startX, double startY, StudentWorld* world, int direction);
    virtual void doSomething();
};

#endif // ACTOR_H_
