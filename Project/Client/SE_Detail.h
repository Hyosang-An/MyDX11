#pragma once
#include "SE_Sub.h"
class SE_Detail :
    public SE_Sub
{
public:
    SE_Detail();
    ~SE_Detail();

public:
    virtual void Init() override;
    virtual void Update() override;

};

