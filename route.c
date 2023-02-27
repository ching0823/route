/*
v1.1 Interchange overhaul
v1.0 Dijkstra implementation
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <wincon.h>

#define MAXSTATION 121+1
#define MAXLINE 10+1
#define MAXLINENAME 50
#define MAXLINECHNAME 30
#define MAXFILESTRING 300
#define MAXSTATIONNAME 30
#define MAXSTATIONCHNAME 40
#define INTERCHANGECOUNT 21+1
#define INF 9999

struct st_station {
    char name[MAXSTATIONNAME];
    char chineseName[MAXSTATIONCHNAME];
    int line;
    int interchange;
};
struct st_station st[MAXSTATION];

struct st_line {
    char abbrev[4];
    char name[MAXLINENAME];
    char chineseName[MAXLINECHNAME];
    int rgb;
};
struct st_line line[MAXLINE];

double graph[MAXSTATION][MAXSTATION]= {0};
bool showSearchScore=false; //input '*' at last digit to show search algorithm weighting

double dist[MAXSTATION];
int prev[MAXSTATION];
int reversePrev[MAXSTATION];

int ori, des = 0;

void dijkstra(int ori) {
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

void checkInterchange() {
    int tempDist=INF, tempOri, tempDes;
    dijkstra(ori);

    if(st[ori].interchange==0 && st[des].interchange==0) return;

    if(st[ori].interchange!=0){
        for(int i=0; i<MAXSTATION; i++){
            if(st[ori].interchange==st[i].interchange){
                dijkstra(i);
                if(dist[des]<tempDist){
                    tempOri=i;
                    tempDist=dist[des];
                }
            }
        }
        ori=tempOri;
    }
    tempDist=INF;
    if(st[des].interchange!=0){
        for(int i=0; i<MAXSTATION; i++){
            if(st[des].interchange==st[i].interchange){
                if(dist[i]<tempDist){
                    tempDes=i;
                    tempDist=dist[i];
                }
            }
        }
        des=tempDes;
    }
    dijkstra(ori);
    return;
}

void dijkstraResult() {
    int i=1;
    int currentLine=st[reversePrev[i]].line;
    printf("\n");
    while(reversePrev[i]>0) {
        if(currentLine!=st[reversePrev[i]].line) {
            printf("\n");
            currentLine=st[reversePrev[i]].line;
        }
        printf("| %3i | %-20s \n",reversePrev[i],st[reversePrev[i]].name);
        i++;
    }
    printf("| %3i | %-20s \n",des,st[des].name);
    printf("\n");
    system("pause");
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

void reverseDijkstra() {
    int prevTemp[MAXSTATION];
    for(int i=0; i<MAXSTATION; i++) {
        reversePrev[i]=0;
        prevTemp[i]=0;
    }
    int count=0;
    prevTemp[count]=prev[des];
    //printf("prevTemp[count]=%3i | %-20s \n",prevTemp[count],st[prevTemp[count]].name);
    count++;

    while(prevTemp[count-1]>0) {
        prevTemp[count] = prev[prevTemp[count-1]];
        //printf("prevTemp[count]=%3i | %-20s \n",prevTemp[count],st[prevTemp[count-1]].name);
        count++;
    }
    for(int i=1; i<count; i++) {
        reversePrev[i]=prevTemp[count-i-1];
    }

    //Print prevTemp[];
    /*
    printf("Destination: %3i | %-20s \n",des,st[des].name);
    for(int i=0; i<count; i++){
        printf("prevTemp[%2i]=%3i | %-20s \n",i,prevTemp[i],st[prevTemp[i]].name);
    }
    system("pause");
    */

    return;
}

void userInput() {
    ori=des=0;

    printInstructions();
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

void printInstructions() {
    printf("\n--- Input ID or English name of the station ---\n");
    printf("Note: Ambiguous name triggers search function\n");
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
        printf("\nDo you mean by:\n\n");
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
                    if(st[i].interchange!=0) {
                        if(check[st[i].interchange]==0) {
                            check[st[i].interchange]=1;
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
    printf("ID");
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

int fileGraphInput() {
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

int fileStationInput() {
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
        token=strtok(NULL,csv);
        st[i].line=atoi(token);
        token=strtok(NULL,csv);
        st[i].interchange=atoi(token);
        i++;
    }
    fclose(fp);
    return 0;
}

int fileLineInput() {
    FILE *fp;
    int i=0; //file row and col counter
    char s[MAXFILESTRING];
    char const csv[2]=",";
    char *token;
    fp=fopen("line.csv","r");
    if(fp==NULL) {
        rgb(79);
        printf("File \"line.csv\" Not Found\n");
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
        strcpy(line[i].abbrev,token);
        token=strtok(NULL,csv);
        strcpy(line[i].name,token);
        token=strtok(NULL,csv);
        strcpy(line[i].chineseName,token);
        i++;
    }
    fclose(fp);
    return 0;
}

void printGraph() {
    printf("\n");
    for(int i=0; i<MAXSTATION; i++) {
        for(int j=0; j<MAXSTATION; j++)
            printf("%2.1f ", graph[i][j]);
        printf("\n");
        fflush(stdout);
    }
    return;
}

void printStructStation() {
    int currentLine=1;
    printf("\n");
    for(int i=1; i<MAXSTATION; i++) {
        if(st[i].line!=currentLine) {
            printf("|\n");
            currentLine=st[i].line;
        }
        printf("| %3i | %-20s | ", i, st[i].name);
        printf("%s", st[i].chineseName);
        for(int j=strlen(st[i].chineseName); j<=18; j=j+3) printf("  ");
        printf("| %s",line[st[i].line].chineseName);
        for(int j=strlen(line[st[i].line].chineseName); j<=12; j=j+3) printf("  ");
        printf("|\n");
    }
    return;
}

void printLine() {
    printf("\n");
    for(int i=1; i<MAXLINE; i++) {
        printf("| %2i | %3s | %-30s | ",i,line[i].abbrev,line[i].name);
        printf("%s", line[i].chineseName);
        for(int j=strlen(line[i].chineseName); j<=18; j=j+3) printf("  ");
        printf("|\n");
    }
    return;
}

void printInterchange() {
    printf("\n");
    for(int i=1; i<MAXSTATION; i++) printf("| %03i | %02i |\n", i, st[i].interchange);
    printf("\n");

    int check[30]= {0};
    printf("\n");
    for(int i=1; i<MAXSTATION; i++) {
        if(st[i].interchange>0)
            if(check[st[i].interchange]==0) {
                printf("| %2i | %-30s \n",st[i].interchange,st[i].name);
                check[st[i].interchange]=1;
            }
    }
    printf("\n");
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

void termsAndAgreement() {
    printf("\n");
    printf("This program is not for commercial use.\n");
    printf("All copyright belongs to MTR.\n");
    printf("\n");
    printf("Press Y to proceed.\n");
    while (getch()!='y') {}
    system("cls");
}

void main() {
    system("chcp 65001");
    system("cls");
    Sleep(50);
    //termsAndAgreement();

    int checkFile=0;
    checkFile+=fileGraphInput();
    checkFile+=fileStationInput();
    checkFile+=fileLineInput();
    if(checkFile>0) {
        system("pause>nul");
        exit(0);
    }

    //printStructStation();
    //printGraph();
    //printInterchange();
    //printLine();

    do {
        printStructStation();
        userInput();
        checkInterchange();
        //printFullDijkstra();
        reverseDijkstra(); //Reverse the inverted dijkstra result map
        dijkstraResult();
        system("cls");
    } while(1);
}