#pragma once

class Cylinder : public ModelClass
{
public:
    bool Initialize(ID3D11Device*);
    // Buffer 생성시 호출 원기둥 cap
    bool GenerateCylinderCap(ID3D11Device*);                //  !!! 나중에 지우기
    bool GenerateCylinderCap(ID3D11Device*, float, float);  // Temp (protected -> public)

protected:
    bool InitializeBuffers(ID3D11Device*);

public:
    void SetSegment(int);
    void SetHeight(float);
    void SetRadius(float, float);

private:
    int segment_ = 50;
    float height_ = 2.0f;
    float radiusUp_ = 1.0f;
    float radiusDown_ = 1.0f;
};