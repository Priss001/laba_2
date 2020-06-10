/* Ќачальный процесс €вл€етс€ управл€ющим. 
ќн принимает поток ввода с клавиатуры и контролирует дочерние процессы. 
ѕо нажатию клавиши С+Т добавл€етс€ новый процесс, 
С-Т Ц удал€етс€ последний добавленный, 
СqТ Ц программа завершаетс€. 
 аждый дочерний процесс посимвольно выводит на экран в вечном цикле свою уникальную строку. 
ѕри этом операци€ вывода строки должна быть атомарной, т.е. процесс вывода должен быть синхронизирован таким образом, 
чтобы строки на экране не перемешивались. 
¬ыполн€етс€ в двух вариантах: под Linux и Windows. 
¬ качества метода синхронизации использовать сигналы/событи€.
*/

#define _UNICODE
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <conio.h>

using namespace std;

int main()
{
	STARTUPINFO si; 
	PROCESS_INFORMATION pi; 

	ZeroMemory(&si, sizeof(si)); 
	ZeroMemory(&pi, sizeof(pi)); 

    wchar_t wline[] = L"D:\\”чЄба\\cods(C++)\\LAB_2_WIN\\Debug\\chld.exe"; 
	
	HANDLE mas_event[100];

	bool plus = false;
	bool minus = false;
	bool q = false;
	bool one = false;
	bool check = false;

	char ch; 
	
	short mas_proc[100];
	short count = 0;
	short src = 0;
	short i = 0;
	
	cout<<"Add: +"<<endl<<"Delete: -"<<endl<<"Exit: q"<<endl<<endl;

	while(1)
	{
	if(count == 1)
		{
			SetEvent(mas_event[0]);
		}

	if(count > 1)
		{
			if((WaitForSingleObject(mas_event[src], 0)) == WAIT_TIMEOUT)
			{
				if((WaitForSingleObject(mas_event[i],0)) == WAIT_TIMEOUT && one == false)
				{
					one = true; 
					if(i!=0){;i++;}
					else if(check == true){i++; check = false;}
					if(src == 0){ i = 0; }
					src = i + 1; 
					if(src == count) { src = 0; }
					SetEvent(mas_event[i]);
				} 
				if((WaitForSingleObject(mas_event[i],0)) == WAIT_TIMEOUT && one == true)
				{  
					one = false;
					if(src != 0){ i++; } 
					else { i = 0; check = true;} 
					src = i + 1; if(src == count) {src = 0; }
					SetEvent(mas_event[i]); 
				}
			}
		}

	if(plus == false) { if(GetAsyncKeyState(0x6B)) {

			if (CreateProcess(NULL, wline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi) == NULL)
			{
				cout << "Error." << endl << "Process was't created!" << endl; plus = true;
			}

			else
			{
			count++;
			cout << endl << endl << count << "    Process ID = " << pi.dwProcessId << endl << endl; 
			
			wchar_t buf[20]; 
			_itow(pi.dwProcessId, buf, 10); 
			
			mas_proc[count-1] = pi.dwProcessId;
			mas_event[count-1] = CreateEvent(NULL, TRUE, TRUE, buf);
			
			if(count > 1)
			{
				for(int i = 0; i < (count-1); i++)
				{
					ResetEvent(mas_event[i]);
				}
				one = false;
				src = count - 1;
				i = 0;
			}

			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess); 
			plus = true;
			}}}
		else if(plus == true) { Sleep(200); plus = false; }

	if(minus == false) { if(GetAsyncKeyState(0x6D)) {

			if (count == 0) 
			{ 
				cout<<"Process did't finded!"<<endl; minus = true;	
			}

			else
			{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, mas_proc[count-1]);
			DWORD dwError = ERROR_SUCCESS;

			if(TerminateProcess(hProcess, ((DWORD)-1)) != NULL)
				cout << endl << endl << count << "    Process is finished"<< endl << endl;

			for(int j = 0; j < count; j++)
				ResetEvent(mas_event[j]);
			
			count--;

			if(count > 0) { 
				if(count == 1) 
					src = 0; 
				else
					src = 1;
				}
				i = 0; one = false; check = false; }
				minus = true;
			}}
		else if(minus == true) { Sleep(200); minus = false; }
		
	if(q == false) { if(GetAsyncKeyState(0x51)) {
		for(int j = 0; j < count; j++)
		{
			HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, mas_proc[j]);
			DWORD dwError = ERROR_SUCCESS;
			TerminateProcess(hProcess, ((DWORD)-1));
			ResetEvent(mas_event[j]);
		}
			cout << endl << endl << "Exit..." << endl;
			return 0;
		}
		}
	} 

	return 0;
	}