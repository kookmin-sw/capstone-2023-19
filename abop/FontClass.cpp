#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
#include "FontClass.h"
#include "TextureClass.h"
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


bool FontClass::Initialize(ID3D11Device* device, char* fontFilename, WCHAR* textureFilename)
{
	bool result;


	// �۲� �����͸� �ҷ��ɴϴ�
	result = LoadFontData(fontFilename);
	if(!result)
	{
		return false;
	}

	// �۲� �ؽ�ó�� �ҷ��ɴϴ�
	result = LoadTexture(device, textureFilename);
	if(!result)
	{
		return false;
	}

	return true;
}


void FontClass::Shutdown()
{
	// �۲� �ؽ�ó ����
	ReleaseTexture();

	// �۲� ������ ����
	ReleaseFontData();

	return;
}

// �ؽ�ó������ ���� ��ġ ������ ��� �ִ� fontdata.txt�� �ҷ��ɴϴ�
bool FontClass::LoadFontData(char* filename)
{
	ifstream fin;
	int i;
	char temp;


	// FontType���� �迭�� ����ϴ�
	font_ = new FontType[95];
	if(!font_)
	{
		return false;
	}

	// ������ ���� �� ������ �о� �迭�� �����մϴ�
	fin.open(filename);
	if(fin.fail())
	{
		return false;
	}

	// �ؽ�Ʈ�� ASCll����, left, right, ���� �ȼ� �ʺ� �о�ɴϴ�
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
	// �о�� �迭 ������ ����
	if(font_)
	{
		delete [] font_;
		font_ = 0;
	}

	return;
}

//font.dds ������ �о� �ؽ��� ���̴��� �����մϴ�
// �ؽ��� ���̴����� ���ڸ� �̾Ƴ� ������ �簢���� ���� ȭ�鿡 �׸��� �˴ϴ�
bool FontClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;


	// Texture ��ü ����
	texture_ = new TextureClass;
	if(!texture_)
	{
		return false;
	}

	// Texture ��ü �ʱ�ȭ
	result = texture_->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	return true;
}


void FontClass::ReleaseTexture()
{
	// Texture ��ü ����
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
	//�۲��� ������ �׷��� �� �ֵ��� �۲� �ؽ�ó�� �������̽��� �����մϴ�
	return texture_->GetTexture();
}


void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters, index, i, letter;


	// ���� ���۷� �������ϴ�
	vertexPtr = (VertexType*)vertices;

	// ������ ���̸� �����ɴϴ�
	numLetters = (int)strlen(sentence);

	// �ε����� ���� �迭�� �ʱ�ȭ�մϴ�
	index = 0;

	// �ݺ����� ���鼭 ����/�ε��� �迭�� ����ϴ�
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
			// ���ڴ� �ΰ��� �ﰢ���� ����� left, right��ǥ�� �ȼ� �ʺ� ���ν�ŵ�ϴ�
			
			// ù��° �ﰢ��
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = D3DXVECTOR2(font_[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3((drawX + font_[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = D3DXVECTOR2(font_[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX, (drawY - 16), 0.0f);  // Bottom left.
			vertexPtr[index].texture = D3DXVECTOR2(font_[letter].left, 1.0f);
			index++;

			// �ι�° �ﰢ��
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);  // Top left.
			vertexPtr[index].texture = D3DXVECTOR2(font_[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX + font_[letter].size, drawY, 0.0f);  // Top right.
			vertexPtr[index].texture = D3DXVECTOR2(font_[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3((drawX + font_[letter].size), (drawY - 16), 0.0f);  // Bottom right.
			vertexPtr[index].texture = D3DXVECTOR2(font_[letter].right, 1.0f);
			index++;

			// ���ڿ� �ش��ϴ� ������ ����� x��ǥ�� ������Ʈ�Ͽ� ���ڰ� �׷��� ��ġ�� ����ϴ�
			drawX = drawX + font_[letter].size + 1.0f;
		}
	}

	return;
}