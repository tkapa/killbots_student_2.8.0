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
	void wander(const BotInput &input, BotOutput &output);

	kf::Xor128 m_rand;
	kf::Vector2 m_moveTarget;
	kf::Vector2 dir;
	kf::Vector2 m_enemyInitPosition; // The enemy's known position when spotted
	BotInitialData m_initialData;

	double lookAngle = 0;

	//Will change the sprite of the bot when wanted
	int gifNo = 0;
};
#endif