/*
v2.4 abbrev implementation
v2.3 Bug fixed on path finding algorithm
v2.2 rgb support
v2.1 Search function bug fixed
v2.0 Fully functional path finding system
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

#define SEARCHCOUNT 2
#define DIJKSTRASLOT 0

#define RGBBLACK 0
#define RGBHIGHLIGHT 13
#define RGBGREY 14
#define RGBWHITE 15

struct st_station {
    char abbrev[4];
    char name[MAXSTATIONNAME];
    char chineseName[MAXSTATIONCHNAME];
    int line;
    int interchange;
};
struct st_station station[MAXSTATION];

struct st_line {
    char abbrev[4];
    char name[MAXLINENAME];
    char chineseName[MAXLINECHNAME];
    long rgb;
};
struct st_line line[MAXLINE];

double graph[MAXSTATION][MAXSTATION]= {0};
bool showSearchScore=false; //input '*' at last digit to show search algorithm weighting

double dist[SEARCHCOUNT][MAXSTATION]= {0};
int prev[SEARCHCOUNT][MAXSTATION]= {0};
int reversePrev[SEARCHCOUNT][MAXSTATION]= {0};

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

    for(int i=0; i<MAXSTATION; i++) { //init
        visit[i]=0;
        dist[DIJKSTRASLOT][i]=cost[ori][i];
        if(cost[ori][i]<INF) prev[DIJKSTRASLOT][i]=ori;
        else prev[DIJKSTRASLOT][i]=-1;
    }
    dist[DIJKSTRASLOT][ori]=0; //set ori as start
    visit[ori]=1;
    count=1;

    while(count<MAXSTATION) { //pick smallest dist[i]
        min=INF;
        for(int i=1; i<MAXSTATION; i++) {
            if(visit[i]==0 && min>dist[DIJKSTRASLOT][i]) {
                min=dist[DIJKSTRASLOT][i];
                target=i;
            }
        }
        visit[target]=1;

        for(int i=1; i<MAXSTATION; i++) {
            //printf("i: %i| count: %i| prev: %i| dist: %.0f| target: %i\n",i,count,prev[i],dist[i],target);
            if(visit[i]==0 && min+cost[target][i] < dist[DIJKSTRASLOT][i]) { //change dist[i] to calculated new dist if smaller than original
                dist[DIJKSTRASLOT][i]=min+cost[target][i];
                prev[DIJKSTRASLOT][i]=target;
            }
        }
        count++;
    }
    return;
}

void checkInterchangeAndDijkstra() {
    int tempDist, tempOri, tempDes;

    dijkstra(ori);

    if(station[ori].interchange==0 && station[des].interchange==0) return;

    if(station[ori].interchange!=0 && station[des].interchange!=0) { //if both ori and des are interchange, select closest DES first
        tempDist=INF;
        for(int i=0; i<MAXSTATION; i++) {
            for(int j=0; j<MAXSTATION; j++) {
                if(station[ori].interchange==station[i].interchange) {
                    dijkstra(i);
                    if(dist[DIJKSTRASLOT][tempDes]<tempDist) {
                        tempOri=i;
                        tempDist=dist[DIJKSTRASLOT][des];
                    }
                }
                if(station[des].interchange==station[j].interchange) {
                    if(dist[DIJKSTRASLOT][j]<tempDist) {
                        tempDes=i;
                        tempDist=dist[DIJKSTRASLOT][i];
                    }
                }
            }
        }
    }

    if(station[des].interchange!=0) { //check des is interchange, swap between and pick closest
        tempDist=INF;//if only des is interchange
        for(int i=0; i<MAXSTATION; i++) {
            if(station[des].interchange==station[i].interchange) {
                if(dist[DIJKSTRASLOT][i]<tempDist) {
                    tempDes=i;
                    tempDist=dist[DIJKSTRASLOT][i];
                }
            }
        }
        des=tempDes;
    }

    if(station[ori].interchange!=0) { //check ori is interchange, swap between and pick closest
        tempDist=INF;
        for(int i=0; i<MAXSTATION; i++) {
            if(station[ori].interchange==station[i].interchange) {
                dijkstra(i);
                if(dist[DIJKSTRASLOT][des]<tempDist) {
                    tempOri=i;
                    tempDist=dist[DIJKSTRASLOT][des];
                }
            }
        }
        ori=tempOri;
    }

    dijkstra(ori); //run again with new closest ori and des

    return;
}

void dijkstraResult() {
    int i=1;
    int currentLine=station[reversePrev[DIJKSTRASLOT][i]].line;

    printf("\n");
    while(reversePrev[DIJKSTRASLOT][i]>0) {
        if(currentLine!=station[reversePrev[DIJKSTRASLOT][i]].line) {
            printf("\n");
            currentLine=station[reversePrev[DIJKSTRASLOT][i]].line;
        }
        //printf("station[reversePrev[0]].line: %2i\n",station[reversePrev[0]].line);
        //printf("| %3i ",reversePrev[i]);
        printStationWithoutId(reversePrev[DIJKSTRASLOT][i]); //print results

        int check1=0, check2=0, check3=0; //show time needed
        if(currentLine!=station[reversePrev[DIJKSTRASLOT][i+1]].line) check1=1; //last station before interchange, included first station, first st line=0, i=1
        if(currentLine!=station[reversePrev[DIJKSTRASLOT][i-1]].line) check2=1; //first station after interchange
        if(reversePrev[DIJKSTRASLOT][i]==des) check3=1; //
        if(check1 || check2 || check3) printf(" Time: %3.0f mins |",dist[DIJKSTRASLOT][reversePrev[DIJKSTRASLOT][i]]);
        printf("\n");
        i++;
    }
    printf("\n");
    system("pause");
    return;
}

void printFullDijkstra() {
    puts("");
    for(int slot=0; slot<SEARCHCOUNT; slot++) {
        printf("| Slot: %i |\n",slot);
        for(int i=1; i<MAXSTATION; i++) {
            printStation(i);
            printf(" Dist: %5.1f |\n",dist[slot][i]);
        }
        puts("");
    }
    system("pause>nul");
    return;
}

void reverseDijkstra() {
    int prevTemp[MAXSTATION];
    for(int i=0; i<MAXSTATION; i++) {
        reversePrev[DIJKSTRASLOT][i]=0;
        prevTemp[i]=0;
    }
    int count=0;
    prevTemp[count]=prev[DIJKSTRASLOT][des];
    //printf("prevTemp[count]=%3i | %-20s \n",prevTemp[count],station[prevTemp[count]].name);
    count++;

    while(prevTemp[count-1]>0) {
        prevTemp[count] = prev[DIJKSTRASLOT][prevTemp[count-1]];
        //printf("prevTemp[count]=%3i | %-20s \n",prevTemp[count],station[prevTemp[count-1]].name);
        count++;
    }
    for(int i=1; i<count; i++) {
        reversePrev[DIJKSTRASLOT][i]=prevTemp[count-i-1];
        //printf("%i\n",reversePrev[i]);
    }

    reversePrev[DIJKSTRASLOT][count]=des; //put des at final of reversePrev[]

    //Print prevTemp[];
    /*
    printf("Destination: %3i | %-20s \n",des,station[des].name);
    for(int i=0; i<count; i++){
        printf("prevTemp[%2i]=%3i | %-20s \n",i,prevTemp[i],station[prevTemp[i]].name);
    }
    system("pause");
    */

    return;
}

/*
void lowestTurnAlgorithm(){

}
*/

void userInput() {
    ori=des=0;

    printInstructions();
    do {
        printf("\nPlease Select Origin: ");
        ori=inputAndSearch();
        if(ori<=0) {
            printInvaild();
        } else registerConfirm(ori);
    } while(ori<=0);

    do {
        printf("\nPlease Select Destination: ");
        des=inputAndSearch();
        if(des<=0) {
            printInvaild();
        } else registerConfirm(des);
    } while(des<=0);
}

void printInstructions() {
    rgb(RGBWHITE);
    printf("\nSelect font MS Gothic in command prompt for best result\n");
    rgb(RGBGREY);
    printf("\n--- Input ");
    rgb(RGBHIGHLIGHT);
    printf("ID or English name");
    rgb(RGBGREY);
    printf(" of the station ---\n");
    printf("Note: Ambiguous name triggers search function\n");
}

void registerConfirm(int i) {
    printf("\n %3i : ",i);
    printColorBlock(station[i].line);
    printf(" %s %s",station[i].name,station[i].chineseName);
    printf(" has been selected\n");
    return;
}

void printInvaild() {
    printf("\nInvaild Input\n");
    system("pause>nul");
    return;
}

int inputAndSearch() {
    char ch, inputString[MAXSTATIONNAME];
    int i=0;
    showSearchScore=false;

    while ((ch=getch())!=EOF && ch!='\n' && ch!='\r') { //input module
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
        exit(0);
    }

    if(inputString[i-1]=='*') { //check last digit contains '*'
        showSearchScore=true;
        inputString[i-1]=0;
    }

    int nonSpaceCount=0;
    for(int i=0; inputString[i]!=0; i++) { //Remove Space In char inputString;
        if(inputString[i]!=' ') {
            inputString[nonSpaceCount] = inputString[i];
            nonSpaceCount++;
        }
    }
    inputString[nonSpaceCount]=0;
    //printf("%s\n",inputString);

    int checkChar=0;
    for(i=0; i<strlen(inputString); i++) {
        if(inputString[i]<'0' || inputString[i]>'9' ) checkChar=1;
    }
    if(checkChar==0) {
        i=atoi(inputString);
        if(i>0 && i<MAXSTATION) {
            return i; //return i = station id = input number valid
        } else {
            return 0;
        }
    } else {
        if(strlen(inputString)<3 || !isalnum(inputString[0])) { //word shorter than 3 byte is not valid
            return 0;
        }

        i=identifyStation(inputString); //throw string to search

        if(i==0) {
            return 0;
        } else {
            return i;
        }
    }
}

int identifyStation(char inputString[MAXSTATIONNAME]) {
    int matchScore[MAXSTATION]= {0}; //matchScore = array of weighting scores
    char name[MAXSTATIONNAME];

    for(int i=0; i<strlen(inputString); i++) //toupper inputString
        inputString[i]=toupper(inputString[i]);

    for(int i=1; i<MAXSTATION; i++) { //Compare and rate inputString in each station[i].name
        matchScore[i]=0;
        strcpy(name,station[i].name); //Copy station[i].name to name

        for(int j=0; j<strlen(name); j++) { //toupper name
            name[j]=toupper(name[j]);
        }

        for(int j=0; j<strlen(inputString)-2; j++) { //Compare 3 letters consecutively include space, if match +10 score
            for(int k=0; k<strlen(name)-2; k++) {
                if(inputString[j]==name[k] && inputString[j+1]==name[k+1] && inputString[j+2]==name[k+2]) {
                    matchScore[i]+=15;
                    break;
                }
            }
        }

        int nonSpaceCount=0;
        for(int j=0; name[j]!=0 ; j++) { //Remove Space in name
            if(name[j]!=' ') {
                name[nonSpaceCount]=name[j];
                nonSpaceCount++;
            }
        }
        name[nonSpaceCount]=0;
        //printf("%s\n",name);

        if(strcmp(name, inputString)==0) { //if station[i].name==inputString, return id
            return i;
        }
        if(strcmp(station[i].abbrev, inputString)==0) {
            return i;
        }

        /*
        Main Scoring System
        */

        int alphabetCount[26]= {0};
        for(int j=0; j<strlen(name); j++) {
            alphabetCount[name[j]-65]=1;
        }
        /*
        for(int k=0; k<26; k++) {
            printf("%c:%i\n",k+65,alphabetCount[k]);
        }
        */

        for(int j=0; j<strlen(inputString); j++) {
            for(int k=0; k<26; k++) {
                if(inputString[j]==k+65 && alphabetCount[k]==1) {
                    matchScore[i]=matchScore[i]+8;
                }
            }
        }

        for(int j=0; j<strlen(inputString)-1; j++) {
            for(int k=0; k<strlen(name)-1; k++) {
                if(inputString[j]==name[k] && inputString[j+1]==name[k+1]) {
                    matchScore[i]+=3;
                    break;
                }
            }
        }

        for(int j=0; j<strlen(inputString)-2; j++) {
            for(int k=0; k<strlen(name)-2; k++) {
                if(inputString[j]==name[k] && inputString[j+1]==name[k+1] && inputString[j+2]==name[k+2]) {
                    matchScore[i]+=5;
                    break;
                }
            }
        }

        for(int j=0; j<strlen(inputString)-3; j++) {
            for(int k=0; k<strlen(name)-3; k++) {
                if(inputString[j]==name[k] && inputString[j+1]==name[k+1] && inputString[j+2]==name[k+2] && inputString[j+3]==name[k+3]) {
                    matchScore[i]+=10;
                    break;
                }
            }
        }

        if(showSearchScore==true) printf("\nS:%02i %s %i",i,station[i].name,matchScore[i]); //input * at the end, show scores of match[i]
    }

    int topScore=0, secScore=0;
    for(int i=1; i<MAXSTATION; i++) { //find topScore and secScore
        if(matchScore[i]>topScore) {
            secScore=topScore;
            topScore=matchScore[i];
        } else if(matchScore[i]>secScore && matchScore[i]<topScore) {
            secScore=matchScore[i];
        }
    }

    if(showSearchScore==true) printf("\n\nHighest: %i Second: %i\n",topScore,secScore); //input * at the end, show scores of match[i]

    if((topScore>25 && secScore>24) || topScore>42) {
        int resultCount=0;
        for(int i=0; i<MAXSTATION; i++) {
            if(matchScore[i]==topScore) resultCount++;
        }
        printf("\nDo you mean by:\n\n");

        int check[MAXSTATION]= {0}; //check[] stores interchange id
        if( (float)topScore / (float)secScore < 1.14) {
            for(int i=1; i<MAXSTATION; i++) {
                if(matchScore[i]==topScore || matchScore[i]==secScore) {
                    if(station[i].interchange!=0) {
                        if(check[station[i].interchange]==0) {
                            check[station[i].interchange]=1;
                            printStationChoose(i);
                        }
                    } else {
                        printStationChoose(i);
                    }
                }
            }
        } else {
            for(int i=1; i<MAXSTATION; i++) {
                if(matchScore[i]==topScore) {
                    if(station[i].interchange!=0) {
                        if(check[station[i].interchange]==0) {
                            check[station[i].interchange]=1;
                            printStationChoose(i);
                        }
                    } else {
                        printStationChoose(i);
                    }
                }
            }
        }

        printf("\nPlease Select: ");
        int input=inputAndSearch();

        if(input>0 && input<MAXSTATION) {
            return input;
        } else return 0;
    } else {
        return 0;
    }
}

void printStation(int i) {
    printf("| ID: %-3i | %3s | ", i,station[i].abbrev);
    printColorBlock(station[i].line);
    printf(" %-20s | ", station[i].name);
    printColorBlock(station[i].line);
    printf(" %s", station[i].chineseName);
    for(int j=strlen(station[i].chineseName); j<=18; j=j+3) printf("  ");
    printf("| %s",line[station[i].line].chineseName);
    for(int j=strlen(line[station[i].line].chineseName); j<=12; j=j+3) printf("  ");
    //system("pause");
    printf("|");
    return;
}

void printStationWithoutId(int i) {
    printf("| ");
    printColorBlock(station[i].line);
    printf(" %-20s | ", station[i].name);
    printColorBlock(station[i].line);
    printf(" %s", station[i].chineseName);
    for(int j=strlen(station[i].chineseName); j<=18; j=j+3) printf("  ");
    printf("| %s",line[station[i].line].chineseName);
    for(int j=strlen(line[station[i].line].chineseName); j<=12; j=j+3) printf("  ");
    //system("pause");
    printf("|");
    return;
}

void printStationChoose(int i) {
    printStation(i);
    printf("\n");
    return;
}

int fileGraphInput() {
    FILE *fp;
    int i=0,j=0;  //file row and col counter
    char s[MAXFILESTRING]="";
    char const csv[2]=",";
    char *token;
    fp=fopen("data/graph.csv","r");
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
    fp=fopen("data/station.csv","r");
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
        strcpy(station[i].abbrev,token);
        token=strtok(NULL,csv);
        strcpy(station[i].name,token);
        token=strtok(NULL,csv);
        strcpy(station[i].chineseName,token);
        token=strtok(NULL,csv);
        station[i].line=atoi(token);
        token=strtok(NULL,csv);
        station[i].interchange=atoi(token);
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
    fp=fopen("data/line.csv","r");
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
        token=strtok(NULL,csv);
        //printf("%i\n",strtol(token,NULL,16)); //rgb
        line[i].rgb=strtol(token,NULL,16);
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
        if(station[i].line!=currentLine) {
            printf("|\n");
            currentLine=station[i].line;
        }
        printStation(i);
        //system("pause");
        printf("\n");
    }
    return;
}

void printStructLine() {
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
    for(int i=1; i<MAXSTATION; i++) printf("| %03i | %02i |\n", i, station[i].interchange);
    printf("\n");

    int check[30]= {0};
    printf("\n");
    for(int i=1; i<MAXSTATION; i++) {
        if(station[i].interchange>0)
            if(check[station[i].interchange]==0) {
                printf("| %2i | %-30s \n",station[i].interchange,station[i].name);
                check[station[i].interchange]=1;
            }
    }
    printf("\n");
}

void printAbbrev(){
    for(int i=0; i<MAXSTATION; i++){
        printf("%3s\n",station[i].abbrev);
    }
}

void printColorBlock(int i) {
    rgb(i);
    printf("■");
    rgb(RGBGREY);
}

void rgb(int color) {  //Set Display Colors
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),color);
    return;
}

void rgbExTypedef() {
    typedef struct _CONSOLE_SCREEN_BUFFER_INFOEX {
        ULONG      cbSize;
        COORD      dwSize;
        COORD      dwCursorPosition;
        WORD       wAttributes;
        SMALL_RECT srWindow;
        COORD      dwMaximumWindowSize;
        WORD       wPopupAttributes;
        BOOL    bFullscreenSupported;
        COLORREF   ColorTable[16];
    } CONSOLE_SCREEN_BUFFER_INFOEX, *PCONSOLE_SCREEN_BUFFER_INFOEX;
}

void rgbEx() {
    rgbExTypedef();

    COLORREF colors[16];
    colors[RGBBLACK]=0x000000;
    colors[RGBHIGHLIGHT]=RGB(252,211,85);
    colors[RGBGREY]=0xCCCCCC;
    colors[RGBWHITE]=0xFFFFFF;

    int r, g, b;
    for(int i=1; i<MAXLINE; i++) {
        r = ((line[i].rgb >> 16) & 0xFF);
        g = ((line[i].rgb >> 8) & 0xFF);
        b = ((line[i].rgb) & 0xFF);

        colors[i]=RGB(r,g,b);
    }

    rgb(RGBGREY);
    CONSOLE_SCREEN_BUFFER_INFOEX csbi = {sizeof(CONSOLE_SCREEN_BUFFER_INFOEX)};
    GetConsoleScreenBufferInfoEx(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    memcpy(csbi.ColorTable, colors, sizeof(colors));
    csbi.srWindow.Bottom=csbi.srWindow.Bottom+1;
    SetConsoleScreenBufferInfoEx(GetStdHandle(STD_OUTPUT_HANDLE),&csbi);
    return;
}

void rgbExDefault() {
    rgbExTypedef();

    COLORREF colors[16];
    colors[0]=RGB(12,12,12);
    colors[1]=RGB(0,55,218);
    colors[2]=RGB(19,161,14);
    colors[3]=RGB(58,150,221);
    colors[4]=RGB(197,15,31);
    colors[5]=RGB(136,23,152);
    colors[6]=RGB(193,156,0);
    colors[7]=RGB(204,204,204);
    colors[8]=RGB(118,118,118);
    colors[9]=RGB(59,120,255);
    colors[10]=RGB(22,198,12);
    colors[11]=RGB(97,214,214);
    colors[12]=RGB(231,72,86);
    colors[13]=RGB(180,0,158);
    colors[14]=RGB(249,241,165);
    colors[15]=RGB(242,242,242);

    CONSOLE_SCREEN_BUFFER_INFOEX csbi = {sizeof(CONSOLE_SCREEN_BUFFER_INFOEX)};
    GetConsoleScreenBufferInfoEx(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    memcpy(csbi.ColorTable, colors, sizeof(colors));
    csbi.srWindow.Bottom=csbi.srWindow.Bottom+1;
    SetConsoleScreenBufferInfoEx(GetStdHandle(STD_OUTPUT_HANDLE),&csbi);
    return;
}

void rgbPrintAll() {
    for(int i=0; i<255; i++) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),i);
        printf("%02i ",i);
    }
    return;
}

/*
void abbrevInit() {
    //Exceptions for MTR abbrev
    strcpy(station[50].abbrev, "TIL");
    strcpy(station[54].abbrev, "TIL");
    strcpy(station[70].abbrev, "TWO");
    strcpy(station[90].abbrev, "TIS");
    strcpy(station[76].abbrev, "SUN");
    strcpy(station[115].abbrev, "SUN");

    for(int i=0; i<MAXSTATION; i++) {
        if(strlen(station[i].abbrev)==0) {
            int spaceCount=0, spaceFirst=0, spaceSecond=0;
            char tempName[MAXSTATIONNAME];
            strcpy(tempName, station[i].name);

            for(int j=0; j<strlen(tempName); j++) {
                if(tempName[j]==' ') {
                    spaceCount++;
                    if(spaceFirst==0) {
                        spaceFirst=j;
                    } else if(spaceSecond==0) {
                        spaceSecond=j;
                    }
                }
                tempName[j] = toupper(tempName[j]);
            }

            if(spaceCount==0) {
                strncpy(station[i].abbrev, tempName, 3);
            }
            if(spaceCount==1) {
                strncpy(station[i].abbrev, tempName, 2);
                station[i].abbrev[2] = tempName[spaceFirst+1];
            }
            if(spaceCount>=2) {
                station[i].abbrev[0] = tempName[0];
                station[i].abbrev[1] = tempName[spaceFirst+1];
                station[i].abbrev[2] = tempName[spaceSecond+1];
            }
        }
    }
}
*/

void termsAndAgreement() {
    printf("\n");
    printf("This program is not for commercial use.\n");
    printf("All copyright belongs to MTR.\n");
    printf("\n");
    printf("Press Y to proceed.\n");
    while (toupper(getch())!='Y') {}
    system("cls");
}

void init() {
    //abbrevInit();
}

void main() {
    system("chcp 65001");
    system("cls");
    Sleep(50);

    int checkFile=0;
    checkFile+=fileGraphInput();
    checkFile+=fileStationInput();
    checkFile+=fileLineInput();
    if(checkFile>0) {
        system("pause>nul");
        exit(0);
    }

    init();

    rgbEx();
    rgb(RGBGREY);
    //termsAndAgreement();

    //printAbbrev();
    //printStructStation();
    //printGraph();
    //printInterchange();
    //printStructLine();

    do {
        printStructStation();
        userInput();
        checkInterchangeAndDijkstra();
        //printFullDijkstra();
        reverseDijkstra(); //Reverse the inverted dijkstra result map
        dijkstraResult();

        //lowestTurnAlogrithm();
        system("cls");
    } while(1);

    rgbExDefault();
}
