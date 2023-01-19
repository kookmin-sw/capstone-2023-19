#pragma once

class Cube : public ModelClass
{
public:
    bool Initialize(ID3D11Device*);

protected:
    bool InitializeBuffers(ID3D11Device*);

public:
    Vector3 GetSize();
    void SetSize(const Vector3&);
    void SetSize(const float&, const float&, const float&);

private:
    Vector3 size_ = { 1, 1, 1 };
};