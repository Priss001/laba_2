/*
Начальный процесс является управляющим. Он принимает поток ввода с
клавиатуры и контролирует дочерние процессы. По нажатию клавиши ‘+’ до-
бавляется новый процесс, ‘-’ – удаляется последний добавленный, ‘q’ – про-
грамма завершается. Каждый дочерний процесс посимвольно выводит на экран
в вечном цикле свою уникальную строку. При этом операция вывода строки
должна быть атомарной, т.е. процесс вывода должен быть синхронизирован та-
ким образом, чтобы строки на экране не перемешивались. Выполняется в двух
вариантах: под Linux и Windows. В качества метода синхронизации использо-
вать сигналы/события.
*/
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ncurses.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sstream>

using namespace std;

pid_t mas_child[200];
int after = 1;
int num_now_process = 1;
int chlength = 0;
int before = 0;

struct sigaction sig_1, sig_2;

void term_process()
{
    if(after)
    {
      kill(getppid(),SIGUSR2);
      exit(0);
    }
}

void handle_1(int signo)
{
    before = 1;
}

void handle_2(int signo)
{
    after = 1;
}

int main(void)
{
    initscr();
    clear();
    refresh();

    int int_id;
    stringstream strs;
    string temp_str = strs.str();
    char* char_type = (char*) temp_str.c_str();
    sig_1.sa_handler = handle_1;
    sigaction(SIGUSR1,&sig_1,NULL);

    sig_2.sa_handler = handle_2;
    sigaction(SIGUSR2,&sig_2,NULL);

    char c = 0;
    int i = 0;
    int j = 0;

    mas_child[0]=getpid();
    printf("Main process ID = %d\n", mas_child[0]);
    printf("\r\nAdd new process - '+' ");
    printf("\r\nDelete last process - '-' ");
    printf("\r\nExit from the program - 'q'\n\r");

    while(c!='q')
    {
        switch(c=getchar())
        {
            case '+':
            {
                if(chlength < 200)
                {
                    chlength++;
                    mas_child[chlength] = fork();
                }

                switch(mas_child[chlength])
                {
                    case 0: //child process
                    {
                        after = 0;
                        while(!after)// = 0
                        {
                            usleep(10000);
                                if(before)// = 1
                                {
                                    term_process();
                                    int_id = getpid();
                                    strs.str("");
                                    strs << int_id;
                                    temp_str = strs.str();
                                    char_type = (char*) temp_str.c_str();
                                    printf("\r\n");
                                    for(j = 0; char_type[j]>=48 && char_type[j]<=57; j++)
                                        { putchar(char_type[j]); }
                                    usleep(1000000);
                                    refresh();
                                    before = 0;
                                    kill(getppid(),SIGUSR2);
                                }
                            term_process();
                        }
                    }
                    break;

                    case -1:
                    {
                        printf("Process - [%d] ERROR!\n",chlength);
                    }
                    default:
                        printf("\r\n Creating new process - %d(%d)\n\n\r", mas_child[chlength], chlength);
                    break;
              }
          }
          break;

          case '-':
          {
              if(chlength==0){ printf("\n\rNo processes!\r\n");break; }
              printf("\n\r Delete the %d process!", chlength);
              kill(mas_child[chlength],SIGUSR2);
              waitpid(mas_child[chlength],NULL,0);
              sleep(1);
              printf("\r\n\n");
              if(num_now_process == chlength) num_now_process = 1;
              chlength--;
          }
          break;
    }
    if(after && chlength>0)
    {
        after = 0;
        kill(mas_child[num_now_process++],SIGUSR1);
        if(num_now_process > chlength)
        num_now_process = 1;
    }
    refresh();
    }

    if(mas_child[chlength]!=0)
    {
        for(;chlength>0;chlength--)
        {
            kill(mas_child[chlength],SIGUSR2);
            waitpid(mas_child[chlength],NULL,0);
        }
    }

    clear();
    endwin();

    return 0;
    }
