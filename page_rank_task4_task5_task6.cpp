#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <ctime>
#include <stack>
#include <vector>

#define INF 9999      // starwars 가중치의 최대가 22이기 때문에 9999도 충분합니다
#define MAX_VTXS 256
using namespace std;

static std::mt19937 gen = std::mt19937((unsigned int)time(NULL));
static std::uniform_real_distribution<> dist(0, 100000); // 확률의 정확성을 높이기 위해 최대한 높였습니다!!
                                                         // dist 범위에 대해서 생각해봤습니다.
                                                         // 예를들어 0~99의 난수 생성인데 노드의 수가 80개이다!  그럼 0~19는 20~99 보다 나올수 있는 확률이 1이 더 올라가기때문에
                                                         // dist의 범위를 최대한 높이는게 좋다고 생각했습니다.                   

inline void error(const char* str){
    std::cerr << str << "\n";
    exit(1);
};

class AdjMatGraph{
    protected:
        int size;
        string vertices[MAX_VTXS];
        int adj[MAX_VTXS][MAX_VTXS];
        string starwarsname[38];  // 스타워즈에 나오는 캐릭터들 이름 저장할 곳!

    public:
        AdjMatGraph(){ reset(); }
        ~AdjMatGraph(){}

        string getVertex(int i){ return vertices[i]; }
        int getEdge(int i, int j) { return adj[i][j]; }
        void setEdge(int i, int j, int val) { adj[i][j] = val; }  // 가중치가 있기 때문에 0과 1이 아닌 가중치를 넣습니다!
        bool isEmpty(){ return size==0; }
        bool isFull(){ return size>=MAX_VTXS; }

        void reset(){
            size=0;
            for(int i=0 ; i<MAX_VTXS ; i++){
                for(int j=0 ; j<MAX_VTXS ; j++){
                    setEdge(i,j,0);
                }
            }
            for(int i = 0; i < 38; i++){  // 사이즈만큼 반복합니다.
                starwarsname[i] = {"0"};  // 이름 저장할 곳을 초기화 해줍니다
            }
        }

        void insertVertex(string name){
            if(!isFull()){ vertices[size++] = name; }  //각 노드에 캐릭터 ID를 넣어줍니다.
            else { cout << "Exceeding maximum number of vertices"; } 
        }

        void insertEdge(int u, int v, int weight){
            if(u < 0 || u >= size){
                cout << "out of range";
                exit(0);
            }
            setEdge(u, v, weight);
            setEdge(v, u, weight);  // 방향성이 없는 가중치 edge를 추가합니다.
        }
        void display(){
            std::cout << "총 출연자의 숫자 : " << size << "\n";
            for(int i=0 ; i<size ; i++){
                std::cout << getVertex(i) << "  ";
                for(int j=0 ; j<size ; j++)  
                    std::cout << getEdge(i, j) << " ";
                std::cout << "\n";
            }
        }

        void loadvertex(string filename){  // 마찬가지로 노드를 추가하는곳과 edge를 추가하는 곳을 나눴습니다.
            ifstream ifs(filename);
            string line;
            getline(ifs, line);
            stringstream ls(line);
            int n;      // 불러오기 쉽게 tsv파일의 맨위에 노드의 수를 저장해놨습니다
            ls >> n;
            for(int i = 0; i < n; i++){
                getline(ifs, line);
                string str;
                stringstream ls(line);
                ls >> str;          // ID를 받아옵니다 ex) 0, 1, 2, ~ 37
                insertVertex(str);
                string temp; 
                ls >> str;
                ls >> temp;
                string temp2;
                temp2 = str+temp; // RIC OLIE 같은 이름을 불러오기 위해서입니다!  EX) RICOLIE 
                starwarsname[i] = temp2;  
            }
            ifs.close();
        }

        void loadEdge(string filename){  // start 와 target 과 weight 를 불러옵니다!
            ifstream ifs(filename);
            string line;
            getline(ifs, line);
            stringstream ls(line);
            int n;  // 마찬가지로 맨 위에 edge수를 불러옵니다
            ls >> n;
            for(int i = 0; i < n; i++){
                getline(ifs, line);
                stringstream ls(line);
                int val1;            
                ls >> val1;     // start 시작노드!!
                for(int j = 0; j < 1; j++){
                    int val2;
                    int _weight;
                    ls >> val2;   // target 타겟노드!!
                    ls >> _weight; // weight 가중치!!
                    insertEdge(val1, val2, _weight); // edge를 가중치를 포함시켜 연결시켜줍니다!
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

class WGraph : public AdjMatGraph{
    public:
        WGraph(){}
        ~WGraph(){}
        bool hasEdge(int i, int j){ return (getEdge(i,j) < INF); }
};

vector<string> s;  // 구현의 편리함을 위해 전역변수 vector<string> s를 사용하였습니다.
vector<int> ls;  // 구현의 편리함을 위해 전역변수 vector<int> ls를 사용하였습니다.

class randomwalker : public WGraph{
    public:
        bool isLinked(int u, int v){ return getEdge(u, v) != 0; }     
        void startwalk(int i, int walking_length_N){ // 시작 노드와 노드를 방문할 횟수를 매개변수로 받아옵니다!
            static int count = 0;
            int p = 0;   // 확률을 나타내는 변수 p!
            int randnum_for_lucky_num = 0;  // 확률이 있는 경우를 위해 미리 만들어 놓습니다!
            s.push_back(this->getVertex(i)); // 초기 시작 캐릭터를 vector s에 넣어두고 시작합니다.
            while(count != walking_length_N-1){
                int number = 0;   // 현재 캐릭터와 함께 출현하는 캐릭터들 수를 나타내기 위한 변수
                if(p != 0){
                randnum_for_lucky_num = (int)dist(gen) % 100; // 확률이 있는경우 난수를 미리 생성해서 저장해둡니다!
                // cout << randnum_for_lucky_num << " 랜덤넘버가 잘 들어갔는지 확인하기 위해서 !!\n";
                }
                for(int j = 0; j < size; j++){
                    if(this->isLinked(i, j)){
                        number++; // 현재 캐릭터와 함께 출현하는 캐릭터 수를 증가시킵니다.
                        ls.push_back(j); // 현재 캐릭터와 함께 출현하는 캐릭터 index를 저장해둡니다!
                    }
                }
                //확률을 설정안할경우 배열에서 랜덤으로 설정
                if(p == 0){ //확률이 없는경우
                    // cout << "여기는 오면 안된다!!\n"; 확률이 있는경우 오면 error찍기위해서 ..
                    int sumweight = 0; // 현재 캐릭터와 출현하는 모든 캐릭터들의 가중치들을 더한 값을 위한 변수
                    for(int z = 0; z < ls.size(); z++){
                        sumweight += getEdge(i, ls[z]);  // 현재 캐릭터와 출현하는 모든 캐릭터들의 가중치를 더합니다.
                    }
                    int* weight_per_array = new int[sumweight]; // 현재 캐릭터와 출현하는 모든 캐릭터들의 가중치만큼 배열을 동적으로 할당합니다!
                    for(int e = 0; e < sumweight; e++){ // 동적할당 초기화 진행!
                        weight_per_array[e] = 0;
                    }
                    int index = 0;
                    int insert_iter_each = 0;
                    // 현재 캐릭터와 같이 출현하는 모든 캐릭터들의 가중치의 합으로 배열을 만들었습니다.
                    // 거기서 각 캐릭터의 가중치만큼 해당 노드들의 ID들을 넣어줍니다.
                    // ex)  YODA : 3, JABBA : 4, R2-D2 : 5 라면
                    //   {YODA, YODA, YODA, JABBA, JABBA, JABBA, JABBA, R2-D2, R2-D2, R2-D2, R2-D2, R2-D2}
                    //   실제로는 해당 캐릭터의 ID가 들어갑니다. 밑에 반복문이 위와같이 추가하는 반복문입니다.
                    while(number != 0){
                        for(int q = 0; q < getEdge(i, ls[insert_iter_each]); q++){
                            weight_per_array[index] = ls[insert_iter_each];
                            index++;
                        }
                        insert_iter_each++;
                        number--;
                    }
                    // for(int w = 0; w < index; w++){  제대로 들어갔는지 확인하는 코드
                    //     cout << weight_per_array[w] << " \n";
                    // }
                    count++;
                    // cout << sumweight << "<--- sumweight!!\n"; 총 가중치 합이 잘 들어갔는지 확인하는 코드
                    int randnum = (int)dist(gen) % sumweight; // 모든 가중치 합으로 난수를 생성합니다.
                    s.push_back(this->getVertex(weight_per_array[randnum])); // 다음 이동할 캐릭터를 s에 저장합니다.
                    i = weight_per_array[randnum]; // 이동할 i를 갱신시켜줍니다.
                    ls.clear(); // 배열을 비워주고
                    delete []weight_per_array; // 동적할당된 배열도 메모리 해제를 시켜줍니다
                }
                //확률을 설정할경우
                else{
                    if(randnum_for_lucky_num <= p){  // 랜덤으로 이동할 확률에 포함되어 랜덤으로 이동하기위한 조건!
                        // cout << "확률대로 왔다!!\n"; 제대로 왔는지 check!
                        count++;
                        int whereareyou = (int)dist(gen) % size;  // 총 캐릭터 수 만큼 random으로 결정
                        s.push_back(this->getVertex(whereareyou)); // 다음 랜덤 캐릭터를 s에 넣어주고
                        i = whereareyou; // 다음 으로 갈 i를 갱신시켜줍니다.
                        ls.clear(); // 동적할당배열을 사용하지 않았으므로 해제불필요!
                    }
                    else{ // 랜덤확률이 아니라 연결된 노드중 가중치 만큼의 확률로 이동하는 조건!
                        int sumweight_dismiss_p = 0; // dismiss_p 는 확률을 무시하는 변수!!
                        for(int z = 0; z < ls.size(); z++){
                            sumweight_dismiss_p += getEdge(i, ls[z]); // 현재 캐릭터와 같이 출현하는 모든캐릭터들의 가중치를 더합니다.
                        }
                        int* weight_per_array_dismiss_p = new int[sumweight_dismiss_p]; // 현재 캐릭터와 출현하는 모든 캐릭터들의 가중치만큼동적할당!
                        for(int e = 0; e < sumweight_dismiss_p; e++){ // 동적할당 초기화 진행
                            weight_per_array_dismiss_p[e] = 0;
                        }
                        int index_dismiss_p = 0;
                        int insert_iter_each_dismiss_p = 0;
                        // 현재 캐릭터와 같이 출현하는 모든 캐릭터들의 가중치의 합으로 배열을 만들었습니다.
                        // 거기서 각 캐릭터의 가중치만큼 해당 노드들의 ID들을 넣어줍니다.
                        // ex)  YODA : 3, JABBA : 4, R2-D2 : 5 라면
                        //   {YODA, YODA, YODA, JABBA, JABBA, JABBA, JABBA, R2-D2, R2-D2, R2-D2, R2-D2, R2-D2}
                        //   실제로는 해당 캐릭터의 ID가 들어갑니다. 밑에 반복문이 위와같이 추가하는 반복문입니다.
                        while(number != 0){
                            for(int q = 0; q < getEdge(i, ls[insert_iter_each_dismiss_p]); q++){
                                weight_per_array_dismiss_p[index_dismiss_p] = ls[insert_iter_each_dismiss_p];
                                index_dismiss_p++;
                            }
                            insert_iter_each_dismiss_p++;
                            number--;
                        }
                        count++;
                        int randnum_dismiss_p = (int)dist(gen) % sumweight_dismiss_p; // 총 가중치합만큼으로 난수를 생성합니다.
                        s.push_back(this->getVertex(weight_per_array_dismiss_p[randnum_dismiss_p])); // 다음 이동할 캐릭터를 s에 넣어줍니다.
                        i = weight_per_array_dismiss_p[randnum_dismiss_p]; // 다음으로 이동할 i를 갱신!
                        ls.clear(); // 현재 출현자와 같이 출현하는 캐릭터들이 들어가있는 벡터 ls를 비워줍니다.
                        delete []weight_per_array_dismiss_p; // 동적할당도 해제해 줍니다!
                    }
                }
            }
        }
        void pagerank(int walklength){ //가중치가 있는 PAGERANK
            int character_num = this->size;  // 캐릭터의 수만큼
            int* arr = new int[character_num]; // 동적할당 진행
            for(int i = 0; i < character_num; i++){arr[i] = 0;} //초기화!
            for(int i = 0; i < s.size(); i++){ // 지나온 모든 캐릭터들이 저장되어있는 벡터 s를 돌며 해당 ID와 일치하는 캐릭터들의 공간을 +1 증가시켜줍니다.
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
                if(arr[k] == 0){continue;}
                cout << "\n" << starwarsname[k] << "의 출현 횟수 " << arr[k] << "\n";
                cout << "총 Trail의 길이 : " << walklength << "\n";
                cout << starwarsname[k] << "의 PageRank = " << arr[k] << "/" << walklength << " = " << (arr[k] / (double)walklength) << "\n"; //해당 캐릭터의 출현횟수를 walklength로 나누어줍니다!
            }
            delete []arr; // 동적할당 메모리 해제!
        }
};

int main(){
    randomwalker g;
    int walklength = 10000;
    g.loadvertex("starwars-episode-1-interactions-allCharacters-nodes.tsv");
    g.loadEdge("starwars-episode-1-interactions-allCharacters-links.tsv");
    g.startwalk(32, walklength);
    g.display();
    cout << "\n";
    ls.clear();
    // for(int i = 0; i < s.size(); i++){ 지나온 경로 display!
    //     cout << s[i] << " ";
    // }
    g.pagerank(walklength);
    return 0;
}