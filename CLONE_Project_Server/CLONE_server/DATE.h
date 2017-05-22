/*=========================================
#17.01.01 ~ 17.01.07
프로젝트 생성
싱글톤 및 객체삭제 매크로 구현
CMainServer 몸체 구현
 - 하나의 객체만 생성하기 위해 싱글톤 사용
 - Initialize, progress, release
thread 객체에 대한 공부(new, join, delete)


#17.01.08 ~ 17.01.14
thread 객체를 이용한 쓰레드 생성(내용은 추가하지 않음)
 - acceptthread
 - workthread
IOCP 내의 CreateIoCompletionPort() 등을 이용한 기본틀 작업
네트워크 초기화(bind listen accept) 작업
네트워크 초기화한 내용을 acceptthread로 옮겨옴


#17.01.15 ~ 17.01.21
thread 객체를 이용해 timer에 대한 쓰레드를 만들어야겠다고 생각
 - 우선순위 큐와 연동하여 데이터에 대한 처리를 생각
 - QueryPerformanceCounter()함수를 이용해 cpu 시간을 이용
CTimeMgr 구현
tagOverLapped_EX(OVERLAPPED_EX) 구현


#17.01.22 ~ 17.01.28
tagClient(CLIENT) 구조체 구현
 - 클라이언트에 대한 관리를 할 싱글톤 매니저를 생성할 예정
 - 클라이언트 관리는 각 클라이언트 객체의 CS를 이용하여 관리
CClientMgr 내의 vector를 이용해 클라이언트 관리


#17.01.29 ~ 17.02.04
CClientMgr 내의 클라이언트 관리를 map으로 변경(키값은 ID값)
 - CLIENT 구조체 내부에 존재하던 ID를 키값으로 빼옴
CMainServer ClearThread, MakeThread 함수 구현


#17.02.05 ~ 17.02.11
CGameMgr을 이용한 게임 내부 방에 대한 관리를 할 예정으로 작성
 -> 하지만 아직 프레임워크가 완성이 되지않아서 싱글톤으로만 작성하고 넘김
유니캐스트(sendpacket)와 브로드캐스트(allsendpacket) 함수 구현
 - ClientMgr에 위치
 - 클라이언트들의 정보를 이용하기 위해 해당 위치에 작성
 - AllSendPakcet함수는 조금 불안한 점을 가지고 있음


#17.02.12 ~ 17.02.18
thread 객체의 멤버함수 중 join이라는 함수에 대한 의문점(정상적으로 작동을 안하는 듯)
 - 단순히 메인 쓰레드에서 F12키를 이용해 종료를 하면 정상적인 Release가 되지 않음
 - 심화된 공부가 필요
채팅, 서버시간을 토대로 테스트를 진행하기로 결정
 - 채팅을 확인하기 위한 더미클라이언트 생성(콘솔로 구성)
채팅, 서버시간에 대한 프로토콜 완성(CS_CHAT, SC_TIME)


#17.02.19 ~ 17.02.25
더미클라이언트 : 채팅 및 서버시간 확인용
 - WSAAsyncSelect모델 사용 -> 추후 클라이언트에서도 사용할 예정
 - WSASocket
 - WSAConnect -> 각 클라이언트마다 ID 배정해서 보내주기
 - WSARecv / WSASend
 - CServerMgr 싱글톤을 이용한 네트워크 관리
프로토콜에서 SC_Chat과 CS_Chat 두가지로 확장(아이디는 서버에서 클라로 보낼때만 사용)
보내고 받기에 대한 성공(그러나 동기화에 대한 문제로 추정되는 오류 발생)


#17.02.26 ~ 17.03.04
채팅에 대한 동기화(WorkThread 내부 Client CS 사용)
 - 다른 부분에 문제로 인해 비정상적 종료 야기
 - 추후 퍼포먼스를 높이기 위한 작업을 추가해야 함
버퍼문제로 서버에 비정상적 종료를 일으키번 AllSendPacket함수 제거 -> SendPacket함수를 응용
서버의 시간을 보내기 위해 SC_Time 구조체 생성
 - 모든 프로토콜 헤더를 수정(인덱스를 이용하는 구조 -> 구조체 상속구조)
 - 처리에 대한 문제발생 => 추후 수정예정


#17.03.05 ~ 17.03.11
게임 내부의 이벤트를 실행하기 위한 TimerQMgr 구현
 - 우선순위(시간)큐를 이용한 TimerQ
 - AddEvent() 함수 : 시간에 따른 이벤트 등록가능
 - ProgressEvent() 함수 : 이벤트를 IOCP에 등록
 - WorkThread 내부에서 처리 완료
더미클라이언트를 이용하여 TimeQMgr 확인할 예정


#17.03.12 ~ 17.03.18
서버의 각 프레임당 시간 전송하는 CS_TIME 프로토콜 구현
서버의 매 프레임당 시간 전송
 - 차후 서버의 현재 시간으로 변경할 예정(chrono)
더미클라이언트를 이용(VK_F4)할 경우 접속중인 모든 클라이언트 전송
 - 키입력을 많이 받기에 한번만 전송되도록 설정
 - 이벤트 종류(시간, 종료) 설정


#17.03.19 ~ 17.03.25
서버실행시 파일입출력을 통해 txt파일에서 옵션 변경하도록 수정할 예정
차월 클라이언트와 연동할 계획 확인
필요 프로토콜 정리


#17.03.26 ~ 17.04.01
채팅시스템 -> 매크로채팅 시스템을 이용해 확인
 - SC_CHAT_ROOM, CS_CHAT_ROOM
 - SC_CHAT_GAME, CS_CHAT_GAME
차주부터 클라이언트와 연동작업 시작
대기실에 필요한 프로토콜 정리(SC_Ready, SC_AllUnready, SC_RemoveRoomPlayer, SC_ConnectRoom)


#17.04.02 ~ 17.04.08
인게임의 프로토콜과 대기실의 프로토콜을 분리
 - 추후 프로토콜의 type은 enum을 이용해 관리할 예정
클라이언트에 unorderedmap을 이용한 CLIENT 관리 컨테이너 생성
서버에서는 Map 컨테이너 사용(동기화 필수가 되야 될거 같음)
 - Delete
 - Insert
 - Find(이 경우에는 Read 부분이므로 동기화가 필요 없어 보임)
CriticalSection을 사용할 예정 -> 확장성을 위해 mutex로 변경할 예정


#17.04.09 ~ 17.04.15
로고에서 대기실로 씬전환 시 네트워크 연결
네트워크 연결시 로딩 CriticalSection과 충돌이 나지 않도록 동기화
대기실 내부의 옵션(라운드 수, 봇의 갯수, 모드) 동기화
 - CriticalSection 사용
 - MAX값을 서버에서 확인 후 모든 클라이언트에게 broadcast
대기실 내부의 준비상태 동기화
 - CriticalSection 사용
 - 옵션과 같이 변경을 할 경우 오류발생(동기화 문제로 예상됨)


#17.04.16 ~ 17.04.22
대기실 내부의 준비상태 오류 수정
모든 플레이어가 레디상태가 되어야 방장의 Start버튼 활성화
인게임 화면 전환 시 모든 클라이언트의 준비상태 다시 false로 변경
인게임 화면 전환 시 카운트다운 동기화
 - 이벤트를 이용한 처리
 - TimerQ thread에 등록하여 4초, 3초, 2초, 1초시 마다 값 전송


#17.04.23 ~ 17.04.29(플레이어동기화)
map 컨테이너에서 unordered_map으로 교체(성능상향을 위해 변경)
글로벌 임계영역 관리는 mutex로 변경(다양한 임계영역 관리를 위해 변경)
WorkThread 생성할 경우 해당 컴퓨터의 정보를 읽어오는 함수 추가
 - 다른 컴퓨터에 최적화된 서버실행 가능
 - 프로세서에 최적화된 WorkThread 갯수에 대한 공부(코어수 X 2)
LogicThread 생성
 - 서버의 시간을 전송할 용도로 사용할 예정


#17.04.30 ~ 17.05.06(봇, 아이템동기화)
MathHelper, GameTimer 클래스 생성
 - MatherHelper는 충돌체크시 확인하는 용도로 사용할 예정
 - GameTimer는 LogicThread에서 사용
NaviMgr 클라이언트에서 가져오기
NaviCell, Line2D 클래스 역시 클라이언트에서 가져오기
 - 모든 객체의 지형충돌 용도
게임 시작 시 모든 클라이언트에게 초기 오브젝트들의 위치 전송
 - SC_INIT_PLAYER
 - SC_INIT_BEAD
 - SC_INIT_BOT


#17.05.07 ~ 17.05.13(충돌체크)
LogicThread에서는 플레이어, 봇들의 동기화까지 확인
TimerThread의 명칭 ReservationThread로 변경
 - 의미가 GameTimer와 비슷하기 때문에 구별
 - LogicThread는 일정 시간 동기화하는 객체들과 서버의 시간변화를 전송
 - ReservationThread는 TimerQ를 이용하여 예약된 메세지들에 대한 처리
ObjMgr 싱글톤 클래스를 이용하여 모든 객체 관리
지형을 이용하는 Player, Bot클래스의 지형충돌 완료
게임 시작 시 CountDown의 동기화 + 게임 내부 시간의 동기화 완료


#17.05.14 ~ 17.05.15(중간발표 전 오류확인)
Player, Bot 클래스의 애니매이션 동기화 완료
Bot은 ReservationThread를 이용하여 일정시간동안 걸어가야할 각도와 거리를 지정 및 예약
충돌체크 동기화 완료
 - Player vs Player
 - Player vs Bot
 - Player vs Bead
클라이언트 우측 상단 KillLog에 대한 정보 서버로 부터 전송
클라이언트 UI연동(구슬, 시간)
 - 라운드에 대한 UI 정보는 추후 예정
승패에 대한 정보 서버에서 계산 후 모든 클라이언트에 전송



//2월까지 서버프레임워크
//3월까지 프로토콜 설계 및 처리
//5월까지 총돌처리 및 동기화
//7월까지 최적화 및 로비 및 로그인
==========================================*/