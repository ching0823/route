//_ching 2019-07-12
#include <stdio.h>
#include <windows.h>
#include <wincon.h>
#define LINE 10
#define STATION 30
#define MAXSTATION 95
#define MAXSTATIONNAME 25

FILE *metro;
int show=0;
int stationchange[MAXSTATION]= {0};
int line[LINE][STATION]= {0};
int lineRoute[MAXSTATION]= {0}, lineRoutePending[MAXSTATION]= {0};
char stationName[MAXSTATION][MAXSTATIONNAME];

main() {
	system("chcp 65001");
	//do {
	system("cls");
	calculate();
	//} while(1);
}

int calculate() {
	int check=0,lineNum=0,ori_code=0,des_code=0;
	rgb(7,1);
	show=0;

	check=FileInput(); //Get File Data;
	if(check!=1) {
		printf("Data File Is Missing Or Corrupted.\n");
		system("pause>nul");
		return 0;
	}

	//listAllStation();

	rgb(15,1);
	printf("Please Select Origin: (Station ID [3 bits] or Text Input)\n\n  ");
	rgb(14,1);
	printf("Origin");
	rgb(7,1);
	printf(": ");
	rgb(15,1);
	ori_code=input();
	if(ori_code==0) {
		printInvaild();
		return 0;
	} else registerConfirm(ori_code);
	rgb(15,1);
	printf("Please Select Destination: (Station ID [3 bits] or Text Input)\n\n  ");
	rgb(14,1);
	printf("Destination");
	rgb(7,1);
	printf(": ");
	rgb(15,1);
	des_code=input();
	if(des_code==0) {
		printInvaild();
		return 0;
	} else registerConfirm(des_code);
	if(checkSameStation(ori_code, des_code)==1) {
		printf("Same station.\n");
		system("pause>nul");
		return 0;
	}
	lineNum=checkSameLine(ori_code, des_code);
	if(lineNum>0) {
		linear(ori_code,des_code,lineNum);
		system("pause>nul");
		return 1;
	} else {
		route(ori_code,des_code);
		system("pause>nul");
		return 1;
	}
}

int input() {
	char ch, inputString[MAXSTATIONNAME];
	int i=0,check;
	show=0;
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
	rgb(7,1);
	check=0;
	if(inputString[i-1]=='*') {
		show=1;
		inputString[i-1]=0;
	}
	for(i=0; i<strlen(inputString); i++) {
		if(inputString[i]<'0' || inputString[i]>'9' ) check++;
	}
	if(check==0) {
		i=atoi(inputString);
		if(i>0 && i<MAXSTATION) {
			return i;
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

int route(int ori_code, int des_code) {
	printf("Finding...\n");
}

int printInvaild() {
	printf("\nInvaild Input\n");
	system("pause>nul");
}

int registerConfirm(int i) {
	rgb(13,1);
	printf("\n %03i",i);
	rgb(7,1);
	printf(": ");
	rgb(15,1);
	printf("%s",stationName[i]);
	rgb(7,1);
	printf(" Is Registered\n\n");
}

int FileInput() {
	int i,x,i_line=1,i_station=1,i_stationCode=1;
	char s[MAXSTATIONNAME],f_line[3],f_station[4],f_stationCode[4];
	metro=fopen("metro.txt","r");
	if(metro!=0) {
		while(!feof(metro)) {
			fgets(s,MAXSTATIONNAME,metro);
			if(s[0]=='-') break; //Finish Line Register;
			strncpy(f_line,s,2);
			f_line[2]=0;
			i_line=atoi(f_line);
			strncpy(f_station,s+2,2);
			f_station[2]=0;
			i_station=atoi(f_station);
			strncpy(f_stationCode,s+4,3);
			f_stationCode[3]=0;
			i_stationCode=atoi(f_stationCode);
			//printf("%02i %02i %03i\n",i_line,i_station,i_stationCode);
			line[i_line][i_station]=i_stationCode;
		}
		/*for(i=1; i<LINE; i++) { //Print Array:line[][]
			for(i_line=1; i_line<STATION; i_line++) //Do not print if no such station exists
				if(line[i][i_line]!=0) printf("%02i %02i %03i\n",i,i_line,line[i][i_line]);
		}
		puts("");*/
		while(!feof(metro)) { //Intersection Points;
			fgets(s,MAXSTATIONNAME,metro);
			if(s[0]=='-') break; //Finish Intersection Register;
			strncpy(f_station,s,3);
			f_station[3]=0;
			i_station=atoi(f_station);
			strncpy(f_stationCode,s+3,3);
			f_stationCode[3]=0;
			i_stationCode=atoi(f_stationCode);
			stationchange[i_station]=i_stationCode;
		}
		/*for(i=1; i<MAXSTATION; i++)
			printf("S%02i To S%02i\n",i,stationchange[i]);
		puts("");*/
		i=1;
		while(!feof(metro)) {
			fgets(s,MAXSTATIONNAME,metro);
			if(s[0]=='-') break;
			for(x=0; x<strlen(s); x++) {
				if(!isalnum(s[x]) && s[x]!=' ') {
					s[x]=0;
					break;
				}
			}
			strcpy(stationName[i],s);
			i++;
		}
		/*for(i=1; i<MAXSTATION; i++)
			printf("%0i: %s\n",i,stationName[i]);
		puts("");*/
		fclose(metro);
		return 1;
	} else {
		fclose(metro);
		return 0;
	}
}

int identifyStation(char s[MAXSTATIONNAME]) {
	int i,j=0,k,l,m,match[MAXSTATION]= {0};
	char ch,input[4],name[MAXSTATIONNAME];
	for(i=0; i<strlen(s); i++) { //Remove Space In char s;
		if(s[i]==' ' || j>0) {
			if(s[i]==' ') j++;
			s[i-j+1]=s[i+1];
		}
	}
	s[i-j]=0;
	for(i=0; i<strlen(s); i++) s[i]=toupper(s[i]);
	for(i=1; i<MAXSTATION; i++) {
		match[i]=0;
		strcpy(name,stationName[i]);
		for(l=0; l<strlen(name); l++) {
			name[l]=toupper(name[l]);
		}
		for(j=0; j<strlen(s)-2; j++) {
			for(k=0; k<strlen(name)-2; k++) {
				if(s[j]==name[k] && s[j+1]==name[k+1] && s[j+2]==name[k+2]) {
					match[i]++;
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
		if(strcmp(name,s)==0) {
			return i;
		}
		for(j=0; j<strlen(s); j++) {
			for(k=0; k<strlen(name); k++) {
				if(s[j]==name[k]) {
					match[i]++;
					break;
				}
			}
		}
		for(j=0; j<strlen(s)-1; j++) {
			for(k=0; k<strlen(name)-1; k++) {
				if(s[j]==name[k] && s[j+1]==name[k+1]) {
					match[i]++;
					break;
				}
			}
		}
		for(j=0; j<strlen(s)-2; j++) {
			for(k=0; k<strlen(name)-2; k++) {
				if(s[j]==name[k] && s[j+1]==name[k+1] && s[j+2]==name[k+2]) {
					match[i]=match[i]+3;
					break;
				}
			}
		}
		for(j=0; j<strlen(s)-3; j++) {
			for(k=0; k<strlen(name)-3; k++) {
				if(s[j]==name[k] && s[j+1]==name[k+1] && s[j+2]==name[k+2] && s[j+3]==name[k+3]) {
					match[i]=match[i]+5;
					break;
				}
			}
		}
		if(show>0) printf("\nS:%02i %s %i",i,stationName[i],match[i]); //scores of match[i]
	}
	k=l=0;
	for(i=1; i<MAXSTATION; i++) {
		if(match[i]>k) {
			l=k;
			k=match[i];
		} else if(match[i]>l && match[i]<k) {
			l=match[i];
		}
	}
	if(show>0) printf("\n\nHighest: %i Second: %i\n",k,l);
	if(k>8) {
		j=0;
		for(i=1; i<MAXSTATION; i++) {
			if(match[i]==k) j++;
		}
		printf("\nPlease Select Station Below:\n\n");
		if(j<2) {
			for(i=1; i<MAXSTATION; i++) {
				if(match[i]==k) {
					printStationChoose(i);
				}
			}
			if(k-l<6) {
				for(i=1; i<MAXSTATION; i++) {
					if(match[i]==l) {
						printStationChoose(i);
					}
				}
			}
		} else {
			m=0;
			for(i=1; i<MAXSTATION; i++) {
				if(match[i]==k) {
					if(stationchange[i]>0) {
						if(i!=m) {
							m=stationchange[i];
							printStationChoose(i);
						}
					} else {
						printStationChoose(i);
					}
				}
			}
		}
		puts("");
		printf("Input ");
		rgb(11,1);
		printf("Code");
		rgb(7,1);
		printf(" Of Station: ");
		j=0;
		rgb(11,1);
		while ((ch=getch())!=EOF && ch!='\n' && ch!='\r') {
			if (ch==8 && j>0) {
				printf("\b \b");
				fflush(stdout);
				j--;
			} else if(ch>='0' && ch<='9' && j<sizeof(input)-1) {
				printf("%c",ch);
				input[j++]=(char)ch;
			}
		}
		input[j]=0;
		rgb(7,1);
		puts("");
		i=atoi(input);
		if(i>0 && i<MAXSTATION) {
			return i;
		} else return 0;
	} else {
		return 0;
	}
}

int printStationChoose(int i) {
	rgb(11,1);
	printf(" %i",i);
	rgb(15,1);
	printf(": %s\n",stationName[i]);
	rgb(7,1);
}

int checkSameLine(int ori_code, int des_code) {
	int a=0,temp=0,temp2=0,invert=0;
	if(stationchange[ori_code]!=0) { //checkIntersection
		temp=stationchange[ori_code];
	}
	if(stationchange[des_code]!=0) { //checkIntersection
		temp2=stationchange[des_code];
	}
	//printf("t1:%i t2:%i\n",temp,temp2);
	a=sameLine(ori_code,des_code);
	if(a>0) return a;
	if(temp!=0 && temp2!=0) {
		a=sameLine(ori_code,temp2);
		if(a>0) return a+2;
		a=sameLine(temp,des_code);
		if(a>0) return a+1;
		a=sameLine(temp,temp2);
		if(a>0) return a+3;
		return 0;
	} else if(temp!=0 && temp2==0) {
		a=sameLine(temp,des_code);
		if(a>0) return a+1;
		return 0;
	} else if(temp==0 && temp2!=0) {
		a=sameLine(ori_code,temp2);
		if(a>0) return a+2;
		return 0;
	} else return 0;
}

int checkChangeMoreThanOne() {
	int i,j;
}

int sameLine(int ori_code, int des_code) {
	int a=0,i,j,x,y;
	for(i=1; i<LINE; i++) {
		if(a==1) {
			i--;
			break;
		}
		for(j=1; j<STATION; j++) {
			if(line[i][j]==ori_code) {
				a=1;
				break;
			}
		}
	}
	a=0;
	x=i;
	y=j;
	for(i=1; i<LINE; i++) {
		if(a==1) {
			i--;
			break;
		}
		for(j=1; j<STATION; j++) {
			if(line[i][j]==des_code) {
				a=1;
				break;
			}
		}
	}
	if(x==i) return i*10;
	else return 0;
}

int listAllStation() {
	int i;
	for(i=1; i<MAXSTATION; i++) {
		printf("%02i: %s\n",i,stationName[i]);
	}
	puts("");
}

int checkSameStation(int ori_code, int des_code) {
	int temp=0;
	temp=stationchange[ori_code];
	if(temp!=0) {
		if(temp==des_code || ori_code==des_code) return 1;
		return 0;
	} else {
		if(ori_code==des_code) return 1;
		return 0;
	}
}

int linear(int ori_code, int des_code, int lineNum) {
	int i,j;
	j=lineNum%10;
	lineNum=lineNum/10;
	if(j==1) ori_code=stationchange[ori_code];
	if(j==2) des_code=stationchange[des_code];
	if(j==3) {
		ori_code=stationchange[ori_code];
		des_code=stationchange[des_code];
	}
	i=ori_code;
	puts("");
	if(ori_code>des_code) {
		rgb(10,1);
		printf(" %02i",i);
		rgb(15,1);
		printf(": %s \n ↓\n",stationName[i]);
		rgb(7,1);
		for(i=ori_code-1; i>des_code; i--) {
			rgb(7,1);
			printf(" %02i",i);
			rgb(15,1);
			printf(": %s \n ↓\n",stationName[i]);
			rgb(7,1);
		}
	} else {
		rgb(10,1);
		printf(" %02i",i);
		rgb(15,1);
		printf(": %s \n ↓\n",stationName[i]);
		rgb(7,1);
		for(i=ori_code+1; i<des_code; i++) {
			rgb(7,1);
			printf(" %02i",i);
			rgb(15,1);
			printf(": %s \n ↓\n",stationName[i]);
			rgb(7,1);
		}
	}
	rgb(12,1);
	printf(" %02i",i);
	rgb(15,1);
	printf(": %s \n",stationName[i]);
	rgb(7,1);
	puts("");
}

int rgb(int color, int mode) {  //Set Display Colors;
	int num;
	if(mode==1) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),color);
	} else {
		for(num=0; num<100; num++) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),num);
			printf("%02i ",num);
		}
	}
}
