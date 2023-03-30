#include "Stdafx.h"
#include "FontshaderClass.hpp"

FontShaderClass::FontShaderClass()
{

}


FontShaderClass::FontShaderClass(const FontShaderClass& other)
{
}


FontShaderClass::~FontShaderClass()
{
}


bool FontShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	WCHAR vs[] = L"./Font.vs";
	WCHAR ps[] = L"./Font.ps";

	return this->InitializeShader(device, hwnd, vs, ps);
}


void FontShaderClass::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}


bool FontShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix, 
							 DirectX::XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, DirectX::XMFLOAT4 pixelColor)
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, pixelColor);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}


bool FontShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC constantBufferDesc;
    D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC pixelBufferDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

    // 버텍스 쉐이더 코드 컴파일
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "FontVertexShader", "vs_5_0", 
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if(FAILED(result))
	{
		// 셰이더 컴파일 실패시 오류 메세지 출력
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우임
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

    // 픽셀 쉐이더 코드 컴파일
	result = D3DCompileFromFile(psFilename, NULL, NULL, "FontPixelShader", "ps_5_0",
		D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if(FAILED(result))
	{
		// 셰이더 컴파일 실패시 오류 메세지 출력
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우임
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

    // 버퍼로부터 정점 셰이더 생성
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, 
										&vertexShader_);
	if(FAILED(result))
	{
		return false;
	}

    // 버퍼로부터 픽셀 셰이더 생성
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, 
									   &pixelShader_);
	if(FAILED(result))
	{
		return false;
	}

	// 정점 입력 레이아웃 구조체 설정
	// ModelClass와 셰이더의 VertextType 구조와 일치 해야한다
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// 레이아웃의 요소 수
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 정점 입력 레이아웃 생성
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), 
									   vertexShaderBuffer->GetBufferSize(), &layout_);
	if(FAILED(result))
	{
		return false;
	}

	// 사용되지 않는 정점 셰이더 버퍼와 픽셀 셰이더 버퍼 해제
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

    // 정점 셰이더에 있는 행렬 상수 버퍼의 구조체 작성
    constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(ConstantBufferType);
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	// 상수 버퍼 포인터를 만들어 이 클래스에서 정점 셰이더 상수 버퍼에 접근할 수 있게 한다
	result = device->CreateBuffer(&constantBufferDesc, NULL, &constantBuffer_);
	if(FAILED(result))
	{
		return false;
	}

	// 텍스처 샘플러 상태 구조체 생성 및 설정
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//텍스처 샘플러 상태
    result = device->CreateSamplerState(&samplerDesc, &sampleState_);
	if(FAILED(result))
	{
		return false;
	}

    // Setup the description of the dynamic pixel constant buffer that is in the pixel shader.
    pixelBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelBufferDesc.ByteWidth = sizeof(PixelBufferType);
    pixelBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    pixelBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    pixelBufferDesc.MiscFlags = 0;
	pixelBufferDesc.StructureByteStride = 0;

	// Create the pixel constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	result = device->CreateBuffer(&pixelBufferDesc, NULL, &pixelBuffer_);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}


void FontShaderClass::ShutdownShader()
{
	// Release the pixel constant buffer.
	if(pixelBuffer_)
	{
		pixelBuffer_->Release();
		pixelBuffer_ = 0;
	}

	// Release the sampler state.
	if(sampleState_)
	{
		sampleState_->Release();
		sampleState_ = 0;
	}

	// Release the constant buffer.
	if(constantBuffer_)
	{
		constantBuffer_->Release();
		constantBuffer_ = 0;
	}

	// Release the layout.
	if(layout_)
	{
		layout_->Release();
		layout_ = 0;
	}

	// Release the pixel shader.
	if(pixelShader_)
	{
		pixelShader_->Release();
		pixelShader_ = 0;
	}

	// Release the vertex shader.
	if(vertexShader_)
	{
		vertexShader_->Release();
		vertexShader_ = 0;
	}

	return;
}


void FontShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

	errorMessage->Release();
	errorMessage = nullptr;

	MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}


bool FontShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix, 
										  DirectX::XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, DirectX::XMFLOAT4 pixelColor)
{
	HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
	ConstantBufferType* dataPtr;
	unsigned int bufferNumber;
	PixelBufferType* dataPtr2;


	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(constantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (ConstantBufferType*)mappedResource.pData;

	// 셰이더에서 사용할 수 있도록 matrix를 transpose
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
    deviceContext->Unmap(constantBuffer_, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &constantBuffer_);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	// Lock the pixel constant buffer so it can be written to.
	result = deviceContext->Map(pixelBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the pixel constant buffer.
	dataPtr2 = (PixelBufferType*)mappedResource.pData;

	// Copy the pixel color into the pixel constant buffer.
	dataPtr2->pixelColor = pixelColor;

	// Unlock the pixel constant buffer.
    deviceContext->Unmap(pixelBuffer_, 0);

	// Set the position of the pixel constant buffer in the pixel shader.
	bufferNumber = 0;

	// Now set the pixel constant buffer in the pixel shader with the updated value.
    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &pixelBuffer_);

	return true;
}


void FontShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(layout_);

    // Set the vertex and pixel shaders that will be used to render the triangles.
    deviceContext->VSSetShader(vertexShader_, NULL, 0);
    deviceContext->PSSetShader(pixelShader_, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &sampleState_);

	// Render the triangles.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}