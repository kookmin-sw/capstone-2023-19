#pragma once

class Plane : public ModelClass
{
public:
    bool Initialize(ID3D11Device*);

protected:
    bool InitializeBuffers(ID3D11Device*);

public:
    void SetWidth(const float&);
    void SetHeight(const float&);
    void SetWidthSegments(const int&);
    void SetHeightSegments(const int&);

private:
    // Plane의 크기
    float width_ = 10;
    float height_ = 10;

    int nWidthSegments_ = 11;
    int nHeightSegments_ = 11;
};