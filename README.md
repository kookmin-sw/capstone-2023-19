# Algorithmic Beauty of Plants
- 식물의 성장을 알고리즘화하여 생성하고 DirectX를 활용하여 Timelapse와 같은 방법으로 시각화하는 프로젝트입니다.
- L-system에 기초하여 직접 나무와 꽃을 생성해볼 수 있습니다.
- Unreal 엔진에서 사용할 수 있도록 라이브러리로 만들어 배포할 예정입니다.

- This is the project of creating a plant's growth by algorithm and visualize it with Timelapse utilizing DirectX.
- Based on L-System, you can create trees and flowers on your own.
- We are going to make it as a library and distribute it so that it can be used at Unreal engine.

## Preview

### 2D Fractal
![](/docs/frac1.png)
```
lSystem->SetWord("F+F+F+F"); // LSystem 초기화
lSystem->AddRule('F', "F+f-FF+F+FF+Ff+FF-f+FF-F-FF-Ff-FFF"); // LRule 1 추가
lSystem->AddRule('f', "fffff"); // LRule 2 추가
lSystem->SetAngleChange(90.0f); // δ = 90º, 각도는 90도
lSystem->Iterate(2); // n = 2, 2회 반복 연산
```


![](/docs/frac2.png)
```
lSystem->SetWord("F-F-F-F"); // LSystem 초기화
lSystem->AddRule('F', "F-FF--F-F"); // LRule 1 추가
lSystem->SetAngleChange(90.0f); // δ = 90º, 각도는 90도
lSystem->Iterate(5); // n = 5, 5회 반복 연산
```


![](/docs/frac3.png)
```
lSystem->SetWord("-L"); // LSystem 초기화
lSystem->AddRule('L', "LF+RFR+FL-F-LFLFL-FRFR+"); // LRule 1 추가
lSystem->AddRule('R', "-LFLF+RFRFR+F+RF-LFL-FR"); // LRule 2 추가
lSystem->SetAngleChange(90.0f); // δ = 90º, 각도는 90도
lSystem->Iterate(3); // n = 3, 3회 반복 연산
```


### 2D Tree
![](/docs/2dTree1.png)
```
lSystem->SetWord("X"); // LSystem 초기화
lSystem->AddRule('X', "F-[[X]+X]+F[+FX]-X"); // LRule 1 추가
lSystem->AddRule('F', "FF"); // LRule 2 추가
lSystem->SetAngleChange(22.5f); // δ = 22.5º, 각도는 22.5도
lSystem->Iterate(5); // n = 5, 5회 반복 연산
```


### 3D


## 팀 소개

#### DirectX Viewer, Library
- ****3143 이제현
- ****1660 이수아
#### Growth System
- ****1655 이동훈
- ****0694 방지완

## Guides
1. Visual studio 2019^를 사용하여 `master` 브랜치 내 `abop.sln` 프로젝트를 열어주세요.
2. `Main.cpp` 파일 wWinMain 함수에서 아래와 같이 작성합니다.
- 시작 Word	설정
```
lSystem->SetWord("A");
```
- Rule 설정
```
lSystem->AddRule('A', "B - F + CFC + F - D & F^D - F + && CFC + F + B//");
lSystem->AddRule('B', "A & F^CFB^F^D^^ - F - D^ | F^B | FC^F^A//");
lSystem->AddRule('C', "D^ | F^B - F + C^F^A && FA & F^C + F + B^F^D//");
lSystem->AddRule('D', "CFB - F + B | FA & F^A && FB - F + B | FC//");
```
- Parameter 설정
```
lSystem->SetDistance(2.0f);
lSystem->SetAngleChange(90.0f);
```
- 반복
```
lSystem->Iterate(2);
```
- 생성된 lSystem 실행
```
SystemClass* system = new SystemClass;

if (system->Initialize(lSystem))
{
    system->Run();
}
```


## Versions