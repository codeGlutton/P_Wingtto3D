# :computer: P_Wingtto
<div align="center">
<img src="https://github.com/user-attachments/assets/9db7669b-a458-43aa-9bd0-1575c134b2f7" width="600" height="450"/>
</div>

This project is an experimental attempt to mimic the Unreal Widget System by developing a custom engine using DirectX 11. It was created with a focus on implementing the Prepass and Painting processes.  

본 프로젝트는 DirectX11을 활용한 자체 엔진을 제작하여 언리얼 위젯 시스템을 모방해본 실험용 프로젝트입니다. Prepass와 Painting 과정 구현에 집중하여 제작되었습니다.

## :pushpin: Main Architecture (핵심 아키텍처)
<div align="center">
  <img src="https://github.com/user-attachments/assets/4104e7b3-f7c2-463b-9625-089331dcfd0a" width="300" height="225"/>
  <img src="https://github.com/user-attachments/assets/e79143ba-cc8e-467f-8ba3-a3afd85f8380" width="300" height="225"/>
</div>

<ol>
<li>Widgets are processed through a tree in two stages: Prepass and Paint. In the Prepass stage, where the desired size is calculated, traversal prioritizes children first (bubbling up).</li>
<li>In the Paint stage, processing follows the parent order based on ZOrder, and the actual rendering elements are stored in the WidgetRenderElementContainer (equivalent to Unreal’s FSlateDrawElement). This stage also computes the final render geometry and stores it in a cell-based Hit Grid.</li>
<li>Once all elements are stored, they are sent to the Render Thread and drawn in batches.</li>
<li>The stored Hit Grid is then used in the next tick for widget input handling, serving as the Widget Path.</li>
</ol>

<ol>
<li>위젯은 트리를 거쳐 Prepass와 Paint 두 단계로 나뉘어 진행됩니다. Desired Size를 구하는 Prepass 단게에서는 자식을 우선적으로 버블링됩니다.</li>
<li>Paint 단계에서는 부모 순으로 ZOrder에 따라 진행되며 실제 랜더링 항목을 WidgetRenderElementContainer에 저장합니다. (언리얼의 FSlateDrawElement와 동일) 최종 Render Geometry를 연산하고 셀 단위의 Hit Grid에 저장하는 단계이기도 합니다.</li>
<li>모든 Element가 저장되었다면 Render Thread에 전달하여 Batch 단위로 출력합니다.</li>
<li>저장된 Hit Grid는 다음 틱의 위젯 Input 처리에서 Widget Path로 사용됩니다.</li>
</ol>

## :wrench: Tools & Technologies (사용한 기술)
- **Client** : DirectX11
- **Lib** : DirectXTex, SimpleMath, FreeType, nlohmann

## :octocat: Git Commit Comment Rule (커밋 규칙)
```
type: Title

body(Optional)
```
```
Types
+ feat : 기능 추가
+ fix : 오류 수정
+ docs : 문서 추가
+ assset : 에셋 추가
```

## :open_file_folder: Project Folder Rules (프로젝트 폴더 규칙)
```
Libraries : 외부 종속 파일들
└ Lib : 동적 및 정적 라이브러리
└ Include : 헤더 포함 파일

Engine : 엔진 프로젝트
└ Core : 핵심 엔진 코드
└ Refelection : C++ 리플렉션 코드

ConsoleTest : 콘솔 테스트 프로젝트

Client : 클라이언트 프로젝트

Resources : 에셋
```

## :pencil: Naming Rules (이름 규칙)
Code Prefix Name Rule
```
On"Verb" : 대리자 Prefix
_ : private 변수 Prefix
m : member 변수 Prefix
```

