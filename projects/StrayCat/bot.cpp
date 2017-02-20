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
	output.text.clear();
	char buf[100];
	output.action = BotOutput::scan;
	//Can and probably should be replaced with more controlled pathing
	output.moveDirection = m_moveTarget - input.position;
	if (output.moveDirection.length() < 2)
	{
		m_moveTarget.set(m_rand() % (m_initialData.mapData.width - 2) + 1.5, m_rand() % (m_initialData.mapData.width - 2) + 1.5);
	}
	output.motor = 1.0;

	//Changing behaviour based on state
	switch (currentState) {
		case ms_Wander:
			//Things Stray Cat will perform in the wandering state
		{
			//If the result from the scan comes back with something in it, check what it is
			if (input.scanResult.size() > 0) {
				for (int i = 0; i <= input.scanResult.size(); ++i) {
					if (input.scanResult[i].type == VisibleThing::e_robot) {
						m_enemyInitPos = input.scanResult[i].position;
						output.lookDirection = m_enemyInitPos - input.position;
						break;
					}
				}
			}


		}
		break;

		case ms_EnemySeen:
		//Things Stray Cat will perform in the enemy seen state
		{
			output.lookDirection = m_enemyInitPos - input.position;
		}
		break;

		case ms_Chase:
		//Things Stray Cat will perform in the chase state
		{

		}
		break;
	}

	sprintf(buf, "%d", input.health);
	output.text.push_back(TextMsg(buf, input.position - kf::Vector2(0.0f, 1.0f), 0.0f, 0.7f, 1.0f,80));

	output.spriteFrame = gifNo;
}

void StrayCat::result(bool won)
{
}

void StrayCat::bulletResult(bool hit)
{

}