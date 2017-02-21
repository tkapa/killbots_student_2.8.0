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
	attrib.health=0.1;
	attrib.motor=0.3;
	attrib.weaponSpeed=0.2;
	attrib.weaponStrength=0.4;
	//dir.set(m_rand.norm()*2.0 - 1.0, m_rand.norm()*2.0 - 1.0);
	m_moveTarget.set(m_rand() % (m_initialData.mapData.width - 2) + 1.5, m_rand() % (m_initialData.mapData.width - 2) + 1.5);
	dir.set(1, 0);
}

void StrayCat::update(const BotInput &input, BotOutput27 &output)
{
	seenEnemy = false;
	
	//Changing behaviour based on state
	//Things Stray Cat will perform in the wandering state		
	//Movement in wander state
	output.moveDirection = m_moveTarget - input.position;
	if (output.moveDirection.length() < 2)
				{
					m_moveTarget.set(m_rand() % (m_initialData.mapData.width - 2) + 1.5, m_rand() % (m_initialData.mapData.width - 2) + 1.5);
				}
		output.motor = 1.0;

	if (input.scanResult.size() > 0){
		for (int i = 0; i < input.scanResult.size(); ++i){
			if (input.scanResult[i].type == VisibleThing::e_robot){
				m_enemyInitPos = input.scanResult[i].position;
				seenEnemy = true;
				break;
			}
		}			
	}

	if (seenEnemy) {

	}
	else {
		// Scanning
		m_lookAngle += m_initialData.scanFOV * 2;
		output.lookDirection.set(cos(m_lookAngle), sin(m_lookAngle));
		output.action = BotOutput::scan;
	}


	gifNo = 3;
		
	//output.text.clear();
	//char buf[100];
	//sprintf(buf, "%d", input.health);
	//output.text.push_back(TextMsg(buf, input.position - kf::Vector2(0.0f, 1.0f), 0.0f, 0.7f, 1.0f,80));

	if (seenEnemy) {
		m_enemyInitPos = m_enemyCurrPos;
	}
	output.spriteFrame = gifNo;
}

void StrayCat::result(bool won)
{
}

void StrayCat::bulletResult(bool hit)
{

}