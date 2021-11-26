#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <ctime>
#include <stack>
#include <vector>

#define MAX_VTXS 256
using namespace std;

static std::mt19937 gen = std::mt19937((unsigned int)time(NULL));   // 교수님께서 강의에서 알려주신 랜덤 number generator 방식을 사용하였습니다!
static std::uniform_real_distribution<> dist(0, 99);   // dist 범위가 고민이였는데 범위가 좁을시에 확률편차가 클 수도있고 최대한 늘리는 방식으로 사용하였습니다. task1은 구현이 목표기때문에 적게 설정하였습니다.

inline void error(const char* str){  // 에러처리 함수
    std::cerr << str << "\n";
    exit(1);
};

class AdjMatGraph{   // 행렬그래프를 사용하였습니다!!  task1은 구현이 목표기 때문에 교수님께서 수업 중에 사용하신 코드를 유사하게 사용하였습니다!
    protected:
        int size;
        string vertices[MAX_VTXS];
        int adj[MAX_VTXS][MAX_VTXS];

    public:
        AdjMatGraph(){ reset(); }
        ~AdjMatGraph(){}

        string getVertex(int i){ return vertices[i]; }
        int getEdge(int i, int j) { return adj[i][j]; }
        void setEdge(int i, int j, int val) { adj[i][j] = val; }  // 가중치가 없는 그래프 이기때문에 val 값은 0 or 1 로 설정하였습니다.
        bool isEmpty(){ return size==0; }
        bool isFull(){ return size>=MAX_VTXS; }

        void reset(){
            size=0;
            for(int i=0 ; i<MAX_VTXS ; i++){
                for(int j=0 ; j<MAX_VTXS ; j++){
                    setEdge(i,j,0);
                }
            }
        }

        void insertVertex(string name){
            if(!isFull()){ vertices[size++] = name; }
            else { cout << "Exceeding maximum number of vertices"; } 
        }

        void insertEdge(int u, int v){
            if(u < 0 || u >= size){
                cout << "out of range";
                exit(0);
            }
            setEdge(u, v, 1);
            setEdge(v, u, 1);       // 무방향 그래프 데이터를 사용할 예정이므로 추가 하였습니다.
        }
        void display(){
            std::cout << "총 요일의 숫자 : " << size << "\n";
            for(int i=0 ; i<size ; i++){
                std::cout << getVertex(i) << "  ";
                for(int j=0 ; j<size ; j++)  
                    std::cout << getEdge(i, j) << " ";
                std::cout << "\n";
            }
        }
};

vector<string> s;   // 구현의 편의성을 위해 전역변수 벡터로 사용했습니다.  vertex 가 들어갈 공간입니다.
vector<int> ls;    // 구현의 편의성을 위해 전역변수 벡터로 사용하였습니다. 해당 vertex와 연결된 노드의 열이 들어갈 공간입니다.
class randomwalker : public AdjMatGraph{
    public:
        bool isLinked(int u, int v){ return getEdge(u, v) != 0; }     
        void startwalk(int i, int walking_length_N){   // i는 시작하는 노드  walking_length_N 은 노드 도는 횟수 입니다!
            static int count = 0;   // 멈춤 조건을 위한 count 변수
            int p = 0;   // p는 확률입니다!!
            s.push_back(this->getVertex(i)); // 맨처음 시작 하는 노드를 s에 넣어 두고 시작합니다!
            while(count != walking_length_N-1){
                int number = 0;            // 현재 노드에 연결된 노드의 수를 체크하기 위한 number!
                for(int j = 0; j < size; j++){   // 
                    if(this->isLinked(i, j)){ // 연결되어 있으면
                        number++;             // 현재 노드의 edge수를 하나 증가시키고
                        ls.push_back(j);      // 해당 edge의 index를 기록 해둡니다.
                    }
                }
                int randnum = (int)dist(gen) % number+p;    // 위의 for문을 성공적으로 돌았다면 number에는 최대 degree가 들어 있습니다. 임의로 제가 만든 그래프의 최대degree는 3차입니다!
                // 맨 초기에 생각했던 확률 코드입니다.  2단계 --> 3단계에는 더욱 큰 데이터 셋과 정확성을 위해 task3번코드부터는 변경 했습니다.
                // task1 초기단계에 했던 생각이기 때문에 basic 생각 으로 냅두었습니다.
                // 잘못된 생각이였는데 이미 처음에 제가 만든 임의의 그래프의 max degree를 3차로 알아서 switch문으로 0, 1, 2 차의 경우를 생각했고
                // p(확률) 이 올라갈수록 default case로 빠질 확률을 높이는 초기의 코드였습니다.
                // 이렇게 코드를 짜면 p를 제외한 나머지의 확률의 오차 및 p확률이 크면 클수록 p의 확률보다 더욱더 default 로 가는 횟수가
                // 많아지는 문제가 발생했었습니다.
                switch (randnum)
                {
                    case 2:
                        count++;
                        s.push_back(this->getVertex(*(ls.begin()+2)));
                        i = *(ls.begin()+2);
                        ls.clear();
                        break;
                    case 1:
                        count++;
                        s.push_back(this->getVertex(*(ls.begin()+1)));
                        i = *(ls.begin()+1);
                        ls.clear();                        
                        break;
                    case 0:
                        count++;
                        s.push_back(this->getVertex(*(ls.begin())));
                        i = *(ls.begin());
                        ls.clear();
                        break;
                    default:
                        count++;
                        int whereareyou = (int)dist(gen) % size;  // p의 확률로 모든 노드를 랜덤으로 방문하므로 모든 노드로 랜덤넘버를 % 처리하여 랜덤 노드로 가도록 했습니다.
                        s.push_back(this->getVertex(whereareyou));
                        i = whereareyou; // 다음 노드를 가리키고
                        ls.clear();  // 벡터를 초기화 시킵니다.
                        break;
                }
            }
        }
        void pagerank(int walklength, string* yoel){ // 제가 만든 임의의 그래프는 각 요일마다 가고싶은 요일로 설정했습니다. ex) 토,일 요일은 다시 금요일로가서 다시 휴식을 만끽하고싶고, 월 -> 화,수 정도로 하루라도 휴일로가자, 목요일은 주말이 되고싶다는 생각이였습니다..  하지만 가중치값이 없기때문에..
            int i = 0;
            int arr[7] = {0};
            for(int i = 0; i < s.size(); i++){  // 벡터에서 꺼내와 해당요일과 일치하면 1씩 높였습니다.
                if(s[i] == "월"){
                    arr[0] += 1;
                }
                else if(s[i] == "화"){
                    arr[1] += 1;
                }
                else if(s[i] == "수"){
                    arr[2] += 1;
                }
                else if(s[i] == "목"){
                    arr[3] += 1;
                }
                else if(s[i] == "금"){
                    arr[4] += 1;
                }
                else if(s[i] == "토"){
                    arr[5] += 1;
                }
                else{
                    arr[6] += 1;
                }
            }
            for(int k = 0; k < sizeof(arr) / sizeof(int); k++){  // 배열을 돌며 해당요일의 방문횟수, 총 길이, 각 요일별 pagerank를 보여줍니다.
                cout << "\n" << yoel[k] << "요일의 방문 횟수 " << arr[k] << "\n";
                cout << "총 Trail의 길이 : " << walklength << "\n";
                cout << yoel[k] << "요일의 PageRank = " << arr[k] << "/" << walklength << " = " << (arr[k] / (double)walklength) << "\n";
            }
        }
};

int main(){
    int start = 0;
    int walkinglength = 10000;
    string arr[7] = {"월", "화", "수", "목", "금", "토", "일"}; //제가 만든 임의의 그래프는 각 요일마다 가고싶은 요일로 설정했습니다. ex) 토,일 요일은 다시 금요일로가서 다시 휴식을 만끽하고싶고, 월 -> 화,수 정도로 하루라도 휴일로가자, 목요일은 주말이 되고싶다는 생각이였습니다..  하지만 가중치값이 없기때문에..
    randomwalker day;
    vector<string> useforprint;
    for(int i = 0; i < 7; i++){
        day.insertVertex(arr[i]);
    }
    day.insertEdge(0, 1);
    day.insertEdge(0, 2);
    day.insertEdge(0, 3);
    day.insertEdge(3, 6);
    day.insertEdge(3, 5);
    day.insertEdge(4, 6);
    day.insertEdge(4, 5);
    day.startwalk(start, walkinglength);
    day.display();
    cout << "\n";
    ls.clear();
    // for(int i = 0; i < s.size(); i++){    지나온 경로를 표시해주는 for문입니다! 
    //     cout << s[i] << " ";
    // }
    day.pagerank(walkinglength, arr);
    return 0;
}