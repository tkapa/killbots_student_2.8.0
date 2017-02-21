#ifndef BOT_H
#define BOT_H
#include "bot_interface.h"
#include "kf/kf_random.h"

#ifdef BOT_EXPORTS
#define BOT_API __declspec(dllexport)
#else
#define BOT_API __declspec(dllimport)
#endif

class StrayCat:public BotInterface27
{
public:
	//Governs bot's behaviour depending on state
	enum MyState {
		ms_Wander,
		ms_EnemySeen,
		ms_Chase
	};
	MyState currentState;

	StrayCat();
	virtual ~StrayCat();
	virtual void init(const BotInitialData &initialData, BotAttributes &attrib);
	virtual void update(const BotInput &input, BotOutput27 &output);
	virtual void result(bool won);
	virtual void bulletResult(bool hit);

	kf::Xor128 m_rand;
	kf::Vector2 m_moveTarget;
	kf::Vector2 dir;
	kf::Vector2 m_enemyInitPos; // The enemy's known position when spotted
	kf::Vector2 m_enemyCurrPos; // The enemy's known position
	BotInitialData m_initialData;

	//Essential
	bool seenEnemy = false;	//true if enemy is seen
	float m_lookAngle = 0;    //Angle Stray Cat will look;
	int fireCount = 4;		//Governs how many bullets Stray Cat can fire at once
	int burstCount = 0;		//Keeps track of how many bullets Stray Cat has fired within this burst
	int m_updateCount = 0; // Keeps track of how many update frames have passed
	int m_enemyUpdateCount = 0; //Notes when Stray Cat last saw the enemy
	double lookAngle = 0;

	//Aesthetic
	int gifNo = 0;		//Will change the sprite of the bot when wanted
};
#endif