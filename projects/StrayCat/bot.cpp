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

	int yay = map.m_nodes.size();
}

void StrayCat::update(const BotInput &input, BotOutput27 &output)
{
	openFieldAction(input, output);
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

	/*
		ManhattanDistance = abs(node.x-destination.x)+ abs(node.y-destination.y);
		ChebychevDistance = max(abs(node.x-destination.x), abs(node.y-destination.y));
		EuclideanDistance = (node-dest).magnitude();

	//Node Member Details
		F – Total cost of the node (G+H)
		G – Cost of the path from the start so far
		H – Heuristic cost of the remaining unexplored path to the destination
		C – Cost to move onto this node
		Wall – bool to say if the node is blocked
		Parent – The node we came from

		StateOpen - Node that has potential for the path
		StateClosed - Nodes that shouldn't be used for this path again
		StateNone - a node that hasn;t been encountered yet.

		- Add the destination node to the list

	//Process
		- Find the smallest f value, made this the current node
		- For each walkable node (not a wall)
			- If the node is StateClosed, skip it
			- If the node's G > G+C
				- Make G = G+C
				- Make the parent of that node the current node
			-If the node is not on either list G = G+C && parent the current node to the assessed node
		- Remove the current node from the open list and put it on the closed list

		- Repeat the process until the destination is reached
		- Translate given information into movement via parent nodes 
	*/

	//Find a random node and set the destination node to that node
	m_nodePos.set(m_rand() % (m_initialData.mapData.width - 2) + 1.5, m_rand() % (m_initialData.mapData.width - 2) + 1.5);

	if(m_initialData.mapData.data[(int)(m_nodePos.x*m_nodePos.y)].wall)
		m_nodePos.set(m_rand() % (m_initialData.mapData.width - 2) + 1.5, m_rand() % (m_initialData.mapData.width - 2) + 1.5);

	m_currentNode = map.getNode(m_nodePos);
	m_currentNode.state = Node::NodeState::StateOpen;

	for (int x = -1; x < 1; ++x) {
		for (int y = -1; y < 1; ++y) {
			//Offset to find the nodes around the currentNode
			kf::Vector2 pos = kf::Vector2(m_nodePos.x + x, m_nodePos.y + y);
			Node checkingNode = map.getNode(pos);

			//If the node is a wall
			if (m_initialData.mapData.data[(int)(pos.x*pos.y)].wall && checkingNode.state == Node::NodeState::StateNone)
				checkingNode.state = Node::NodeState::StateClosed;			
			else
				checkingNode.state = Node::NodeState::StateOpen;
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