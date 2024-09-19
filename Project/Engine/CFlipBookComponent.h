#pragma once
#include "CComponent.h"
class CFlipBookComponent :
    public CComponent
{
private:
    map<wstring, Ptr<CFlipBook>>    m_mapFlipBook;  // FliBook ������Ʈ�� ������ ��� FlipBook ���
    Ptr<CFlipBook>                  m_CurFlipBook;  // ���� ������� FlipBook
    Ptr<CSprite>                    m_CurFrmSprite; // ���� ������� FlipBook ���� ���� ������ �ε����� �ش��ϴ� ��������Ʈ
    int                             m_CurFrmIdx;    // ���� ������� FlipBook ���� ���° Sprite �� ��������� �ε��� ���
    float                           m_FPS = 0;          // ���� ������� FlipBook �� �ʴ� ������ ���� ��
    float                           m_AccTime = 0;      // ���� �ð��� üũ
    bool                            m_Repeat = true;
    bool                            m_Finish = false;       // FlipBook ����� ���� �����ߴ��� ����
	bool                            m_Pause = false;        // �Ͻ����� ����

public:
    virtual void FinalTick() override;
    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;


public:
    vector<wstring> GetFlipBookNames();

    void AddFlipBook(Ptr<CFlipBook> _Flipbook);
	void RemoveFlipBook(const wstring& _FlipBookName);
    Ptr<CFlipBook> FindFlipBook(const wstring& _FlipBookName);
    void Play(wstring _FliBookName, float _FPS, bool _Repeat);
    void Play(wstring _FliBookName, bool _Repeat = true);

	bool IsFinished() { return m_Finish; }

	void Pause() { m_Pause = true; }
	void Resume() { m_Pause = false; }

	void SetCurFlipBook(wstring _FlipBookName);
	void SetCurFrameIdx(int _Idx) { m_CurFrmIdx = _Idx; }

    wstring GetCurFlipBookName() { return path(m_CurFlipBook->GetKey()).stem().wstring(); }
	int GetCurFrmIdx() { return m_CurFrmIdx; }
    Ptr<CSprite> GetCurSprite() { return m_CurFrmSprite; }
	void SetCurSpriteByIdx(int _Idx) { m_CurFrmIdx = _Idx; m_CurFrmSprite = m_CurFlipBook->GetSprite(_Idx); }

    void Reset();
    void Binding();
    static void Clear();


public:
    CFlipBookComponent();
    CFlipBookComponent(CFlipBookComponent& _Origin);
    ~CFlipBookComponent();
    virtual CFlipBookComponent* Clone() { return new CFlipBookComponent(*this); }
};

