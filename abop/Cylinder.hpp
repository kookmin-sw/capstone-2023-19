#pragma once

class Cylinder : public ModelClass
{
public:
    bool Initialize(ID3D11Device*);

protected:
    bool InitializeBuffers(ID3D11Device*);

public:
    void SetSegment(int);
    void Setheight(float);
    void SetRadius(float);

private:
    int segment_ = 10;
    float height_ = 2.0f;
    float radius_ = 1.0f;
};