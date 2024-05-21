#pragma once
class CPathMgr
	: public CSingleton<CPathMgr>
{
private:
	friend class CSingleton<CPathMgr>;
	CPathMgr();
	~CPathMgr();

private:
	wstring m_strContentsPath;
	wstring m_strSolutionPath;

public:
	void Init();
	void Render();

	const wstring& GetContentsPath() { return m_strContentsPath; }
	const wstring& GetSolutionPath() { return m_strSolutionPath; }

private:
	wstring GetParentPath(const wstring& _strPath);
};

