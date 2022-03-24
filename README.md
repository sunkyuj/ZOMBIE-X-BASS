# ZOMBIE-X-BASS

사용한 보드: Arduino Nano 33 BLE Sense

- central: 컴퓨터에 연결하는 쪽 아두이노
  - central 나노가 peripheral 나노에 정보 요청
  
  - 받은 정보를 토대로 화면에 결과를 수행 
    1. 악기의 위치, 방향 -> 총 조준점 (마우스 커서의 위치)
    2. 악기 연주 -> 총 발사 또는 수류탄 투척 (마우스 좌클릭 또는 마우스 휠 클릭 명령)
    3. 버튼 입력 -> 장전 (마우스 우클릭 명령)

- peripheral: 악기에 연결하는 쪽 아두이노 
  - central 나노에 정보를 주는 역할
  
  - 물리적인 동작 수행을 peripheral 나노가 측정하고 이를 정보로 넘겨줌
    1. 악기의 위치, 방향 (Arduino Nano 33 BLE Sense 에 내장된 자이로센서 이용) <- 칼만필터 적용 요망
    2. 악기 연주 (각 줄마다 진동센서 부착)
    3. 버튼
    
  - 각 동작(총,장전,수류탄)마다 서로 다른 LED 색 출력

--- 

테크노페럿 666버전 사용

---

![image](https://user-images.githubusercontent.com/38223044/160002180-c667fe0b-6d8a-4a26-a17c-4bc03f29fab3.png)
![image](https://user-images.githubusercontent.com/38223044/160002237-cbc2a6cc-5685-49ca-af1d-afd66822f111.png)
![image](https://user-images.githubusercontent.com/38223044/160002287-8c2bfeb1-8747-4eb4-9fa4-1e1b69190531.png)
![image](https://user-images.githubusercontent.com/38223044/160002318-d809ee33-57d7-4c71-a4f5-87cd817b422b.png)

---
- 시연 영상

![ezgif-4-3f1bf9776c](https://user-images.githubusercontent.com/38223044/160005275-86a11197-173b-4b46-9099-2cd8222f6075.gif)

