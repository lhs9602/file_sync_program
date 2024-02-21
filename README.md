# 파일 동기화 프로그램


![image](https://github.com/lhs9602/file_sync_program/assets/34961388/342a2007-ca25-4ccb-9104-e6db1bd8c2f7)



목차

## 프로젝트 소개

<p align="justify">
서버-클라이언트 간에 주기적으로 변경되는 파일들을 동기화해주는 리눅스 프로그램
</p>

<p align="center">
GIF Images
</p>

<br>

## 기술 스택
 <img src="https://img.shields.io/badge/c-007396?style=for-the-badge&logo=c&logoColor=white"> <img src="https://img.shields.io/badge/c++-00599?style=for-the-badge&logo=cplusplus&logoColor=white"> <img src="https://img.shields.io/badge/ubuntu-E95420?style=for-the-badge&logo=ubuntu&logoColor=white"> <img src="https://img.shields.io/badge/virtualbox-394EFF?style=for-the-badge&logo=virtualbox&logoColor=white">

<br>

## 구현 기능

### TCP 소켓 통신
- 동일한 로켈 환경과 다른 pc에서 소켓 통신 구현
- select를 사용한 다중 클라이언트 연결 구현
- setsocket 함수로 소켓에 다양한 옵션 설정

### uthash 라이브러리를 통한 파일 관리
- 해쉬 테이블을 사용하여, 파일 정보를 빠르고 효율적인 CRUD 구현  
- 업데이트 시간을 해쉬 테이블에 저장하고, 비교하며 파일의 변경여부 파악

### 스레드를 통한 병렬 전송
- pthread를 사용한 멀티 스레드 구현.
- 복수의 대상에 데이터 전송 시, 스레드를 사용하여 전체 전송 시간 감소
  
### 데이터 직렬화
- 다양한 타입의 데이터들을 바이트로 변환하여 한번에 전송하여 오버헤드 감소
- 직렬화 데이터 앞에 헤더를 붙여 직렬화 데이터의 정보를 수신자에게 알림

### 압축화
- zlib를 사용한 데이터 압축화를 구현하여, 대용량의 데이터를 효율적으로 전송

### gtest를 사용한 유닛 테스트
- 주요 함수마다 유닛 테스트를 작성하여, 유지보수에 용이
- test fixture로 테스트 실행 전에 필요한 환경을 구축

<br>

## 배운 점 & 아쉬운 점

<p align="justify">

</p>

<br>
