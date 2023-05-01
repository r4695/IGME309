#include "Octant.h"
using namespace BTX;
//  Octant
uint Octant::m_uOctantCount = 0;
uint Octant::m_uMaxLevel = 3;
uint Octant::m_uIdealEntityCount = 5;
Octant::Octant(uint a_nMaxLevel, uint a_nIdealEntityCount)
{
	/*
	* This constructor is meant to be used ONLY on the root node, there is already a working constructor
	* that will take a size and a center to create a new space
	*/
	Init();//Init the default values
	m_uOctantCount = 0;
	m_uMaxLevel = a_nMaxLevel;
	m_uIdealEntityCount = a_nIdealEntityCount;
	m_uID = m_uOctantCount;

	m_pRoot = this;
	m_lChild.clear();

	//create a rigid body that encloses all the objects in this octant, it necessary you will need
	//to subdivide the octant based on how many objects are in it already an how many you IDEALLY
	//want in it, remember each subdivision will create 8 children for this octant but not all children
	//of those children will have children of their own

	//getting max and mins for the overall rigid body
	std::vector<vector3> lMinMax;
	vector3 curMin = vector3(0.0f), curMax = vector3(0.0f);
	
	for (uint i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	{
		vector3 indexMin = m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetMinGlobal();
		vector3 indexMax = m_pEntityMngr->GetEntity(i)->GetRigidBody()->GetMaxGlobal();

		//Gives the root all of the entities within it
		m_EntityList.push_back(i);

		//getting min
		if (curMin.x > indexMin.x)
			curMin.x = indexMin.x;
		if (curMin.y > indexMin.y)
			curMin.y = indexMin.y;
		if (curMin.z > indexMin.z)
			curMin.z = indexMin.z;
		//getting max
		if (curMax.x < indexMax.x)
			curMax.x = indexMax.x;
		if (curMax.y < indexMax.y)
			curMax.y = indexMax.y;
		if (curMax.z < indexMax.z)
			curMax.z = indexMax.z;
	}
	lMinMax.push_back(curMin);
	lMinMax.push_back(curMax);
	RigidBody pRigidBody = RigidBody(lMinMax);


	//The following will set up the values of the octant, make sure the are right, the rigid body at start
	//is NOT fine, it has made-up values
	m_fSize = pRigidBody.GetHalfWidth().x * 2.0f;
	m_v3Center = pRigidBody.GetCenterLocal();
	m_v3Min = m_v3Center - pRigidBody.GetHalfWidth();
	m_v3Max = m_v3Center + pRigidBody.GetHalfWidth();

	m_uOctantCount++; //When we add an octant we increment the count
	ConstructTree(m_uMaxLevel); //Construct the children

}

bool Octant::IsColliding(uint a_uRBIndex)
{
	//Get how many objects there are in the world
	//If the index given is larger than the number of elements in the bounding object there is no collision
	//As the Octree will never rotate or scale this collision is as easy as an Axis Alligned Bounding Box
	//Get all vectors in global space (the octant ones are already in Global)

	if (a_uRBIndex > m_pEntityMngr->GetEntityCount())
	{
		return false;
	}

	bool bColliding = true;
	
	if (m_v3Min.x > m_pEntityMngr->GetEntity(a_uRBIndex)->GetRigidBody()->GetMaxGlobal().x)
		bColliding = false;
	if (m_v3Max.x < m_pEntityMngr->GetEntity(a_uRBIndex)->GetRigidBody()->GetMinGlobal().x)
		bColliding = false;
	if (m_v3Min.y > m_pEntityMngr->GetEntity(a_uRBIndex)->GetRigidBody()->GetMaxGlobal().y)
		bColliding = false;
	if (m_v3Max.y < m_pEntityMngr->GetEntity(a_uRBIndex)->GetRigidBody()->GetMinGlobal().y)
		bColliding = false;
	if (m_v3Min.z > m_pEntityMngr->GetEntity(a_uRBIndex)->GetRigidBody()->GetMaxGlobal().z)
		bColliding = false;
	if (m_v3Max.z < m_pEntityMngr->GetEntity(a_uRBIndex)->GetRigidBody()->GetMinGlobal().z)
		bColliding = false;

	return bColliding;
}
void Octant::Display(uint a_nIndex, vector3 a_v3Color)
{
	if (a_nIndex > m_uOctantCount - 1)
		return;

	// Display the specified octant
	if(a_nIndex == m_uID)
	{
		m_pModelMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) *
			glm::scale(vector3(m_fSize)), a_v3Color);
	}
	else//If the octant is not found
	{
		//If there are children
		if (m_uChildren == 8)
		{
			for (int i = 0; i < m_uChildren; i++)
			{
				//Calls display on all children
				m_pChild[i]->Display();
			}
		}
	}
}
void Octant::Display(vector3 a_v3Color)
{
	//this is meant to be a recursive method, in starter code will only display the root
	//even if other objects are created

	//displays this octant
	m_pModelMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) *
		glm::scale(vector3(m_fSize)), a_v3Color);

	//If there are children
	if (m_uChildren == 8)
	{
		for (int i = 0; i < m_uChildren; i++)
		{
			//Calls display on all children
			m_pChild[i]->Display();
		}
	}
}
void Octant::Subdivide(void)
{
	//If this node has reach the maximum depth return without changes
	if (m_uLevel >= m_uMaxLevel)
		return;

	//If this node has been already subdivided return without changes
	if (m_uChildren != 0)
		return;

	//Subdivide the space and allocate 8 children

	//If this octant does not contain enough entities within it returns without subdividing
	if (!ContainsAtLeast(m_uIdealEntityCount))
		return;

	//if all checks are passed creates all the children
	m_uChildren = 8;
	m_pChild[0] = new Octant(vector3(m_v3Center.x + (m_fSize / 4), m_v3Center.y + (m_fSize / 4), m_v3Center.z + (m_fSize / 4)), m_fSize/2);
	m_pChild[1] = new Octant(vector3(m_v3Center.x - (m_fSize / 4), m_v3Center.y + (m_fSize / 4), m_v3Center.z + (m_fSize / 4)), m_fSize / 2);
	m_pChild[2] = new Octant(vector3(m_v3Center.x - (m_fSize / 4), m_v3Center.y + (m_fSize / 4), m_v3Center.z - (m_fSize / 4)), m_fSize / 2);
	m_pChild[3] = new Octant(vector3(m_v3Center.x + (m_fSize / 4), m_v3Center.y + (m_fSize / 4), m_v3Center.z - (m_fSize / 4)), m_fSize / 2);
	m_pChild[4] = new Octant(vector3(m_v3Center.x + (m_fSize / 4), m_v3Center.y - (m_fSize / 4), m_v3Center.z + (m_fSize / 4)), m_fSize / 2);
	m_pChild[5] = new Octant(vector3(m_v3Center.x - (m_fSize / 4), m_v3Center.y - (m_fSize / 4), m_v3Center.z + (m_fSize / 4)), m_fSize / 2);
	m_pChild[6] = new Octant(vector3(m_v3Center.x - (m_fSize / 4), m_v3Center.y - (m_fSize / 4), m_v3Center.z - (m_fSize / 4)), m_fSize / 2);
	m_pChild[7] = new Octant(vector3(m_v3Center.x + (m_fSize / 4), m_v3Center.y - (m_fSize / 4), m_v3Center.z - (m_fSize / 4)), m_fSize / 2);



	//For each child
	for (int i = 0; i < m_uChildren; i++)
	{
		//Sets parent
		m_pChild[i]->m_pParent = this;
		//Sets root
		m_pChild[i]->m_pRoot = this;
		//Sets level
		m_pChild[i]->m_uLevel = m_uLevel + 1;

		//adds each entity that is colliding with the child to the child's entity list
		//only checks the entities that the parent is colliding with
		for (uint j = 0; j < m_EntityList.size(); j++)
		{
			if (m_pChild[i]->IsColliding(m_EntityList[j]))
			{
				m_pChild[i]->m_EntityList.push_back(m_EntityList[j]);
			}
		}

		//once the childrens entity list has been populated, clears the parents list
		//this->m_EntityList.clear();

		//then subdivies each child
		m_pChild[i]->Subdivide();
	}

}
bool Octant::ContainsAtLeast(uint a_nEntities)
{
	//You need to check how many entity objects live within this octant

	if (m_EntityList.size() > a_nEntities)
	{
		return false;
	}
	else
	{
		return true;
	}
}
void Octant::AssignIDtoEntity(void)
{
	//Recursive method
	//Have to traverse the tree and make sure to tell the entity manager
	//what octant (space) each object is at
	//m_pEntityMngr->AddDimension(0, m_uID);//example only, take the first entity and tell it its on this space

	//Adds the id of this octant to each entity that collides with this octant
	for (uint i = 0; i < m_EntityList.size(); i++)
	{
		if (IsColliding(m_EntityList[i]))
		{
			m_pEntityMngr->AddDimension(i, m_uID);
		}
	}

	//Only if there are children
	if (m_uChildren == 8)
	{
		//will call assign on each child octant
		for (int i = 0; i < m_uChildren; i++)
		{
			m_pChild[i]->AssignIDtoEntity();
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------
// You can assume the following is fine and does not need changes, you may add onto it but the code is fine as is
// in the proposed solution.
void Octant::Init(void)
{
	m_uChildren = 0;

	m_fSize = 0.0f;

	m_uID = m_uOctantCount;
	m_uLevel = 0;

	m_v3Center = vector3(0.0f);
	m_v3Min = vector3(0.0f);
	m_v3Max = vector3(0.0f);

	m_pModelMngr = ModelManager::GetInstance();
	m_pEntityMngr = EntityManager::GetInstance();

	m_pRoot = nullptr;
	m_pParent = nullptr;
	for (uint n = 0; n < 8; n++)
	{
		m_pChild[n] = nullptr;
	}
}
void Octant::Swap(Octant& other)
{
	std::swap(m_uChildren, other.m_uChildren);

	std::swap(m_fSize, other.m_fSize);
	std::swap(m_uID, other.m_uID);
	std::swap(m_pRoot, other.m_pRoot);
	std::swap(m_lChild, other.m_lChild);

	std::swap(m_v3Center, other.m_v3Center);
	std::swap(m_v3Min, other.m_v3Min);
	std::swap(m_v3Max, other.m_v3Max);

	m_pModelMngr = ModelManager::GetInstance();
	m_pEntityMngr = EntityManager::GetInstance();

	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_pParent, other.m_pParent);
	for (uint i = 0; i < 8; i++)
	{
		std::swap(m_pChild[i], other.m_pChild[i]);
	}
}
void Octant::Release(void)
{
	//this is a special kind of release, it will only happen for the root
	if (m_uLevel == 0)
	{
		KillBranches();
	}
	m_uChildren = 0;
	m_fSize = 0.0f;
	m_EntityList.clear();
	m_lChild.clear();
}
void Octant::ConstructTree(uint a_nMaxLevel)
{
	//If this method is tried to be applied to something else
	//other than the root, don't.
	if (m_uLevel != 0)
		return;

	m_uMaxLevel = a_nMaxLevel; //Make sure you mark which is the maximum level you are willing to reach
	m_uOctantCount = 1;// We will always start with one octant

	//If this was initialized before make sure its empty
	m_EntityList.clear();//Make sure the list of entities inside this octant is empty
	KillBranches();
	m_lChild.clear();

	//If we have more entities than those that we ideally want in this octant we subdivide it
	if (ContainsAtLeast(m_uIdealEntityCount))
	{
		Subdivide();
	}
	AssignIDtoEntity();//Add octant ID to Entities
	ConstructList();//construct the list of objects
}
//The big 3
Octant::Octant(vector3 a_v3Center, float a_fSize)
{
	//Will create the octant object based on the center and size but will not construct children
	Init();
	m_v3Center = a_v3Center;
	m_fSize = a_fSize;

	m_v3Min = m_v3Center - (vector3(m_fSize) / 2.0f);
	m_v3Max = m_v3Center + (vector3(m_fSize) / 2.0f);

	m_uOctantCount++;
}
Octant::Octant(Octant const& other)
{
	m_uChildren = other.m_uChildren;
	m_v3Center = other.m_v3Center;
	m_v3Min = other.m_v3Min;
	m_v3Max = other.m_v3Max;

	m_fSize = other.m_fSize;
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_pParent = other.m_pParent;

	m_pRoot, other.m_pRoot;
	m_lChild, other.m_lChild;

	m_pModelMngr = ModelManager::GetInstance();
	m_pEntityMngr = EntityManager::GetInstance();

	for (uint i = 0; i < 8; i++)
	{
		m_pChild[i] = other.m_pChild[i];
	}
}
Octant& Octant::operator=(Octant const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		Octant temp(other);
		Swap(temp);
	}
	return *this;
}
Octant::~Octant() { Release(); };
//Accessors
float Octant::GetSize(void) { return m_fSize; }
vector3 Octant::GetCenterGlobal(void) { return m_v3Center; }
vector3 Octant::GetMinGlobal(void) { return m_v3Min; }
vector3 Octant::GetMaxGlobal(void) { return m_v3Max; }
uint Octant::GetOctantCount(void) { return m_uOctantCount; }
bool Octant::IsLeaf(void) { return m_uChildren == 0; }
Octant* Octant::GetParent(void) { return m_pParent; }
//--- other methods
Octant * Octant::GetChild(uint a_nChild)
{
	//if its asking for more than the 8th children return nullptr, as we don't have one
	if (a_nChild > 7) return nullptr;
	return m_pChild[a_nChild];
}
void Octant::KillBranches(void)
{
	/*This method has recursive behavior that is somewhat hard to explain line by line but
	it will traverse the whole tree until it reaches a node with no children and
	once it returns from it to its parent it will kill all of its children, recursively until
	it reaches back the node that called it.*/
	for (uint nIndex = 0; nIndex < m_uChildren; nIndex++)
	{
		m_pChild[nIndex]->KillBranches();
		delete m_pChild[nIndex];
		m_pChild[nIndex] = nullptr;
	}
	m_uChildren = 0;
}
void Octant::DisplayLeaves(vector3 a_v3Color)
{
	/*
	* Recursive method
	* it will traverse the tree until it find leaves and once found will draw them
	*/
	uint nLeafs = m_lChild.size(); //get how many children this will have (either 0 or 8)
	for (uint nChild = 0; nChild < nLeafs; nChild++)
	{
		m_lChild[nChild]->DisplayLeaves(a_v3Color);
	}
	//Draw the cube
	m_pModelMngr->AddWireCubeToRenderList(glm::translate(IDENTITY_M4, m_v3Center) *
		glm::scale(vector3(m_fSize)), a_v3Color);
}
void Octant::ClearEntityList(void)
{
	/*
	* Recursive method
	* It will traverse the tree until it find leaves and once found will clear its content
	*/
	for (uint nChild = 0; nChild < m_uChildren; nChild++)
	{
		m_pChild[nChild]->ClearEntityList();
	}
	m_EntityList.clear();
}
void Octant::ConstructList(void)
{
	/*
	* Recursive method
	* It will traverse the tree adding children
	*/
	for (uint nChild = 0; nChild < m_uChildren; nChild++)
	{
		m_pChild[nChild]->ConstructList();
	}
	//if we find a non-empty child add it to the tree
	if (m_EntityList.size() > 0)
	{
		m_pRoot->m_lChild.push_back(this);
	}
}
