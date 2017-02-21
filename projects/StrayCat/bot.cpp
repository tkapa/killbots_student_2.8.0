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
				m_enemyCurrPos = input.scanResult[i].position;
				seenEnemy = true;
				break;
			}
		}			
	}

	if (seenEnemy) {
		//Try to shoot and stuff.
		kf::Vector2 estimatedEnemyPosition = m_enemyCurrPos;
		if (m_enemyUpdateCount>-1)
		{
			kf::Vector2 delta = m_enemyCurrPos - m_enemyInitPos;
			estimatedEnemyPosition = m_enemyCurrPos + (delta / (m_updateCount - m_enemyUpdateCount)) * 5;
			Line l;
			l.start = m_enemyCurrPos;
			l.end = estimatedEnemyPosition;
			l.r = 1;
			l.g = 1;
			l.b = 1;
			output.lines.push_back(l);
		}
		// Shooting
		output.lookDirection = estimatedEnemyPosition - input.position;
		output.moveDirection = output.lookDirection;
		m_moveTarget = estimatedEnemyPosition;
		output.action = BotOutput::shoot;
		m_lookAngle -= m_initialData.scanFOV * 3;
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

	output.spriteFrame = gifNo;

	if (seenEnemy)
	{
		m_enemyUpdateCount = m_updateCount;
		m_enemyInitPos = m_enemyCurrPos;
	}
	m_updateCount++;
}

void StrayCat::result(bool won)
{
}

void StrayCat::bulletResult(bool hit)
{

}