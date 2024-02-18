# MyCracker
2학년 2학기 시스템프로그래밍및보안 과제 : 크래커 만들기

### 0.	환경 설정
우분투 22.04.3 환경에서 과제를 진행하였다. 쉘에서 /etc/shadow를 열어서 확인해본 결과 해시 문자열이 $y$로 시작하였으며, 이는 yescrypt 해시 함수를 사용함을 뜻하며, 이에 따라 github.com/openwall/yecrypt에서 라이브러리를 설치해 과제를 진행하였다.

### 1.	전처리기

yescrypt 해시 함수를 사용하기 위해 yescrypt.h를 include 하였으며, sysendian은 라이브러리를 설치했지만 /usr/include에서 찾을 수 없어서 지역 파일로 include 해주었다.
MAX_AVAILABLE : 패스워드로 가능한 문자의 개수이다.
MAX_BUFF : 앞으로 다룰 코드에서 문자열 하나에 들어갈 수 있는 최대 길이이다.
DELEMITER : /etc/shadow 에 적힌 문자열을 strtok() 하여 분리할 때 사용할 문자이다.
MAX_USERS : /etc/shadow에 있는 유저들(해시가 * 또는 !가 아닌 유저들)의 유저이름, 세팅, 해시 값을 배열로 다룰 예정이다. 이 때 한 번에 다룰 수 있는 최대 유저수 이다.
MAX_SETTING_LEN : setting 문자열에 들어갈 최대 길이이다. NULL 문자를 생각하여 1 추가해주었다. 이 때 setting이란 yescrypt 함수로 패스워드를 암호화한 다음 나오는 문자열 중에 마지막 $까지의 문자열을 뜻한다. 예를 들어, /etc/shadow의 어떤 사용자의 해시 문자열이  
\$y\$j9T$O9hIeMkIae2MTR3MopVqk/$CViJWSSPAyj1nqUGqnbMfDNcqp8TmgEwq4IUVlRsOxD
라면, \$y\$는 yescrypt, $j9T$는 yescrypt의 매개변수(메모리, 인코딩 정보), $O9…./$는 솔트를, 그 뒤의 나머지 문자열은 실제 해시 값을 뜻한다.
MAX_HASH_LEN : 해시 문자열에 들어갈 수 있는 최대 길이이다. NULL 문자를 생각하여 1 추가해주었다.
MAX_PASSWD_LEN : 비밀번호 문자열의 최대 길이이다. NULL 문자를 생각하여 1 추가해주었다.
MAX_USERNAME_LEN : 사용자명 문자열의 최대 길이이다. NULL 문자를 생각하여 1 추가해주었다.
MAX_LINE_LENGTH : 파일에서 한줄씩 받아들일 때 저장할 최대 길이이다.
char available[MAX_AVAILABLE]; : 비밀번호로 가능한 문자 68개를 이 배열에 차례대로 저장한 다음, 인덱스로 접근하여 문자를 얻어 무작위 대입법에 사용할 것이다.
typedef struct shadow {}; : 사용자명, 세팅, 해시 정보를 담을 구조체이다.
shadow hashArr[MAX_USERS] : /etc/shadow파일에서 해시를 얻을 수 있는 사용자들의 정보가 담긴 배열이다.
int sizeHashArr = 0; : hashArr 배열의 크기를 나타내는 변수이다. 배열에 값을 하나 채울 때마다 1씩 더하며, 나중에 패스워드를 크래킹할 때 사용할 반복문을 위하여 전역변수로 선언해두었다.

### 2.	parse_shadow_file(char* line, int i)

/etc/shadow에서 가져온 문자열 한 줄 에서 :로 구분해낸 해시 테이블을 매개변수 line으로 받아 hashArr에 setting과 hash를 저장한다. 매개변수 int i hashArr에 접근하기 위한 인덱스이다.

### 3.	getHashFromFile(FILE* fp)

parse_shadow_file()이 호출되는 곳이다. main()에서 open한 파일 포인터를 받아 한 줄씩 읽으며, 받은 문자열을 :로 구분하여서 첫 번째 문자열이 username, 두 번째 문자열이 해시 테이블인데 이 때 이 해시테이블이 가려져있다면(*, !) 다시 문자열을 가져오고, 성공적으로 가져왔다면 사용자명을 배열에 저장한 다음 같은 인덱스의 공간에 parse_shadow_file()을 통해 setting과 hash값도 함께 저장해준다.

### 4.	crackFromWordlist(FILE* wordlist)

hashArr에 저장된 setting 값을 바탕으로 wordlist에 있는 사전 단어들을 yescrypt함수를 통해 해싱한 다음 같은 인덱스에 저장된 hashArr의 hash값과 비교하여 같다면 그 단어가 패스워드이므로 이를 출력하고 반복을 종료한다.

### 5.	void setArr()

 available 배열에 가능한 문자들을 저장해준다. 가능한 문자는 대소문자, 숫자, 특수문자 !,@,#,$,^,* 이다.
 
### 6.	void crackRandom(int num)

한자리 비밀번호부터 6자리 비밀번호 까지 가능한 경우의 수를 모두 검사하여 해시 값을 비교한다. 해시 값이 같은 패스워드를 발견했다면 패스워드와 실행시간을 출력하고 함수를 종료한다. 

### 7.	int main(int argc, char* argv[])

먼저 명령행 인수를 검사한다. 인수가 3개가 들어왔다면 플래그가 들어온 것이므로 argv[1]을 검사하여서, DELIMITER :로 나누어서 플래그가 올바르게 들어왔는지, 들어왔다면 wordlist 파일 열기를 시도하고 파일 열기에 성공했는지, 또 passwd-file이 담긴 target 파일도 열기를 시도해서 실패했다면 에러메시지를 출력하고 프로그램을 종료하도록 해두었다. 인수가 2개가 들어왔다면 wordlist 없이 무작위 대입법만 시도하는 것이므로 target 파일만 열어준다.
target에 저장된 /etc/shadow 파일 포인터를 바탕으로 먼저 getHashFromFile() 함수를 호출하여 hashArr 배열에 해시 테이블과 관련 정보를 저장해준다. 또한 제대로 저장되었는지 확인하기 위하여 콘솔창에 현재 프로그램에 저장된 해시 값과 세팅 값, 사용자 명을 출력해준다.
만약 wordlist 파일 포인터가 NULL이 아니라면 wordlist 파일을 성공적으로 연 것이므로 이 파일 포인터를 매개변수로 crackFromWordlist() 함수를 호출하여 크래킹을 진행해준다.
사전 대입 방식 크래킹 함수 종료 후에는 무작위 대입법 크래킹을 진행해준다. crackRandom() 함수는 현재 확인중인 hashArr의 인덱스 번호를 매개변수로 받게 되어있으니 sizeHashArr 전역변수를 이용해 반복뭍을 돌려준다.
