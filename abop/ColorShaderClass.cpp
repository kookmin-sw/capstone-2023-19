#include "Stdafx.h"
#include "ColorShaderClass.hpp"

ColorShaderClass::ColorShaderClass()
{

}

ColorShaderClass::ColorShaderClass(const ColorShaderClass&)
{

}

ColorShaderClass::~ColorShaderClass()
{

}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    WCHAR vs[] = L"./ColorVertex.hlsl";
    WCHAR ps[] = L"./ColorPixel.hlsl";

    return this->InitializeShader(device, hwnd, vs, ps);
}

void ColorShaderClass::Shutdown()
{
    this->ShutdownShader();
}

bool ColorShaderClass::Render(
    ID3D11DeviceContext* deviceContext, int indexCount, 
    DirectX::XMMATRIX worldMatrix,
    DirectX::XMMATRIX viewMatrix,
    DirectX::XMMATRIX projectionMatrix,
    DirectX::XMFLOAT3 lightDirection,
    DirectX::XMFLOAT4 ambientColor,
    DirectX::XMFLOAT4 diffuseColor,
    DirectX::XMFLOAT3 cameraPosition,
    DirectX::XMFLOAT4 specularColor,
    float specularPower)
{
    if (!this->SetShaderParameters(deviceContext, 
        worldMatrix, viewMatrix, projectionMatrix,
        lightDirection, ambientColor, diffuseColor,
        cameraPosition, specularColor, specularPower))
    {
        // 렌더링에 사용될 매개변수 설정
        return false;
    }

    this->RenderShader(deviceContext, indexCount);

    return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd,
    WCHAR* vsFilename, WCHAR* psFilename)
{
    ID3D10Blob* errorMessage = nullptr;

    ID3D10Blob* vertexShaderBuffer = nullptr;
    if (FAILED(D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", 
        "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage)))
    {
        if (errorMessage)
        {
            // VertexShader 컴파일 실패시 오류 메시지 출력
            this->OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
        }
        else
        {
            // vsFilename이 잘못된 경우
            MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    ID3D10Blob* pixelShaderBuffer = nullptr;
    if (FAILED(D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader",
        "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage)))
    {
        if (errorMessage)
        {
            // PixelShader 컴파일 실패시 오류 메시지 출력
            this->OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
        }
        else
        {
            // psFilename이 잘못된 경우
            MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    // 버퍼로부터 셰이더 생성
    if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), NULL, &(this->vertexShader_))))
    {
        return false;
    }

    if (FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(),
        pixelShaderBuffer->GetBufferSize(), NULL, &(this->pixelShader_))))
    {
        return false;   
    }

    // 정점 입력 레이아웃 구조체 설정
    // ModelClass와 셰이더 VertexType 구조와 일치해야 함
    D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "COLOR";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    polygonLayout[2].SemanticName = "NORMAL";
    polygonLayout[2].SemanticIndex = 0;
    polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[2].InputSlot = 0;
    polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[2].InstanceDataStepRate = 0;
    
    unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // Input Layout 생성
    if (FAILED(device->CreateInputLayout(polygonLayout, numElements,
        vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &(this->layout_))))
    {
        return false;
    }

    // 셰이더 버퍼 해제
    vertexShaderBuffer->Release();
    vertexShaderBuffer = nullptr;
    pixelShaderBuffer->Release();
    pixelShaderBuffer = nullptr;

    // Vertex Shader의 행렬 상수 버퍼의 구조체 작성
    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // 버퍼 포인터 생성
    if (FAILED(device->CreateBuffer(&matrixBufferDesc, NULL, &this->matrixBuffer_)))
    {
        return false;
    }

    // vertex shader 카메라 버퍼 설정
    D3D11_BUFFER_DESC cameraBufferDesc;
    cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
    cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    cameraBufferDesc.MiscFlags = 0;
    cameraBufferDesc.StructureByteStride = 0;

    if (FAILED(device->CreateBuffer(&cameraBufferDesc, NULL, &this->cameraBuffer_)))
    {
        return false;
    }

    // Pixel Shader에 있는 광원 동적 상수 버퍼 description 설정
    // D3D11_BIND_CONSTANT_BUFFER를 사용하면 ByteWidth는 항상 16의 배수여야 함
    D3D11_BUFFER_DESC lightBufferDesc;
    lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightBufferDesc.ByteWidth = sizeof(LightBufferType);
    lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightBufferDesc.MiscFlags = 0;
    lightBufferDesc.StructureByteStride = 0;

    if (FAILED(device->CreateBuffer(&lightBufferDesc, NULL, &this->lightBuffer_)))
    {
        return false;
    }

    return true;
}

void ColorShaderClass::ShutdownShader()
{
    if (this->lightBuffer_)
    {
        this->lightBuffer_->Release();
        this->lightBuffer_ = nullptr;
    }

    if (this->cameraBuffer_)
    {
        this->cameraBuffer_->Release();
        this->cameraBuffer_ = nullptr;
    }

    if (this->matrixBuffer_)
    {
        this->matrixBuffer_->Release();
        this->matrixBuffer_ = nullptr;
    }

    if (this->layout_)
    {
        this->layout_->Release();
        this->layout_ = nullptr;
    }

    if (this->pixelShader_)
    {
        this->pixelShader_->Release();
        this->pixelShader_ = nullptr;
    }

    if (this->vertexShader_)
    {
        this->vertexShader_->Release();
        this->vertexShader_ = nullptr;
    }
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
    OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

    errorMessage->Release();
    errorMessage = nullptr;

    MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}

bool ColorShaderClass::SetShaderParameters(
    ID3D11DeviceContext* deviceContext, 
    DirectX::XMMATRIX worldMatrix, 
    DirectX::XMMATRIX viewMatrix,
    DirectX::XMMATRIX projectionMatrix,
    DirectX::XMFLOAT3 lightDirection,
    DirectX::XMFLOAT4 ambientColor,
    DirectX::XMFLOAT4 diffuseColor,
    DirectX::XMFLOAT3 cameraPosition,
    DirectX::XMFLOAT4 specularColor,
    float specularPower)
{
    // 셰이더에서 사용할 수 있도록 matrix를 transpose
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    // 상수 버퍼 내용을 쓸 수 있도록 수정
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    // ### Vertex Shader
    // MatrixBuffer 전달
    // Map
    if (FAILED(deviceContext->Map(this->matrixBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 
        0, &mappedResource)))
    {
        return false;
    }

    MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    // Unmap
    deviceContext->Unmap(this->matrixBuffer_, 0);

    unsigned bufferNumber = 0;      // 버퍼 위치 설정
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &this->matrixBuffer_);

    // CameraBuffer 전달
    // Map
    if (FAILED(deviceContext->Map(this->cameraBuffer_, 0, D3D11_MAP_WRITE_DISCARD,
        0, &mappedResource)))
    {
        return false;
    }

    CameraBufferType* dataPtr2 = (CameraBufferType*)mappedResource.pData;
    dataPtr2->cameraPosition = cameraPosition;
    dataPtr2->padding = 0.0f;

    // Unmap
    deviceContext->Unmap(this->cameraBuffer_, 0);
    bufferNumber = 1;
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &this->cameraBuffer_);

    // ### Pixel Shader
    // LightBuffer 전달
    // Map
    if (FAILED(deviceContext->Map(this->lightBuffer_, 0, D3D11_MAP_WRITE_DISCARD,
        0, &mappedResource)))
    {
        return false;
    }

    LightBufferType* dataPtr3 = (LightBufferType*)mappedResource.pData;
    dataPtr3->ambientColor = ambientColor;
    dataPtr3->diffuseColor = diffuseColor;
    dataPtr3->lightDirection = lightDirection;
    dataPtr3->specularColor = specularColor;
    dataPtr3->specularPower = specularPower;

    // Unmap
    deviceContext->Unmap(this->lightBuffer_, 0);

    bufferNumber = 0;

    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &(this->lightBuffer_));

    return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    // Input Layout 설정
    deviceContext->IASetInputLayout(this->layout_);

    // Shader 설정
    deviceContext->VSSetShader(this->vertexShader_, NULL, 0);
    deviceContext->PSSetShader(this->pixelShader_, NULL, 0);

    deviceContext->DrawIndexed(indexCount, 0, 0);
}

