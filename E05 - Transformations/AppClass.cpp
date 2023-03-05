#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";
	vector3 v3Position(0.0f, 0.0f, 10.0f);
	vector3 v3Target = ZERO_V3;
	vector3 v3Upward = AXIS_Y;
	m_pCameraMngr->SetPositionTargetAndUpward(v3Position, v3Target, v3Upward);

	//Allocate the memory for the Meshes
	m_pMesh = new MyMesh();
	m_pMesh->GenerateCube(1.0f, C_BLACK);
		
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Update Entity Manager
	m_pEntityMngr->Update();

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//Calculate the model, view and projection matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	matrix4 m4View = m_pCameraMngr->GetViewMatrix();

	matrix4 worldSpace = ToMatrix4(m_qArcBall);
	static float value = 0.0f;

	//column 0
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value, 0.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value, 1.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value, 2.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value, -1.0f, 0.0f)));

	//column -1
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value - 1.0f, 0.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value - 1.0f, 1.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value - 1.0f, 2.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value - 1.0f, -1.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value -1.0f, -3.0f, 0.0f)));
	//column -2
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value - 2.0f, 0.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value - 2.0f, 3.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value - 2.0f, 2.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value - 2.0f, -1.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value -2.0f, -3.0f, 0.0f)));
	//column -3
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value - 3.0f, 0.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value - 3.0f, 1.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value - 3.0f, 2.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value - 3.0f, 4.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value - 3.0f, -1.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value -3.0f, -2.0f, 0.0f)));
	//column -4
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value-4.0f, 0.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value-4.0f, 1.0f, 0.0f)));
	//column 5
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value-5.0f, 0.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value-5.0f, -1.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value-5.0f, -2.0f, 0.0f)));

	//column 1
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value + 1.0f, 0.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value + 1.0f, 1.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value + 1.0f, 2.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value + 1.0f, -1.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value + 1.0f, -3.0f, 0.0f)));
	//column 2
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value + 2.0f, 0.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value + 2.0f, 3.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value + 2.0f, 2.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value + 2.0f, -1.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value + 2.0f, -3.0f, 0.0f)));
	//column 3
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value + 3.0f, 0.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value + 3.0f, 1.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value + 3.0f, 2.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value + 3.0f, 4.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value + 3.0f, -1.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value + 3.0f, -2.0f, 0.0f)));
	//column 4
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value+4.0f, 0.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value+4.0f, 1.0f, 0.0f)));
	//column 5
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value+5.0f, 0.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value+5.0f, -1.0f, 0.0f)));
	m_pMesh->Render(m4Projection, m4View, glm::translate(worldSpace, vector3(value+5.0f, -2.0f, 0.0f)));


	value += 0.01f;

	// draw a skybox
	m_pModelMngr->AddSkyboxToRenderList();

	//render list call
	m_uRenderCallCount = m_pModelMngr->Render();

	//clear the render list
	m_pModelMngr->ClearRenderList();

	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//Release meshes
	SafeDelete(m_pMesh);

	//release GUI
	ShutdownGUI();
}