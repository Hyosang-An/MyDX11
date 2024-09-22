#pragma once
class CLevelSaveLoad
{
public:
	static void SaveLevel(const wstring& _FilePath, CLevel* _Level);
	static void SaveGameObject(FILE* _File, CGameObject* _Object);


	static CLevel* LoadLevel(const wstring& _FilePath);
	static CGameObject* LoadGameObject(FILE* _File, int& _objIdx);
	static class CComponent* GetComponent(COMPONENT_TYPE _Type);

	static void LoadInitialGameLevel();

private:
	static CLevel* pLoadingLevel;
};

