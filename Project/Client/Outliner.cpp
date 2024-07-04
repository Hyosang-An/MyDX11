#include "pch.h"
#include "Outliner.h"

#include "TreeUI.h"

Outliner::Outliner()
{
	m_Tree = new TreeUI;
	m_Tree->SetName("OltlinerTree");
	AddChild(m_Tree);
}

Outliner::~Outliner()
{
}

void Outliner::Update()
{

}