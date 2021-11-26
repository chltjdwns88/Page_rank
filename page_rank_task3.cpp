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

static std::mt19937 gen = std::mt19937((unsigned int)time(NULL));
static std::uniform_real_distribution<> dist(0, 100000);  // 1번 cpp 자료에서 말씀드린대로 dist의 랜덤확률 우수성을 위해 숫자를 늘렸습니다.

inline void error(const char* str){
    std::cerr << str << "\n";
    exit(1);
};

class AdjMatGraph{
    protected:
        int size;
        string vertices[MAX_VTXS];
        int adj[MAX_VTXS][MAX_VTXS];
        string starwarsname[38];   // starwars 캐릭터들의 이름을 저장하기위한 배열입니다.

    public:
        AdjMatGraph(){ reset(); }
        ~AdjMatGraph(){}

        string getVertex(int i){ return vertices[i]; }
        int getEdge(int i, int j) { return adj[i][j]; }
        void setEdge(int i, int j, int val) { adj[i][j] = val; }
        bool isEmpty(){ return size==0; }
        bool isFull(){ return size>=MAX_VTXS; }

        void reset(){
            size=0;
            for(int i=0 ; i<MAX_VTXS ; i++){
                for(int j=0 ; j<MAX_VTXS ; j++){
                    setEdge(i,j,0);
                }
            }
            for(int i = 0; i < 38; i++){
                starwarsname[i] = {"0"};   // starwarsname을 초기화 시켜줍니다!
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
            setEdge(v, u, 1);  // starwars 는 방향성이 없는 그래프이기 때문에 추가하였습니다. 가중치가 없기 때문에 1로 넣었습니다.
        }
        void display(){
            std::cout << "총 출연자의 숫자 : " << size << "\n";  // 총 출현자의 크기정보 ==> size
            for(int i=0 ; i<size ; i++){
                std::cout << getVertex(i) << "  ";     // 행렬 출력 함수 display
                for(int j=0 ; j<size ; j++)  
                    std::cout << getEdge(i, j) << " ";
                std::cout << "\n";
            }
        }

        void loadvertex(string filename){         // 파일이 id + 캐릭터 이름 과 edge들로 나뉘어서 총 2개의 파일을 로드 하도록 각각 설정하였습니다. 이곳에선 id와 캐릭터 이름을 불러옵니다!
            ifstream ifs(filename);
            string line;
            getline(ifs, line);
            stringstream ls(line);
            int n;                 // 불러오기 편하게 하기위해 파일 맨위에 총 id의 숫자를 넣었습니다.
            ls >> n;
            for(int i = 0; i < n; i++){
                getline(ifs, line);
                string str;
                stringstream ls(line);
                ls >> str;
                insertVertex(str); // 해당 캐릭터들의 ID 들을 vertex에 넣습니다 ex) 0, 1, 2, 3, 4, 5... 37
                string temp;
                ls >> str;
                ls >> temp;
                string temp2;  // 캐릭터 이름들중 NUTE GUNRAY 처럼 띄어쓰기가있어 NUTEGUNRAY로 붙여왔습니다.
                temp2 = str+temp;
                starwarsname[i] = temp2; // 캐릭터 이름 배열에 넣어둡니다!
            }
            ifs.close();
        }

        void loadEdge(string filename){
            ifstream ifs(filename);
            string line;
            getline(ifs, line);
            stringstream ls(line);
            int n;           // 맨위에 edge들의 수를 받아옵니다 ex) 135
            ls >> n;
            for(int i = 0; i < n; i++){
                getline(ifs, line);
                stringstream ls(line);
                int val1;   //      start target  가중치   
                ls >> val1; // 파일에 15    0    11   이렇게 저장되어있습니다. 가중치는 뺏습니다. 
                for(int j = 0; j < 1; j++){  // 가중치가 있을경우 j < 2 로 바꿉니다. 하지만 여기는 없는걸 구하므로 pass!
                    int val2;
                    ls >> val2;
                    insertEdge(val1, val2);  // start 와 target edge를 연결합니다!
                }
            }
        }

        void store(std::string filename) {
            std::ofstream ofs(filename);
            ofs << size << "\n";
            for(int i = 0 ; i < size ; i++){
                ofs << getVertex(i) << "  ";
                for(int j = 0 ; j < size ; j++)  
                    ofs << getEdge(i, j) << " ";
                ofs << "\n";
            }
            ofs.close();
        }
};

vector<string> s;   // 편리성을 위해 전역 vector 를 사용하였습니다.
vector<int> ls;   // 편리성을 위해 전역 vector 를 사용하였습니다.
class randomwalker : public AdjMatGraph{
    public:
        bool isLinked(int u, int v){ return getEdge(u, v) != 0; }     
        void startwalk(int i, int walking_length_N){
            static int count = 0;
            int p = 50;  // 확률 p  !!
            s.push_back(this->getVertex(i));
            while(count != walking_length_N-1){
                int number = 0;   // 1번째 소스코드와 같이 연결된 edge수를 파악하기 위한 number!
                for(int j = 0; j < size; j++){
                    if(this->isLinked(i, j)){
                        number++;
                        ls.push_back(j);  // 연결되있으면 number++ 하고 ls에 target 값을 넣어둡니다!
                    }
                }
                
                if(p == 0){ // 확률을 설정하지 않았을 경우 연결된 노드중 랜덤으로 설정!! 
                    int randnum = (int)dist(gen) % number; // 해당 노드의 edge수만큼 확률을 구함
                    count++;
                    s.push_back(this->getVertex(*(ls.begin()+randnum))); // 다음노드를 벡터 s에 추가합니다.
                    i = *(ls.begin()+randnum); // start 인 i를 갱신시켜줍니다.
                    ls.clear(); // 배열을 한번 비워줍니다.
                }

                //확률을 설정할경우
                else{
                    int randnum = (int)dist(gen) % 100; // 앞의 소스코드 1번과 다르게 확률을 정확하게 계산하였습니다.
                    if(randnum <= p){ // 확률 안에 들어왔을 경우 연결되어있지 않더라도 랜덤 노드로 이동!!
                        count++;
                        int whereareyou = (int)dist(gen) % size; // 전체 노드의 수만큼을 랜덤으로 갑니다 연결되어있지 않더라도
                        s.push_back(this->getVertex(whereareyou)); // 다음 노드를 저장해두고
                        i = whereareyou; // start 인 i를 갱신시켜줍니다
                        ls.clear();
                    }
                    else{ // 확률안에 들어가지 않는다!!  --> 랜덤으로 가지않고 연결된 노드중에서만 가겠다!!
                        count++;
                        int mypercent = (int)dist(gen) % number; // 전체노드의 수가 아니라 연결된 노드의 수만큼 해서 연결된 노드로 확률이동!
                        s.push_back(this->getVertex(*(ls.begin()+mypercent))); // 다음 노드 s에 넣어주고
                        i = *(ls.begin()+mypercent); // start i 갱신시켜주고 
                        ls.clear();
                    }
                }
                // 숫자가 유의미 하게 커졌을때 더이상 소스코드 1에서 사용했던case문은 문제가 있다고 판단해서 삭제했습니다!
                // switch (randnum)
                // {
                //     case randnum > p:
                //         count++;
                //         s.push_back(this->getVertex(*(ls.begin()+2)));
                //         i = *(ls.begin()+2);
                //         ls.clear();
                //         break;
                //     case 1:
                //         count++;
                //         s.push_back(this->getVertex(*(ls.begin()+1)));
                //         i = *(ls.begin()+1);
                //         ls.clear();                        
                //         break;
                //     case 0:
                //         count++;
                //         s.push_back(this->getVertex(*(ls.begin())));
                //         i = *(ls.begin());
                //         ls.clear();
                //         break;
                //     default:
                //         count++;
                //         int whereareyou = (int)dist(gen) % size;
                //         s.push_back(this->getVertex(whereareyou));
                //         i = whereareyou;
                //         ls.clear();
                //         break;
                // }
            }
        }
        void pagerank(int walklength){ // 실제 그래프에서의 pageRank 수행
            int character_num = this->size;
            int* arr = new int[character_num]; // 총 캐릭터의 수만큼 동적할당을 해줍니다!
            for(int i = 0; i < character_num; i++){arr[i] = 0;} // 초기화 단계
            for(int i = 0; i < s.size(); i++){ // 이동한 모든 노드의 정보를 각 ID랑 비교해 해당 캐릭터의 배열을 증가시켜줍니다.
                if(s[i] == "0"){
                    arr[0] += 1;
                }
                else if(s[i] == "1"){
                    arr[1] += 1;
                }
                else if(s[i] == "2"){
                    arr[2] += 1;
                }
                else if(s[i] == "3"){
                    arr[3] += 1;
                }
                else if(s[i] == "4"){
                    arr[4] += 1;
                }
                else if(s[i] == "5"){
                    arr[5] += 1;
                }
                else if(s[i] == "6"){
                    arr[6] += 1;
                }
                else if(s[i] == "7"){
                    arr[7] += 1;
                }
                else if(s[i] == "8"){
                    arr[8] += 1;
                }
                else if(s[i] == "9"){
                    arr[9] += 1;
                }
                else if(s[i] == "10"){
                    arr[10] += 1;
                }
                else if(s[i] == "11"){
                    arr[11] += 1;
                }
                else if(s[i] == "12"){
                    arr[12] += 1;
                }
                else if(s[i] == "13"){
                    arr[13] += 1;
                }
                else if(s[i] == "14"){
                    arr[14] += 1;
                }
                else if(s[i] == "15"){
                    arr[15] += 1;
                }
                else if(s[i] == "16"){
                    arr[16] += 1;
                }
                else if(s[i] == "17"){
                    arr[17] += 1;
                }
                else if(s[i] == "18"){
                    arr[18] += 1;
                }
                else if(s[i] == "19"){
                    arr[19] += 1;
                }
                else if(s[i] == "20"){
                    arr[20] += 1;
                }
                else if(s[i] == "21"){
                    arr[21] += 1;
                }
                else if(s[i] == "22"){
                    arr[22] += 1;
                }
                else if(s[i] == "23"){
                    arr[23] += 1;
                }
                else if(s[i] == "24"){
                    arr[24] += 1;
                }
                else if(s[i] == "25"){
                    arr[25] += 1;
                }
                else if(s[i] == "26"){
                    arr[26] += 1;
                }
                else if(s[i] == "27"){
                    arr[27] += 1;
                }
                else if(s[i] == "28"){
                    arr[28] += 1;
                }
                else if(s[i] == "29"){
                    arr[29] += 1;
                }
                else if(s[i] == "30"){
                    arr[30] += 1;
                }
                else if(s[i] == "31"){
                    arr[31] += 1;
                }
                else if(s[i] == "32"){
                    arr[32] += 1;
                }
                else if(s[i] == "33"){
                    arr[33] += 1;
                }
                else if(s[i] == "34"){
                    arr[34] += 1;
                }
                else if(s[i] == "35"){
                    arr[35] += 1;
                }
                else if(s[i] == "36"){
                    arr[36] += 1;
                }
                else{
                    arr[37] += 1;
                }
            }
            for(int k = 0; k < character_num; k++){
                if(arr[k] == 0){continue;}  // 이동하지 않은 노드는 건너뜁니다
                cout << "\n" << starwarsname[k] << "의 출현 횟수 " << arr[k] << "\n";
                cout << "총 Trail의 길이 : " << walklength << "\n";
                cout << starwarsname[k] << "의 PageRank = " << arr[k] << "/" << walklength << " = " << (arr[k] / (double)walklength) << "\n";  // 출현횟수를 walklength로 나눠줍니다!!
            }
            delete []arr;
        }
};

int main(){
    randomwalker g;
    int walklength = 1;
    g.loadvertex("starwars-episode-1-interactions-allCharacters-nodes.tsv");
    g.loadEdge("starwars-episode-1-interactions-allCharacters-links.tsv");
    g.startwalk(32, walklength);
    g.display();
    cout << "\n";
    ls.clear();
    for(int i = 0; i < s.size(); i++){
        cout << s[i] << " ";
    }
    g.pagerank(walklength);
    return 0;
}