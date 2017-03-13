#include "bot.h"
#include "time.h"
#include "pathfind.h"

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
}

StrayCat::~StrayCat()
{
	delete(
		&m_rand,
		&m_moveTarget,
		&dir,
		&m_enemyInitPos,
		&m_enemyCurrPos
		);
}

void StrayCat::init(const BotInitialData &initialData, BotAttributes &attrib)
{
	m_initialData = initialData;
	attrib.health=0.3;
	attrib.motor=0.25;
	attrib.weaponSpeed=0.225;
	attrib.weaponStrength=0.225;
	m_moveTarget.set(m_rand() % (m_initialData.mapData.width - 2) + 1.5, m_rand() % (m_initialData.mapData.width - 2) + 1.5);
	dir.set(1, 0);
	map.clear();
	int yay = map.m_nodes.size();
}

void StrayCat::update(const BotInput &input, BotOutput27 &output)
{
	//openFieldAction(input, output);
	mazeAction(input, output);
}

void StrayCat::result(bool won)
{
}

void StrayCat::bulletResult(bool hit)
{

}

//Actions for open field map
void StrayCat::openFieldAction(const BotInput &input, BotOutput27 &output) {
	seenEnemy = false;

	output.moveDirection = m_moveTarget - input.position;
	if (output.moveDirection.length() < 2)
	{
		m_moveTarget.set(m_rand() % (m_initialData.mapData.width - 2) + 1.5, m_rand() % (m_initialData.mapData.width - 2) + 1.5);
	}
	output.motor = 1.0;

	//If the scanning outpur returns more than one object, scan through them
	if (input.scanResult.size() > 0) {
		for (int i = 0; i < input.scanResult.size(); ++i) {
			//if the scanned object is an enemy change some varables
			if (input.scanResult[i].type == VisibleThing::e_robot) {
				m_enemyCurrPos = input.scanResult[i].position;
				burstCount = m_updateCount + 4;
				seenEnemy = true;
			}
		}
	}

	if (seenEnemy) {

		estimatedEnemyPosition = m_enemyCurrPos;
		//Predict the enemys position some time in the future
		if (m_enemyUpdateCount>-1)
		{
			kf::Vector2 delta = m_enemyCurrPos - m_enemyInitPos;
			estimatedEnemyPosition = m_enemyCurrPos + (delta / (m_updateCount - m_enemyUpdateCount)) * 4;
		}

		// Shooting
		output.lookDirection = estimatedEnemyPosition - input.position;
		output.action = BotOutput::shoot;
	}
	else {
		if (m_updateCount <= burstCount) {
			// Shooting
			output.lookDirection = estimatedEnemyPosition - input.position;
			output.action = BotOutput::shoot;
		}
		else {
			m_lookAngle -= m_initialData.scanFOV * 2;
			output.lookDirection.set(cos(m_lookAngle), sin(m_lookAngle));

			//Output scan
			output.action = BotOutput::scan;
		}
	}

	gifNo = 3;

	output.spriteFrame = gifNo;

	if (seenEnemy)
	{
		m_enemyUpdateCount = m_updateCount;
		m_enemyInitPos = m_enemyCurrPos;
	}
	m_updateCount++;
}

void StrayCat::mazeAction(const BotInput &input, BotOutput27 &output) {
	seenEnemy = false;

	if (initPass || destinationReached) {
		m_currentNode = NodePos(m_rand() % (map.m_width - 2) + 1, m_rand() % (map.m_height - 2) + 1);
		
		if (initPass)
			initPass = false;
	}

	//Find a random node and set the destination node to that node
	openList.push_back(m_currentNode);
	m_smallestFNode = m_currentNode;
	map.getNode(m_smallestFNode).state = Node::NodeState::StateOpen;

	while (openList.size() >= 1 && !pathFound) {
		for (auto it = openList.begin(); it != openList.end(); ++it) {
			if (map.getNode(*it).f <= map.getNode(m_smallestFNode).f) {
				m_smallestFNode = *it;
			}
		}

		for (int oy = -1; oy <= 2; ++oy) {
			for (int ox = -1; ox <= 2; ++ox) {
				if (ox == 0 && oy == 0)
					continue;		

				NodePos adj = NodePos(m_currentNode.x + ox, m_enemyCurrPos.y + oy);
				int m_newG = m_g + map.getNode(adj).c;
				if (map.getNode(adj).state == Node::NodeState::StateClosed) {
					continue;
				}
				else if (map.getNode(adj).state == Node::NodeState::StateOpen && m_newG < map.getNode(adj).g) {
					map.getNode(adj).g = m_newG;
					map.getNode(adj).h = abs(adj.x - m_destination.x) + abs(adj.y - m_destination.y);
					map.getNode(adj).parent = m_currentNode;
					map.getNode(adj).f = map.getNode(adj).g + map.getNode(adj).h;
				}
				else if (map.getNode(adj).state == Node::NodeState::StateNone) {
					map.getNode(adj).g = m_newG;
					map.getNode(adj).h = abs(adj.x - m_destination.x) + abs(adj.y - m_destination.y);
					map.getNode(adj).parent = m_currentNode;
					map.getNode(adj).f = map.getNode(adj).g + map.getNode(adj).h;
					map.getNode(adj).state = Node::NodeState::StateOpen;
					openList.push_back(adj);
				}
				map.getNode(m_currentNode).state = Node::NodeState::StateClosed;
				openList.erase(itPos);
				if (adj.x == m_destination.x && adj.y == m_destination.y) {
					pathFound = true;
				}
			}
		}
	}

	//Set Sprite
	gifNo = 3;
	output.spriteFrame = gifNo;

	//Update Required vars if seen
	if (seenEnemy)
	{
		m_enemyUpdateCount = m_updateCount;
		m_enemyInitPos = m_enemyCurrPos;
	}
	m_updateCount++;
}