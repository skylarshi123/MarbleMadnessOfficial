// Actor.cpp
#include "Actor.h"
#include "StudentWorld.h"

StudentWorld* Actor::getWorld() {
    return m_world;
}
// Actor class implementation
Actor::Actor(int imageID, double startX, double startY, int startDirection, StudentWorld* world, int health)
    : GraphObject(imageID, startX, startY, startDirection) {
        m_world = world;
        m_health = health;
}

bool Actor::isBlocking(int direction){
    return false;
}

bool Actor::blockRobot() const{
    return false;
}

bool Actor::isPushableInto() const{
    return false;
}

void Actor::setDead(){
    m_dead = true;
}

bool Actor::isDead() const{
    return m_dead;
}

void Actor::recieveAmmoCrate(){}

void Actor::decrementAmmo(){}

void Actor::decHealth(int decrement){
    m_health-=decrement;
}

int Actor::getHealth() const{
    return m_health*5;
}

int Actor::getAmmo() const{
    return 0;
}

void Actor::extraLife(){
    getWorld()->incLives();
}

void Actor::restoreHealth(){
    m_health = 20;
}

// Avatar class implementation
Avatar::Avatar(double startX, double startY, StudentWorld* world)
    : Actor(IID_PLAYER, startX, startY, GraphObject::right, world, 20) {
        setVisible(true);
}

bool Avatar::blockRobot() const{
    return true;
}

int Avatar::getAmmo() const{
    return m_ammo;
}

void Avatar::decrementAmmo(){
    m_ammo--;
}

void Avatar::recieveAmmoCrate(){
    m_ammo = m_ammo + 20;
}

void Avatar::takeDamage(){
    decHealth(2);
    getWorld()->playSound(SOUND_PLAYER_IMPACT);
    if(getHealth()<=0) {
        getWorld()->decLives();
        setDead();
        getWorld()->playSound(SOUND_PLAYER_DIE);
    }
    
}

//move only if it empty or a non blocking actor
void Avatar::doSomething() {
    if(isDead()) return;
    int predX = this->getX();
    int predY = this->getY();
    Actor* predActor;
    int ch;
    if (getWorld()->getKey(ch)) {
        switch(ch) {
            case KEY_PRESS_LEFT:
                setDirection(left);
                predX-=1;
                predActor = getWorld()->getActor(predX, predY, this);
                if(predActor != nullptr && predActor->isBlocking(left)) break; //if not empty space and not blocking then you can move
                this->moveTo(predX, predY);
                break;
            case KEY_PRESS_RIGHT:
                setDirection(right);
                predX+=1;
                predActor = getWorld()->getActor(predX, predY, this);
                if(predActor != nullptr && predActor->isBlocking(right)) break;
                this->moveTo(predX, predY);
                break;
            case KEY_PRESS_UP:
                setDirection(up);
                predY+=1;
                predActor = getWorld()->getActor(predX, predY, this);
                if(predActor != nullptr && predActor->isBlocking(up)) break;
                this->moveTo(predX, predY);
                break;
            case KEY_PRESS_DOWN:
                setDirection(down);
                predY-=1;
                predActor = getWorld()->getActor(predX, predY, this);
                if(predActor != nullptr && predActor->isBlocking(down)) break;
                this->moveTo(predX, predY);
                break;
            case KEY_PRESS_SPACE: {
                if(this->getAmmo()<=0)break;
                getWorld()->playSound(SOUND_PLAYER_FIRE);
                this->decrementAmmo();
                int currDirection = this->getDirection();
                switch(currDirection){
                    case up:
                        predX = getX();
                        predY = getY()+1;
                        break;
                    case left:
                        predX = getX()-1;
                        predY = getY();
                        break;
                    case down:
                        predX = getX();
                        predY = getY()-1;
                        break;
                    case right:
                        predX = getX()+1;
                        predY = getY();
                        break;
                }
                getWorld()->createBullet(predX, predY, currDirection);
                break;
            }
            case KEY_PRESS_ESCAPE:
                getWorld()->decLives();
                setDead();
                break;
        }
    }
}

Wall::Wall(double startX, double startY, StudentWorld* world)
    : Actor(IID_WALL, startX, startY, GraphObject::none, world, 999) {
        setVisible(true);
}

bool Wall::isBlocking(int direction){
    return true;
}

bool Wall::blockRobot() const{
    return true;
}

void Wall::doSomething(){}

Marble::Marble(double startX, double startY, StudentWorld* world)
    : Actor(IID_MARBLE, startX, startY, GraphObject::none, world, 10) {
        setVisible(true);
}

//Marble is only Blocking if it can't be pushed into the right spot
bool Marble::isBlocking(int direction){
    int predX = this->getX();
    int predY = this->getY();
    Actor* predActor;
    switch(direction){
        case right:
            predX++;
            break;
        case left:
            predX--;
            break;
        case up:
            predY++;
            break;
        case down:
            predY--;
            break;
    }
    predActor = getWorld()->getActor(predX, predY, this);
    if(predActor != nullptr && predActor->isPushableInto() == false){
        return true;
    }
    this->moveTo(predX, predY);
    return false;
}

bool Marble::blockRobot() const{
    return true;
}

void Marble::doSomething(){
    if(isDead()) return;
    if(getHealth()<=0) setDead();
}


Pit::Pit(double startX, double startY, StudentWorld* world)
    : Actor(IID_PIT, startX, startY, GraphObject::none, world, 999) {
        setVisible(true);
}

bool Pit::isBlocking(int direction){
    return true;
}

bool Pit::blockRobot() const{
    return true;
}

void Pit::doSomething(){
    Actor* swallowedMarble;
    swallowedMarble = getWorld()->getActor(getX(), getY(), this);
    if (swallowedMarble==nullptr)return;
    if(swallowedMarble->blockRobot()){ //makes sure it's just marbles that die when pushed and not bullets
        swallowedMarble->setDead();
        this->setDead();
    }
}

bool Pit::isPushableInto() const{
    return true;
}

Crystal::Crystal(double startX, double startY, StudentWorld* world)
    : Actor(IID_CRYSTAL, startX, startY, GraphObject::none, world, 999) {
        setVisible(true);
}

void Crystal::doSomething(){
    Actor* avatarCollected;
    avatarCollected = getWorld()->getAvatar();
    if (avatarCollected->getX() == this->getX() && avatarCollected->getY() == this->getY()){
        this->setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->changeNumOfCrystals(-1);
        getWorld()->increaseScore(50);
    }
}

Exit::Exit(double startX, double startY, StudentWorld* world)
    : Actor(IID_EXIT, startX, startY, GraphObject::none, world, 999) {
        setVisible(false);
}


void Exit::doSomething(){
    if(isVisible()){
        if(getX() == getWorld()->getAvatar()->getX() && getY() == getWorld()->getAvatar()->getY()){ //if player is there
            getWorld()->increaseScore(getWorld()->getBonusPoints());
            getWorld()->playSound(SOUND_FINISHED_LEVEL);
            getWorld()->finishedLevel();
            return;
        }
        return;
    }
    else if(getWorld()->getNumOfCrystals() == 0){
        this->setVisible(true);
        getWorld()->playSound(SOUND_REVEAL_EXIT);
    }
    else  {
        return;
    }
//    Actor* avatarCollected;
//    avatarCollected = getWorld()->getAvatar();
//    if (avatarCollected->getX() == this->getX() && avatarCollected->getY() == this->getY()){
//
//}
}

Goodies::Goodies(double startX, double startY, StudentWorld* world, int IID)
    : Actor(IID, startX, startY, GraphObject::none, world, 999) {
        setVisible(true);
}

RestoreHealthGoodies::RestoreHealthGoodies(double startX, double startY, StudentWorld* world) : Goodies(startX, startY, world, IID_RESTORE_HEALTH) {
}
void RestoreHealthGoodies::doSomething(){
    if(isDead()) return;
    if(getX() == getWorld()->getAvatar()->getX() && getY() == getWorld()->getAvatar()->getY() && isVisible()){
        getWorld()->getAvatar()->restoreHealth();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        setDead();
        getWorld()->increaseScore(500);

    }
}

AmmoGoodies::AmmoGoodies(double startX, double startY, StudentWorld* world) : Goodies(startX, startY, world, IID_AMMO) {
}
void AmmoGoodies::doSomething(){
    if(isDead()) return;
    if(getX() == getWorld()->getAvatar()->getX() && getY() == getWorld()->getAvatar()->getY() &&isVisible()){
        getWorld()->getAvatar()->recieveAmmoCrate();
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(100);
    }
}

ExtraLifeGoodies::ExtraLifeGoodies(double startX, double startY, StudentWorld* world)
: Goodies(startX, startY, world, IID_EXTRA_LIFE)
{
}
void ExtraLifeGoodies::doSomething(){
    if(isDead()) return;
    if(getX() == getWorld()->getAvatar()->getX() && getY() == getWorld()->getAvatar()->getY() &&isVisible()){
        getWorld()->getAvatar()->extraLife();
        setDead();
        getWorld()->playSound(SOUND_GOT_GOODIE);
        getWorld()->increaseScore(1000);
    }
}

Robot::Robot(double startX, double startY, StudentWorld* world, int IID, int direction, int health)
    : Actor(IID, startX, startY, direction, world, health) {
        setVisible(true);
}

bool Robot::isBlocking(int direction){
    return true;
}

bool Robot::blockRobot() const{
    return true;
}

void Robot::takeDamage() {
    decHealth(2);
    getWorld()->playSound(SOUND_ROBOT_IMPACT);
    if(getHealth()<=0) {
        setDead();
        getWorld()->increaseScore(100);
        
        getWorld()->playSound(SOUND_ROBOT_DIE);
    }
}

bool Robot::shootPlayer(){
    int currentDirection = getDirection();
    int robotX = getX();
    int robotY = getY();
    Actor* predActor;
    for(int i = 0;i<14;i++){
        //get all the
        switch(currentDirection){
            case up:
                robotY++;
                break;
            case left:
                robotX--;
                break;
            case right:
                robotX++;
                break;
            case down:
                robotY--;
                break;
        }
        //call function with this x and y to see if something is there in the way
        predActor = getWorld()->getActorThatCanBeShot(robotX, robotY, this);
        if(predActor == getWorld()->getAvatar())return true;
        if(predActor != nullptr){
            if(getWorld()->getActor(robotX, robotY, this)->canBeShot()) return false;
        }
    }
    return true;
}

void Robot::executeShot(){
    if(this->getTick() == 1) {
        int currDirection = this->getDirection();
        int predX = getX();
        int predY = getY();
        if(Robot::shootPlayer()){
            switch(currDirection){
                case up:
                    predX = getX();
                    predY = getY()+1;
                    break;
                case left:
                    predX = getX()-1;
                    predY = getY();
                    break;
                case down:
                    predX = getX();
                    predY = getY()-1;
                    break;
                case right:
                    predX = getX()+1;
                    predY = getY();
                    break;
            }
            getWorld()->createBullet(predX, predY, currDirection);
            getWorld()->playSound(SOUND_ENEMY_FIRE);
            int levelNumber = getWorld()->getLevel();
            this->setTick((28 - levelNumber) / 4);
            return;
        }
    }
}

RageBot::RageBot(double startX, double startY, StudentWorld* world, int direction)
    : Robot(startX, startY, world, IID_RAGEBOT, direction, 10)
{
    int levelNumber = getWorld()->getLevel();
    setTick((28 - levelNumber) / 4);
}

int Robot::getTick() {
    return m_tick;
}

void Robot::setTick(int tick){
    m_tick = tick;
}

void RageBot::flipDirection(int direction){
    int currDirection = this->getDirection();
    if(currDirection==right){
        this->setDirection(left);
    }
    else if(currDirection==left){
        this->setDirection(right);
    }
    else if(currDirection==up){
        this->setDirection(down);
    }
    else{
        this->setDirection(up);
    }
}

void RageBot::doSomething(){
    if(isDead()) return;
    Robot::executeShot();
    if(this->getTick() == 1) {
        Actor* predActor;
        Actor* Avatar;
        int currDirection = this->getDirection();
        int predX = getX();
        int predY = getY();
        Avatar = getWorld()->getAvatar();
        int avatarX = Avatar->getX();
        int avatarY = Avatar->getY();
        switch(currDirection){
            case(right):
                predX++;
                predActor = getWorld()->getActor(predX, predY, this);
                if(predActor==nullptr && (avatarX != predX || avatarY != predY)){
                    moveTo(predX, predY);
                    break;
                }
                else if(predActor!=nullptr && predActor->blockRobot() == false && (avatarX != predX || avatarY != predY)) {
                    moveTo(predX, predY);
                    break;
                }
                else this->flipDirection(currDirection);
                break;
            case(left):
                predX--;
                predActor = getWorld()->getActor(predX, predY, this);
                if(predActor==nullptr && (avatarX != predX || avatarY != predY)){
                    moveTo(predX, predY);
                    break;
                }
                else if(predActor!=nullptr && predActor->blockRobot() == false && (avatarX != predX || avatarY != predY)){
                    moveTo(predX, predY);
                    break;
                }
                else this->flipDirection(currDirection);
                break;
            case(up):
                predY++;
                predActor = getWorld()->getActor(predX, predY, this);
                if(predActor==nullptr && (avatarX != predX || avatarY != predY)){
                    moveTo(predX, predY);
                    break;
                }
                else if(predActor!=nullptr && predActor->blockRobot() == false && (avatarX != predX || avatarY != predY)) {
                    moveTo(predX, predY);
                    break;
                }
                else this->flipDirection(currDirection);
                break;
            case(down):
                predY--;
                predActor = getWorld()->getActor(predX, predY, this);
                if(predActor==nullptr && (avatarX != predX || avatarY != predY)){
                    moveTo(predX, predY);
                    break;
                }
                else if(predActor!=nullptr && predActor->blockRobot() == false && (avatarX != predX || avatarY != predY)){
                    moveTo(predX, predY);
                    break;
                }
                else this->flipDirection(currDirection);
                break;
        }
        int levelNumber = getWorld()->getLevel();
        this->setTick((28 - levelNumber) / 4);
    }
    else {
        this->setTick((this->getTick())-1);
    }
}

HorizontalRageBot::HorizontalRageBot(double startX, double startY, StudentWorld* world)
: RageBot(startX, startY, world, right) {
    
}
//flip direction if blocked, otherwise move in same trajectory
void HorizontalRageBot::doSomething(){
    RageBot::doSomething();
}

VerticalRageBot::VerticalRageBot(double startX, double startY, StudentWorld* world)
: RageBot(startX, startY, world, down) {
    
}
//same as horizontal but vertical
void VerticalRageBot::doSomething(){
    RageBot::doSomething();
}

ThiefBot::ThiefBot(double startX, double startY, StudentWorld* world, int health, int image) :
    Robot(startX, startY, world, image, right, health){
        distanceBeforeTurning = randInt(1, 6);
        hasPickedUpGoodie = false;
        int levelNumber = getWorld()->getLevel();
        this->setTick((28 - levelNumber) / 4);
};

void ThiefBot::doSomething(){
    if(isDead()) return;
    if(this->getTick() == 1) {
        Actor* predActor;
        Actor* Avatar;
        int currDirection = this->getDirection();
        int predX = getX();
        int predY = getY();
        Avatar = getWorld()->getAvatar();
        int avatarX = Avatar->getX();
        int avatarY = Avatar->getY();
        int levelNumber = getWorld()->getLevel();
//        Implementation for if robot on goodie
        predActor = getWorld()->getActor(getX(), getY(), this);
        if(predActor!=nullptr && predActor->isConsumable() && !hasPickedUpGoodie){
            int random = randInt(1,10);
            if(random==1){
                getWorld()->playSound(SOUND_ROBOT_MUNCH);
                hasPickedUpGoodie = true;
                goodie = getWorld()->getActor(getX(), getY(), this);
                if(goodie->isVisible()) goodie->setVisible(false); //goodie should be invisible
                this->setTick((28 - levelNumber) / 4);
                return;
            }
        }
        
        if(distanceBeforeTurning > 0){
            distanceBeforeTurning--;
            switch(currDirection){
                case up:
                    predY++;
                    break;
                case down:
                    predY--;
                    break;
                case left:
                    predX--;
                    break;
                case right:
                    predX++;
                    break;
            }
            predActor = getWorld()->getActor(predX, predY, this);
            if(predActor==nullptr && (avatarX != predX || avatarY != predY)){
                moveTo(predX, predY);
                this->setTick((28 - levelNumber) / 4);
                return;
            }
            if (predActor!=nullptr && predActor->blockRobot() == false && (avatarX != predX || avatarY != predY)) {
                moveTo(predX, predY);
                this->setTick((28 - levelNumber) / 4);
                return;
            }
            //if not blocked or distanceBeforeTurning is 0, this should not be ran
        }
        //otherwise blocked or distanceBeforeTurning = 0,
        distanceBeforeTurning = randInt(1,6);
        int randomDirection = randInt(1,4);
        switch(randomDirection){
            case 1:
                currDirection = right;
                break;
            case 2:
                currDirection = left;
                break;
            case 3:
                currDirection = up;
                break;
            case 4:
                currDirection = down;
                break;
        }
        setDirection(currDirection);
        for(int i = 1; i<=4; i++){
            switch(currDirection){
                case up:
                    predX = getX();
                    predY = getY()+1;
                    break;
                case left:
                    predX = getX()-1;
                    predY = getY();
                    break;
                case down:
                    predX = getX();
                    predY = getY()-1;
                    break;
                case right:
                    predX = getX()+1;
                    predY = getY();
                    break;
            }
            
            predActor = getWorld()->getActor(predX, predY, this);
            if(predActor==nullptr && (avatarX != predX || avatarY != predY)){
                distanceBeforeTurning--;
                setDirection(currDirection);
                moveTo(predX, predY);
                break;
            }
            if (predActor!=nullptr && predActor->blockRobot() == false && (avatarX != predX || avatarY != predY)) {
                distanceBeforeTurning--;
                setDirection(currDirection);
                moveTo(predX, predY);
                break;
            }
            
            //what if it's still blocked?
            
            switch(currDirection){
                case up:
                    std::cerr<<"right";
                    currDirection = left;
                    break;
                case left:
                    std::cerr<<"left";
                    currDirection = down;
                    break;
                case down:
                    std::cerr<<"up";
                    currDirection = right;
                    break;
                case right:
                    std::cerr<<"down";
                    currDirection = up;
                    break;
            }
            setDirection(currDirection);
        }
        this->setTick((28 - levelNumber) / 4);
    }
    else {
        this->setTick((this->getTick())-1);
    }
}

MeanThiefBot::MeanThiefBot(double startX, double startY, StudentWorld* world) : ThiefBot(startX, startY, world, 8, IID_MEAN_THIEFBOT){
};

void MeanThiefBot::doSomething(){
    Robot::executeShot();
    ThiefBot::doSomething();
}

RobotFactory::RobotFactory(double startX, double startY, StudentWorld* world, int type): Actor(IID_ROBOT_FACTORY, startX, startY, GraphObject::none, world, 999) {
    m_type = type;
    //Get bounds of factory (0 - 14) for both sides x & y
    setVisible(true);
}
void RobotFactory::doSomething()
{
    int amtThiefBots = 0;
    int x = getX();
    int y = getY();
    int lowerBound = y - 3;
    int upperBound = y + 3;
    int leftBound = x - 3;
    int rightBound = x + 3;

    if (lowerBound < 0) lowerBound = 0;
    if (upperBound > 14) upperBound = 14;
    if (leftBound < 0) leftBound = 0;
    if (rightBound > 14) rightBound = 14;
    for (int k = leftBound; k <= rightBound; k++) //check each square in 3x3 radius of factory to check for thief bots (<= since bound is 14 or 0)
    {
        for (int n = lowerBound; n <= upperBound; n++)
        {
            if (getWorld()->getActor(k,n,this)!=nullptr && getWorld()->getActor(k,n, this)->canSteal()) //if find thief bot is not nullptr that there is a thief bot at current position
            {
                amtThiefBots++;
            }
        }
    }

    //get coords of factory to check if there is currently a thief bot there
    if (amtThiefBots < 3 && getWorld()->getActor(x, y, this) == nullptr) //this means thief bot count in range is less than 3 and currently no thief bot on factory TO DO and robot count < 3
    {
        int rand = randInt(1, 50);
        if (rand == 1) //1 in 50 chance that the factory makes a thief bot
        {
            int type1 = m_type;
            getWorld()->playSound(SOUND_ROBOT_BORN);
            getWorld()->createThiefBot(type1, getX(), getY());
        }
    }
}

void ThiefBot::takeDamage() {
    decHealth(2);
    getWorld()->playSound(SOUND_ROBOT_IMPACT);
    if(getHealth()<=0){
        if(hasPickedUpGoodie){
            goodie->moveTo(getX(),getY());
            goodie->setVisible(true);
        }
        setDead();
        getWorld()->playSound(SOUND_ROBOT_DIE);
        getWorld()->increaseScore(10);
    }
}

void MeanThiefBot::takeDamage() {
    decHealth(2);
    getWorld()->playSound(SOUND_ROBOT_IMPACT);
    if(getHealth()<=0){
        if(getHasPickedUpGoodie()){
            getGoodie()->moveTo(getX(),getY());
            getGoodie()->setVisible(true);
        }
        setDead();
        getWorld()->playSound(SOUND_ROBOT_DIE);
        getWorld()->increaseScore(20);
    }
}

Bullet::Bullet(double startX, double startY, StudentWorld* world, int direction) : Actor(IID_PEA, startX, startY, direction, world, 999){
    setVisible(true);
}
void Bullet::doSomething(){
    //if dead return immediately
    if(isDead())return;
    Actor* predActor;
    Actor* Avatar = getWorld()->getAvatar();
    int predX = getX();
    int predY = getY();
    int currDirection = getDirection();
    int avatarX = getWorld()->getAvatar()->getX();
    int avatarY = getWorld()->getAvatar()->getY();
    //VERY IMPORTANT THAT IN THE ARRAY YOU PRIORITIZE GETTING THINGS THAT CAN TAKE DAMAGE FIRST, BC BOT ON FACTORY TAKES DAMAGE
    predActor = getWorld()->getActorThatCanBeShot(predX, predY, this);
    //if the predActor is something that can be hit, run this if statement
    if(avatarX == predX && avatarY == predY) {
        Avatar->takeDamage();
        setDead();
        return;
    }
    if(predActor != nullptr && predActor->canBeShot()){
        predActor->takeDamage();
        setDead();
        return;
    }
//    else pea can pas over it
    switch(currDirection){
        case up:
            predX = getX();
            predY = getY()+1;
            break;
        case left:
            predX = getX()-1;
            predY = getY();
            break;
        case down:
            predX = getX();
            predY = getY()-1;
            break;
        case right:
            predX = getX()+1;
            predY = getY();
            break;
    }
        moveTo(predX, predY);
        predActor = getWorld()->getActorThatCanBeShot(predX, predY, this);
        if(predActor != nullptr && predActor->canBeShot()){
            predActor->takeDamage();
            setDead();
        }
}
