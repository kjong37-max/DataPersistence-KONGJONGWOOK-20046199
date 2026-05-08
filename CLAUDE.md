# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 프로젝트 개요

데이터 영속성(Data Persistence) 처리를 위한 PoC(Proof of Concept) 프로젝트입니다.  
여기서 영속성이란 **애플리케이션을 재실행해도 데이터를 유지할 수 있는 성질**을 의미합니다.

## 빌드 환경

- **언어**: C++20 (`stdcpp20`)
- **툴체인**: MSVC v145 (Visual Studio 2022)
- **대상 플랫폼**: Windows 10.0, Win32/x64 모두 지원
- **출력 형식**: 콘솔 애플리케이션

### 빌드 명령어

Visual Studio IDE에서 빌드하거나, MSBuild CLI를 사용합니다.

```powershell
# Debug x64 빌드
msbuild Project1.vcxproj /p:Configuration=Debug /p:Platform=x64

# Release x64 빌드
msbuild Project1.vcxproj /p:Configuration=Release /p:Platform=x64
```

빌드 결과물은 `x64\Debug\` 또는 `x64\Release\` 디렉터리에 생성됩니다.

## 프로젝트 구성

현재 소스 파일이 없는 초기 상태의 PoC 프로젝트입니다. `.vcxproj`의 필터 구조는 다음과 같습니다.

- **소스 파일** — `.cpp`, `.c`, `.cxx` 등
- **헤더 파일** — `.h`, `.hpp`, `.hxx` 등
- **리소스 파일** — `.rc`, `.ico`, `.bmp` 등

새 파일을 추가할 때는 `.vcxproj`의 `<ItemGroup>` 및 `.vcxproj.filters`에 각각 항목을 등록해야 합니다.

## 데이터 영속성 구현 방향 (PoC 범위)

이 프로젝트에서 검토할 수 있는 영속성 방식의 예시입니다.

| 방식 | 설명 |
|------|------|
| 파일 I/O (바이너리/텍스트) | `std::fstream`으로 구조체·객체 직렬화 |
| JSON/XML 직렬화 | 외부 라이브러리(nlohmann/json, pugixml 등) 활용 |
| SQLite | 경량 내장형 관계형 DB, 단일 파일로 관리 |
| Windows Registry | `RegOpenKeyEx` / `RegSetValueEx` 등 Win32 API |
| Memory-Mapped File | `CreateFileMapping` / `MapViewOfFile` Win32 API |

외부 라이브러리를 추가할 경우 vcpkg 또는 NuGet을 통해 관리하는 것을 권장합니다.
