#include "Stdafx.h"
#include <fstream>
#include "FontClass.hpp"
#include "TextureClass.hpp"
using namespace std;

FontClass::FontClass()
{

}


FontClass::FontClass(const FontClass& other)
{
}


FontClass::~FontClass()
{
}


bool FontClass::Initialize(ID3D11Device* device, char* fontFilename, char* textureFilename)
{
	bool result;


	// 글꼴 데이터를 불러옵니다
	result = LoadFontData(fontFilename);
	if(!result)
	{
		return false;
	}

	// 글꼴 텍스처를 불러옵니다
	result = LoadTexture(device, textureFilename);
	if(!result)
	{
		return false;
	}

	return true;
}


void FontClass::Shutdown()
{
	// 글꼴 텍스처 해제
	ReleaseTexture();

	// 글꼴 데이터 해제
	ReleaseFontData();

	return;
}

// 텍스처에서의 글자 위치 정보를 담고 있는 fontdata.txt를 불러옵니다
bool FontClass::LoadFontData(char* filename)
{
	ifstream fin;
	int i;
	char temp;


	// FontType형의 배열을 만듭니다
	font_ = new FontType[95];
	if(!font_)
	{
		return false;
	}

	// 파일을 열고 각 라인을 읽어 배열에 저장합니다
	fin.open(filename);
	if(fin.fail())
	{
		return false;
	}

	// 텍스트의 ASCll문자, left, right, 문자 픽셀 너비를 읽어옵니다
	for(i=0; i<95; i++)
	{
		fin.get(temp);
		while(temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while(temp != ' ')
		{
			fin.get(temp);
		}

		fin >> font_[i].left;
		fin >> font_[i].right;
		fin >> font_[i].size;
	}

	fin.close();

	return true;
}


void FontClass::ReleaseFontData()
{
	// 읽어온 배열 데이터 해제
	if(font_)
	{
		delete [] font_;
		font_ = 0;
	}

	return;
}

//font.dds 파일을 읽어 텍스쳐 셰이더에 전달합니다
// 텍스쳐 셰이더에서 글자를 뽑아내 각각의 사각형에 입혀 화면에 그리게 됩니다
bool FontClass::LoadTexture(ID3D11Device* device, char* filename)
{
	bool result;


	// Texture 객체 생성
	texture_ = new TextureClass;
	if(!texture_)
	{
		return false;
	}

	// Texture 객체 초기화
	result = texture_->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	return true;
}


void FontClass::ReleaseTexture()
{
	// Texture 객체 해제
	if(texture_)
	{
		texture_->Shutdown();
		delete texture_;
		texture_ = 0;
	}

	return;
}


ID3D11ShaderResourceView* FontClass::GetTexture()
{
	//글꼴이 실제로 그려질 수 있도록 글꼴 텍스처의 인터페이스를 리턴합니다
	return texture_->GetTexture();
}


void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters, index, i, letter;


	// 정점 버퍼로 만들어냅니다
	vertexPtr = (VertexType*)vertices;

	// 문장의 길이를 가져옵니다
	numLetters = (int)strlen(sentence);

	// 인덱스를 정점 배열로 초기화합니다
	index = 0;

	// 반복문을 돌면서 정점/인덱스 배열을 만듭니다
	for(i=0; i<numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;

		// If the letter is a space then just move over three pixels.
		if(letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			// 문자당 두개의 삼각형을 만들어 left, right좌표와 픽셀 너비를 매핑시킵니다
			
			// 첫번째 삼각형
			vertexPtr[index].position = DirectX::XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = DirectX::XMFLOAT2(font_[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = DirectX::XMFLOAT3((drawX + font_[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = DirectX::XMFLOAT2(font_[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = DirectX::XMFLOAT3(drawX, (drawY - 16), 0.0f);  // Bottom left.
			vertexPtr[index].texture = DirectX::XMFLOAT2(font_[letter].left, 1.0f);
			index++;

			// 두번째 삼각형
			vertexPtr[index].position = DirectX::XMFLOAT3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = DirectX::XMFLOAT2(font_[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = DirectX::XMFLOAT3(drawX + font_[letter].size, drawY, 0.0f);  // Top right.
			vertexPtr[index].texture = DirectX::XMFLOAT2(font_[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = DirectX::XMFLOAT3((drawX + font_[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = DirectX::XMFLOAT2(font_[letter].right, 1.0f);
			index++;

			// 글자에 해당하는 도형이 생기면 x좌표를 업데이트하여 글자가 그려질 위치를 잡습니다
			drawX = drawX + font_[letter].size + 1.0f;
		}
	}

	return;
}