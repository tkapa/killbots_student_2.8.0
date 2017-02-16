#include "bot.h"
#include "time.h"

extern "C"
{
	BotInterface27 BOT_API *CreateBot27()
	{
		return new StrayCat();
	}
}

StrayCat::StrayCat()
{
	m_rand(time(0)+(int)this);
	currentState = ms_Wander;
}

StrayCat::~StrayCat()
{
}

void StrayCat::init(const BotInitialData &initialData, BotAttributes &attrib)
{
	m_initialData = initialData;
	attrib.health=1.0;
	attrib.motor=1.0;
	attrib.weaponSpeed=1.0;
	attrib.weaponStrength=1.0;
	//dir.set(m_rand.norm()*2.0 - 1.0, m_rand.norm()*2.0 - 1.0);
	m_moveTarget.set(m_rand() % (m_initialData.mapData.width - 2) + 1.5, m_rand() % (m_initialData.mapData.width - 2) + 1.5);
	dir.set(1, 0);
}

void StrayCat::update(const BotInput &input, BotOutput27 &output)
{
	//Changing behaviour based on state
	switch (currentState) {
	case ms_Wander:
	//Movement in wandering state
	{
		//Can and probably should be replaced with more controlled pathing
		output.moveDirection = m_moveTarget - input.position;
		if (output.moveDirection.length() < 2)
		{
			m_moveTarget.set(m_rand() % (m_initialData.mapData.width - 2) + 1.5, m_rand() % (m_initialData.mapData.width - 2) + 1.5);
		}
		output.motor = 1.0;
	}

	//Looking in wandering state
	{
		output.lookDirection.set(0, 1);

		if (input.scanResult.size >= 0) {
			for (int i = 0; i < input.scanResult.size; ++i) {
				if (input.scanResult.size[i].type == VisibleThing::e_robot) {
					//enemyInitPosition = &input.scanResult.size[i].position;
					currentState = ms_EnemySeen;
				}
			}
		}

		output.action = BotOutput::scan;
	}
	break;

	case ms_EnemySeen:
	{
		output.lookDirection.set(enemyInitPosition);
	}
	break;

	case ms_Alerted:
	{

	}
	break;

	case ms_Chase:
	{

	}
	break;

	}

	output.text.clear();
	char buf[100];
	sprintf(buf, "%d", input.health);
	output.text.push_back(TextMsg(buf, input.position - kf::Vector2(0.0f, 1.0f), 0.0f, 0.7f, 1.0f,80));

	//Change sprite
	SpriteChange();
	output.spriteFrame = gifNo;
}

void StrayCat::result(bool won)
{
}

void StrayCat::bulletResult(bool hit)
{

}

//Alter the current sprite based upon state
void StrayCat::SpriteChange(){
	switch (currentState) {
	case ms_Wander:
		gifNo = 6;
		break;

	case ms_Alerted:
		gifNo = 3;
		break;

	case ms_Chase:
		gifNo = 4;
		break;
	}
}
