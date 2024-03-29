#include "Stdafx.h"
#include "FontClass.hpp"
#include "FontshaderClass.hpp"
#include "TextClass.hpp"

TextClass::TextClass()
{

}


TextClass::TextClass(const TextClass& other)
{
}


TextClass::~TextClass()
{
}


bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, 
						   DirectX::XMMATRIX baseViewMatrix)
{
	bool result;


	// Store the screen width and height.
	screenWidth_ = screenWidth;
	screenHeight_ = screenHeight;

	// Store the base view matrix.
	baseViewMatrix_ = baseViewMatrix;

	// Create the font object.
	font_ = new FontClass;
	if(!font_)
	{
		return false;
	}

	// Initialize the font object.
	WCHAR dds[] = L"./data/font.dds";
	result = font_->Initialize(device, (char*)"./data/fontdata.txt", dds);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}
	
	// Create the font shader object.
	fontShader_ = new FontShaderClass;
	if(!fontShader_)
	{
		return false;
	}

	// Initialize the font shader object.
	result = fontShader_->Initialize(device, hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}
	
	// intro 문장 초기화
	result = InitializeSentence(&sentence1_intro_, 16, device);
	if(!result)
	{
		return false;
	}
	result = UpdateSentence(sentence1_intro_, (char*)"Hello, DX World!", 80, 50, 0.0f, 0.0f, 0.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	// fps 문장 초기화
	result = InitializeSentence(&sentence2_fps_, 16, device);
	if(!result)
	{
		return false;
	}
	result = UpdateSentence(sentence2_fps_, (char*)"temp", 650, 50, 0.0f, 0.0f, 0.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	// mouseX 문장 초기화
	result = InitializeSentence(&sentence3_mouseX_, 16, device);
	if (!result)
	{
		return false;
	}
	result = UpdateSentence(sentence3_mouseX_, (char*)"temp", 650, 100, 0.0f, 0.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	// mouseY 문장 초기화
	result = InitializeSentence(&sentence4_mouseY_, 16, device);
	if (!result)
	{
		return false;
	}
	result = UpdateSentence(sentence4_mouseY_, (char*)"temp", 650, 120, 0.0f, 0.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}


void TextClass::Shutdown()
{
	// Release the first sentence.
	ReleaseSentence(&sentence1_intro_);

	// Release the second sentence.
	ReleaseSentence(&sentence2_fps_);

	// Release the second sentence.
	ReleaseSentence(&sentence3_mouseX_);

	// Release the second sentence.
	ReleaseSentence(&sentence4_mouseY_);

	// Release the font shader object.
	if(fontShader_)
	{
		fontShader_->Shutdown();
		delete fontShader_;
		fontShader_ = 0;
	}

	// Release the font object.
	if(font_)
	{
		font_->Shutdown();
		delete font_;
		font_ = 0;
	}

	return;
}


bool TextClass::Render(ID3D11DeviceContext* deviceContext, DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX orthoMatrix)
{
	bool result;

	result = RenderSentence(deviceContext, sentence1_intro_, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	result = RenderSentence(deviceContext, sentence2_fps_, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	result = RenderSentence(deviceContext, sentence3_mouseX_, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	result = RenderSentence(deviceContext, sentence4_mouseY_, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	return true;
}


bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Create a new sentence object.
	*sentence = new SentenceType;
	if(!*sentence)
	{
		return false;
	}

	// Initialize the sentence buffers to null.
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	// Set the maximum length of the sentence.
	(*sentence)->maxLength = maxLength;

	// Set the number of vertices in the vertex array.
	(*sentence)->vertexCount = 6 * maxLength;

	// Set the number of indexes in the index array.
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// Create the vertex array.
	vertices = new VertexType[(*sentence)->vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[(*sentence)->indexCount];
	if(!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// Initialize the index array.
	for(i=0; i<(*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;

	// Release the index array as it is no longer needed.
	delete [] indices;
	indices = 0;

	return true;
}


bool TextClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green, float blue,
							   ID3D11DeviceContext* deviceContext)
{
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;


	// Store the color of the sentence.
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	// Get the number of letters in the sentence.
	numLetters = (int)strlen(text);

	// Check for possible buffer overflow.
	if(numLetters > sentence->maxLength)
	{
		return false;
	}

	// Create the vertex array.
	vertices = new VertexType[sentence->vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	// Calculate the X and Y pixel position on the screen to start drawing to.
	drawX = (float)(((screenWidth_ / 2) * -1) + positionX);
	drawY = (float)((screenHeight_ / 2) - positionY);

	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	font_->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;

	return true;
}


void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if(*sentence)
	{
		// Release the sentence vertex buffer.
		if((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		// Release the sentence index buffer.
		if((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		// Release the sentence.
		delete *sentence;
		*sentence = 0;
	}

	return;
}


bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, DirectX::XMMATRIX worldMatrix, 
							   DirectX::XMMATRIX orthoMatrix)
{
	unsigned int stride, offset;
	DirectX::XMFLOAT4 pixelColor;
	bool result;


	// Set vertex buffer stride and offset.
    stride = sizeof(VertexType); 
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create a pixel color vector with the input sentence color.
	pixelColor = DirectX::XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// Render the text using the font shader.
	result = fontShader_->Render(deviceContext, sentence->indexCount, worldMatrix, baseViewMatrix_, orthoMatrix, font_->GetTexture(), 
								  pixelColor);
	if(!result)
	{
		false;
	}

	return true;
}

bool TextClass::SetMousePosition(int mouseX, int mouseY, ID3D11DeviceContext* deviceContext)
{	
	char tempString[16];
	char mouseString[16];
	//float red, green, blue;
	bool result;
	

	_itoa_s(mouseX, tempString, 10);

	strcpy_s(mouseString, "Mouse X: ");
	strcat_s(mouseString, tempString);

	result = UpdateSentence(sentence3_mouseX_, mouseString, 650, 80, 0.0f, 0.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	// mouseY 정수를 문자열 형식으로 변환
	_itoa_s(mouseY, tempString, 10);

	strcpy_s(mouseString, "Mouse Y: ");
	strcat_s(mouseString, tempString);

	// 문장 정점 버퍼를 새 문자열 정보로 업데이트
	result = UpdateSentence(sentence4_mouseY_, mouseString, 650, 100, 0.0f, 0.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetFps(int fps, ID3D11DeviceContext* deviceContext) 
{
	char tempString[16];
	char fpsString[16];
	float red, green, blue;
	bool result;

	if (fps > 9999)
	{
		fps = 9999;
	}

	_itoa_s(fps, tempString, 10);

	strcpy_s(fpsString, "FPS : ");
	strcat_s(fpsString, tempString);

	// Green
	if (fps >= 60)
	{
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}
	// Pink
	else if (30 < fps && fps < 60)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 1.0f;
	}
	// Red
	else
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	result = UpdateSentence(sentence2_fps_, fpsString, 650, 50, red, green, blue, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}