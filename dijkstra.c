//v1.0 Dijkstra implementation

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <wincon.h>

#define MAXSTATION 122
#define MAXFILESTRING 300
#define MAXSTATIONNAME 30
#define MAXSTATIONCHNAME 40
#define INF 9999

struct st_station {
    char name[MAXSTATIONNAME];
    char chineseName[MAXSTATIONCHNAME];
};
struct st_station st[MAXSTATION];

double graph[MAXSTATION][MAXSTATION]= {0};
bool showSearchScore=false; //input '*' at last digit to show search algorithm weighting
int interchange[MAXSTATION]= {0}; //station is a transfer station

double dist[MAXSTATION];
int prev[MAXSTATION];
int reversePrev[MAXSTATION];

int ori, des = 0;

void dijkstra() {
    int min, target, count=0;
    int visit[MAXSTATION];
    double cost[MAXSTATION][MAXSTATION]= {0};

    for(int i=0; i<MAXSTATION; i++) { //cost matrix
        for(int j=0; j<MAXSTATION; j++)
            if(graph[i][j]==0) cost[i][j]=INF;
            else cost[i][j]=graph[i][j];
    }
    /*
    for(int i=0; i<MAXSTATION; i++) {
        for(int j=0; j<MAXSTATION; j++)
            printf("%.0f ",cost[i][j]);
        printf("\n");
    }
    */

    for(int i=0; i<MAXSTATION; i++) { //init
        visit[i]=0;
        dist[i]=cost[ori][i];
        if(cost[ori][i]<INF) prev[i]=ori;
        else prev[i]=-1;
    }
    dist[ori]=0; //set ori as start
    visit[ori]=1;
    count=1;

    while(count<MAXSTATION) {
        min=INF;
        for(int i=1; i<MAXSTATION; i++) {
            if(visit[i]==0 && min>dist[i]) {
                min=dist[i];
                target=i;
            }
        }
        visit[target]=1;

        for(int i=1; i<MAXSTATION; i++) {
            //printf("i: %i| count: %i| prev: %i| dist: %.0f| target: %i\n",i,count,prev[i],dist[i],target);
            if(visit[i]==0 && min+cost[target][i] < dist[i]) {
                dist[i]=min+cost[target][i];
                prev[i]=target;
            }
        }
        count++;
    }
    return;
}

void printFullDijkstra() {
    puts("");
    for(int i=1; i<MAXSTATION; i++) {
        printf(" %03i | %-20s | ", i, st[i].name);
        printf("%s",st[i].chineseName);
        for(int j=strlen(st[i].chineseName); j<=18; j=j+3) printf("  ");
        printf(" | prev: %3i | prev: %s",prev[i],st[prev[i]].chineseName);
        for(int j=strlen(st[prev[i]].chineseName); j<=18; j=j+3) printf("  ");
        printf("| dist: %5.1f |\n",dist[i]);
    }
    puts("");
    system("pause>nul");
    return;
}

void printDijkstra() {

}

void reverseDijkstra() {
    int prevTemp[MAXSTATION];
    for(int i=0; i<MAXSTATION; i++){
        reversePrev[i]=0;
        prevTemp[i]=0;
    }
    int count=0;
    prevTemp[count]=prev[des];
    //printf("prevTemp[count]=%3i | %-20s \n",prevTemp[count],st[prevTemp[count]].name);
    count++;

    while(prevTemp[count-1]>0){
        prevTemp[count] = prev[prevTemp[count-1]];
        //printf("prevTemp[count]=%3i | %-20s \n",prevTemp[count],st[prevTemp[count-1]].name);
        count++;
    }

    printf("Destination: %3i | %-20s \n",des,st[des].name);
    for(int i=0; i<count; i++){
        printf("prevTemp[%2i]=%3i | %-20s \n",i,prevTemp[i],st[prevTemp[i]].name);
    }
    system("pause");
}

void initInterchange() {

}

int userInput() {
    ori=des=0;
    do {
        printf("\nPlease Select Origin: ");
        ori=inputAndSearch();
        if(ori==0) {
            printInvaild();
        } else if(ori<0) {
            return;
        } else registerConfirm(ori);
    } while(ori==0);

    do {
        printf("\nPlease Select Destination: ");
        des=inputAndSearch();
        if(des==0) {
            printInvaild();
        } else if(ori<0) {
            return;
        } else registerConfirm(des);
    } while(des==0);
}

void registerConfirm(int i) {
    rgb(14);
    printf("\n %03i",i);
    rgb(7);
    printf(": ");
    rgb(15);
    printf("%s",st[i].name);
    rgb(7);
    printf(" Is Registered\n");
    return;
}

void printInvaild() {
    printf("\nInvaild Input\n");
    system("pause>nul");
    return;
}

int inputAndSearch() {
    char ch, inputString[MAXSTATIONNAME];
    int i=0,check=0;
    showSearchScore=false;
    while ((ch=getch())!=EOF && ch!='\n' && ch!='\r') {
        if (ch==8 && i>0) {
            printf("\b \b");
            fflush(stdout);
            i--;
        } else if((isalnum(ch) || ch==' ' || ch=='*') && i<sizeof(inputString)-1) {
            printf("%c",ch);
            inputString[i++]=(char)ch;
        }
    }
    inputString[i]=0;
    puts("");
    if(strcmp(inputString,"q")==0) { //input "q" = quit program
        return -1;
    }
    if(inputString[i-1]=='*') { //check last digit contains '*'
        showSearchScore=true;
        inputString[i-1]=0;
    }
    for(i=0; i<strlen(inputString); i++) {
        if(inputString[i]<'0' || inputString[i]>'9' ) check++;
    }
    if(check==0) {
        i=atoi(inputString);
        if(i>0 && i<MAXSTATION) {
            return i; //return i = station id = input number valid
        } else {
            return 0;
        }
    } else {
        if(strlen(inputString)<3 || !isalnum(inputString[0])) {
            return 0;
        }
        i=identifyStation(inputString);
        if(i==0) {
            return 0;
        } else {
            return i;
        }
    }
}

int identifyStation(char inputString[MAXSTATIONNAME]) {
    int i, j=0, k, l, m, matchScore[MAXSTATION]= {0}; //matchScore = array of weighting scores
    char ch, name[MAXSTATIONNAME];
    for(i=0; i<strlen(inputString); i++) { //Remove Space In char s;
        if(inputString[i]==' ' || j>0) {
            if(inputString[i]==' ') j++;
            inputString[i-j+1]=inputString[i+1];
        }
    }
    inputString[i-j]=0;
    for(i=0; i<strlen(inputString); i++) inputString[i]=toupper(inputString[i]);
    for(i=1; i<MAXSTATION; i++) {
        matchScore[i]=0;
        strcpy(name,st[i].name);
        for(l=0; l<strlen(name); l++) {
            name[l]=toupper(name[l]);
        }
        for(j=0; j<strlen(inputString)-2; j++) {
            for(k=0; k<strlen(name)-2; k++) {
                if(inputString[j]==name[k] && inputString[j+1]==name[k+1] && inputString[j+2]==name[k+2]) {
                    matchScore[i]++;
                    break;
                }
            }
        }
        j=0;
        for(m=0; m<strlen(name); m++) { //Remove Space In char name;
            if(name[m]==' ' || j>0) {
                if(name[m]==' ') j++;
                name[m-j+1]=name[m+1];
            }
        }
        name[m-j]=0;
        for(l=0; l<strlen(name); l++) {
            name[l]=toupper(name[l]);
        }
        if(strcmp(name,inputString)==0) {
            return i;
        }
        for(j=0; j<strlen(inputString); j++) {
            for(k=0; k<strlen(name); k++) {
                if(inputString[j]==name[k]) {
                    matchScore[i]++;
                    break;
                }
            }
        }
        for(j=0; j<strlen(inputString)-1; j++) {
            for(k=0; k<strlen(name)-1; k++) {
                if(inputString[j]==name[k] && inputString[j+1]==name[k+1]) {
                    matchScore[i]++;
                    break;
                }
            }
        }
        for(j=0; j<strlen(inputString)-2; j++) {
            for(k=0; k<strlen(name)-2; k++) {
                if(inputString[j]==name[k] && inputString[j+1]==name[k+1] && inputString[j+2]==name[k+2]) {
                    matchScore[i]=matchScore[i]+3;
                    break;
                }
            }
        }
        for(j=0; j<strlen(inputString)-3; j++) {
            for(k=0; k<strlen(name)-3; k++) {
                if(inputString[j]==name[k] && inputString[j+1]==name[k+1] && inputString[j+2]==name[k+2] && inputString[j+3]==name[k+3]) {
                    matchScore[i]=matchScore[i]+5;
                    break;
                }
            }
        }
        if(showSearchScore==true) printf("\nS:%02i %s %i",i,st[i].name,matchScore[i]); //scores of match[i]
    }

    int topScore=0, secScore=0;
    for(i=1; i<MAXSTATION; i++) {
        if(matchScore[i]>topScore) {
            secScore=topScore;
            topScore=matchScore[i];
        } else if(matchScore[i]>secScore && matchScore[i]<topScore) {
            secScore=matchScore[i];
        }
    }
    if(showSearchScore==true) printf("\n\nHighest: %i Second: %i\n",topScore,secScore);
    if(topScore>8) {
        int resultCount=0;
        for(i=0; i<MAXSTATION; i++) {
            if(matchScore[i]==topScore) resultCount++;
        }
        printf("\nPlease Select Station Below:\n\n");
        if(resultCount<2) {
            for(i=1; i<MAXSTATION; i++) {
                if(matchScore[i]==topScore) {
                    printStationChoose(i);
                }
            }
            if(topScore-secScore<6) {
                for(i=1; i<MAXSTATION; i++) {
                    if(matchScore[i]==secScore) {
                        printStationChoose(i);
                    }
                }
            }
        } else {
            int check[MAXSTATION]= {0}; //check[] stores interchange id
            for(i=1; i<MAXSTATION; i++) {
                if(matchScore[i]==topScore) {
                    if(interchange[i]!=0) {
                        if(check[interchange[i]]==0) {
                            check[interchange[i]]=1;
                            printStationChoose(i);
                        }
                    } else {
                        printStationChoose(i);
                    }
                }
            }
        }

        i=inputCode();

        if(i>0 && i<MAXSTATION) {
            return i;
        } else return 0;
    } else {
        return 0;
    }
}

int inputCode() {
    printf("\nInput ");
    printf("Code");
    printf(" Of Station: ");
    int i=0;
    char ch, input[4]= {0};
    while ((ch=getch())!=EOF && ch!='\n' && ch!='\r') {
        if (ch==8 && i>0) {
            printf("\b \b");
            fflush(stdout);
            i--;
        } else if(ch>='0' && ch<='9' && i<sizeof(input)-1) {
            printf("%c",ch);
            input[i++]=(char)ch;
        }
    }
    input[i]=0;
    puts("");
    return atoi(input);
}

void printStationChoose(int i) {
    rgb(14);
    printf(" %03i",i);
    rgb(7);
    printf(": %-20s | %s\n",st[i].name, st[i].chineseName);
    return;
}

int graphFileInput() {
    FILE *fp;
    int i,j=0; //file row and col counter
    char s[MAXFILESTRING]="";
    char const csv[2]=",";
    char *token;
    fp=fopen("graph.csv","r");
    if(fp==NULL) {
        rgb(79);
        printf("File \"graph.csv\" Not Found\n");
        rgb(7);
        return 1;
    }
    while(!feof(fp)) {
        fgets(s,MAXFILESTRING,fp);
        if(feof(fp)) break;
        s[strlen(s)-1]=0;
        //printf("%s\n",s);
        token=strtok(s,csv);
        j=0;
        while(token!=0) {
            graph[i][j]=strtod(token,0);
            token=strtok(NULL,csv);
            j++;
        }
        i++;
    }
    fclose(fp);
    return 0;
}

int stationFileInput() {
    FILE *fp;
    int i=0; //file row and col counter
    char s[MAXFILESTRING];
    char const csv[2]=",";
    char *token;
    fp=fopen("station.csv","r");
    if(fp==NULL) {
        rgb(79);
        printf("File \"station.csv\" Not Found\n");
        rgb(7);
        return 1;
    }
    fgets(s,MAXFILESTRING,fp); //get rid of first line of utf8 bom csv
    i=1;
    while(!feof(fp)) {
        fgets(s,MAXFILESTRING,fp);
        if(feof(fp)) break;
        s[strlen(s)-1]=0;
        //printf("%s\n",s);
        token=strtok(s,csv);
        //printf("Token:%s\t",token);
        strcpy(st[i].name,token);
        token=strtok(NULL,csv);
        strcpy(st[i].chineseName,token);
        i++;
    }
    fclose(fp);
    return 0;
}

int interchangeFileInput() {
    FILE *fp;
    int i=0; //file row and col counter
    char s[MAXFILESTRING];
    char const csv[2]=",";
    char *token;
    fp=fopen("interchange.csv","r");
    if(fp==NULL) {
        rgb(79);
        printf("File \"interchange.csv\" Not Found\n");
        rgb(7);
        return 1;
    }
    while(!feof(fp)) {
        fgets(s,MAXFILESTRING,fp);
        if(feof(fp)) break;
        s[strlen(s)-1]=0;
        //printf("%s\n",s);
        token=strtok(s,csv);
        //printf("Token:%s\t",token);
        interchange[i]=atoi(token);
        i++;
    }
    fclose(fp);
    return 0;
}

void printGraph() {
    for(int i=0; i<MAXSTATION; i++) {
        for(int j=0; j<MAXSTATION; j++)
            printf("%2.1f ", graph[i][j]);
        printf("\n");
        fflush(stdout);
    }
    return;
}

void printStructStation() {
    for(int i=0; i<MAXSTATION; i++) printf(" %03i | %-20s | %s |\n", i, st[i].name, st[i].chineseName);
    return;
}

void printInterchange() {
    for (int i=0; i<MAXSTATION; i++) printf(" %03i | %02i |\n", i, interchange[i]);
}

void rgb(int color) {  //Set Display Colors
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),color);
    return;
}

void rgbPrintAll() {
    for(int i=0; i<100; i++) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),i);
        printf("%02i ",i);
    }
    return;
}

void main() {
    system("chcp 65001");
    system("cls");

    int checkFile=0;
    checkFile+=graphFileInput();
    checkFile+=stationFileInput();
    checkFile+=interchangeFileInput();
    if(checkFile>0) {
        system("pause>nul");
        exit(0);
    }

    //printStructStation();
    //printGraph();
    //printInterchange();

    //initInterchange();

    do {
        userInput();
        dijkstra();
        reverseDijkstra();
        printFullDijkstra();
        printDijkstra();
        system("cls");
    } while(1);
}
