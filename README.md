# 파일 동기화 프로그램


![image](https://github.com/lhs9602/file_sync_program/assets/34961388/342a2007-ca25-4ccb-9104-e6db1bd8c2f7)



목차

## 프로젝트 소개

<p align="justify">
서버-클라이언트와 마스터 서버-슬레이브 서버에서 주기적으로 변경되는 파일들을 동기화해주는 리눅스 프로그램

자세한 개발 기록은 블로그 참조:
https://lhs9602.tistory.com/category/%ED%94%84%EB%A1%9C%EC%A0%9D%ED%8A%B8/%ED%8C%8C%EC%9D%BC%20%EB%8F%99%EA%B8%B0%ED%99%94
</p>

<br>

### 개발 및 운영 환경
1. c언어로 개발
2. vs 코드 사용
3. ubuntu 20.04 에서 동작
4. 주요 기능에 대해 google test로 유닛테스트 작성.
5. cmake로 컴파일하는 cli 기반 프로그램


### 1차 목표 - 서버-클라이언트 동기화
1. 서버에서 클라이언트에게 파일을 전송하여, 두 기기 간의 파일 동기화를 진행한다.
2. 파일의 경로가 저장된 동기화 리스트.txt를 서버가 가지고 있으며, 이 리스트는 주기적으로 업데이트 된다.
3. 이때 동기화 리스트의 경로를 실행시 인자로 입력한다.
4. 최초로 서버와 클라이언트 연결 시, 서버는 동기화 파일들을 클라이언트에 전송한다.

### 1차 목표 - 서버-클라이언트 동기화
1. 서버에서 클라이언트에게 파일을 전송하여, 두 기기 간의 파일 동기화를 진행한다.
2. 파일의 경로가 저장된 동기화 리스트.txt를 서버가 가지고 있으며, 이 리스트는 주기적으로 업데이트 된다.
3. 이때 동기화 리스트의 경로를 실행시 인자로 입력한다.
4. 최초로 서버와 클라이언트 연결 시, 서버는 동기화 파일들을 클라이언트에 전송한다.
5. 이후 서버의 동기화 리스트 혹은 동기화 파일이 업데이트 되었을 경우, 변경 혹은 추가된 파일만 클라이언트에 전송한다.
6. 모든 전송은 데이터들을 바이트로 변환하고 직렬화하여 전송한다.
7. 일정 크기 이상의 데이터를 전송시, 압축하여 전송한다.
8. 클라이언트는 파일을 수신 후, 파일 명과 전송시간,크기를 출력한다.
9. 최대 20개의 클라이언트가 서버에 동시에 요청을 보낼 수 있으며, 서버는 스레드를 사용해 요청을 병렬적으로 처리한다. 

### 초기 동기화
![초기 동기화](https://github.com/lhs9602/file_sync_program/assets/34961388/88dae019-096d-4d23-8084-2c381d206b94)


### 업데이트 된 동기화 파일 전송
![업데이트 추가](https://github.com/lhs9602/file_sync_program/assets/34961388/351afedd-d7f9-448b-a4f2-9e7003a68fd5)


![업데이트 변경](https://github.com/lhs9602/file_sync_program/assets/34961388/fc770359-6b40-4292-8868-1ea6d19531b7)

### 2차 목표- 서버 간 동기화
1. 하나의 서버에 주기적으로 업데이트 되는 다른 서버의 ipv4 주소가 담긴 서버 리스트.txt 을 보유하며, 이를 통해 다른 서버와의 통신을 요청한다.
2. 데이터를 전송하는 서버를 마스터, 수신하는 쪽은 슬레이브라고 부른다.
3. 동기화 리스트처럼 서버 리스트도 인자로 입력받는데, 이 인자의 유무에 따라 마스터-슬레이브가 구분된다.
4. 각 서버와의 통신을 스레드를 생성하여 병렬적으로 처리한다.
5. 동기화시 마스터 서버에서 파일의 체크썸(sha256)을 경로와 함께 전송한다.
6. 슬레이브 서버는 이를 수신하고, 자신의 동기화 파일들을 중 없는 경로와 체크썸이 다른 파일들의 경로를 마스터에 전송한다.
7. 그럼 마스터 서버는 해당 경로들의 파일 데이터를 슬레이브 서버에 전송한다.
8. 양쪽의 전송간 직렬화와 압축은 1차와 동일하다.


### 다른 서버와의 동기화
![다른 서버 동기화](https://github.com/lhs9602/file_sync_program/assets/34961388/d2868be6-e374-4ae3-9f9f-c7c8b7233599)




## 기술 스택
 <img src="https://img.shields.io/badge/c-007396?style=for-the-badge&logo=c&logoColor=white"> <img src="https://img.shields.io/badge/c++-00599?style=for-the-badge&logo=cplusplus&logoColor=white"> <img src="https://img.shields.io/badge/ubuntu-E95420?style=for-the-badge&logo=ubuntu&logoColor=white"> <img src="https://img.shields.io/badge/virtualbox-394EFF?style=for-the-badge&logo=virtualbox&logoColor=white"> <img src="https://img.shields.io/badge/makefile-E95420?style=for-the-badge&logo=makefile&logoColor=white"> <img src="https://img.shields.io/badge/cmake-394EFF?style=for-the-badge&logo=cmake&logoColor=white">

<br>

## 구현 기능

### TCP 소켓 통신
- 동일한 로컬 환경과 다른 pc에서 소켓 통신 구현
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
- 주요 함수마다 유닛 테스트를 작성하여, 유지보수에 용이하게 구성
- test fixture로 테스트 실행 전에 필요한 환경을 구축

<br>

## 사용법
리눅스 환경에서 실행가능하며, bin 폴더에 있는 server와 client 파일을 실행하면 됩니다.
동기화 리스트의 예시는 shared_dir에 있는 sync_list.txt를 참조

### bin폴더 이동
```
cd /file_sync_program/bin
```

### server
``` 
./server [동기화 리스트 경로]
```

### master server
``` 
./server [동기화 리스트 경로] [서버 리스트 경로]
```

### client
``` 
./client
```

### test
```
./test_main
```

### 다른 서버 동기화 시, 방화벽이 때문에 실패할 경우
```
#12346포트에 tcp통신 허용
sudo ufw allow 12346/tcp
```

```
#방화벽 비활성화
sudo ufw enable
```

<p align="justify">

</p>

<br>
