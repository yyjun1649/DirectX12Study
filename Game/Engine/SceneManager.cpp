#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Engine.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"

#include "TestCameraScript.h"
#include "Resources.h"
#include "ParticleSystem.h"
#include "Terrain.h"
#include "SphereCollider.h"
#include "MeshData.h"
#include "TestDragon.h"


void SceneManager::Update()
{
	if (_activeScene == nullptr)
		return;

	_activeScene->Update();
	_activeScene->LateUpdate();
	_activeScene->FinalUpdate();
}

//Temp
void SceneManager::Render()
{
	if (_activeScene)
		_activeScene->Render();
}


void SceneManager::LoadScene(wstring sceneName)
{
	// TODO : ���� Scene ����
	// TODO : ���Ͽ��� Scene ���� �ε�

	_activeScene = LoadTestScene();

	_activeScene->Awake();
	_activeScene->Start();
}

void SceneManager::SetLayerName(uint8 index, const wstring& name)
{
	// ���� ������ ����
	const wstring& prevName = _layerNames[index];
	_layerIndex.erase(prevName);

	_layerNames[index] = name;
	_layerIndex[name] = index;
}

uint8 SceneManager::LayerNameToIndex(const wstring& name)
{
	auto findIt = _layerIndex.find(name);
	if (findIt == _layerIndex.end())
		return 0;

	return findIt->second;
}

shared_ptr<GameObject> SceneManager::Pick(int32 screenX, int32 screenY)
{
	shared_ptr<Camera> camera = GetActiveScene()->GetMainCamera();

	float width = static_cast<float>(GEngine->GetWindow().width);
	float height = static_cast<float>(GEngine->GetWindow().height);

	Matrix projectionMatrix = camera->GetProjectionMatrix();

	// ViewSpace���� Picking ����
	float viewX = (+2.0f * screenX / width - 1.0f) / projectionMatrix(0, 0);
	float viewY = (-2.0f * screenY / height + 1.0f) / projectionMatrix(1, 1);

	Matrix viewMatrix = camera->GetViewMatrix();
	Matrix viewMatrixInv = viewMatrix.Invert();

	auto& gameObjects = GET_SINGLE(SceneManager)->GetActiveScene()->GetGameObjects();

	float minDistance = FLT_MAX;
	shared_ptr<GameObject> picked;

	for (auto& gameObject : gameObjects)
	{
		if (gameObject->GetCollider() == nullptr)
			continue;

		// ViewSpace������ Ray ����
		Vec4 rayOrigin = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		Vec4 rayDir = Vec4(viewX, viewY, 1.0f, 0.0f);

		// WorldSpace������ Ray ����
		rayOrigin = XMVector3TransformCoord(rayOrigin, viewMatrixInv);
		rayDir = XMVector3TransformNormal(rayDir, viewMatrixInv);
		rayDir.Normalize();

		// WorldSpace���� ����
		float distance = 0.f;
		if (gameObject->GetCollider()->Intersects(rayOrigin, rayDir, OUT distance) == false)
			continue;

		if (distance < minDistance)
		{
			minDistance = distance;
			picked = gameObject;
		}
	}

	return picked;
}

shared_ptr<Scene> SceneManager::LoadTestScene()
{
#pragma region LayerMask
	SetLayerName(0, L"Default");
	SetLayerName(1, L"UI");
#pragma endregion

#pragma region ComputeShader
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"ComputeShader");

		// UAV �� Texture ����
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->CreateTexture(L"UAVTexture",
			DXGI_FORMAT_R8G8B8A8_UNORM, 1024, 1024,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"ComputeShader");
		material->SetShader(shader);
		material->SetInt(0, 1);
		GEngine->GetComputeDescHeap()->SetUAV(texture->GetUAVHandle(), UAV_REGISTER::u0);

		// ������ �׷� (1 * 1024 * 1)
		material->Dispatch(1, 1024, 1);
	}
#pragma endregion

	shared_ptr<Scene> scene = make_shared<Scene>();
	
#pragma region MakeTrangle Vertex Buffer

	/*
	vector<Vertex> vec(6);
	vec[0].pos = Vec3(-0.5f, 0.5f, 0.5f);
	vec[0].color = Vec4(1.f, 0.f, 0.f, 1.f);
	vec[1].pos = Vec3(0.5f, 0.5f, 0.5f);
	vec[1].color = Vec4(0.f, 1.0f, 0.f, 1.f);
	vec[2].pos = Vec3(0.5f, -0.5f, 0.5f);
	vec[2].color = Vec4(0.f, 0.f, 1.f, 1.f);

	vec[3].pos = Vec3(0.5f, -0.5f, 0.5f);
	vec[3].color = Vec4(0.f, 0.f, 1.f, 1.f);
	vec[4].pos = Vec3(-0.5f, -0.5f, 0.5f);
	vec[4].color = Vec4(0.f, 1.0f, 0.f, 1.f);
	vec[5].pos = Vec3(-0.5f, 0.5f, 0.5f);
	vec[5].color = Vec4(1.f, 0.f, 0.f, 1.f);

	mesh->Init(vec);

	shader->Init(L"..\\Resources\\Shader\\default.fx");
	*/
#pragma endregion
#pragma region MakeTrangle Index Buffer
	/*
	vector<Vertex> vec(4);
	vec[0].pos = Vec3(-0.5f, 0.5f, 0.5f);
	vec[0].color = Vec4(1.f, 0.f, 0.f, 1.f);

	vec[1].pos = Vec3(0.5f, 0.5f, 0.5f);
	vec[1].color = Vec4(0.f, 1.f, 0.f, 1.f);

	vec[2].pos = Vec3(0.5f, -0.5f, 0.5f);
	vec[2].color = Vec4(0.f, 0.f, 1.f, 1.f);

	vec[3].pos = Vec3(-0.5f, -0.5f, 0.5f);
	vec[3].color = Vec4(0.f, 1.f, 0.f, 1.f);

	vector<uint32> indexVec;
	{
		indexVec.push_back(0);
		indexVec.push_back(1);
		indexVec.push_back(2);
	}
	{
		indexVec.push_back(0);
		indexVec.push_back(2);
		indexVec.push_back(3);
	}

	mesh->Init(vec, indexVec);

	shader->Init(L"..\\Resources\\Shader\\default.fx");
	*/
#pragma endregion
#pragma region Texture Mapping
	/*
	vector<Vertex> vec(4);
	vec[0].pos = Vec3(-0.5f, 0.5f, 0.5f);
	vec[0].color = Vec4(1.f, 0.f, 0.f, 1.f);
	vec[0].uv = Vec2(0.f, 0.f);

	vec[1].pos = Vec3(0.5f, 0.5f, 0.5f);
	vec[1].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[1].uv = Vec2(1.f, 0.f);

	vec[2].pos = Vec3(0.5f, -0.5f, 0.5f);
	vec[2].color = Vec4(0.f, 0.f, 1.f, 1.f);
	vec[2].uv = Vec2(1.f, 1.f);

	vec[3].pos = Vec3(-0.5f, -0.5f, 0.5f);
	vec[3].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[3].uv = Vec2(0.f, 1.f);

	vector<uint32> indexVec;
	{
		indexVec.push_back(0);
		indexVec.push_back(1);
		indexVec.push_back(2);
	}
	{
		indexVec.push_back(0);
		indexVec.push_back(2);
		indexVec.push_back(3);
	}

	mesh->Init(vec, indexVec);

	shader->Init(L"..\\Resources\\Shader\\default.fx");
	texture->Init(L"..\\Resources\\Texture\\EldenRing.jpg");
	*/

#pragma endregion
#pragma region Material
	/*
	vector<Vertex> vec(4);
	vec[0].pos = Vec3(-0.5f, 0.5f, 0.5f);
	vec[0].color = Vec4(1.f, 0.f, 0.f, 1.f);
	vec[0].uv = Vec2(0.f, 0.f);

	vec[1].pos = Vec3(0.5f, 0.5f, 0.5f);
	vec[1].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[1].uv = Vec2(1.f, 0.f);

	vec[2].pos = Vec3(0.5f, -0.5f, 0.5f);
	vec[2].color = Vec4(0.f, 0.f, 1.f, 1.f);
	vec[2].uv = Vec2(1.f, 1.f);

	vec[3].pos = Vec3(-0.5f, -0.5f, 0.5f);
	vec[3].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[3].uv = Vec2(0.f, 1.f);

	vector<uint32> indexVec;
	{
		indexVec.push_back(0);
		indexVec.push_back(1);
		indexVec.push_back(2);
	}
	{
		indexVec.push_back(0);
		indexVec.push_back(2);
		indexVec.push_back(3);
	}

	mesh->Init(vec, indexVec);

	shared_ptr<Shader> shader = make_shared<Shader>();
	shared_ptr<Texture> texture = make_shared<Texture>();
	shader->Init(L"..\\Resources\\Shader\\default.fx");
	texture->Init(L"..\\Resources\\Texture\\EldenRing.jpg");

	shared_ptr<Material> material = make_shared<Material>();
	material->SetShader(shader);
	material->SetFloat(0, 0.3f);
	material->SetFloat(1, 0.4f);
	material->SetFloat(2, 0.3f);
	material->SetTexture(0, texture);
	mesh->SetMaterial(material);*/
#pragma endregion
#pragma region Component Test
/*
shared_ptr<GameObject> gameObject = make_shared<GameObject>();

	vector<Vertex> vec(4);
	vec[0].pos = Vec3(-0.5f, 0.5f, 0.5f);
	vec[0].color = Vec4(1.f, 0.f, 0.f, 1.f);
	vec[0].uv = Vec2(0.f, 0.f);
	vec[1].pos = Vec3(0.5f, 0.5f, 0.5f);
	vec[1].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[1].uv = Vec2(1.f, 0.f);
	vec[2].pos = Vec3(0.5f, -0.5f, 0.5f);
	vec[2].color = Vec4(0.f, 0.f, 1.f, 1.f);
	vec[2].uv = Vec2(1.f, 1.f);
	vec[3].pos = Vec3(-0.5f, -0.5f, 0.5f);
	vec[3].color = Vec4(0.f, 1.f, 0.f, 1.f);
	vec[3].uv = Vec2(0.f, 1.f);

	vector<uint32> indexVec;
	{
		indexVec.push_back(0);
		indexVec.push_back(1);
		indexVec.push_back(2);
	}
	{
		indexVec.push_back(0);
		indexVec.push_back(2);
		indexVec.push_back(3);
	}

	gameObject->AddComponent(make_shared<Transform>());
	shared_ptr<Transform> transform = gameObject->GetTransform();
	transform->SetLocalPosition(Vec3(0.f, 100.f, 200.f));
	transform->SetLocalScale(Vec3(100.f, 100.f, 1.f));

	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->Init(vec, indexVec);
		meshRenderer->SetMesh(mesh);
	}
	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shared_ptr<Texture> texture = make_shared<Texture>();
		shader->Init(L"..\\Resources\\Shader\\default.fx");
		texture->Init(L"..\\Resources\\Texture\\EldenRing.jpg");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetFloat(0, 0.3f);
		material->SetFloat(1, 0.4f);
		material->SetFloat(2, 0.3f);
		material->SetTexture(0, texture);
		meshRenderer->SetMaterial(material);
	}
	gameObject->AddComponent(meshRenderer);
	scene->AddGameObject(gameObject);
	*/
#pragma endregion
#pragma region Sphere
	//{
	//shared_ptr<GameObject> sphere = make_shared<GameObject>();
	//sphere->AddComponent(make_shared<Transform>());
	//sphere->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
	//sphere->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 150.f));
	//shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	//{
	//	shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
	//	meshRenderer->SetMesh(sphereMesh);
	//}
	//{
	//	shared_ptr<Shader> shader = make_shared<Shader>();
	//	shared_ptr<Texture> texture = make_shared<Texture>();
	//	shader->Init(L"..\\Resources\\Shader\\Deferred.fx");
	//	texture->Init(L"..\\Resources\\Texture\\EldenRing.jpg");
	//	shared_ptr<Material> material = make_shared<Material>();
	//	material->SetShader(shader);
	//	material->SetTexture(0, texture);
	//	meshRenderer->SetMaterial(material);
	//}
	//sphere->AddComponent(meshRenderer);
	//scene->AddGameObject(sphere);
	//}
#pragma endregion

#pragma region Camera
	{
	shared_ptr<GameObject> camera = make_shared<GameObject>();
	camera->SetName(L"Main_Camera");
	camera->AddComponent(make_shared<Transform>());
	camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, FOV=45��
	camera->AddComponent(make_shared<TestCameraScript>());
	camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
	uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
	camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, true); // UI�� �� ����
	scene->AddGameObject(camera);
	}
#pragma endregion

#pragma region UI_Camera
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();
		camera->SetName(L"Orthographic_Camera");
		camera->AddComponent(make_shared<Transform>());
		camera->AddComponent(make_shared<Camera>()); // Near=1, Far=1000, 800*600
		camera->AddComponent(make_shared<TestCameraScript>());
		camera->GetCamera()->SetFar(10000);
		camera->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 0.f));
		camera->GetCamera()->SetProjectionType(PROJECTION_TYPE::ORTHOGRAPHIC);
		uint8 layerIndex = GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI");
		camera->GetCamera()->SetCullingMaskAll(); // �� ����
		camera->GetCamera()->SetCullingMaskLayerOnOff(layerIndex, false); // UI�� ����
		scene->AddGameObject(camera);
	}
#pragma endregion

#pragma region SkyBox
	{
	shared_ptr<GameObject> skybox = make_shared<GameObject>();
	skybox->AddComponent(make_shared<Transform>());
	skybox->SetCheckFrustum(false);
	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	{
		shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
		meshRenderer->SetMesh(sphereMesh);
	}
	{
		shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Skybox");
		shared_ptr<Texture> texture = GET_SINGLE(Resources)->Load<Texture>(L"skybox", L"..\\Resources\\Texture\\skybox.jpg");
		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(shader);
		material->SetTexture(0, texture);
		meshRenderer->SetMaterial(material);
	}
	skybox->AddComponent(meshRenderer);
	scene->AddGameObject(skybox);
	}
#pragma endregion


#pragma region Object
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->SetName(L"OBJ");
		obj->AddComponent(make_shared<Transform>());
		obj->AddComponent(make_shared<SphereCollider>());
		obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		obj->GetTransform()->SetLocalPosition(Vec3(0, 0.f, 500.f));
		obj->SetStatic(false);
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> sphereMesh = GET_SINGLE(Resources)->LoadSphereMesh();
			meshRenderer->SetMesh(sphereMesh);
		}
		{
			shared_ptr<Material> material = GET_SINGLE(Resources)->Get<Material>(L"GameObject");
			meshRenderer->SetMaterial(material->Clone());
		}
		dynamic_pointer_cast<SphereCollider>(obj->GetCollider())->SetRadius(0.5f);
		dynamic_pointer_cast<SphereCollider>(obj->GetCollider())->SetCenter(Vec3(0.f, 0.f, 0.f));
		obj->AddComponent(meshRenderer);
		scene->AddGameObject(obj);
	}
#pragma endregion

#pragma region Terrain
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->AddComponent(make_shared<Transform>());
		obj->AddComponent(make_shared<Terrain>());
		obj->AddComponent(make_shared<MeshRenderer>());

		obj->GetTransform()->SetLocalScale(Vec3(50.f, 250.f, 50.f));
		obj->GetTransform()->SetLocalPosition(Vec3(-100.f, -200.f, 300.f));
		obj->SetStatic(true);
		obj->GetTerrain()->Init(64, 64);
		obj->SetCheckFrustum(false);

		scene->AddGameObject(obj);
	}
#pragma endregion

#pragma region UI_Test
	for (int32 i = 0; i < 6; i++)
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->SetLayerIndex(GET_SINGLE(SceneManager)->LayerNameToIndex(L"UI")); // UI
		obj->AddComponent(make_shared<Transform>());
		obj->GetTransform()->SetLocalScale(Vec3(100.f, 100.f, 100.f));
		obj->GetTransform()->SetLocalPosition(Vec3(-350.f + (i * 120), 250.f, 500.f));
		shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
		{
			shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
			meshRenderer->SetMesh(mesh);
		}
		{
			shared_ptr<Shader> shader = GET_SINGLE(Resources)->Get<Shader>(L"Texture");

			shared_ptr<Texture> texture;
			if (i < 3)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->GetRTTexture(i);
			else if (i < 5)
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->GetRTTexture(i - 3);
			else
				texture = GEngine->GetRTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->GetRTTexture(0);

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(shader);
			material->SetTexture(0, texture);
			meshRenderer->SetMaterial(material);
		}
		obj->AddComponent(meshRenderer);
		scene->AddGameObject(obj);
	}
#pragma endregion

#pragma region Directional Light
	{
		shared_ptr<GameObject> light = make_shared<GameObject>();
		light->AddComponent(make_shared<Transform>());
		light->GetTransform()->SetLocalPosition(Vec3(0, 1000, 500));
		light->AddComponent(make_shared<Light>());
		light->GetLight()->SetLightDirection(Vec3(0, -1, 0.f));
		light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
		light->GetLight()->SetDiffuse(Vec3(1.f, 1.f, 1.f));
		light->GetLight()->SetAmbient(Vec3(0.1f, 0.1f, 0.1f));
		light->GetLight()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));

		scene->AddGameObject(light);
	}
#pragma endregion

#pragma region Tessellation Test
	//{
	//	shared_ptr<GameObject> gameObject = make_shared<GameObject>();
	//	gameObject->AddComponent(make_shared<Transform>());
	//	gameObject->GetTransform()->SetLocalPosition(Vec3(0, 0, 300));
	//	gameObject->GetTransform()->SetLocalScale(Vec3(100, 100, 100));
	//	gameObject->GetTransform()->SetLocalRotation(Vec3(0, 0, 0));

	//	shared_ptr<MeshRenderer> meshRenderer = make_shared<MeshRenderer>();
	//	{
	//		shared_ptr<Mesh> mesh = GET_SINGLE(Resources)->LoadRectangleMesh();
	//		meshRenderer->SetMesh(mesh);
	//		meshRenderer->SetMaterial(GET_SINGLE(Resources)->Get<Material>(L"Tessellation"));
	//	}
	//	gameObject->AddComponent(meshRenderer);

	//	scene->AddGameObject(gameObject);
	//}
#pragma endregion

#pragma region FBX
	{
		shared_ptr<MeshData> meshData = GET_SINGLE(Resources)->LoadFBX(L"..\\Resources\\FBX\\Dragon.fbx");

		vector<shared_ptr<GameObject>> gameObjects = meshData->Instantiate();

		for (auto& gameObject : gameObjects)
		{
			gameObject->SetName(L"Dragon");
			gameObject->SetCheckFrustum(false);
			gameObject->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 300.f));
			gameObject->GetTransform()->SetLocalScale(Vec3(1.f, 1.f, 1.f));
			scene->AddGameObject(gameObject);
			gameObject->AddComponent(make_shared<TestDragon>());
		}
	}
#pragma endregion

#pragma region ParticleSystem
	//{
	//	shared_ptr<GameObject> particle = make_shared<GameObject>();
	//	particle->AddComponent(make_shared<Transform>());
	//	particle->AddComponent(make_shared<ParticleSystem>());
	//	particle->SetCheckFrustum(false);
	//	particle->GetTransform()->SetLocalPosition(Vec3(0.f, 0.f, 100.f));
	//	scene->AddGameObject(particle);
	//}
#pragma endregion
#pragma region Point Light
	//{
	//	shared_ptr<GameObject> light = make_shared<GameObject>();
	//	light->AddComponent(make_shared<Transform>());
	//	light->GetTransform()->SetLocalPosition(Vec3(0.f, 100.f, 150.f));
	//	light->AddComponent(make_shared<Light>());
	//	//light->GetLight()->SetLightDirection(Vec3(-1.f, -1.f, 0));
	//	light->GetLight()->SetLightType(LIGHT_TYPE::POINT_LIGHT);
	//	light->GetLight()->SetDiffuse(Vec3(0.0f, 0.5f, 0.0f));
	//	light->GetLight()->SetAmbient(Vec3(0.0f, 0.3f, 0.0f));
	//	light->GetLight()->SetSpecular(Vec3(0.0f, 0.3f, 0.0f));
	//	light->GetLight()->SetLightRange(200.f);

	//	scene->AddGameObject(light);
	//}
#pragma endregion
#pragma region Spot Light
	//{
	//	shared_ptr<GameObject> light = make_shared<GameObject>();
	//	light->AddComponent(make_shared<Transform>());
	//	light->GetTransform()->SetLocalPosition(Vec3(75.f, 0.f, 150.f));
	//	light->AddComponent(make_shared<Light>());
	//	light->GetLight()->SetLightDirection(Vec3(-1.f, 0, 0));
	//	light->GetLight()->SetLightType(LIGHT_TYPE::SPOT_LIGHT);
	//	light->GetLight()->SetDiffuse(Vec3(0.0f, 0.f, 0.5f));
	//	light->GetLight()->SetAmbient(Vec3(0.0f, 0.0f, 0.1f));
	//	light->GetLight()->SetSpecular(Vec3(0.0f, 0.0f, 0.1f));
	//	light->GetLight()->SetLightRange(200.f);
	//	light->GetLight()->SetLightAngle(3.14f / 2);

	//	scene->AddGameObject(light);
	//}
#pragma endregion
#pragma region Green Directional Light
	//{
	//	shared_ptr<GameObject> light = make_shared<GameObject>();
	//	light->AddComponent(make_shared<Transform>());
	//	//light->GetTransform()->SetLocalPosition(Vec3(0.f, 150.f, 150.f));
	//	light->AddComponent(make_shared<Light>());
	//	light->GetLight()->SetLightDirection(Vec3(1.f, 0.f, 1.f));
	//	light->GetLight()->SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
	//	light->GetLight()->SetDiffuse(Vec3(0.5f, 0.5f, 0.5f));
	//	light->GetLight()->SetAmbient(Vec3(0.1f, 0.1f, 0.1f));
	//	light->GetLight()->SetSpecular(Vec3(0.3f, 0.3f, 0.3f));

	//	scene->AddGameObject(light);
	//}

#pragma endregion
#pragma region Red Point Light
	//{
	//	shared_ptr<GameObject> light = make_shared<GameObject>();
	//	light->AddComponent(make_shared<Transform>());
	//	light->GetTransform()->SetLocalPosition(Vec3(150.f, 150.f, 150.f));
	//	light->AddComponent(make_shared<Light>());
	//	//light->GetLight()->SetLightDirection(Vec3(0.f, -1.f, 0.f));
	//	light->GetLight()->SetLightType(LIGHT_TYPE::POINT_LIGHT);
	//	light->GetLight()->SetDiffuse(Vec3(1.f, 0.1f, 0.1f));
	//	light->GetLight()->SetAmbient(Vec3(0.1f, 0.f, 0.f));
	//	light->GetLight()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));
	//	light->GetLight()->SetLightRange(10000.f);
	//	//light->GetLight()->SetLightAngle(XM_PI / 4);
	//	scene->AddGameObject(light);
	//}
#pragma endregion
#pragma region Blue Spot Light
	//{
	//	shared_ptr<GameObject> light = make_shared<GameObject>();
	//	light->AddComponent(make_shared<Transform>());
	//	light->GetTransform()->SetLocalPosition(Vec3(-150.f, 0.f, 150.f));
	//	light->AddComponent(make_shared<Light>());
	//	light->GetLight()->SetLightDirection(Vec3(1.f, 0.f, 0.f));
	//	light->GetLight()->SetLightType(LIGHT_TYPE::SPOT_LIGHT);
	//	light->GetLight()->SetDiffuse(Vec3(0.f, 0.1f, 1.f));
	//	//light->GetLight()->SetAmbient(Vec3(0.f, 0.f, 0.1f));
	//	light->GetLight()->SetSpecular(Vec3(0.1f, 0.1f, 0.1f));
	//	light->GetLight()->SetLightRange(10000.f);
	//	light->GetLight()->SetLightAngle(XM_PI / 4);
	//	scene->AddGameObject(light);
	//}
#pragma endregion

	return scene;
}
